// SPDX-License-Identifier: Apache-2.0
//
// commands/assign.js
//
// /assign command: assigns the commenter to the issue. Enforces skill-level
// prerequisites, assignment limits, and status labels. See bot-on-comment.js
// for high-level docs (limits, skill levels, required labels).

const {
  LABELS,
  ISSUE_STATE,
  getLogger,
  isNonNegativeInteger,
  isSafeSearchToken,
  hasLabel,
  addLabels,
  removeLabel,
  addAssignees,
  postComment,
} = require('../helpers');

const {
  MAX_OPEN_ASSIGNMENTS,
  SKILL_PREREQUISITES,
  buildWelcomeComment,
  buildAlreadyAssignedComment,
  buildNotReadyComment,
  buildPrerequisiteNotMetComment,
  buildNoSkillLevelComment,
  buildAssignmentLimitExceededComment,
  buildApiErrorComment,
  buildLabelUpdateFailureComment,
  buildAssignmentFailureComment,
} = require('./assign-templates');

// Delegate to the active logger set by the dispatcher (bot-on-comment.js).
// This ensures the correct prefix is used after command parsing.
const logger = {
  log: (...args) => getLogger().log(...args),
  error: (...args) => getLogger().error(...args),
};

function getIssueSkillLevel(issue) {
  const skillLevels = [LABELS.GOOD_FIRST_ISSUE, LABELS.BEGINNER, LABELS.INTERMEDIATE, LABELS.ADVANCED];
  for (const level of skillLevels) {
    if (hasLabel(issue, level)) return level;
  }
  return null;
}

/**
 * Counts issues assigned to a user matching specified criteria.
 * When state is OPEN and no label is given, issues with "status: blocked" are excluded.
 * @returns {Promise<number|null>} - Count or null on error.
 */
async function countAssignedIssues(github, owner, repo, username, state, label = null) {
  if (!isSafeSearchToken(owner) || !isSafeSearchToken(repo) || !isSafeSearchToken(username)) {
    logger.log('[assign] Invalid search inputs:', { owner, repo, username, label });
    return null;
  }
  if (state !== ISSUE_STATE.OPEN && state !== ISSUE_STATE.CLOSED) {
    logger.log('[assign] Invalid state:', { state });
    return null;
  }

  try {
    const queryParts = [
      `repo:${owner}/${repo}`,
      'is:issue',
      `is:${state}`,
      `assignee:${username}`,
    ];
    if (label) {
      if (typeof label !== 'string' || !label.trim() || label.includes('"')) {
        logger.log('[assign] Invalid label parameter:', { label });
        return null;
      }
      queryParts.push(`label:"${label}"`);
    }
    if (state === ISSUE_STATE.OPEN && !label) {
      queryParts.push(`-label:"${LABELS.BLOCKED}"`);
    }
    const searchQuery = queryParts.join(' ');
    const queryType = label
      ? state === ISSUE_STATE.CLOSED
        ? `completed "${label}"`
        : `open "${label}"`
      : `${state} assigned`;
    logger.log(`[assign] GraphQL search (${queryType}):`, searchQuery);

    const result = await github.graphql(
      `query ($searchQuery: String!) {
        search(type: ISSUE, query: $searchQuery) { issueCount }
      }`,
      { searchQuery }
    );
    const count = result?.search?.issueCount ?? 0;
    logger.log(`[assign] ${queryType} issues for ${username}: ${count}`);
    return count;
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    logger.log(`[assign] Failed to count ${state} issues for ${username}: ${message}`);
    return null;
  }
}

/**
 * Handler for the /assign command. Runs all checks, then assigns and updates labels.
 * @param {object} botContext - Bot context (github, owner, repo, number, issue, comment).
 */
