// SPDX-License-Identifier: Apache-2.0
//
// bot-assign-on-comment.js
//
// Assigns contributors to issues when they comment "/assign".
// Enforces skill-level prerequisites, assignment limits, and manages status labels.
//
// Assignment Limit:
//   - Contributors can have at most 2 open issues assigned at a time
//
// Skill Level Requirements:
//   - Good First Issue: No prerequisites
//   - Beginner: 2 completed Good First Issues
//   - Intermediate: 3 completed Beginner Issues
//   - Advanced: 3 completed Intermediate Issues
//
// Required Labels:
//   - "status: ready for dev" (must be present for assignment)
//   - "status: in progress" (added after assignment)
//   - One of: "skill: good first issue", "skill: beginner",
//             "skill: intermediate", "skill: advanced"

const {
  MAINTAINER_TEAM,
  LABELS,
  createLogger,
  isSafeSearchToken,
  addLabels,
  removeLabel,
  addAssignees,
  postComment,
  hasLabel,
} = require('./bot-helpers');

// Prerequisites for each skill level
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

// Create logger for this bot
const logger = createLogger('assign-bot');

// Maximum number of open issues a contributor can be assigned to at once
const MAX_OPEN_ASSIGNMENTS = 2;

/**
 * Returns true if the comment is exactly "/assign" (with optional whitespace).
 * @param {string} body - The comment body.
 * @returns {boolean} - True if the comment is the /assign command.
 */
function commentRequestsAssignment(body) {
  const matches = typeof body === 'string' && /^\s*\/assign\s*$/i.test(body);
  logger.log('commentRequestsAssignment:', { body: body?.substring(0, 100), matches });
  return matches;
}

/**
 * Gets the skill level of an issue based on its labels.
 * @param {object} issue - The issue object.
 * @returns {string|null} - The skill level label or null if none found.
 */
function getIssueSkillLevel(issue) {
  const skillLevels = [LABELS.GOOD_FIRST_ISSUE, LABELS.BEGINNER, LABELS.INTERMEDIATE, LABELS.ADVANCED];

  for (const level of skillLevels) {
    if (hasLabel(issue, level)) {
      return level;
    }
  }

  return null;
}

/**
 * Counts issues assigned to a user matching specified criteria.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {string} username - The username to check.
 * @param {object} options - Query options.
 * @param {string} options.state - Issue state: 'open' or 'closed'.
 * @param {string} [options.label] - Optional label to filter by.
 * @returns {Promise<number|null>} - The count of matching issues or null on error.
 */
async function countAssignedIssues(github, owner, repo, username, { state, label = null }) {
  if (
    !isSafeSearchToken(owner) ||
    !isSafeSearchToken(repo) ||
    !isSafeSearchToken(username)
  ) {
    logger.log('[assign-bot] Invalid search inputs:', { owner, repo, username, label });
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
      queryParts.push(`label:"${label}"`);
    }

    const searchQuery = queryParts.join(' ');
    const queryType = label ? `completed "${label}"` : `${state} assigned`;

    logger.log(`[assign-bot] GraphQL search query (${queryType}):`, searchQuery);

    const result = await github.graphql(
      `
      query ($searchQuery: String!) {
        search(type: ISSUE, query: $searchQuery) {
          issueCount
        }
      }
      `,
      { searchQuery }
    );

    const count = result?.search?.issueCount ?? 0;
    logger.log(`[assign-bot] ${queryType} issues for ${username}: ${count}`);
    return count;
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    logger.log(`[assign-bot] Failed to count ${state} issues for ${username}: ${message}`);
    return null;
  }
}

/**
 * Builds a welcome comment for successful assignment.
 * @param {string} username - The assigned username.
 * @param {string} skillLevel - The skill level of the issue.
 * @returns {string} - The welcome comment.
 */
function buildWelcomeComment(username, skillLevel) {
  const isGoodFirstIssue = skillLevel === LABELS.GOOD_FIRST_ISSUE;
  const skillDisplayName = SKILL_PREREQUISITES[skillLevel]?.displayName || 'issue';

  // Welcome message - extra warm for first-time contributors
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

  // Message for returning contributors (Beginner, Intermediate, Advanced)
  return [
    `👋 Hi @${username}, thanks for continuing to contribute to the Hiero C++ SDK! You've been assigned this **${skillDisplayName}** issue. 🙌`,
    '',
    'If this task involves any design decisions or you\'d like early feedback, feel free to share your plan here before diving into the code.',
    '',
    'Good luck! 🚀',
  ].join('\n');
}

