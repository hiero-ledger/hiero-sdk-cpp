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
  createDelegatingLogger,
  isNonNegativeInteger,
  isSafeSearchToken,
  hasLabel,
  swapLabels,
  addAssignees,
  postComment,
  acknowledgeComment,
} = require("../helpers");

const {
  MAX_OPEN_ASSIGNMENTS,
  MAX_GFI_COMPLETIONS,
  SKILL_HIERARCHY,
  SKILL_PREREQUISITES,
  buildWelcomeComment,
  buildAlreadyAssignedComment,
  buildNotReadyComment,
  buildPrerequisiteNotMetComment,
  buildNoSkillLevelComment,
  buildSkillLevelChangedComment,
  buildAssignmentLimitExceededComment,
  buildGfiLimitExceededComment,
  buildApiErrorComment,
  buildLabelUpdateFailureComment,
  buildAssignmentFailureComment,
} = require("./assign-comments");

// Delegate to the active logger set by the dispatcher (bot-on-comment.js).
// This ensures the correct prefix is used after command parsing.
const logger = createDelegatingLogger();

/**
 * Formats a count for user-facing text when a threshold short-circuit may have
 * capped the value. If count reaches the short-circuit threshold, return an
 * "at least" style display (e.g. "3+") rather than implying an exact value.
 *
 * @param {number} count - Count returned by countAssignedIssues.
 * @param {number} threshold - Threshold used for short-circuiting.
 * @returns {string} User-facing display string.
 */
function formatThresholdedCount(count, threshold) {
  return count >= threshold ? `${threshold}+` : String(count);
}

/**
 * Returns the skill-level label on an issue, checking in ascending order:
 * Good First Issue -> Beginner -> Intermediate -> Advanced.
 * Returns the first match, or null if the issue has no skill-level label.
 *
 * @param {{ labels: Array<string|{ name: string }> }} issue - The issue object from the GitHub payload.
 * @returns {string|null} The matching LABELS constant (e.g. LABELS.BEGINNER), or null.
 */
function getIssueSkillLevel(issue) {
  for (const level of SKILL_HIERARCHY) {
    if (hasLabel(issue, level)) return level;
  }
  return null;
}

/**
 * Fetches the number of issues assigned to a specific user that match a given
 * state and optional label using the GitHub REST API.
 *
 * Note: When state is OPEN and no label filter is provided, issues with the
 * "status: blocked" label are explicitly EXCLUDED from the count.
 * The search is constrained to the repo specified in the context.
 *
 * @param {object} github - Octokit GitHub API client (must support github.rest).
 * @param {string} owner - Repository owner (e.g. 'hiero-ledger').
 * @param {string} repo - Repository name (e.g. 'hiero-sdk-cpp').
 * @param {string} username - GitHub username to search for.
 * @param {string} state - Issue state filter: ISSUE_STATE.OPEN or ISSUE_STATE.CLOSED.
 * @param {string|null} [label=null] - Optional label filter (e.g. 'skill: good first issue').
 * @param {number|null} [threshold=null] - Optional threshold to short-circuit pagination.
 *   When provided, the function returns a capped count (the threshold value)
 *   once that threshold is reached.
 * @returns {Promise<number|null>} Matching issue count, or null if inputs are invalid or the API call fails.
 *   When threshold is provided and reached, returns the threshold value (capped),
 *   not necessarily the exact total.
 */
