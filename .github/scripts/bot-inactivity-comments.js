// SPDX-License-Identifier: Apache-2.0
//
// bot-inactivity-comments.js
//
// Comment builders for the inactivity bot. Pure formatting functions
// separated from the scheduled execution logic for readability.
//
// Duration values (WARN_AFTER_MS, CLOSE_AFTER_MS, BLOCKED_CHECKIN_AFTER_MS)
// remain in bot-inactivity.js and are passed in as parameters here so this
// file stays free of module-level constants that belong to the scheduler.

const { LABELS } = require('./helpers/constants');

// ─── HTML markers ────────────────────────────────────────────────────────────

const WARN_MARKER           = '<!-- bot:inactivity-warning -->';
const BLOCKED_CHECKIN_MARKER = '<!-- bot:blocked-checkin -->';

// ─── Comment builders ─────────────────────────────────────────────────────────

/**
 * Builds the inactivity warning comment body.
 * @param {string[]} assigneeLogins
 * @param {string} itemType - 'issue' or 'PR'
 * @param {number} warnAfterMs - Duration in ms after which a warning is issued.
 * @param {number} closeAfterMs - Duration in ms after which the item is closed.
 * @returns {string}
 */
function buildWarningComment(assigneeLogins, itemType, warnAfterMs, closeAfterMs) {
  const mentions = assigneeLogins.length > 0
    ? assigneeLogins.map(l => `@${l}`).join(', ')
    : 'there';

  const activityHint = itemType === 'PR'
    ? 'To stay active, leave a comment on this PR or the linked **issue**, or push a new commit.'
    : "If you're still on it, leave a comment to let us know!";

  const warnDays = warnAfterMs / (24 * 60 * 60 * 1000);
  const remainingDays = (closeAfterMs - warnAfterMs) / (24 * 60 * 60 * 1000);

  return [
    WARN_MARKER,
    `👋 Hey ${mentions}! This ${itemType} has been inactive for ${warnDays} days.`,
    '',
    `Are you still working on this? We will close this in ${remainingDays} days if we see no further activity.`,
    '',
    `${activityHint} If you'd like to step down, comment \`/unassign\`.`,
  ].join('\n');
}

/**
 * Builds the closure comment body.
 * @param {string} itemType - 'issue' or 'PR'
 * @param {number} closeAfterMs - Duration in ms after which the item is closed.
 * @returns {string}
 */
function buildClosureComment(itemType, closeAfterMs) {
  const closeDays = closeAfterMs / (24 * 60 * 60 * 1000);

  if (itemType === 'issue') {
    return [
      `⏱️ This issue has been unassigned and reset to \`${LABELS.READY_FOR_DEV}\` due to ${closeDays} days of inactivity.`,
      '',
      "If you'd like to continue working on this, feel free to comment `/assign` to be reassigned.",
    ].join('\n');
  }
  return [
    `⏱️ This PR has been closed due to ${closeDays} days of inactivity.`,
    '',
    `It has been unassigned and reset to \`${LABELS.READY_FOR_DEV}\` so another contributor can pick it up.`,
    '',
    "If you'd like to continue working on this, feel free to comment `/assign` to be reassigned.",
  ].join('\n');
}

/**
 * Builds the comment posted on an issue when its linked PR was closed for inactivity.
 * @returns {string}
 */
function buildLinkedPRClosedComment() {
  return [
    '🔗 The pull request linked to this issue was closed due to inactivity.',
    '',
    `This issue has been unassigned and reset to \`${LABELS.READY_FOR_DEV}\`.`,
    '',
    "If you'd like to continue working on this, feel free to comment `/assign` to be reassigned.",
  ].join('\n');
}

/**
 * Builds the 30-day blocked check-in comment body.
 * @param {string[]} assigneeLogins
 * @param {string} itemType - 'issue' or 'PR'
 * @returns {string}
 */
function buildBlockedCheckinComment(assigneeLogins, itemType) {
  const mentions = assigneeLogins.length > 0
    ? assigneeLogins.map(l => `@${l}`).join(', ')
    : 'there';

  return [
    BLOCKED_CHECKIN_MARKER,
    `👋 Hey ${mentions}, just checking in! Is this ${itemType} still blocked?`,
    '',
    `If it has been unblocked, please remove the \`${LABELS.BLOCKED}\` label so we can track progress.`,
  ].join('\n');
}

module.exports = {
  WARN_MARKER,
  BLOCKED_CHECKIN_MARKER,
  buildWarningComment,
  buildClosureComment,
  buildLinkedPRClosedComment,
  buildBlockedCheckinComment,
};
