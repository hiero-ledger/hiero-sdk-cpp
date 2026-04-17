// SPDX-License-Identifier: Apache-2.0
//
// commands/recommend-issues.js
//
// Issue recommendation command: suggests relevant issues to contributors
// after a PR is closed. Uses skill progression logic to recommend the next
// suitable issues based on difficulty level.

const {
    MAINTAINER_TEAM,
    LABELS,
    SKILL_HIERARCHY,
    SKILL_PREREQUISITES,
    hasLabel,
    postComment,
    getLogger,
    getHighestIssueSkillLevel,
    countIssuesByAssignee,
} = require('../helpers');

// Logger delegation 
const logger = {
    log: (...args) => getLogger().log(...args),
    error: (...args) => getLogger().error(...args),
};

/**
 * Groups issues by their matching difficulty level.
 *
 * Each issue is assigned to the first matching level in levelsPriority.
 *
 * @param {Array<object>} issues
 * @param {string[]} levelsPriority
 * @returns {Object<string, Array<object>>} Issues grouped by level.
 */
function groupIssuesByLevel(issues, levelsPriority) {
    const grouped = Object.fromEntries(
        levelsPriority.map(level => [level, []])
    );

    for (const issue of issues) {
        const level = levelsPriority.find(l => hasLabel(issue, l));
        if (level) grouped[level].push(issue);
    }

    return grouped;
}

/**
 * Returns issues from the highest-priority level with results.
 *
 * Limits output to 5 issues.
 *
 * @param {Object<string, Array<object>>} grouped
 * @param {string[]} levelsPriority
 * @returns {Array<object>} Selected issues or empty array.
 */
function pickFirstAvailableLevel(grouped, levelsPriority) {
    for (const level of levelsPriority) {
        if (grouped[level].length > 0) {
            return grouped[level].slice(0, 5);
        }
    }
    return [];
}

/**
 * Fetches issues for multiple levels in a single query.
 *
 * @param {object} github
 * @param {string} owner
 * @param {string} repo
 * @returns {Promise<Array<{ title: string, html_url: string, labels: Array }> | null>}
 */
async function fetchIssuesBatch(github, owner, repo) {
    try {

        const query = [
            `repo:${owner}/${repo}`,
            'is:issue',
            'is:open',
            'no:assignee',
            `label:"${LABELS.READY_FOR_DEV}"`
        ].join(' ');

        const result = await github.rest.search.issuesAndPullRequests({
            q: query,
            per_page: 50,
        });

        return result.data.items || [];
    } catch (error) {
        logger.error('Failed to fetch issues:', {
            message: error.message,
            status: error.status,
        });
        return null;
    }
}

/**
 * Builds the success comment listing recommended issues.
 *
 * When `unlockedLevel` is provided, a congratulatory block is prepended to
 * acknowledge that this contribution crossed the threshold into a new skill
 * level. The display name is sourced from SKILL_PREREQUISITES so it stays
 * consistent with the rest of the bot.
 *
 * @param {string} username
 * @param {Array<{ title: string, html_url: string }>} issues
 * @param {string|null} unlockedLevel - LABELS constant for the newly unlocked
 *   level, or null if no threshold was crossed.
 * @returns {string}
 */
function buildRecommendationComment(username, issues, unlockedLevel = null) {
    const list = issues.map(
        (issue) => `- [${issue.title}](${issue.html_url})`
    );

    const congratsBlock = unlockedLevel
        ? [
            `🏆 Milestone unlocked: you've just reached **${SKILL_PREREQUISITES[unlockedLevel].displayName}** level!`,
            `That's a big step — well done. 🎊`,
            '',
        ]
        : [];

    return [
        `👋 Hi @${username}! Great work on your recent contribution! 🎉`,
        '',
        ...congratsBlock,
        `Here are some issues you might want to explore next:`,
        '',
        ...list,
        '',
        `Happy coding! 🚀`,
    ].join('\n');
}

/**
 * Builds an error comment when recommendations fail.
 *
 * @param {string} username
 * @returns {string}
 */
function buildRecommendationErrorComment(username) {
    return [
        `👋 Hi @${username}!`,
        '',
        `I ran into an issue while generating recommendations for you.`,
        '',
        `${MAINTAINER_TEAM} — could you please take a look?`,
        '',
        `Sorry for the inconvenience — feel free to explore open issues in the meantime!`,
    ].join('\n');
}

