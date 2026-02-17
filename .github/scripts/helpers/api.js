// SPDX-License-Identifier: Apache-2.0
//
// helpers/api.js
//
// Bot context builder and GitHub API wrappers (labels, assignees, comments).

const fs = require('fs');
const { getLogger } = require('./logger');
const {
  isObject,
  isSafeSearchToken,
  requireObject,
  requireNonEmptyString,
  requirePositiveInt,
  requireSafeUsername,
} = require('./validation');

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

        if (!isSafeSearchToken(comment.user.login)) {
          throw new Error('Bot context invalid: comment.user.login contains invalid characters');
        }
        if (typeof comment.body !== 'string') {
          throw new Error('Bot context invalid: comment.body must be a string');
        }
        
        payloadPart = { ...payloadPart, comment };
      }
      break;
    }
    default:
      throw new Error(`Bot context invalid: unsupported event type "${eventType}"`);
  }
  return { ...base, eventType, ...payloadPart };
}

/**
 * Writes key-value pairs to GITHUB_OUTPUT when set (e.g. in a workflow step).
 * Use this to pass outputs to later steps via steps.<id>.outputs.<key>.
 *
 * @param {Record<string, string|number|boolean>} keyValues - Object of output names to values (coerced to string).
 */
function writeGithubOutput(keyValues) {
  const path = process.env.GITHUB_OUTPUT;
  if (!path) return;
  if (!keyValues || !isObject(keyValues)) return;

  try {
    for (const [key, value] of Object.entries(keyValues)) {
      if (key === undefined || key === null) {
        continue;
      }

      const line = `${key}=${String(value)}\n`;
      fs.appendFileSync(path, line, 'utf8');
    }
    getLogger().log('Wrote to GITHUB_OUTPUT:', Object.keys(keyValues).join(', '));
  } catch (err) {
    getLogger().error('Failed to write GITHUB_OUTPUT:', err.message);
  }
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
  } catch (error) {
    getLogger().error(`Could not post comment: ${error.message}`);
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

module.exports = {
  buildBotContext,
  writeGithubOutput,
  addLabels,
  removeLabel,
  addAssignees,
  postComment,
  hasLabel,
};
