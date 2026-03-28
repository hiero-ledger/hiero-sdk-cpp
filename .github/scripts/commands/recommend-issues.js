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
    hasLabel,
    postComment,
    getLogger,
} = require('../helpers');

// Difficulty hierarchy used to determine progression for recommendations
const SKILL_HIERARCHY = [
    LABELS.GOOD_FIRST_ISSUE,
    LABELS.BEGINNER,
    LABELS.INTERMEDIATE,
    LABELS.ADVANCED,
];

// Logger delegation 
const logger = {
    log: (...args) => getLogger().log(...args),
    error: (...args) => getLogger().error(...args),
};

/**
 * Determines the difficulty level of an issue/PR based on its labels.
 *
 * Strategy:
 *   - Checks labels against SKILL_HIERARCHY in order
 *   - Returns the first matching level (lowest takes precedence)
 *
 * Behavior:
 *   - Returns matching LABELS constant if found
 *   - Returns null if no difficulty label is present
 *
 * @param {{ labels: Array<string|{ name: string }> }} issue - Issue or PR object.
 * @returns {string|null} Detected difficulty level or null if none found.
 */
function getIssueSkillLevel(issue) {
    for (const level of SKILL_HIERARCHY) {
        if (hasLabel(issue, level)) return level;
    }
    return null;
}

/**
 * Computes the next difficulty level for progression.
 *
 * Strategy:
 *   - Move one level higher in SKILL_HIERARCHY
 *   - If already at highest level, return the same level
 *
 * Behavior:
 *   - Returns next level if available
 *   - Returns same level if already at maximum
 *   - Returns null if input is invalid
 * 
 * @param {string} currentLevel
 * @returns {string|null}
 */
function getNextLevel(currentLevel) {
    const index = SKILL_HIERARCHY.indexOf(currentLevel);
    if (index === -1) return null;

    return SKILL_HIERARCHY[index + 1] || currentLevel;
}

/**
 * Computes the fallback difficulty level when no suitable issues are found.
 *
 * Strategy:
 *   - Move one level lower in SKILL_HIERARCHY
 *   - Used as a last resort when higher and same-level issues are unavailable
 *
 * Behavior:
 *   - Returns the previous level in the hierarchy
 *   - Returns null if already at the lowest level
 *   - Returns null if input level is invalid
 *
 * @param {string} currentLevel - Current difficulty level.
 * @returns {string|null} Fallback level or null if none exists.
 */
function getFallbackLevel(currentLevel) {
    const index = SKILL_HIERARCHY.indexOf(currentLevel);
    if (index <= 0) return null;

    return SKILL_HIERARCHY[index - 1];
}

/**
 * Fetches open, unassigned issues for a given difficulty level using the GitHub search API.
 *
 * Strategy:
 *   - Query issues within the target repository
 *   - Filter by:
 *       - Open state
 *       - No assignee
 *       - Matching difficulty label
 *       - "ready for dev" status label
 *   - Limit results to a small, relevant set
 *
 * Behavior:
 *   - Returns an array of issues (may be empty if none found)
 *   - Returns null if the API request fails
 *
 * @param {object} github - Octokit GitHub API client.
 * @param {string} owner - Repository owner.
 * @param {string} repo - Repository name.
 * @param {string} level - Difficulty level used for filtering.
 * @returns {Promise<Array<{ title: string, html_url: string }>|null>}
 *   List of issues, [] if none found, or null on failure.
 */
