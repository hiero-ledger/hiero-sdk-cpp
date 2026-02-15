// SPDX-License-Identifier: Apache-2.0
//
// commands/assign.js
//
// /assign command: assigns the commenter to the issue. Enforces skill-level
// prerequisites, assignment limits, and status labels. See bot-on-comment.js
// for high-level docs (limits, skill levels, required labels).

const {
  MAINTAINER_TEAM,
  LABELS,
  ISSUE_STATE,
  isNonNegativeInteger,
  isSafeSearchToken,
  hasLabel,
  addLabels,
  removeLabel,
  addAssignees,
  postComment,
} = require('../helpers');

const logger = createLogger("on-assign");

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
    if (label) queryParts.push(`label:"${label}"`);
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
    await botContext.github.rest.issues.createComment({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
      body: buildApiErrorComment(requesterUsername),
    });
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
    await botContext.github.rest.issues.createComment({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
      body: buildAssignmentLimitExceededComment(
        requesterUsername,
        openAssignmentCount,
        botContext.owner,
        botContext.repo,
        blockedCount
      ),
    });
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