async function handleAssign(botContext) {
  const requesterUsername = botContext.comment.user.login;
  const commentId = botContext.comment.id;

  try {
    await botContext.github.rest.reactions.createForIssueComment({
      owner: botContext.owner,
      repo: botContext.repo,
      comment_id: commentId,
      content: '+1',
    });
    logger.log('Added thumbs-up reaction to comment');
  } catch (error) {
    logger.log('Could not add reaction:', error.message);
  }

  if (botContext.issue.assignees?.length > 0) {
    logger.log('Exit: issue already assigned to', botContext.issue.assignees.map((a) => a.login));
    await postComment(botContext, buildAlreadyAssignedComment(requesterUsername, botContext.issue, botContext.owner, botContext.repo));
    logger.log('Posted already-assigned comment');
    return;
  }

  if (!hasLabel(botContext.issue, LABELS.READY_FOR_DEV)) {
    logger.log('Exit: issue missing ready for dev label');
    await postComment(botContext, buildNotReadyComment(requesterUsername, botContext.owner, botContext.repo));
    logger.log('Posted not-ready comment');
    return;
  }

  const skillLevel = getIssueSkillLevel(botContext.issue);
  if (!skillLevel) {
    logger.log('Exit: issue has no skill level label');
    await postComment(botContext, buildNoSkillLevelComment(requesterUsername));
    logger.log('Posted no-skill-level comment');
    return;
  }
  logger.log('Issue skill level:', skillLevel);

  const openAssignmentCount = await countAssignedIssues(
    botContext.github,
    botContext.owner,
    botContext.repo,
    requesterUsername,
    ISSUE_STATE.OPEN
  );

  if (openAssignmentCount === null) {
    logger.log('Exit: could not verify open assignments due to API error');
    await postComment(botContext, buildApiErrorComment(requesterUsername));
    logger.log('Posted API error comment, tagged maintainers');
    return;
  }

  if (openAssignmentCount >= MAX_OPEN_ASSIGNMENTS) {
    logger.log('Exit: contributor has too many open assignments', {
      maxAllowed: MAX_OPEN_ASSIGNMENTS,
      currentCount: openAssignmentCount,
    });
    let blockedCount = 0;
    const blockedIssueCount = await countAssignedIssues(
      botContext.github,
      botContext.owner,
      botContext.repo,
      requesterUsername,
      ISSUE_STATE.OPEN,
      LABELS.BLOCKED
    );
    if (blockedIssueCount !== null && isNonNegativeInteger(blockedIssueCount)) {
      blockedCount = Math.floor(blockedIssueCount);
    }
    await postComment(botContext, buildAssignmentLimitExceededComment(
      requesterUsername,
      openAssignmentCount,
      botContext.owner,
      botContext.repo,
      blockedCount
    ));
    logger.log('Posted assignment-limit-exceeded comment');
    return;
  }

  logger.log('Open assignment count OK:', { maxAllowed: MAX_OPEN_ASSIGNMENTS, currentCount: openAssignmentCount });

  const prereq = SKILL_PREREQUISITES[skillLevel];
  if (prereq.requiredLabel && prereq.requiredCount > 0) {
    const completedCount = await countAssignedIssues(
      botContext.github,
      botContext.owner,
      botContext.repo,
      requesterUsername,
      ISSUE_STATE.CLOSED,
      prereq.requiredLabel
    );
    if (completedCount === null) {
      logger.log('Exit: could not verify prerequisites due to API error');
      await postComment(botContext, buildApiErrorComment(requesterUsername));
      logger.log('Posted API error comment, tagged maintainers');
      return;
    }
    if (completedCount < prereq.requiredCount) {
      logger.log('Exit: prerequisites not met', { required: prereq.requiredCount, completed: completedCount });
      await postComment(botContext,
        buildPrerequisiteNotMetComment(requesterUsername, skillLevel, completedCount, botContext.owner, botContext.repo));
      logger.log('Posted prerequisite-not-met comment');
      return;
    }
    logger.log('Prerequisites met:', { required: prereq.requiredCount, completed: completedCount });
  }

  logger.log('Assigning issue to', requesterUsername);
  const assignResult = await addAssignees(botContext, [requesterUsername]);
  if (!assignResult.success) {
    await postComment(botContext, buildAssignmentFailureComment(requesterUsername, assignResult.error));
    logger.log('Posted assignment failure comment, tagged maintainers');
    return;
  }

  let labelUpdateFailed = false;
  let labelUpdateError = '';
  const removeResult = await removeLabel(botContext, LABELS.READY_FOR_DEV);
  if (!removeResult.success) {
    labelUpdateFailed = true;
    labelUpdateError = `Failed to remove "${LABELS.READY_FOR_DEV}" label: ${removeResult.error}`;
  }
  const addResult = await addLabels(botContext, [LABELS.IN_PROGRESS]);
  if (!addResult.success) {
    labelUpdateFailed = true;
    labelUpdateError += (labelUpdateError ? '; ' : '') + `Failed to add "${LABELS.IN_PROGRESS}" label: ${addResult.error}`;
  }

  await postComment(botContext, buildWelcomeComment(requesterUsername, skillLevel));
  logger.log('Posted welcome comment');

  if (labelUpdateFailed) {
    await postComment(botContext, buildLabelUpdateFailureComment(requesterUsername, labelUpdateError));
    logger.log('Posted label update failure comment, tagged maintainers');
  }

  logger.log('Assignment flow completed successfully');
}

module.exports = { handleAssign };
