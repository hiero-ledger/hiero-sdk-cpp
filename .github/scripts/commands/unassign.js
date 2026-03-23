// SPDX-License-Identifier: Apache-2.0
//
// commands/unassign.js
//
// /unassign command: allows a currently assigned contributor to unassign themselves.
// Enforces authorization (only assignees can unassign themselves) and reverts
// status labels back to the community pool.

const {
  LABELS,
  ISSUE_STATE,
  getLogger,
  addLabels,
  removeLabel,
  removeAssignees,
  postComment,
} = require('../helpers');

const {
  buildSuccessfulUnassignComment,
  buildNotAssignedToUserComment,
  buildNoAssigneeComment,
  buildIssueClosedComment,
  buildUnassignFailureComment,
} = require('./unassign-comments');

// Delegate to the active logger set by the dispatcher.
const logger = {
  log: (...args) => getLogger().log(...args),
  error: (...args) => getLogger().error(...args),
};

/**
 * Main handler for the /unassign command. Runs the following gates in order:
 *
 * 1. Is the issue already closed? -> issue-closed comment.
 * 2. Does the issue have no assignees? -> no-assignee comment.
 * 3. Is the commenter not the current assignee? -> unauthorized comment.
 *
 * On success: removes the user as an assignee, reverts the "in progress"
 * label to "ready for dev", and posts an acknowledgment comment.
 *
 * @param {{ github: object, owner: string, repo: string, number: number,
 * issue: object, comment: { user: { login: string } } }} botContext
 * @returns {Promise<void>}
 */
async function handleUnassign(botContext) {
  const requesterUsername = botContext.comment.user.login;
  const issue = botContext.issue;

  // GATE 1: Issue is closed
  if (issue.state === ISSUE_STATE.CLOSED) {
    logger.log('Exit: issue is closed');
    await postComment(botContext, buildIssueClosedComment(requesterUsername));
    return;
  }

  const assignees = issue.assignees || [];

  // GATE 2: No one is assigned at all
  if (assignees.length === 0) {
    logger.log('Exit: issue has no assignees');
    await postComment(botContext, buildNoAssigneeComment(requesterUsername));
    return;
  }

  // GATE 3: Authorization check (case-insensitive)
  const isAssigned = assignees.some(
    (a) => (a?.login || '').toLowerCase() === requesterUsername.toLowerCase()
  );
  if (!isAssigned) {
    logger.log(`Exit: @${requesterUsername} is not assigned to this issue`);
    const currentAssignee = assignees[0]?.login; // Grab the actual assignee for the message
    await postComment(botContext, buildNotAssignedToUserComment(requesterUsername, currentAssignee));
    return;
  }

  // ACTION 1: Remove the assignee
  logger.log(`Unassigning @${requesterUsername}`);
  const removeResult = await removeAssignees(botContext, [requesterUsername]);
  if (!removeResult.success) {
    await postComment(botContext, buildUnassignFailureComment(requesterUsername));
    return;
  }

  // ACTION 2: Label Swapping (Mirroring assign.js style - no stale checks)
  logger.log(`Swapping labels: removing ${LABELS.IN_PROGRESS}, adding ${LABELS.READY_FOR_DEV}`);
  
  const removeLabelResult = await removeLabel(botContext, LABELS.IN_PROGRESS);
  if (!removeLabelResult.success) {
    logger.error(`Failed to remove ${LABELS.IN_PROGRESS}: ${removeLabelResult.error}`);
  }

  const addLabelResult = await addLabels(botContext, [LABELS.READY_FOR_DEV]);
  if (!addLabelResult.success) {
    logger.error(`Failed to add ${LABELS.READY_FOR_DEV}: ${addLabelResult.error}`);
  }

  // ACTION 3: Post success acknowledgment
  await postComment(botContext, buildSuccessfulUnassignComment(requesterUsername));
  logger.log(`Successfully unassigned @${requesterUsername} and reverted labels`);
}

module.exports = { handleUnassign };