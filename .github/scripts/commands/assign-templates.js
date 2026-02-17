// SPDX-License-Identifier: Apache-2.0
//
// commands/assign-templates.js
//
// Comment templates for the /assign command. Pure formatting functions
// separated from assignment logic for readability.

const { MAINTAINER_TEAM, LABELS, ISSUE_STATE } = require('../helpers');

const MAX_OPEN_ASSIGNMENTS = 2;

const SKILL_PREREQUISITES = {
  [LABELS.GOOD_FIRST_ISSUE]: {
    requiredLabel: null,
    requiredCount: 0,
    displayName: 'Good First Issue',
  },
  [LABELS.BEGINNER]: {
    requiredLabel: LABELS.GOOD_FIRST_ISSUE,
    requiredCount: 2,
    displayName: 'Beginner',
    prerequisiteDisplayName: 'Good First Issues',
  },
  [LABELS.INTERMEDIATE]: {
    requiredLabel: LABELS.BEGINNER,
    requiredCount: 3,
    displayName: 'Intermediate',
    prerequisiteDisplayName: 'Beginner Issues',
  },
  [LABELS.ADVANCED]: {
    requiredLabel: LABELS.INTERMEDIATE,
    requiredCount: 3,
    displayName: 'Advanced',
    prerequisiteDisplayName: 'Intermediate Issues',
  },
};

function buildWelcomeComment(username, skillLevel) {
  const isGoodFirstIssue = skillLevel === LABELS.GOOD_FIRST_ISSUE;
  const skillDisplayName = SKILL_PREREQUISITES[skillLevel]?.displayName || 'issue';
  if (isGoodFirstIssue) {
    return [
      `👋 Hi @${username}, welcome to the Hiero C++ SDK community! Thank you for choosing to contribute — we're thrilled to have you here! 🎉`,
      '',
      'You\'ve been assigned this **Good First Issue**, and the **Good First Issue Support Team** (@hiero-ledger/hiero-sdk-good-first-issue-support) is ready to help you succeed.',
      '',
      'The issue description above has everything you need: implementation steps, contribution workflow, and links to guides. If anything is unclear, just ask — we\'re happy to help.',
      '',
      'Good luck, and welcome aboard! 🚀',
    ].join('\n');
  }
  return [
    `👋 Hi @${username}, thanks for continuing to contribute to the Hiero C++ SDK! You've been assigned this **${skillDisplayName}** issue. 🙌`,
    '',
    'If this task involves any design decisions or you\'d like early feedback, feel free to share your plan here before diving into the code.',
    '',
    'Good luck! 🚀',
  ].join('\n');
}

function buildAlreadyAssignedComment(requesterUsername, issue, owner, repo) {
  const isAssignedToSelf = issue?.assignees?.some((a) => a.login === requesterUsername);
  if (isAssignedToSelf) {
    return [
      `👋 Hi @${requesterUsername}! You're already assigned to this issue. You're all set to start working on it!`,
      '',
      'If you have any questions, feel free to ask here or reach out to the team.',
    ].join('\n');
  }
  const currentAssignee = issue?.assignees?.[0]?.login ?? 'someone';
  return [
    `👋 Hi @${requesterUsername}! This issue is already assigned to @${currentAssignee}.`,
    '',
    '👉 **Find another issue to work on:**',
    `[Browse unassigned issues](https://github.com/${owner}/${repo}/issues?q=is%3Aissue+is%3Aopen+no%3Aassignee+label%3A%22status%3A+ready+for+dev%22)`,
    '',
    'Once you find one you like, comment `/assign` to get started!',
  ].join('\n');
}

function buildNotReadyComment(requesterUsername, owner, repo) {
  return [
    `👋 Hi @${requesterUsername}! This issue is not ready for development yet.`,
    '',
    `Issues must have the \`${LABELS.READY_FOR_DEV}\` label before they can be assigned.`,
    '',
    '👉 **Find an issue that\'s ready:**',
    `[Browse ready issues](https://github.com/${owner}/${repo}/issues?q=is%3Aissue+is%3Aopen+no%3Aassignee+label%3A%22status%3A+ready+for+dev%22)`,
    '',
    'Once you find one you like, comment `/assign` to get started!',
  ].join('\n');
}

function buildPrerequisiteNotMetComment(requesterUsername, skillLevel, completedCount, owner, repo) {
  const prereq = SKILL_PREREQUISITES[skillLevel];
  const { requiredLabel, requiredCount, prerequisiteDisplayName, displayName } = prereq;
  const searchQuery = `is:issue is:open no:assignee label:"${requiredLabel}" label:"${LABELS.READY_FOR_DEV}"`;
  const searchUrl = `https://github.com/${owner}/${repo}/issues?q=${encodeURIComponent(searchQuery)}`;
  return [
    `👋 Hi @${requesterUsername}! Thanks for your interest in contributing!`,
    '',
    `This is a **${displayName}** issue. Before taking it on, you need to complete at least **${requiredCount} ${prerequisiteDisplayName}** to build familiarity with the codebase.`,
    '',
    `📊 **Your Progress:** You've completed **${completedCount}** so far.`,
    '',
    `👉 **Find ${prerequisiteDisplayName} to work on:**`,
    `[Browse available ${prerequisiteDisplayName}](${searchUrl})`,
    '',
    `Once you've completed ${requiredCount}, come back and we'll be happy to assign this to you! 🎯`,
  ].join('\n');
}

