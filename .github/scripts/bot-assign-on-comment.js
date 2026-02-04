// SPDX-License-Identifier: Apache-2.0
//
// bot-assign-on-comment.js
//
// Assigns contributors to issues when they comment "/assign".
// Enforces skill-level prerequisites and manages status labels.
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

// Label constants
const LABELS = {
  READY_FOR_DEV: 'status: ready for dev',
  IN_PROGRESS: 'status: in progress',
  GOOD_FIRST_ISSUE: 'skill: good first issue',
  BEGINNER: 'skill: beginner',
  INTERMEDIATE: 'skill: intermediate',
  ADVANCED: 'skill: advanced',
};

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

// Team to tag when manual intervention is needed
const MAINTAINER_TEAM = '@hiero-ledger/hiero-sdk-cpp-maintainers';

/**
 * Validates a string for safe use in GitHub search queries.
 * @param {string} value - The value to validate.
 * @returns {boolean} - True if the value is safe for search queries.
 */
function isSafeSearchToken(value) {
  return typeof value === 'string' && /^[a-zA-Z0-9._/-]+$/.test(value);
}

/**
 * Returns true if the comment is exactly "/assign" (with optional whitespace).
 * @param {string} body - The comment body.
 * @returns {boolean} - True if the comment is the /assign command.
 */
function commentRequestsAssignment(body) {
  const matches = typeof body === 'string' && /^\s*\/assign\s*$/i.test(body);
  console.log('[assign-bot] commentRequestsAssignment:', { body: body?.substring(0, 100), matches });
  return matches;
}

/**
 * Checks if an issue has a specific label.
 * @param {object} issue - The issue object.
 * @param {string} labelName - The label name to check for.
 * @returns {boolean} - True if the issue has the label.
 */