/**
 * Determines the highest skill level a user is genuinely eligible for,
 * mirroring the two-step eligibility check used by the assignment bot.
 *
 * Walks the hierarchy top-down (Advanced → … → Good First Issue), stopping
 * at the first level where the contributor passes either check:
 *
 *   Bypass check  — contributor already has ≥ 1 closed issue at this level
 *                   or higher. Prior demonstrated competence skips the normal
 *                   prerequisite count entirely.
 *
 *   Normal check  — contributor has accumulated at least requiredCount closed
 *                   issues at the prerequisite level defined in
 *                   SKILL_PREREQUISITES.
 *
 * Levels with no requiredLabel (i.e. Good First Issue) are always eligible
 * and act as the guaranteed floor — the walk always resolves to at least GFI.
 *
 * The walk is entirely history-based and ignores the triggering issue's label,
 * so a contributor who completes a GFI after already having three closed
 * Intermediate issues is correctly resolved to Intermediate, not GFI.
 *
 * API failures degrade gracefully: if countIssuesByAssignee returns null the
 * candidate is skipped and the walk continues downward.
 *
 * Example (contributor has 2 closed GFIs, 0 Beginner):
 *   ADVANCED     → bypass: 0 closed Advanced+ → fail
 *                  normal: needs 3 Intermediate closed → 0 → fail → skip
 *   INTERMEDIATE → bypass: 0 closed Intermediate+ → fail
 *                  normal: needs 3 Beginner closed → 0 → fail → skip
 *   BEGINNER     → bypass: 0 closed Beginner+ → fail
 *                  normal: needs 2 GFI closed → 2 → pass ✓ → return BEGINNER
 *
 * @param {object} botContext
 * @param {string} username
 * @returns {Promise<string>} The highest verified eligible level label.
 */
async function resolveEligibleLevel(botContext, username) {
    const { github, owner, repo } = botContext;

    // Walk from highest to lowest so we always return the best eligible level.
    const topDown = [...SKILL_HIERARCHY].reverse();

    for (const candidate of topDown) {
        const prereq = SKILL_PREREQUISITES[candidate];

        // Guard against misconfiguration — a label in the hierarchy with no
        // corresponding prerequisites entry.
        if (!prereq) {
            logger.log('resolveEligibleLevel: unknown candidate, skipping', { candidate });
            continue;
        }

        // Floor level — no prerequisites, always eligible.
        // Reached only if every higher level failed both checks.
        if (!prereq.requiredLabel) {
            logger.log('resolveEligibleLevel: floor level reached, eligible by default', {
                user: username, candidate,
            });
            return candidate;
        }

        // ── Bypass check ────────────────────────────────────────────────────
        // A single closed issue at the candidate level or above proves the
        // contributor can already handle this difficulty. Threshold: 1.
        const candidateIndex = SKILL_HIERARCHY.indexOf(candidate);
        const atOrAboveLabels = SKILL_HIERARCHY.slice(candidateIndex);

        for (const bypassLabel of atOrAboveLabels) {
            const bypassCount = await countIssuesByAssignee(
                github, owner, repo, username,
                'closed',
                bypassLabel,
                1,
            );

            if (bypassCount === null) {
                logger.log('resolveEligibleLevel: bypass countIssuesByAssignee failed, skipping label', {
                    candidate, bypassLabel, user: username,
                });
                continue;
            }

            if (bypassCount >= 1) {
                logger.log('resolveEligibleLevel: bypass check passed', {
                    user: username, candidate, bypassLabel, bypassCount,
                });
                return candidate;
            }
        }

        // ── Normal check ────────────────────────────────────────────────────
        // Contributor must have completed at least requiredCount closed issues
        // at the prerequisite level. Pass requiredCount as the threshold so
        // the API short-circuits as soon as the bar is cleared.
        const count = await countIssuesByAssignee(
            github, owner, repo, username,
            'closed',
            prereq.requiredLabel,
            prereq.requiredCount,
        );

        if (count === null) {
            logger.log('resolveEligibleLevel: normal countIssuesByAssignee failed, skipping candidate', {
                candidate, user: username,
            });
            continue;
        }

        if (count >= prereq.requiredCount) {
            logger.log('resolveEligibleLevel: normal check passed', {
                user: username, candidate, required: prereq.requiredCount, actual: count,
            });
            return candidate;
        }

        logger.log('resolveEligibleLevel: both checks failed, trying lower level', {
            candidate, required: prereq.requiredCount, actual: count, user: username,
        });
    }

    // Should be unreachable — the floor level always returns above — but
    // defend against an empty or misconfigured SKILL_HIERARCHY.
    logger.log('resolveEligibleLevel: hierarchy exhausted, defaulting to GFI', { user: username });
    return LABELS.GOOD_FIRST_ISSUE;
}

/**
 * Checks whether the just-completed issue pushed the contributor over the
 * threshold into the level directly above `currentLevel`.
 *
 * The trigger condition (from the issue spec): after this PR merged, the
 * contributor's closed-issue count at `currentLevel` equals exactly the
 * requiredCount that unlocks the next level. That exact count means this was
 * the contribution that crossed the line for the first time.
 *
 * Only inspects the single step immediately above `currentLevel` — higher
 * promotions would have been earned in prior sessions and should not be
 * re-announced.
 *
 * Returns null if:
 *   - currentLevel is already the top of the hierarchy
 *   - the level above has a different requiredLabel than currentLevel
 *     (shouldn't occur with the current config, but guards against gaps)
 *   - the count doesn't land exactly on the threshold
 *   - the API call fails
 *
 * @param {object} botContext
 * @param {string} username
 * @param {string} currentLevel - Skill label from the triggering PR/issue.
 * @returns {Promise<string|null>} The newly unlocked level label, or null.
 */
