// SPDX-License-Identifier: Apache-2.0
//
// bot-helpers.js
//
// Shared utilities for GitHub Actions bot scripts.
// Contains common constants, API wrappers, and helper functions.

/**
 * Team to tag when manual intervention is needed.
 */
const MAINTAINER_TEAM = '@hiero-ledger/hiero-sdk-cpp-maintainers';

/**
 * Common label constants used across bot scripts.
 */
const LABELS = {
  // Status labels
  READY_FOR_DEV: 'status: ready for dev',
  IN_PROGRESS: 'status: in progress',
  NEEDS_REVIEW: 'status: needs review',
  NEEDS_REVISION: 'status: needs revision',

  // Skill level labels
  GOOD_FIRST_ISSUE: 'skill: good first issue',
  BEGINNER: 'skill: beginner',
  INTERMEDIATE: 'skill: intermediate',
  ADVANCED: 'skill: advanced',
};

/**
 * Validates a string for safe use in GitHub search queries.
 * Prevents injection attacks by ensuring the value contains only safe characters.
 * @param {string} value - The value to validate.
 * @returns {boolean} - True if the value is safe for search queries.
 */
function isSafeSearchToken(value) {
  return typeof value === 'string' && /^[a-zA-Z0-9._/-]+$/.test(value);
}

/**
 * Creates a logger with a consistent prefix for bot scripts.
 * @param {string} botName - The name of the bot (e.g., 'assign-bot', 'pr-bot').
 * @returns {object} - Logger object with log and error methods.
 */
function createLogger(botName) {
  const prefix = `[${botName}]`;
  return {
    log: (...args) => console.log(prefix, ...args),
    error: (...args) => console.error(prefix, ...args),
  };
}

/**
 * Safely adds labels to an issue or PR.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} issueNumber - The issue or PR number.
 * @param {string[]} labels - Array of label names to add.
 * @param {object} [logger] - Optional logger object.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function addLabels(github, owner, repo, issueNumber, labels, logger) {
  const log = logger?.log || console.log;
  try {
    await github.rest.issues.addLabels({
      owner,
      repo,
      issue_number: issueNumber,
      labels,
    });
    log(`Added labels: ${labels.join(', ')}`);
    return { success: true };
  } catch (error) {
    log(`Could not add labels "${labels.join(', ')}": ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Safely removes a label from an issue or PR.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} issueNumber - The issue or PR number.
 * @param {string} labelName - The label name to remove.
 * @param {object} [logger] - Optional logger object.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function removeLabel(github, owner, repo, issueNumber, labelName, logger) {
  const log = logger?.log || console.log;
  try {
    await github.rest.issues.removeLabel({
      owner,
      repo,
      issue_number: issueNumber,
      name: labelName,
    });
    log(`Removed label: ${labelName}`);
    return { success: true };
  } catch (error) {
    log(`Could not remove label "${labelName}": ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Safely adds assignees to an issue or PR.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} issueNumber - The issue or PR number.
 * @param {string[]} assignees - Array of usernames to assign.
 * @param {object} [logger] - Optional logger object.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function addAssignees(github, owner, repo, issueNumber, assignees, logger) {
  const log = logger?.log || console.log;
  try {
    await github.rest.issues.addAssignees({
      owner,
      repo,
      issue_number: issueNumber,
      assignees,
    });
    log(`Added assignees: ${assignees.join(', ')}`);
    return { success: true };
  } catch (error) {
    log(`Could not add assignees "${assignees.join(', ')}": ${error.message}`);
    return { success: false, error: error.message };
  }
}

/**
 * Safely posts a comment on an issue or PR.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} issueNumber - The issue or PR number.
 * @param {string} body - The comment body.
 * @param {object} [logger] - Optional logger object.
 * @returns {Promise<{success: boolean, error?: string}>} - Result object.
 */
async function postComment(github, owner, repo, issueNumber, body, logger) {
  const log = logger?.log || console.log;
  try {
    await github.rest.issues.createComment({
      owner,
      repo,
      issue_number: issueNumber,
      body,
    });
    log('Posted comment');
    return { success: true };
  } catch (error) {
    log(`Could not post comment: ${error.message}`);
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

module.exports = {
  MAINTAINER_TEAM,
  LABELS,
  createLogger,
  isSafeSearchToken,
  addLabels,
  removeLabel,
  addAssignees,
  postComment,
  hasLabel,
};