async function fetchIssuesByLevel(github, owner, repo, level) {
    try {
        const query = [
            `repo:${owner}/${repo}`,
            'is:issue',
            'is:open',
            'no:assignee',
            `label:"${level}"`,
            `label:"${LABELS.READY_FOR_DEV}"`
        ].join(' ');

        const result = await github.rest.search.issuesAndPullRequests({
        q: query,
        per_page: 5,
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
 * Safely fetches issues for a given difficulty level with centralized error handling.
 *
 * Behavior:
 *   - Calls GitHub search API via fetchIssuesByLevel
 *   - Returns [] if no issues are found (valid case)
 *   - Returns null if API fails (error case)
 *   - Posts a single error comment (once per execution) on failure
 *
 * Important:
 *   - []   → valid response, no issues found
 *   - null → API failure, caller should abort execution
 *
 * @param {object} botContext - Bot execution context (GitHub client, repo info, etc.).
 * @param {string} username - GitHub username to mention in error comment.
 * @param {string} level - Difficulty label used for filtering.
 * @param {{ hasErrored: boolean }} errorState - Tracks whether error comment has been posted.
 *
 * @returns {Promise<Array<{ title: string, html_url: string }>|null>} List of issues, or null if API failed.
 */
async function safeFetchIssues(botContext, username, level, errorState) {
    const issues = await fetchIssuesByLevel(
        botContext.github,
        botContext.owner,
        botContext.repo,
        level
    );

    if (issues === null) {
        if (!errorState.hasErrored) {
            await postComment(
                botContext,
                buildRecommendationErrorComment(username)
            );
            errorState.hasErrored = true;
        }
        return null;
    }

    return issues;
}

/**
 * Attempts to fetch issues for a given difficulty level.
 *
 * Behavior:
 *   - Skips if level is null/undefined
 *   - Returns [] if no issues found (valid case)
 *   - Returns null if API fails (error case)
 *
 * @param {object} botContext - Bot execution context.
 * @param {string} username - GitHub username (used for error reporting).
 * @param {string|null} level - Difficulty level to query.
 * @param {{ hasErrored: boolean }} errorState - Tracks if error comment was posted.
 *
 * @returns {Promise<Array<{ title: string, html_url: string }>|null>}
 *   List of issues, [] if none found, or null if API failed.
 */
async function tryLevel(botContext, username, level, errorState) {
    if (!level) return [];

    const issues = await safeFetchIssues(botContext, username, level, errorState);
    if (issues === null) return null;

    return issues;
}

/**
 * Resolves recommended issues using a progressive fallback strategy.
 *
 * Strategy:
 *   1. Try next difficulty level (progression)
 *   2. Try same level
 *   3. Try lower level (fallback), except for Beginner → Good First Issue
 *
 * Stops at the first non-empty result set.
 *
 * Behavior:
 *   - Returns first non-empty list of issues
 *   - Returns [] if no issues found across all levels
 *   - Returns null if any API call fails (caller should abort)
 *
 * @param {object} botContext - Bot execution context.
 * @param {string} username - GitHub username.
 * @param {string} skillLevel - Current difficulty level.
 * @param {{ hasErrored: boolean }} errorState - Shared error state across fetch calls.
 *
 * @returns {Promise<Array<{ title: string, html_url: string }>|null>}
 *   Recommended issues, [] if none found, or null on failure.
 */
async function getRecommendedIssues(botContext, username, skillLevel, errorState) {
    const levelsToTry = [
        getNextLevel(skillLevel),
        skillLevel,
        skillLevel !== LABELS.BEGINNER ? getFallbackLevel(skillLevel) : null,
    ];

    for (const level of levelsToTry) {
        const issues = await tryLevel(botContext, username, level, errorState);

        if (issues === null) return null;     // API failure
        if (issues.length > 0) return issues; // first valid result
    }

    return [];
}

/**
 * Builds the recommendation comment posted after a successful PR completion.
 *
 * Formats a list of suggested issues as clickable links for easy navigation.
 * Designed to be concise, readable, and actionable within a PR discussion thread.
 *
 * @param {string} username - GitHub username to mention.
 * @param {Array<{ title: string, html_url: string }>} issues - List of recommended issues.
 * @returns {string} Markdown-formatted comment body.
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
 * Builds the error comment posted when issue recommendation fails due to API errors.
 *
 * Notifies the contributor and tags the maintainer team for manual intervention.
 * This is only posted once per execution to avoid spam.
 *
 * @param {string} username - GitHub username to mention.
 * @returns {string} Markdown-formatted error message.
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
 * Main entry point for issue recommendation workflow.
 *
 * Trigger:
 *   - Invoked after a PR is merged
 *
 * Flow:
 *   1. Validate context (sender, linked issue)
 *   2. Determine skill level from issue labels
 *   3. Resolve recommended issues via getRecommendedIssues
 *   4. Post recommendation comment if issues are found
 *
 * Behavior:
 *   - Skips silently if:
 *       - sender is missing
 *       - issue is missing
 *       - no skill label is found
 *       - no recommendations are available
 *   - Stops execution if API failure occurs (error comment handled upstream)
 *
 * @param {{
 *   github: object,
 *   owner: string,
 *   repo: string,
 *   issue: object,
 *   sender: { login: string }
 * }} botContext - Bot execution context.
 *
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

    const skillLevel = getIssueSkillLevel(botContext.issue);
    if (!skillLevel) {
        logger.log('No skill level found, skipping recommendation', {
            issueNumber: botContext.issue?.number,
        });
        return;
    }

    logger.log('Recommendation context:', {
        user: username,
        level: skillLevel,
    });

    const errorState = { hasErrored: false };

    const issues = await getRecommendedIssues(
        botContext,
        username,
        skillLevel,
        errorState
    );

    if (issues === null) return;

    if (issues.length === 0) {
        logger.log('No recommendations available for user:', username);
        return;
    }

    const comment = buildRecommendationComment(username, issues);
    await postComment(botContext, comment);
    logger.log('Posted recommendation comment');
}

module.exports = { handleRecommendIssues };
