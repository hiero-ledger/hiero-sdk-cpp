// SPDX-License-Identifier: Apache-2.0
//
// helpers/api.js
//
// Bot context builder and GitHub API wrappers (labels, assignees, comments,
// commit/issue fetching, and label swap helpers).

const { getLogger } = require('./logger');
const {
  isSafeSearchToken,
  requireObject,
  requireNonEmptyString,
  requirePositiveInt,
  requireSafeUsername,
} = require('./validation');
const { LABELS } = require('./constants');
const { checkDCO, checkGPG, checkMergeConflict, checkIssueLink } = require('./checks');
const { buildBotComment } = require('./comments');

/**
 * Builds the bot context for any bot. Validates github, context, and payload; throws if invalid.
 * Returned object always includes eventType; then event-specific fields (number, pr/issue, and comment for issue_comment).
 *
 * @param {{ github: object, context: object }} args - The arguments from the workflow.
 * @returns {{ github: object, owner: string, repo: string, eventType: string, ... }}
 *   - pull_request / pull_request_target: also number, pr
 *   - issues: also number, issue
 *   - issue_comment: also number, issue, comment
 * @throws {Error} If input is invalid or event type is unsupported.
 */
function buildBotContext({ github, context }) {
  requireObject(github, 'github');
  requireObject(context, 'context');
  requireObject(context.repo, 'context.repo');
  requireObject(context.payload, 'context.payload');

  const owner = context.repo.owner;
  const repo = context.repo.repo;
  requireNonEmptyString(owner, 'context.repo.owner');
  requireNonEmptyString(repo, 'context.repo.repo');
  if (!isSafeSearchToken(owner) || !isSafeSearchToken(repo)) {
    throw new Error('Bot context invalid: owner or repo contains invalid characters');
  }

  const base = { github, owner, repo };

  requireNonEmptyString(context.eventName, 'context.eventName');
  const eventType = context.eventName;

  const { payload } = context;
  let payloadPart;
  switch (eventType) {
    case 'pull_request':
    case 'pull_request_target': {
      const pr = payload.pull_request;
      requireObject(pr, 'context.payload.pull_request');
      requirePositiveInt(pr.number, 'pull_request.number');

      if (pr.user) {
        requireNonEmptyString(pr.user.login, 'pull_request.user.login');
        if (!isSafeSearchToken(pr.user.login)) {
          throw new Error('Bot context invalid: pull_request.user.login contains invalid characters');
        }
      }

      payloadPart = { number: pr.number, pr };
      break;
    }

    case 'issues':
    case 'issue_comment': {
      const issue = payload.issue;
      requireObject(issue, 'context.payload.issue');
      requirePositiveInt(issue.number, 'issue.number');
      payloadPart = { number: issue.number, issue };

      if (eventType === 'issue_comment') {
        const comment = payload.comment;
        requireObject(comment, 'context.payload.comment');
        requireObject(comment.user, 'context.payload.comment.user');
        requireNonEmptyString(comment.user.login, 'context.payload.comment.user.login');

        // Flag bot users early so callers can skip processing without
        // hitting the stricter username validation below.
        const isBot = comment.user.type === 'Bot';
        if (!isBot && !isSafeSearchToken(comment.user.login)) {
          throw new Error('Bot context invalid: comment.user.login contains invalid characters');
        }
        if (typeof comment.body !== 'string') {
          throw new Error('Bot context invalid: comment.body must be a string');
        }
        
        payloadPart = { ...payloadPart, comment, isBot };
      }
      break;
    }
    default:
      throw new Error(`Bot context invalid: unsupported event type "${eventType}"`);
  }
  return { ...base, eventType, ...payloadPart };
}