async function countAssignedIssues(
  github,
  owner,
  repo,
  username,
  state,
  label = null,
  threshold = null,
) {
  if (
    !isSafeSearchToken(owner) ||
    !isSafeSearchToken(repo) ||
    !isSafeSearchToken(username)
  ) {
    logger.log("[assign] Invalid search inputs:", {
      owner,
      repo,
      username,
      label,
    });
    return null;
  }
  if (state !== ISSUE_STATE.OPEN && state !== ISSUE_STATE.CLOSED) {
    logger.log("[assign] Invalid state:", { state });
    return null;
  }
  if (
    label &&
    (typeof label !== "string" || !label.trim() || label.includes('"'))
  ) {
    logger.log("[assign] Invalid label parameter:", { label });
    return null;
  }

  try {
    let page = 1;
    let matchingIssuesCount = 0;
    const perPage = 100;

    logger.log(`[assign] Fetching ${state} assigned issues via REST...`);
    while (true) {
      const params = {
        owner,
        repo,
        state: state.toLowerCase(),
        assignee: username,
        per_page: perPage,
        page,
      };
      if (label) params.labels = label;

      const result = await github.rest.issues.listForRepo(params);
      // Filter out Pull Requests (which are returned by the issues endpoint)
      const actualIssues = result.data.filter((item) => !item.pull_request);

      let pageMatchCount = 0;
      if (state === ISSUE_STATE.OPEN && !label) {
        pageMatchCount = actualIssues.filter(
          (issue) =>
            !issue.labels?.some((l) => (l.name || l) === LABELS.BLOCKED),
        ).length;
      } else {
        pageMatchCount = actualIssues.length;
      }

      matchingIssuesCount += pageMatchCount;

      if (threshold !== null && matchingIssuesCount >= threshold) {
        logger.log(
          `[assign] Reached threshold (${threshold}), short-circuiting fetch.`,
        );
        matchingIssuesCount = threshold; // Cap at threshold logically for callers
        break;
      }

      // Pagination must evaluate the raw result size, not the filtered size.
      if (result.data.length < perPage) break;
      page++;
    }

    if (label) {
      logger.log(
        `[assign] ${state} assigned issues for ${username} with label ${label}: ${matchingIssuesCount}`,
      );
    } else {
      logger.log(
        `[assign] ${state} assigned issues for ${username}${state === ISSUE_STATE.OPEN ? " (excluding blocked)" : ""}: ${matchingIssuesCount}`,
      );
    }
    return matchingIssuesCount;
  } catch (error) {
    const message = error instanceof Error ? error.message : String(error);
    logger.log(
      `[assign] Failed to count ${state} issues for ${username}: ${message}`,
    );
    return null;
  }
}

/**
 * Returns the number of open issues assigned to the user that carry the
 * "status: blocked" label. Used to provide context in the assignment-limit-exceeded
 * comment (blocked issues don't count toward the limit).
 *
 * @param {object} github - Octokit GitHub API client.
 * @param {string} owner - Repository owner.
 * @param {string} repo - Repository name.
 * @param {string} username - GitHub username to search for.
 * @returns {Promise<number>} The blocked-issue count (defaults to 0 on any error).
 */
async function getBlockedCount(github, owner, repo, username) {
  const count = await countAssignedIssues(
    github,
    owner,
    repo,
    username,
    ISSUE_STATE.OPEN,
    LABELS.BLOCKED,
    1,
  );
  if (count !== null && isNonNegativeInteger(count)) return Math.floor(count);
  return 0;
}

/**
 * Checks skill-level prerequisites for the requesting user. Looks up the
 * required number of completed issues at the prerequisite level (e.g. a
 * Beginner issue requires 2 completed Good First Issues). Posts a comment
 * and returns false when the check fails or an API error occurs; returns
 * true when prerequisites are satisfied (or not required for the level).
 *
 * @param {object} botContext - Bot context from buildBotContext (github, owner, repo, number).
 * @param {string} skillLevel - The skill-level label on the issue (a LABELS constant).
 * @param {string} requesterUsername - The GitHub username requesting assignment.
 * @returns {Promise<boolean>} True if prerequisites are met or none required; false otherwise.
 */
async function checkPrerequisites(botContext, skillLevel, requesterUsername) {
  const prereq = SKILL_PREREQUISITES[skillLevel];
  if (!prereq.requiredLabel || prereq.requiredCount <= 0) return true;

  // Bypass: If the user already has worked on any level or higher then bypass them.
  const skillIndex = SKILL_HIERARCHY.indexOf(skillLevel);
  if (skillIndex !== -1) {
    for (let i = skillIndex; i < SKILL_HIERARCHY.length; i++) {
      const checkCurrLevel = SKILL_HIERARCHY[i];
      const countAtLevel = await countAssignedIssues(
        botContext.github,
        botContext.owner,
        botContext.repo,
        requesterUsername,
        ISSUE_STATE.CLOSED,
        checkCurrLevel,
        1,
      );

      if (countAtLevel !== null && countAtLevel > 0) {
        logger.log(
          `Bypassing prerequisites: user has completed ${countAtLevel} issues with label "${checkCurrLevel}"`,
        );
        return true;
      }
    }
  }

  // Normal validation
  const completedCount = await countAssignedIssues(
    botContext.github,
    botContext.owner,
    botContext.repo,
    requesterUsername,
    ISSUE_STATE.CLOSED,
    prereq.requiredLabel,
    prereq.requiredCount,
  );
  if (completedCount === null) {
    logger.log("Exit: could not verify prerequisites due to API error");
    await postComment(botContext, buildApiErrorComment(requesterUsername));
    logger.log("Posted API error comment, tagged maintainers");
    return false;
  }
  if (completedCount < prereq.requiredCount) {
    logger.log("Exit: prerequisites not met", {
      required: prereq.requiredCount,
      completed: completedCount,
    });
    await postComment(
      botContext,
      buildPrerequisiteNotMetComment(
        requesterUsername,
        skillLevel,
        completedCount,
        botContext.owner,
        botContext.repo,
      ),
    );
    logger.log("Posted prerequisite-not-met comment");
    return false;
  }
  logger.log("Prerequisites met:", {
    required: prereq.requiredCount,
    completed: completedCount,
  });
  return true;
}