function buildNoSkillLevelComment(requesterUsername) {
  return [
    `👋 Hi @${requesterUsername}! This issue doesn't have a skill level label yet.`,
    '',
    `${MAINTAINER_TEAM} — could you please add one of the following labels?`,
    `- \`${LABELS.GOOD_FIRST_ISSUE}\``,
    `- \`${LABELS.BEGINNER}\``,
    `- \`${LABELS.INTERMEDIATE}\``,
    `- \`${LABELS.ADVANCED}\``,
    '',
    `@${requesterUsername}, once a maintainer adds the label, comment \`/assign\` again to request assignment.`,
  ].join('\n');
}

function buildIssuesSearchUrl(owner, repo, searchQuery) {
  return `https://github.com/${owner}/${repo}/issues?q=${encodeURIComponent(searchQuery)}`;
}

function formatAssignmentLimitExceededComment(
  requesterUsername,
  openCount,
  assignedIssuesUrl,
  blockedIssuesUrl
) {
  const lines = [
    `👋 Hi @${requesterUsername}! Thanks for your enthusiasm to contribute!`,
    '',
    `To help contributors stay focused and ensure issues remain available for others, we limit assignments to **${MAX_OPEN_ASSIGNMENTS} open issues** at a time. Issues labeled \`${LABELS.BLOCKED}\` are not counted toward this limit.`,
    '',
    `📊 **Your Current Assignments:** You're currently assigned to **${openCount}** open issues.`,
    '',
    '👉 **View your assigned issues:**',
    `[Your open assignments](${assignedIssuesUrl})`,
  ];
  if (blockedIssuesUrl) {
    lines.push('', '👉 **View your blocked issues:**', `[Your blocked issues](${blockedIssuesUrl})`);
  }
  lines.push('', 'Once you complete or unassign from one of your current issues, come back and we\'ll be happy to assign this to you! 🎯');
  return lines.join('\n');
}

function buildAssignmentLimitExceededComment(requesterUsername, openCount, owner, repo, blockedCount = 0) {
  const assignedQuery = `is:issue is:${ISSUE_STATE.OPEN} assignee:${requesterUsername} -label:"${LABELS.BLOCKED}"`;
  const assignedIssuesUrl = buildIssuesSearchUrl(owner, repo, assignedQuery);
  const blockedIssuesUrl =
    blockedCount > 0
      ? buildIssuesSearchUrl(
          owner,
          repo,
          `is:issue is:${ISSUE_STATE.OPEN} assignee:${requesterUsername} label:"${LABELS.BLOCKED}"`
        )
      : null;
  return formatAssignmentLimitExceededComment(
    requesterUsername,
    openCount,
    assignedIssuesUrl,
    blockedIssuesUrl
  );
}

function buildApiErrorComment(requesterUsername) {
  return [
    `👋 Hi @${requesterUsername}! I encountered an error while trying to verify your eligibility for this issue.`,
    '',
    `${MAINTAINER_TEAM} — could you please help with this assignment request?`,
    '',
    `@${requesterUsername}, a maintainer will review your request and assign you manually if appropriate. Sorry for the inconvenience!`,
  ].join('\n');
}

function buildLabelUpdateFailureComment(username, error) {
  return [
    `⚠️ @${username} has been successfully assigned to this issue, but I encountered an error updating the labels.`,
    '',
    `${MAINTAINER_TEAM} — please manually:`,
    `- Remove the \`${LABELS.READY_FOR_DEV}\` label`,
    `- Add the \`${LABELS.IN_PROGRESS}\` label`,
    '',
    `Error details: ${error}`,
  ].join('\n');
}

function buildAssignmentFailureComment(requesterUsername, error) {
  return [
    `⚠️ Hi @${requesterUsername}! I tried to assign you to this issue, but encountered an error.`,
    '',
    `${MAINTAINER_TEAM} — could you please manually assign @${requesterUsername} to this issue?`,
    '',
    `Error details: ${error}`,
  ].join('\n');
}

module.exports = {
  MAX_OPEN_ASSIGNMENTS,
  SKILL_PREREQUISITES,
  buildWelcomeComment,
  buildAlreadyAssignedComment,
  buildNotReadyComment,
  buildPrerequisiteNotMetComment,
  buildNoSkillLevelComment,
  buildIssuesSearchUrl,
  formatAssignmentLimitExceededComment,
  buildAssignmentLimitExceededComment,
  buildApiErrorComment,
  buildLabelUpdateFailureComment,
  buildAssignmentFailureComment,
};