function hasLabel(issue, labelName) {
  if (!issue?.labels?.length) {
    return false;
  }

  return issue.labels.some((label) => {
    const name = typeof label === 'string' ? label : label?.name;
    return typeof name === 'string' && name.toLowerCase() === labelName.toLowerCase();
  });
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
 * Counts completed issues for a user with a specific label.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {string} username - The username to check.
 * @param {string} label - The label to filter by.
 * @returns {Promise<number|null>} - The count of completed issues or null on error.
 */
async function countCompletedIssues(github, owner, repo, username, label) {
  if (
    !isSafeSearchToken(owner) ||
    !isSafeSearchToken(repo) ||
    !isSafeSearchToken(username)
  ) {
    console.log('[assign-bot] Invalid search inputs:', { owner, repo, username, label });
    return null;
  }

  try {
    const searchQuery = [
      `repo:${owner}/${repo}`,
      `label:"${label}"`,
      'is:issue',
      'is:closed',
      `assignee:${username}`,
    ].join(' ');

    console.log('[assign-bot] GraphQL search query:', searchQuery);

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
    console.log(`[assign-bot] Completed "${label}" issues for ${username}: ${count}`);
    return count;
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    console.log(`[assign-bot] Failed to count issues for ${username}: ${message}`);
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
 * 6. Verify contributor meets skill prerequisites (via GraphQL)
 * 7. Assign contributor and update labels
 * 8. Post welcome comment
 *
 * On any failure, posts an informative comment explaining why.
 * On API errors, tags maintainers for manual intervention.
 */
module.exports = async ({ github, context }) => {
  try {
    // Extract issue and comment data from the webhook payload
    const { issue, comment } = context.payload;
    const { owner, repo } = context.repo;

    console.log('[assign-bot] Payload snapshot:', {
      issueNumber: issue?.number,
      commenter: comment?.user?.login,
      commenterType: comment?.user?.type,
      commentBody: comment?.body?.substring(0, 100),
    });

    // =========================================================================
    // VALIDATION: Ensure payload has required data
    // =========================================================================

    if (!issue?.number) {
      console.log('[assign-bot] Exit: missing issue number');
      return;
    }

    if (!comment?.body) {
      console.log('[assign-bot] Exit: missing comment body');
      return;
    }

    if (!comment?.user?.login) {
      console.log('[assign-bot] Exit: missing comment user login');
      return;
    }

    // Ignore comments from bots to prevent infinite loops
    if (comment.user.type === 'Bot') {
      console.log('[assign-bot] Exit: comment authored by bot');
      return;
    }

    // Only proceed if the comment is exactly "/assign"
    if (!commentRequestsAssignment(comment.body)) {
      console.log('[assign-bot] Exit: comment does not request assignment');
      return;
    }

    console.log('[assign-bot] Assignment command detected');

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
      console.log('[assign-bot] Added thumbs-up reaction to comment');
    } catch (error) {
      // Non-critical - continue even if reaction fails
      console.log('[assign-bot] Could not add reaction:', error.message);
    }

    // =========================================================================
    // CHECK 1: Is the issue already assigned?
    // =========================================================================
    // If yes, inform the user. Handles self-assignment case with friendly message.

    if (issue.assignees?.length > 0) {
      console.log('[assign-bot] Exit: issue already assigned to', issue.assignees.map((a) => a.login));

      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body: buildAlreadyAssignedComment(requesterUsername, issue, owner, repo),
      });

      console.log('[assign-bot] Posted already-assigned comment');
      return;
    }

    // =========================================================================
    // CHECK 2: Does the issue have the "ready for dev" label?
    // =========================================================================
    // Issues must be explicitly marked ready before contributors can self-assign.

    if (!hasLabel(issue, LABELS.READY_FOR_DEV)) {
      console.log('[assign-bot] Exit: issue missing ready for dev label');

      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body: buildNotReadyComment(requesterUsername, owner, repo),
      });

      console.log('[assign-bot] Posted not-ready comment');
      return;
    }

    // =========================================================================
    // CHECK 3: Does the issue have a skill level label?
    // =========================================================================
    // Every assignable issue must have exactly one skill level label.

    const skillLevel = getIssueSkillLevel(issue);

    if (!skillLevel) {
      console.log('[assign-bot] Exit: issue has no skill level label');

      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body: buildNoSkillLevelComment(requesterUsername),
      });

      console.log('[assign-bot] Posted no-skill-level comment');
      return;
    }

    console.log('[assign-bot] Issue skill level:', skillLevel);

    // =========================================================================
    // CHECK 4: Does the contributor meet skill prerequisites?
    // =========================================================================
    // For Beginner/Intermediate/Advanced issues, verify the contributor has
    // completed the required number of issues at the previous level.
    // Uses GitHub's GraphQL API to count closed issues by assignee and label.

    const prereq = SKILL_PREREQUISITES[skillLevel];

    // Only check prerequisites if this skill level requires them
    if (prereq.requiredLabel && prereq.requiredCount > 0) {
      const completedCount = await countCompletedIssues(
        github,
        owner,
        repo,
        requesterUsername,
        prereq.requiredLabel
      );

      // API error - can't verify prerequisites, tag maintainers
      if (completedCount === null) {
        console.log('[assign-bot] Exit: could not verify prerequisites due to API error');

        await github.rest.issues.createComment({
          owner,
          repo,
          issue_number: issueNumber,
          body: buildApiErrorComment(requesterUsername),
        });

        console.log('[assign-bot] Posted API error comment, tagged maintainers');
        return;
      }

      // Prerequisites not met - show progress and link to prerequisite issues
      if (completedCount < prereq.requiredCount) {
        console.log('[assign-bot] Exit: prerequisites not met', {
          required: prereq.requiredCount,
          completed: completedCount,
        });

        await github.rest.issues.createComment({
          owner,
          repo,
          issue_number: issueNumber,
          body: buildPrerequisiteNotMetComment(requesterUsername, skillLevel, completedCount, owner, repo),
        });

        console.log('[assign-bot] Posted prerequisite-not-met comment');
        return;
      }

      console.log('[assign-bot] Prerequisites met:', {
        required: prereq.requiredCount,
        completed: completedCount,
      });
    }

    // =========================================================================
    // ALL CHECKS PASSED - Assign the contributor
    // =========================================================================

    console.log('[assign-bot] Assigning issue to', requesterUsername);

    try {
      await github.rest.issues.addAssignees({
        owner,
        repo,
        issue_number: issueNumber,
        assignees: [requesterUsername],
      });
      console.log('[assign-bot] Assignment successful');
    } catch (error) {
      // Assignment failed - tag maintainers to assign manually
      console.log('[assign-bot] Assignment failed:', error.message);

      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body: buildAssignmentFailureComment(requesterUsername, error.message),
      });

      console.log('[assign-bot] Posted assignment failure comment, tagged maintainers');
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
    try {
      await github.rest.issues.removeLabel({
        owner,
        repo,
        issue_number: issueNumber,
        name: LABELS.READY_FOR_DEV,
      });
      console.log('[assign-bot] Removed "ready for dev" label');
    } catch (error) {
      console.log('[assign-bot] Could not remove "ready for dev" label:', error.message);
      labelUpdateFailed = true;
      labelUpdateError = `Failed to remove "${LABELS.READY_FOR_DEV}" label: ${error.message}`;
    }

    // Add "in progress" label
    try {
      await github.rest.issues.addLabels({
        owner,
        repo,
        issue_number: issueNumber,
        labels: [LABELS.IN_PROGRESS],
      });
      console.log('[assign-bot] Added "in progress" label');
    } catch (error) {
      console.log('[assign-bot] Could not add "in progress" label:', error.message);
      labelUpdateFailed = true;
      labelUpdateError += (labelUpdateError ? '; ' : '') + `Failed to add "${LABELS.IN_PROGRESS}" label: ${error.message}`;
    }

    // =========================================================================
    // POST WELCOME COMMENT
    // =========================================================================
    // Welcome the contributor with a message tailored to the skill level.
    // GFI contributors get extra warm welcome; returning contributors get
    // a shorter acknowledgment.

    await github.rest.issues.createComment({
      owner,
      repo,
      issue_number: issueNumber,
      body: buildWelcomeComment(requesterUsername, skillLevel),
    });
    console.log('[assign-bot] Posted welcome comment');

    // =========================================================================
    // NOTIFY MAINTAINERS IF LABEL UPDATE FAILED
    // =========================================================================
    // Post a separate comment so maintainers know to fix labels manually.
    // This comes after the welcome comment so the contributor sees their
    // welcome first.

    if (labelUpdateFailed) {
      await github.rest.issues.createComment({
        owner,
        repo,
        issue_number: issueNumber,
        body: buildLabelUpdateFailureComment(requesterUsername, labelUpdateError),
      });
      console.log('[assign-bot] Posted label update failure comment, tagged maintainers');
    }

    console.log('[assign-bot] Assignment flow completed successfully');

  } catch (error) {
    // Unexpected error - log details and re-throw to fail the workflow
    console.error('[assign-bot] Error:', {
      message: error.message,
      status: error.status,
      issueNumber: context.payload.issue?.number,
      commenter: context.payload.comment?.user?.login,
    });
    throw error;
  }
};