/**
 * Transitions issue status labels after a successful assignment: removes
 * "status: ready for dev" and adds "status: in progress". If either label
 * operation fails, posts a comment tagging maintainers with manual instructions.
 *
 * @param {object} botContext - Bot context from buildBotContext (github, owner, repo, number).
 * @param {string} requesterUsername - The username being assigned (for the failure comment).
 * @returns {Promise<void>}
 */
async function updateLabels(botContext, requesterUsername) {
  const { success, errorDetails } = await swapLabels(
    botContext,
    LABELS.READY_FOR_DEV,
    LABELS.IN_PROGRESS,
  );
  if (!success) {
    await postComment(
      botContext,
      buildLabelUpdateFailureComment(requesterUsername, errorDetails),
    );
    logger.log("Posted label update failure comment, tagged maintainers");
  }
}

/**
 * Checks whether the requester has reached the GFI completion cap. Only applies
 * when skillLevel is LABELS.GOOD_FIRST_ISSUE. Posts an encouraging comment and
 * returns false when the cap is reached; returns true otherwise.
 *
 * @param {object} botContext - Bot context from buildBotContext.
 * @param {string} skillLevel - The skill-level label on the issue (a LABELS constant).
 * @param {string} requesterUsername - GitHub username requesting assignment.
 * @returns {Promise<boolean>} True if under the cap or not a GFI; false otherwise.
 */
async function enforceGfiCompletionLimit(
  botContext,
  skillLevel,
  requesterUsername,
) {
  if (skillLevel !== LABELS.GOOD_FIRST_ISSUE) return true;

  const completedCount = await countAssignedIssues(
    botContext.github,
    botContext.owner,
    botContext.repo,
    requesterUsername,
    ISSUE_STATE.CLOSED,
    LABELS.GOOD_FIRST_ISSUE,
    MAX_GFI_COMPLETIONS + 1,
  );
  if (completedCount === null) {
    logger.log("Exit: could not verify GFI completion count due to API error");
    await postComment(botContext, buildApiErrorComment(requesterUsername));
    logger.log("Posted API error comment, tagged maintainers");
    return false;
  }
  if (completedCount >= MAX_GFI_COMPLETIONS) {
    logger.log("Exit: contributor has reached GFI completion cap", {
      maxAllowed: MAX_GFI_COMPLETIONS,
      completedCount,
    });
    await postComment(
      botContext,
      buildGfiLimitExceededComment(
        requesterUsername,
        formatThresholdedCount(completedCount, MAX_GFI_COMPLETIONS + 1),
        botContext.owner,
        botContext.repo,
      ),
    );
    logger.log("Posted GFI-limit-exceeded comment");
    return false;
  }
  logger.log("GFI completion count OK:", {
    maxAllowed: MAX_GFI_COMPLETIONS,
    completedCount,
  });
  return true;
}

/**
 * Validates that the issue is in a state that allows assignment. Checks three
 * gates in order: no existing assignees, "status: ready for dev" label present,
 * and a skill-level label present. Posts an informative comment and returns null
 * when any gate fails.
 *
 * @param {object} botContext - Bot context from buildBotContext.
 * @param {string} requesterUsername - GitHub username requesting assignment.
 * @returns {Promise<string|null>} The skill-level label, or null if a gate failed.
 */
