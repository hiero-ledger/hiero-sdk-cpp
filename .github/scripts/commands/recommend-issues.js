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
 * Determines the skill-level label applied to an issue/PR.
 *
 * Iterates through SKILL_HIERARCHY in ascending order and returns the
 * first matching label found on the issue. This ensures consistent
 * detection even if multiple labels are present (lower levels take precedence).
 *
 * @param {{ labels: Array<string|{ name: string }> }} issue - Issue or PR object from GitHub payload.
 * @returns {string|null} Matching LABELS constant (e.g. LABELS.BEGINNER), or null if none found.
 */
function getIssueSkillLevel(issue) {
    for (const level of SKILL_HIERARCHY) {
        if (hasLabel(issue, level)) return level;
    }
    return null;
}

/**
 * Computes the next recommended difficulty level based on progression.
 *
 * Strategy:
 *   - Move one level higher in SKILL_HIERARCHY
 *   - If already at highest level, return the same level (no upward progression possible)
 *
 * @param {string} currentLevel - Current skill-level label.
 * @returns {string|null} Next level label, or null if currentLevel is invalid.
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
 *   - Used as a last resort when both next-level and same-level issues are unavailable
 *
 * @param {string} currentLevel - Current skill-level label.
 * @returns {string|null} Lower level label, or null if already at lowest level.
 */
function getFallbackLevel(currentLevel) {
    const index = SKILL_HIERARCHY.indexOf(currentLevel);
    if (index <= 0) return null;

    return SKILL_HIERARCHY[index - 1];
}

/**
 * Fetches open, unassigned issues for a given difficulty level using GitHub search API.
 *
 * Filters applied:
 *   - Only issues (no PRs)
 *   - Open state
 *   - No assignee (available to contributors)
 *   - Matching skill-level label
 *   - Must be "status: ready for dev"
 *
 * Returns:
 *   - Array of issues if successful (possibly empty)
 *   - null if API call fails (caller must handle failure explicitly)
 *
 * @param {object} github - Octokit GitHub API client.
 * @param {string} owner - Repository owner.
 * @param {string} repo - Repository name.
 * @param {string} level - Skill-level label used for filtering.
 * @returns {Promise<Array|null>} List of issues or null on failure.
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
 * Wraps fetchIssuesByLevel to:
 *   - Detect API failures (null response)
 *   - Post a single error comment tagging maintainers
 *   - Prevent duplicate error comments using errorState.hasErrored
 *   - Signal failure to caller by returning null
 *
 * Important distinction:
 *   - []   → valid response, but no issues found
 *   - null → API failure, caller should stop execution
 *
 * @param {object} botContext - Bot execution context (GitHub client, repo info, etc.).
 * @param {string} username - GitHub username to mention in error comment.
 * @param {string} level - Skill-level label used for fetching issues.
 * @param {{ hasErrored: boolean }} errorState - Mutable state to avoid duplicate error comments.
 *
 * @returns {Promise<Array|null>} Issues array, or null if API failed.
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
 * Builds the recommendation comment posted after a successful PR completion.
 *
 * Formats a list of suggested issues as clickable links for easy navigation.
 * Designed to be concise, readable, and actionable within a PR discussion thread.
 *
 * @param {string} username - GitHub username to mention.
 * @param {Array} issues - List of recommended issues.
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
 * Main handler for issue recommendation workflow.
 *
 * Triggered after a PR is closed (typically merged). Executes the following steps:
 *
 *   1. Determine the skill level of the completed issue
 *   2. Attempt to fetch issues at the next higher level
 *   3. If none found, fallback to same level
 *   4. If still none, fallback to lower level (except Beginner → Good First Issue)
 *   5. If API failure occurs at any stage, post a single error comment and exit
 *   6. If suitable issues are found, post a recommendation comment
 *
 * Behavior:
 *   - No comment is posted if no issues are available (avoids noise)
 *   - API failures are handled gracefully with maintainer notification
 *   - Ensures contributors are guided progressively without overwhelming them
 *
 * @param {{
 *   github: object,
 *   owner: string,
 *   repo: string,
 *   issue: object,
 *   sender: { login: string }
 * }} botContext - Bot execution context from GitHub event payload.
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

    // If no skill label is present, skip recommendation entirely
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

    const nextLevel = getNextLevel(skillLevel);
    const errorState = { hasErrored: false };

    let issues = [];

    // ─────────────────────────────────────────────────────────────
    // Step 1: Try next-level recommendations (progression path)
    // ─────────────────────────────────────────────────────────────
    if (nextLevel) {
        issues = await safeFetchIssues(botContext, username, nextLevel, errorState);
        if (issues === null) return;
    }

    // ─────────────────────────────────────────────────────────────
    // Step 2: Fallback to same-level recommendations
    // (ensures contributors still get relevant work if progression is unavailable)
    // ─────────────────────────────────────────────────────────────
    if (issues.length === 0) {
        logger.log('No issues at next level, trying same level');

        issues = await safeFetchIssues(botContext, username, skillLevel, errorState);
        if (issues === null) return;
    }

    // ─────────────────────────────────────────────────────────────
    // Step 3: Fallback to lower-level recommendations
    // (used as last resort to avoid leaving contributor without guidance)
    //
    // Special case:
    //   - Skip fallback for Beginner → Good First Issue
    //     since GFI is intended only for first-time contributors
    // ─────────────────────────────────────────────────────────────
    if (issues.length === 0) {
        const fallback = getFallbackLevel(skillLevel);

        if (fallback && skillLevel !== LABELS.BEGINNER) {
            logger.log('Falling back to lower level:', fallback);

            issues = await safeFetchIssues(botContext, username, fallback, errorState);
            if (issues === null) return;
        }
    }

    // ─────────────────────────────────────────────────────────────
    // Step 4: No recommendations found
    // (intentionally silent to avoid unnecessary bot noise)
    // ─────────────────────────────────────────────────────────────
    if (issues.length === 0) {
        logger.log('No recommendations available for user:', username);
        return;
    }

    //Build and post recommendation comment
    const comment = buildRecommendationComment(username, issues);
    await postComment(botContext, comment);
    logger.log('Posted recommendation comment');
}

module.exports = { handleRecommendIssues };
