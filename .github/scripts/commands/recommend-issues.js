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
 * Returns the next difficulty level in the hierarchy.
 *
 * @param {string} currentLevel
 * @returns {string|null} Next level, same level if max, or null if invalid.
 */
function getNextLevel(currentLevel) {
    const index = SKILL_HIERARCHY.indexOf(currentLevel);
    if (index === -1) return null;

    return SKILL_HIERARCHY[index + 1] || currentLevel;
}

/**
 * Returns the previous (fallback) difficulty level.
 *
 * @param {string} currentLevel
 * @returns {string|null} Lower level or null if none.
 */
function getFallbackLevel(currentLevel) {
    const index = SKILL_HIERARCHY.indexOf(currentLevel);
    if (index <= 0) return null;

    return SKILL_HIERARCHY[index - 1];
}

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
 * @param {string} username
 * @param {Array<{ title: string, html_url: string }>} issues
 * @returns {string}
 */
function buildRecommendationComment(username, issues) {
    const list = issues.map(
        (issue) => `- [${issue.title}](${issue.html_url})`
    );
    return [
        `👋 Hi @${username}! Great work on your recent contribution! 🎉`,
        '',
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
 * by walking the prerequisite chain upward from their current level and
 * verifying each step against their actual closed-issue history.
 *
 * Starts at currentLevel (trusted — the user just closed an issue at that
 * level) and attempts to advance upward. Stops at the first level whose
 * prerequisites are unmet or when a GitHub API call fails, returning the
 * highest level fully verified so far.
 *
 * Example progression check for a user at BEGINNER:
 *   BEGINNER   → no requiredLabel      → eligible by default, continue
 *   INTERMEDIATE → needs 3 Beginner   → count closed Beginner issues
 *     ≥ 3 → verifiedLevel = INTERMEDIATE, continue
 *     < 3 → stop, return BEGINNER
 *
 * @param {object} botContext
 * @param {string} username
 * @param {string} currentLevel - Skill label from the triggering PR/issue.
 * @returns {Promise<string>}   - Highest verified eligible level label.
 */
async function resolveEligibleLevel(botContext, username, currentLevel) {
    const { github, owner, repo } = botContext;
    let verifiedLevel = currentLevel;

    // Slice the hierarchy at currentLevel so we only walk upward from
    // where the user already is, skipping levels they've clearly passed.
    const startIndex = SKILL_HIERARCHY.indexOf(currentLevel);
    for (const candidate of SKILL_HIERARCHY.slice(startIndex)) {
        const prereq = SKILL_PREREQUISITES[candidate];

        // Guard against labels that exist in SKILL_HIERARCHY but have no
        // corresponding entry in SKILL_PREREQUISITES (misconfiguration).
        if (!prereq) {
            logger.log('resolveEligibleLevel: unknown candidate, stopping walk', { candidate });
            break;
        }

        // Some levels (e.g. Good First Issue) have no prerequisite —
        // the user is eligible just by reaching them.
        if (!prereq.requiredLabel) {
            verifiedLevel = candidate;
            continue;
        }

        // Count closed issues the user completed at the required prerequisite
        // level. Pass requiredCount as the threshold so the API call
        // short-circuits as soon as the bar is cleared — no over-fetching.
        const count = await countIssuesByAssignee(
            github, owner, repo, username,
            'closed',
            prereq.requiredLabel,
            prereq.requiredCount,
        );

        if (count === null) {
            // API failure — degrade gracefully by keeping whatever level
            // has been verified so far rather than crashing the whole walk.
            logger.log('resolveEligibleLevel: countIssuesByAssignee failed, stopping walk', {
                candidate, user: username,
            });
            break;
        }

        if (count >= prereq.requiredCount) {
            // Prerequisites met — promote and try the next level.
            verifiedLevel = candidate;
        } else {
            // Prerequisites not met — the chain is strict, so no higher
            // level can be eligible either. Stop here.
            logger.log('resolveEligibleLevel: prerequisites not met, stopping walk', {
                candidate, required: prereq.requiredCount, actual: count, user: username,
            });
            break;
        }
    }

    if (verifiedLevel !== currentLevel) {
        logger.log('resolveEligibleLevel: resolved to higher level', {
            user: username, from: currentLevel, to: verifiedLevel,
        });
    }

    return verifiedLevel;
}

/**
 * Returns recommended issues based on priority: next → same → fallback.
 *
 * Uses a single batch API call for issues and filters locally.
 * The level anchor is now the user's *verified* eligible level (resolved via
 * closed-issue history) rather than the raw label on the triggering issue.
 *
 * @param {object} botContext
 * @param {string} username
 * @param {string} skillLevel  - Level label from the closed PR/issue.
 * @returns {Promise<Array<{ title: string, html_url: string }>|null>}
 */
async function getRecommendedIssues(botContext, username, skillLevel) {
    // Resolve the user's actual eligible level via prerequisite history.
    const eligibleLevel = await resolveEligibleLevel(botContext, username, skillLevel);

    logger.log('getRecommendedIssues: using eligible level', {
        user: username,
        labelLevel: skillLevel,
        eligibleLevel,
    });

    const fallback = getFallbackLevel(eligibleLevel);
    const nextLevel = getNextLevel(eligibleLevel);

    // Priority: next (if different) → current eligible → one step down (unless already beginner).
    const levelsPriority = [
        nextLevel !== eligibleLevel ? nextLevel : null,
        eligibleLevel,
        eligibleLevel !== LABELS.BEGINNER ? fallback : null,
    ].filter(Boolean);

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

    const grouped = groupIssuesByLevel(issues, levelsPriority);
    return pickFirstAvailableLevel(grouped, levelsPriority);
}

/**
 * Main handler for issue recommendations after a PR is merged.
 *
 * - Determines skill level
 * - Fetches recommended issues
 * - Posts a comment if results exist
 *
 * Skips silently if context is incomplete or no results found.
 * Returns early on API failure .
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

    const issues = await getRecommendedIssues(
        botContext,
        username,
        skillLevel,
    );

    if (issues === null) return;

    if (issues.length === 0) {
        logger.log('recommendation.empty', { user: username });
        return;
    }

    const comment = buildRecommendationComment(username, issues);
    logger.log('recommendation.postComment', {
        target: botContext.number,
        issueSource: botContext.issue?.number,
        recommendations: issues.length,
    });
    const result = await postComment(botContext, comment);

    if (!result.success) {
        logger.error('recommendation.postCommentFailed', {
            error: result.error,
        });
        return;
    }

    logger.log('recommendation.posted');
}

module.exports = { 
    handleRecommendIssues,
    getNextLevel,
    getFallbackLevel,
    getRecommendedIssues,
    resolveEligibleLevel,
};