async function validateIssueState(botContext, requesterUsername) {
  if (botContext.issue.assignees?.length > 0) {
    logger.log(
      "Exit: issue already assigned to",
      botContext.issue.assignees.map((a) => a.login),
    );
    await postComment(
      botContext,
      buildAlreadyAssignedComment(
        requesterUsername,
        botContext.issue,
        botContext.owner,
        botContext.repo,
      ),
    );
    logger.log("Posted already-assigned comment");
    return null;
  }

  if (!hasLabel(botContext.issue, LABELS.READY_FOR_DEV)) {
    logger.log("Exit: issue missing ready for dev label");
    await postComment(
      botContext,
      buildNotReadyComment(
        requesterUsername,
        botContext.owner,
        botContext.repo,
      ),
    );
    logger.log("Posted not-ready comment");
    return null;
  }

  const skillLevel = getIssueSkillLevel(botContext.issue);
  if (!skillLevel) {
    logger.log("Exit: issue has no skill level label");
    await postComment(botContext, buildNoSkillLevelComment(requesterUsername));
    logger.log("Posted no-skill-level comment");
    return null;
  }
  logger.log("Issue skill level:", skillLevel);
  return skillLevel;
}

/**
 * Verifies the requester has not reached the open-assignment limit. Posts an
 * API-error or limit-exceeded comment and returns false when the check fails;
 * returns true when the user is within limits.
 *
 * @param {object} botContext - Bot context from buildBotContext.
 * @param {string} requesterUsername - GitHub username requesting assignment.
 * @returns {Promise<boolean>} True if within assignment limits; false otherwise.
 */
async function enforceAssignmentLimit(botContext, requesterUsername) {
  const openAssignmentCount = await countAssignedIssues(
    botContext.github,
    botContext.owner,
    botContext.repo,
    requesterUsername,
    ISSUE_STATE.OPEN,
    null,
    MAX_OPEN_ASSIGNMENTS + 1,
  );
  if (openAssignmentCount === null) {
    logger.log("Exit: could not verify open assignments due to API error");
    await postComment(botContext, buildApiErrorComment(requesterUsername));
    logger.log("Posted API error comment, tagged maintainers");
    return false;
  }

  if (openAssignmentCount >= MAX_OPEN_ASSIGNMENTS) {
    logger.log("Exit: contributor has too many open assignments", {
      maxAllowed: MAX_OPEN_ASSIGNMENTS,
      currentCount: openAssignmentCount,
    });
    const blockedCount = await getBlockedCount(
      botContext.github,
      botContext.owner,
      botContext.repo,
      requesterUsername,
    );
    await postComment(
      botContext,
      buildAssignmentLimitExceededComment(
        requesterUsername,
        formatThresholdedCount(openAssignmentCount, MAX_OPEN_ASSIGNMENTS + 1),
        botContext.owner,
        botContext.repo,
        blockedCount,
      ),
    );
    logger.log("Posted assignment-limit-exceeded comment");
    return false;
  }

  logger.log("Open assignment count OK:", {
    maxAllowed: MAX_OPEN_ASSIGNMENTS,
    currentCount: openAssignmentCount,
  });
  return true;
}

/**
 * Performs the actual issue assignment with a same-issue race defense:
 *
 * 1. Pre-flight check: fetches fresh issue state via issues.get() to detect
 *    same-issue collisions (another user assigned while this run was queued;
 *    the workflow concurrency key is serialized per issue).
 *
 * On success, posts a welcome comment and transitions status labels.
 * Posts failure comments if any API call fails.
 *
 * @param {object} botContext - Bot context from buildBotContext.
 * @param {string} requesterUsername - GitHub username being assigned.
 * @param {string} skillLevel - The skill-level label on the issue.
 * @returns {Promise<void>}
 */