/**
 * Safely adds labels to an issue or PR.
 * @param {object} botContext - Bot context (github, owner, repo, number).
 * @param {string[]} labels - Array of label names to add.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function addLabels(botContext, labels) {
  if (!Array.isArray(labels)) {
    return { success: false, error: 'labels must be an array' };
  }
  
  try {
    for (let i = 0; i < labels.length; i++) {
      requireNonEmptyString(labels[i], `labels[${i}]`);
    }

    await botContext.github.rest.issues.addLabels({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
      labels,
    });

    getLogger().log(`Added labels: ${labels.join(', ')}`);
    return { success: true };
  } catch (error) {
    getLogger().error(`Could not add labels "${labels.join(', ')}": ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Safely removes a label from an issue or PR.
 * @param {object} botContext - Bot context (github, owner, repo, number).
 * @param {string} labelName - The label name to remove.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function removeLabel(botContext, labelName) {
  try {
    requireNonEmptyString(labelName, 'labelName');

    await botContext.github.rest.issues.removeLabel({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
      name: labelName,
    });

    getLogger().log(`Removed label: ${labelName}`);
    return { success: true };
  } catch (error) {
    getLogger().error(`Could not remove label "${labelName}": ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Safely adds assignees to an issue or PR.
 * @param {object} botContext - Bot context (github, owner, repo, number).
 * @param {string[]} assignees - Array of usernames to assign.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function addAssignees(botContext, assignees) {
  if (!Array.isArray(assignees)) {
    return { success: false, error: 'assignees must be an array' };
  }

  try {
    for (let i = 0; i < assignees.length; i++) {
      requireSafeUsername(assignees[i], `assignees[${i}]`);
    }

    await botContext.github.rest.issues.addAssignees({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
      assignees,
    });

    getLogger().log(`Added assignees: ${assignees.join(', ')}`);
    return { success: true };
  } catch (error) {
    getLogger().error(`Could not add assignees "${assignees.join(', ')}": ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Safely removes assignees from an issue or PR.
 * @param {object} botContext - Bot context (github, owner, repo, number).
 * @param {string[]} assignees - Array of usernames to remove.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function removeAssignees(botContext, assignees) {
  if (!Array.isArray(assignees)) {
    return { success: false, error: 'assignees must be an array' };
  }

  try {
    for (let i = 0; i < assignees.length; i++) {
      requireSafeUsername(assignees[i], `assignees[${i}]`);
    }

    await botContext.github.rest.issues.removeAssignees({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
      assignees,
    });

    getLogger().log(`Removed assignees: ${assignees.join(', ')}`);
    return { success: true };
  } catch (error) {
    getLogger().error(`Could not remove assignees "${assignees.join(', ')}": ${error.message}`);
    return { success: false, error: error.message };
  }
}


/**
 * Safely posts a comment on an issue or PR.
 * @param {object} botContext - Bot context (github, owner, repo, number).
 * @param {string} body - The comment body.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function postComment(botContext, body) {
  try {
    requireNonEmptyString(body, 'comment body');

    await botContext.github.rest.issues.createComment({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
      body,
    });
    getLogger().log('Posted comment');
    return { success: true };
  } catch (error) {
    getLogger().error(`Could not post comment: ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Checks if an issue or PR has a specific label.
 * @param {object} issueOrPr - The issue or PR object.
 * @param {string} labelName - The label name to check for.
 * @returns {boolean} - True if the label is present.
 */
function hasLabel(issueOrPr, labelName) {
  if (!issueOrPr?.labels?.length) {
    return false;
  }

  return issueOrPr.labels.some((label) => {
    const name = typeof label === 'string' ? label : label?.name;
    return typeof name === 'string' && name.toLowerCase() === labelName.toLowerCase();
  });
}

/**
 * Posts a new comment or updates an existing one identified by an HTML marker.
 * Paginates through all comments to find a match.
 * @param {object} botContext
 * @param {string} marker - HTML comment marker (e.g. '<!-- bot:pr-helper -->').
 * @param {string} body - Full comment body (must include the marker).
 * @returns {Promise<{success: boolean, error?: string}>}
 */