/**
 * Builds a comment for when the issue is already assigned.
 * @param {string} requesterUsername - The username requesting assignment.
 * @param {object} issue - The issue object.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @returns {string} - The already assigned comment.
 */
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

/**
 * Builds a comment for when the issue is not ready for development.
 * @param {string} requesterUsername - The username requesting assignment.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @returns {string} - The not ready comment.
 */
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

/**
 * Builds a comment for when skill prerequisites are not met.
 * @param {string} requesterUsername - The username requesting assignment.
 * @param {string} skillLevel - The skill level of the issue.
 * @param {number} completedCount - The number of completed prerequisite issues.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @returns {string} - The prerequisite not met comment.
 */
function buildPrerequisiteNotMetComment(requesterUsername, skillLevel, completedCount, owner, repo) {
  const prereq = SKILL_PREREQUISITES[skillLevel];
  const requiredLabel = prereq.requiredLabel;
  const requiredCount = prereq.requiredCount;
  const prerequisiteDisplayName = prereq.prerequisiteDisplayName;
  const displayName = prereq.displayName;

  // Build search URL for the prerequisite issues
  const searchLabel = encodeURIComponent(requiredLabel);
  const searchUrl = `https://github.com/${owner}/${repo}/issues?q=is%3Aissue+is%3Aopen+no%3Aassignee+label%3A%22${searchLabel}%22+label%3A%22status%3A+ready+for+dev%22`;

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

/**
 * Builds a comment for when the issue has no skill level label.
 * @param {string} requesterUsername - The username requesting assignment.
 * @returns {string} - The no skill level comment.
 */
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

/**
 * Builds a comment for when the contributor has too many open assignments.
 * @param {string} requesterUsername - The username requesting assignment.
 * @param {number} openCount - The number of open issues currently assigned.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @returns {string} - The assignment limit exceeded comment.
 */
function buildAssignmentLimitExceededComment(requesterUsername, openCount, owner, repo) {
  const assignedIssuesUrl = `https://github.com/${owner}/${repo}/issues?q=is%3Aissue+is%3Aopen+assignee%3A${requesterUsername}`;

  return [
    `👋 Hi @${requesterUsername}! Thanks for your enthusiasm to contribute!`,
    '',
    `To help contributors stay focused and ensure issues remain available for others, we limit assignments to **${MAX_OPEN_ASSIGNMENTS} open issues** at a time.`,
    '',
    `📊 **Your Current Assignments:** You're currently assigned to **${openCount}** open issue${openCount === 1 ? '' : 's'}.`,
    '',
    '👉 **View your assigned issues:**',
    `[Your open assignments](${assignedIssuesUrl})`,
    '',
    'Once you complete or unassign from one of your current issues, come back and we\'ll be happy to assign this to you! 🎯',
  ].join('\n');
}

/**
 * Builds a comment for when an API error prevents prerequisite verification.
 * @param {string} requesterUsername - The username requesting assignment.
 * @returns {string} - The API error comment.
 */
function buildApiErrorComment(requesterUsername) {
  return [
    `👋 Hi @${requesterUsername}! I encountered an error while trying to verify your eligibility for this issue.`,
    '',
    `${MAINTAINER_TEAM} — could you please help with this assignment request?`,
    '',
    `@${requesterUsername}, a maintainer will review your request and assign you manually if appropriate. Sorry for the inconvenience!`,
  ].join('\n');
}

/**
 * Builds a comment for when label updates fail after successful assignment.
 * @param {string} username - The assigned username.
 * @param {string} error - The error message.
 * @returns {string} - The label update failure comment.
 */
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

/**
 * Builds a comment for when the assignment API call fails.
 * @param {string} requesterUsername - The username requesting assignment.
 * @param {string} error - The error message.
 * @returns {string} - The assignment failure comment.
 */
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
 * Main handler for the /assign command.
 *
 * Flow:
 * 1. Validate payload (issue exists, comment exists, not a bot)
 * 2. Check if comment is exactly "/assign"
 * 3. Verify issue is not already assigned
 * 4. Verify issue has "status: ready for dev" label
 * 5. Verify issue has a skill level label
 * 6. Verify contributor doesn't have too many open assignments (via GraphQL)
 * 7. Verify contributor meets skill prerequisites (via GraphQL)
 * 8. Assign contributor and update labels
 * 9. Post welcome comment
 *
 * On any failure, posts an informative comment explaining why.
 * On API errors, tags maintainers for manual intervention.
 */
module.exports = async ({ github, context }) => {
  try {
    // Extract issue and comment data from the webhook payload
    const { issue, comment } = context.payload;
    const { owner, repo } = context.repo;

    logger.log('Payload snapshot:', {
      issueNumber: issue?.number,
      commenter: comment?.user?.login,
      commenterType: comment?.user?.type,
      commentBody: comment?.body?.substring(0, 100),
    });

    // =========================================================================
    // VALIDATION: Ensure payload has required data
    // =========================================================================

    if (!issue?.number) {
      logger.log('Exit: missing issue number');
      return;
    }

    if (!comment?.body) {
      logger.log('Exit: missing comment body');
      return;
    }

    if (!comment?.user?.login) {
      logger.log('Exit: missing comment user login');
      return;
    }

    // Ignore comments from bots to prevent infinite loops
    if (comment.user.type === 'Bot') {
      logger.log('Exit: comment authored by bot');
      return;
    }

    // Only proceed if the comment is exactly "/assign"
    if (!commentRequestsAssignment(comment.body)) {
      logger.log('Exit: comment does not request assignment');
      return;
    }

    logger.log('Assignment command detected');

    const requesterUsername = comment.user.login;
    const issueNumber = issue.number;
    const commentId = comment.id;

    // Acknowledge the /assign comment with a thumbs-up reaction
    try {
      await github.rest.reactions.createForIssueComment({
        owner,
        repo,
        comment_id: commentId,
        content: '+1',
      });
      logger.log('Added thumbs-up reaction to comment');
    } catch (error) {
      // Non-critical - continue even if reaction fails
      logger.log('Could not add reaction:', error.message);
    }

    // =========================================================================
    // CHECK 1: Is the issue already assigned?
    // =========================================================================
    // If yes, inform the user. Handles self-assignment case with friendly message.

    if (issue.assignees?.length > 0) {
      logger.log('Exit: issue already assigned to', issue.assignees.map((a) => a.login));

      await postComment(github, owner, repo, issueNumber,
        buildAlreadyAssignedComment(requesterUsername, issue, owner, repo), logger);

      logger.log('Posted already-assigned comment');
      return;
    }

    // =========================================================================
    // CHECK 2: Does the issue have the "ready for dev" label?
    // =========================================================================
    // Issues must be explicitly marked ready before contributors can self-assign.

    if (!hasLabel(issue, LABELS.READY_FOR_DEV)) {
      logger.log('Exit: issue missing ready for dev label');

      await postComment(github, owner, repo, issueNumber,
        buildNotReadyComment(requesterUsername, owner, repo), logger);

      logger.log('Posted not-ready comment');
      return;
    }

    // =========================================================================
    // CHECK 3: Does the issue have a skill level label?
    // =========================================================================
    // Every assignable issue must have exactly one skill level label.

    const skillLevel = getIssueSkillLevel(issue);

    if (!skillLevel) {
      logger.log('Exit: issue has no skill level label');

      await postComment(github, owner, repo, issueNumber,
        buildNoSkillLevelComment(requesterUsername), logger);

      logger.log('Posted no-skill-level comment');
      return;
    }

    logger.log('Issue skill level:', skillLevel);

    // =========================================================================
    // CHECK 4: Does the contributor have too many open assignments?
    // =========================================================================
    // Contributors are limited to MAX_OPEN_ASSIGNMENTS open issues at a time
    // to help them stay focused and ensure issues remain available for others.

    const openAssignmentCount = await countAssignedIssues(github, owner, repo, requesterUsername, { state: 'open' });

    // API error - can't verify open assignments, tag maintainers
    if (openAssignmentCount === null) {
      console.log('[assign-bot] Exit: could not verify open assignments due to API error');

      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body: buildApiErrorComment(requesterUsername),
      });

      console.log('[assign-bot] Posted API error comment, tagged maintainers');
      return;
    }

    // Too many open assignments - show current count and link to assigned issues
    if (openAssignmentCount >= MAX_OPEN_ASSIGNMENTS) {
      console.log('[assign-bot] Exit: contributor has too many open assignments', {
        maxAllowed: MAX_OPEN_ASSIGNMENTS,
        currentCount: openAssignmentCount,
      });

      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body: buildAssignmentLimitExceededComment(requesterUsername, openAssignmentCount, owner, repo),
      });

      console.log('[assign-bot] Posted assignment-limit-exceeded comment');
      return;
    }

    console.log('[assign-bot] Open assignment count OK:', {
      maxAllowed: MAX_OPEN_ASSIGNMENTS,
      currentCount: openAssignmentCount,
    });

    // =========================================================================
    // CHECK 5: Does the contributor meet skill prerequisites?
    // =========================================================================
    // For Beginner/Intermediate/Advanced issues, verify the contributor has
    // completed the required number of issues at the previous level.
    // Uses GitHub's GraphQL API to count closed issues by assignee and label.

    const prereq = SKILL_PREREQUISITES[skillLevel];

    // Only check prerequisites if this skill level requires them
    if (prereq.requiredLabel && prereq.requiredCount > 0) {
      const completedCount = await countAssignedIssues(
        github,
        owner,
        repo,
        requesterUsername,
        { state: 'closed', label: prereq.requiredLabel }
      );

      // API error - can't verify prerequisites, tag maintainers
      if (completedCount === null) {
        logger.log('Exit: could not verify prerequisites due to API error');

        await postComment(github, owner, repo, issueNumber,
          buildApiErrorComment(requesterUsername), logger);

        logger.log('Posted API error comment, tagged maintainers');
        return;
      }

      // Prerequisites not met - show progress and link to prerequisite issues
      if (completedCount < prereq.requiredCount) {
        logger.log('Exit: prerequisites not met', {
          required: prereq.requiredCount,
          completed: completedCount,
        });

        await postComment(github, owner, repo, issueNumber,
          buildPrerequisiteNotMetComment(requesterUsername, skillLevel, completedCount, owner, repo), logger);

        logger.log('Posted prerequisite-not-met comment');
        return;
      }

      logger.log('Prerequisites met:', {
        required: prereq.requiredCount,
        completed: completedCount,
      });
    }

    // =========================================================================
    // ALL CHECKS PASSED - Assign the contributor
    // =========================================================================

    logger.log('Assigning issue to', requesterUsername);

    const assignResult = await addAssignees(github, owner, repo, issueNumber, [requesterUsername], logger);

    if (!assignResult.success) {
      // Assignment failed - tag maintainers to assign manually
      await postComment(github, owner, repo, issueNumber,
        buildAssignmentFailureComment(requesterUsername, assignResult.error), logger);

      logger.log('Posted assignment failure comment, tagged maintainers');
      return;
    }

    // =========================================================================
    // UPDATE LABELS: "ready for dev" -> "in progress"
    // =========================================================================
    // Track any failures so we can notify maintainers while still welcoming
    // the contributor (assignment succeeded even if labels fail).

    let labelUpdateFailed = false;
    let labelUpdateError = '';

    // Remove "ready for dev" label
    const removeResult = await removeLabel(github, owner, repo, issueNumber, LABELS.READY_FOR_DEV, logger);
    if (!removeResult.success) {
      labelUpdateFailed = true;
      labelUpdateError = `Failed to remove "${LABELS.READY_FOR_DEV}" label: ${removeResult.error}`;
    }

    // Add "in progress" label
    const addResult = await addLabels(github, owner, repo, issueNumber, [LABELS.IN_PROGRESS], logger);
    if (!addResult.success) {
      labelUpdateFailed = true;
      labelUpdateError += (labelUpdateError ? '; ' : '') + `Failed to add "${LABELS.IN_PROGRESS}" label: ${addResult.error}`;
    }

    // =========================================================================
    // POST WELCOME COMMENT
    // =========================================================================
    // Welcome the contributor with a message tailored to the skill level.
    // GFI contributors get extra warm welcome; returning contributors get
    // a shorter acknowledgment.

    await postComment(github, owner, repo, issueNumber,
      buildWelcomeComment(requesterUsername, skillLevel), logger);
    logger.log('Posted welcome comment');

    // =========================================================================
    // NOTIFY MAINTAINERS IF LABEL UPDATE FAILED
    // =========================================================================
    // Post a separate comment so maintainers know to fix labels manually.
    // This comes after the welcome comment so the contributor sees their
    // welcome first.

    if (labelUpdateFailed) {
      await postComment(github, owner, repo, issueNumber,
        buildLabelUpdateFailureComment(requesterUsername, labelUpdateError), logger);
      logger.log('Posted label update failure comment, tagged maintainers');
    }

    logger.log('Assignment flow completed successfully');

  } catch (error) {
    // Unexpected error - log details and re-throw to fail the workflow
    logger.error('Error:', {
      message: error.message,
      status: error.status,
      issueNumber: context.payload.issue?.number,
      commenter: context.payload.comment?.user?.login,
    });
    throw error;
  }
};