async function detectUnlockedLevel(botContext, username, currentLevel) {
    const { github, owner, repo } = botContext;
    const currentIndex = SKILL_HIERARCHY.indexOf(currentLevel);
    const immediateNextLevel = SKILL_HIERARCHY[currentIndex + 1] ?? null;

    if (!immediateNextLevel) return null;

    const nextPrereq = SKILL_PREREQUISITES[immediateNextLevel];

    // The next level's prerequisite must be the current level for this
    // completion to be the relevant crossing event.
    if (!nextPrereq?.requiredLabel || nextPrereq.requiredLabel !== currentLevel) return null;

    const count = await countIssuesByAssignee(
        github, owner, repo, username,
        'closed',
        currentLevel,
        nextPrereq.requiredCount,
    );

    if (count === null) {
        logger.log('detectUnlockedLevel: countIssuesByAssignee failed', {
            user: username, currentLevel,
        });
        return null;
    }

    if (count === nextPrereq.requiredCount) {
        logger.log('detectUnlockedLevel: threshold crossed', {
            user: username, unlockedLevel: immediateNextLevel, count,
        });
        return immediateNextLevel;
    }

    return null;
}

/**
 * Returns recommended issues for the contributor based on their true
 * eligibility level, determined by a history-based top-down walk.
 *
 * Issues are fetched in a single batch and filtered locally. Recommendations
 * target the contributor's resolved eligible level — the same level they would
 * be permitted to self-assign via the assignment bot — so every suggestion is
 * immediately actionable.
 *
 * Also runs a focused threshold check (detectUnlockedLevel) to determine
 * whether this specific completion crossed a new level boundary, so the caller
 * can include a congratulatory message when appropriate.
 *
 * @param {object} botContext
 * @param {string} username
 * @param {string} currentLevel - Skill label from the triggering PR/issue.
 * @returns {Promise<{ issues: Array<{ title: string, html_url: string }>, unlockedLevel: string|null } | null>}
 *   Returns null on API failure (error comment already posted).
 */
async function getRecommendedIssues(botContext, username, currentLevel) {
    // Run both async operations concurrently — they're independent.
    const [eligibleLevel, unlockedLevel] = await Promise.all([
        resolveEligibleLevel(botContext, username),
        detectUnlockedLevel(botContext, username, currentLevel),
    ]);

    logger.log('getRecommendedIssues: resolved', {
        user: username,
        currentLevel,
        eligibleLevel,
        unlockedLevel,
    });

    const issues = await fetchIssuesBatch(
        botContext.github,
        botContext.owner,
        botContext.repo,
    );

    if (issues === null) {
        await postComment(
            botContext,
            buildRecommendationErrorComment(username)
        );
        return null;
    }

    // Target the contributor's resolved eligible level directly. No next/fallback
    // priority list — every recommended issue should be one they can actually claim.
    const grouped = groupIssuesByLevel(issues, [eligibleLevel]);
    return {
        issues: pickFirstAvailableLevel(grouped, [eligibleLevel]),
        unlockedLevel,
    };
}

/**
 * Main handler for issue recommendations after a PR is merged.
 *
 * - Determines skill level
 * - Fetches recommended issues
 * - Posts a comment if results exist, with a congratulatory prefix when the
 *   just-completed issue crossed a new level threshold
 *
 * Skips silently if context is incomplete or no results found.
 * Returns early on API failure.
 *
 * @param {{
 *   github: object,
 *   owner: string,
 *   repo: string,
 *   issue: object,
 *   sender: { login: string }
 * }} botContext
 * @returns {Promise<void>}
 */
async function handleRecommendIssues(botContext) {
    const username = botContext.sender?.login;
    if (!username) {
        logger.log('Missing sender login, skipping recommendation');
        return;
    }

    if (!botContext.issue) {
        logger.log('Missing issue in context, skipping recommendation');
        return;
    }

    const skillLevel = getHighestIssueSkillLevel(botContext.issue);
    if (!skillLevel) {
        logger.log('No skill level found, skipping recommendation', {
            issueNumber: botContext.issue?.number,
        });
        return;
    }

    logger.log('recommendation.context', {
        user: username,
        level: skillLevel,
        issue: botContext.issue?.number,
    });

    const result = await getRecommendedIssues(
        botContext,
        username,
        skillLevel,
    );

    if (result === null) return;

    const { issues, unlockedLevel } = result;

    if (issues.length === 0) {
        logger.log('recommendation.empty', { user: username });
        return;
    }

    const comment = buildRecommendationComment(username, issues, unlockedLevel);
    logger.log('recommendation.postComment', {
        target: botContext.number,
        issueSource: botContext.issue?.number,
        recommendations: issues.length,
        unlockedLevel,
    });
    const postResult = await postComment(botContext, comment);

    if (!postResult.success) {
        logger.error('recommendation.postCommentFailed', {
            error: postResult.error,
        });
        return;
    }

    logger.log('recommendation.posted');
}

module.exports = { 
    handleRecommendIssues,
    getRecommendedIssues,
    resolveEligibleLevel,
    detectUnlockedLevel,
};