async function assignAndFinalize(botContext, requesterUsername, skillLevel) {
  logger.log("Assigning issue to", requesterUsername);

  // Pre-flight same-issue collision check: fetch live issue state BEFORE writing.
  // The per-issue concurrency key serializes runs on the same issue, so by the
  // time this run executes, another user may already be assigned.
  let freshIssue;
  try {
    const response = await botContext.github.rest.issues.get({
      owner: botContext.owner,
      repo: botContext.repo,
      issue_number: botContext.number,
    });
    freshIssue = response.data;
  } catch (err) {
    const errorMsg = err instanceof Error ? err.message : String(err);
    logger.error("Failed to pre-fetch fresh issue state:", errorMsg);
    await postComment(botContext, buildApiErrorComment(requesterUsername));
    return;
  }

  const alreadyAssigned = freshIssue.assignees?.some(
    (a) => a.login === requesterUsername,
  );

  if (alreadyAssigned) {
    logger.log("Exit: user is already assigned (caught during fresh fetch)");
    await postComment(
      botContext,
      buildAlreadyAssignedComment(
        requesterUsername,
        freshIssue,
        botContext.owner,
        botContext.repo,
      ),
    );
    return;
  }

  if (freshIssue.assignees?.length > 0) {
    logger.log("Exit: issue was assigned by another user while queued");
    await postComment(
      botContext,
      buildAlreadyAssignedComment(
        requesterUsername,
        freshIssue,
        botContext.owner,
        botContext.repo,
      ),
    );
    return;
  }

  // Revalidate labels from the fresh issue snapshot to prevent stale-event
  // payload eligibility checks when labels change while this run is queued.
  if (!hasLabel(freshIssue, LABELS.READY_FOR_DEV)) {
    logger.log("Exit: fresh issue state is missing ready for dev label");
    await postComment(
      botContext,
      buildNotReadyComment(
        requesterUsername,
        botContext.owner,
        botContext.repo,
      ),
    );
    return;
  }

  const freshSkillLevel = getIssueSkillLevel(freshIssue);
  if (!freshSkillLevel) {
    logger.log("Exit: fresh issue state has no skill level label");
    await postComment(botContext, buildNoSkillLevelComment(requesterUsername));
    return;
  }

  // If the skill level changed while this run was queued, the earlier prerequisite
  // and GFI-cap gates were run against a stale label. Abort and ask user to retry.
  if (freshSkillLevel !== skillLevel) {
    logger.log("Exit: skill level changed while queued", {
      stalePayloadLevel: skillLevel,
      freshReadLevel: freshSkillLevel,
    });
    await postComment(
      botContext,
      buildSkillLevelChangedComment(
        requesterUsername,
        skillLevel,
        freshSkillLevel,
      ),
    );
    logger.log("Posted skill-level-changed comment");
    return;
  }

  const assignResult = await addAssignees(botContext, [requesterUsername]);
  if (!assignResult.success) {
    await postComment(
      botContext,
      buildAssignmentFailureComment(requesterUsername, assignResult.error),
    );
    logger.log("Posted assignment failure comment, tagged maintainers");
    return;
  }

  await postComment(
    botContext,
    buildWelcomeComment(requesterUsername, freshSkillLevel),
  );
  logger.log("Posted welcome comment");
  await updateLabels(botContext, requesterUsername);
  logger.log("Assignment flow completed successfully");
}

/**
 * Main handler for the /assign command. Runs the following gates in order, posting
 * an informative comment and returning early if any gate fails:
 *
 *   1. Acknowledge the comment with a thumbs-up reaction.
 *   2. Issue already assigned? -> already-assigned comment.
 *   3. Missing "status: ready for dev" label? -> not-ready comment.
 *   4. No skill-level label? -> no-skill-level comment (tags maintainers).
 *   5. Open-assignment count API error? -> API-error comment (tags maintainers).
 *   6. At or above MAX_OPEN_ASSIGNMENTS? -> limit-exceeded comment.
 *   7. GFI cap reached (skill: good first issue only)? -> GFI-limit-exceeded comment.
 *   8. Skill prerequisites not met? -> prerequisite-not-met comment.
 *   9. Issue snatched while queued (fresh fetch)? -> already-assigned comment.
 *   10. Fresh labels invalid while queued? -> not-ready/no-skill-level comment.
 *   11. Skill level changed while queued? -> skill-level-changed comment (ask to retry).
 *   12. Assignment API failure? -> assignment-failure comment (tags maintainers).
 *
 * If all checks pass, the bot assigns the issue, posts a welcome comment,
 * and swaps the "status: ready for dev" label with "status: in progress".
 *
 * @param {{ github: object, owner: string, repo: string, number: number,
 *           issue: object, comment: { id: number, user: { login: string } } }} botContext
 *   Bot context from buildBotContext (issue_comment event).
 * @returns {Promise<void>}
 */
async function handleAssign(botContext) {
  const requesterUsername = botContext.comment.user.login;

  const ack = await acknowledgeComment(botContext, botContext.comment.id);
  if (!ack.success) {
    logger.log(
      "Aborting /assign: triggering comment was deleted or could not be acknowledged",
    );
    return;
  }

  const skillLevel = await validateIssueState(botContext, requesterUsername);
  if (!skillLevel) return;

  const withinLimit = await enforceAssignmentLimit(
    botContext,
    requesterUsername,
  );
  if (!withinLimit) return;

  const withinGfiCap = await enforceGfiCompletionLimit(
    botContext,
    skillLevel,
    requesterUsername,
  );
  if (!withinGfiCap) return;

  const prereqsPassed = await checkPrerequisites(
    botContext,
    skillLevel,
    requesterUsername,
  );
  if (!prereqsPassed) return;

  await assignAndFinalize(botContext, requesterUsername, skillLevel);
}

module.exports = { handleAssign };