async function postOrUpdateComment(botContext, marker, body) {
  try {
    requireNonEmptyString(marker, 'marker');
    requireNonEmptyString(body, 'comment body');

    let existingCommentId = null;
    let page = 1;
    const perPage = 100;

    while (!existingCommentId) {
      const { data: comments } = await botContext.github.rest.issues.listComments({
        owner: botContext.owner,
        repo: botContext.repo,
        issue_number: botContext.number,
        per_page: perPage,
        page,
      });

      for (const c of comments) {
        if (c.body && c.body.startsWith(marker)) {
          existingCommentId = c.id;
          break;
        }
      }

      if (comments.length < perPage) break;
      page++;
    }

    if (existingCommentId) {
      await botContext.github.rest.issues.updateComment({
        owner: botContext.owner,
        repo: botContext.repo,
        comment_id: existingCommentId,
        body,
      });
      getLogger().log('Updated existing bot comment');
    } else {
      await botContext.github.rest.issues.createComment({
        owner: botContext.owner,
        repo: botContext.repo,
        issue_number: botContext.number,
        body,
      });
      getLogger().log('Created new bot comment');
    }
    return { success: true };
  } catch (error) {
    getLogger().error(`Could not post/update comment: ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Fetches all commits for a pull request via the GitHub API (paginated).
 * @param {object} botContext
 * @returns {Promise<Array>}
 */
async function fetchPRCommits(botContext) {
  const commits = [];
  let page = 1;
  const perPage = 100;

  while (true) {
    const response = await botContext.github.rest.pulls.listCommits({
      owner: botContext.owner,
      repo: botContext.repo,
      pull_number: botContext.number,
      per_page: perPage,
      page,
    });

    commits.push(...response.data);

    if (response.data.length < perPage) break;
    page++;
  }

  getLogger().log(`Fetched ${commits.length} commits for PR #${botContext.number}`);
  return commits;
}

/**
 * Fetches a single issue by number.
 * @param {object} botContext
 * @param {number} issueNumber
 * @returns {Promise<object>}
 */
async function fetchIssue(botContext, issueNumber) {
  const { data: issue } = await botContext.github.rest.issues.get({
    owner: botContext.owner,
    repo: botContext.repo,
    issue_number: issueNumber,
  });
  return issue;
}

/**
 * Fetches issue numbers linked to a PR via GitHub's closingIssuesReferences GraphQL field.
 * @param {object} botContext
 * @returns {Promise<number[]>}
 */
async function fetchClosingIssueNumbers(botContext) {
  try {
    const query = `query($owner:String!,$repo:String!,$number:Int!){
      repository(owner:$owner,name:$repo){
        pullRequest(number:$number){
          closingIssuesReferences(first:10){
            nodes { number }
          }
        }
      }
    }`;
    const result = await botContext.github.graphql(query, {
      owner: botContext.owner,
      repo: botContext.repo,
      number: botContext.number,
    });
    const nodes = result.repository.pullRequest.closingIssuesReferences.nodes || [];
    return nodes.map(n => n.number);
  } catch (error) {
    getLogger().error(`GraphQL closingIssuesReferences failed: ${error.message}`);
    return [];
  }
}

/**
 * Swaps between needs-review and needs-revision labels based on check results.
 * By default only changes the label if the opposite label is currently applied.
 * When force is true, unconditionally applies the target label (used on PR open
 * to guarantee a status label is always present).
 * @param {object} botContext
 * @param {boolean} allPassed
 * @param {{ force?: boolean }} [options]
 * @returns {Promise<void>}
 */
async function swapStatusLabel(botContext, allPassed, { force = false } = {}) {
  const pr = botContext.pr;
  const labelToAdd = allPassed ? LABELS.NEEDS_REVIEW : LABELS.NEEDS_REVISION;
  const labelToRemove = allPassed ? LABELS.NEEDS_REVISION : LABELS.NEEDS_REVIEW;

  if (force) {
    if (hasLabel(pr, labelToRemove)) {
      await removeLabel(botContext, labelToRemove);
    }
    await addLabels(botContext, [labelToAdd]);
  } else {
    if (hasLabel(pr, labelToRemove)) {
      await removeLabel(botContext, labelToRemove);
      await addLabels(botContext, [labelToAdd]);
    }
  }
}

/**
 * Adds a thumbs-up (+1) reaction to a comment as visual acknowledgement that
 * a bot command was received. Failures are silently logged (non-critical).
 *
 * @param {object} botContext - Bot context from buildBotContext (github, owner, repo).
 * @param {number} commentId - The ID of the comment to react to.
 * @returns {Promise<void>}
 */
async function acknowledgeComment(botContext, commentId) {
  try {
    await botContext.github.rest.reactions.createForIssueComment({
      owner: botContext.owner,
      repo: botContext.repo,
      comment_id: commentId,
      content: '+1',
    });
    getLogger().log('Added thumbs-up reaction to comment');
  } catch (error) {
    getLogger().log('Could not add reaction:', error.message);
  }
}

/**
 * Runs all 4 PR checks (DCO, GPG, merge conflict, issue link) with error
 * resilience, builds the unified dashboard comment, and posts/updates it.
 * Returns { allPassed } so callers can decide on label handling.
 * @param {object} botContext
 * @returns {Promise<{ allPassed: boolean }>}
 */
async function runAllChecksAndComment(botContext) {
  let dco, gpg, merge, issueLink;
  let commits = [];

  try {
    commits = await fetchPRCommits(botContext);
  } catch (e) {
    getLogger().error('Failed to fetch PR commits:', e.message);
    dco = { error: true, errorMessage: e.message };
    gpg = { error: true, errorMessage: e.message };
  }

  if (!dco) {
    try { dco = checkDCO(commits); }
    catch (e) { dco = { error: true, errorMessage: e.message }; }
  }

  if (!gpg) {
    try { gpg = checkGPG(commits); }
    catch (e) { gpg = { error: true, errorMessage: e.message }; }
  }

  try { merge = await checkMergeConflict(botContext); }
  catch (e) { merge = { error: true, errorMessage: e.message }; }

  try { issueLink = await checkIssueLink(botContext, { fetchIssue, fetchClosingIssueNumbers }); }
  catch (e) { issueLink = { error: true, errorMessage: e.message }; }

  const prAuthor = botContext.pr?.user?.login;
  const { marker, body, allPassed } = buildBotComment({ prAuthor, dco, gpg, merge, issueLink });
  await postOrUpdateComment(botContext, marker, body);

  return { allPassed };
}

/**
 * Resolves the primary issue linked to a PR.
 *
 * Strategy:
 *   - Fetch closing issue references via GraphQL
 *   - Return the first linked issue (if multiple exist)
 *   - Return null if no linked issues found
 *
 * Notes:
 *   - Logs informational messages for traceability
 *   - Does NOT throw — failures are handled gracefully
 *
 * @param {object} botContext
 * @param {object} logger
 * @returns {Promise<object|null>}
 */
async function resolveLinkedIssue(botContext) {
    try {
        const issueNumbers = await fetchClosingIssueNumbers(botContext);

        if (!issueNumbers.length) {
            getLogger().log('No linked issue found', {
              prNumber: botContext.number,
            });
            return null;
        }

        if (issueNumbers.length > 1) {
            getLogger().log('Multiple linked issues found (using first)', {
                issueNumbers,
                selected: issueNumbers[0],
            });
        }

        const issue = await fetchIssue(botContext, issueNumbers[0]);

        if (!issue) {
            getLogger().log('Linked issue fetch returned empty', {
                issueNumber: issueNumbers[0],
            });
            return null;
        }

        return issue;

    } catch (error) {
        getLogger().error('Failed to resolve linked issue:', {
            message: error.message,
        });
        return null;
    }
}

module.exports = {
  buildBotContext,
  addLabels,
  removeLabel,
  addAssignees,
  removeAssignees,
  postComment,
  hasLabel,
  postOrUpdateComment,
  fetchPRCommits,
  fetchIssue,
  fetchClosingIssueNumbers,
  swapStatusLabel,
  runAllChecksAndComment,
  resolveLinkedIssue,
  acknowledgeComment,
};
