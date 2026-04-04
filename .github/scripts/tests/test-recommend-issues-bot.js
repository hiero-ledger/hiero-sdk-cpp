// SPDX-License-Identifier: Apache-2.0
//
// tests/test-recommend-issues-bot.js
//
// Unit tests for commands/recommend-issues.js
// Run with: node .github/scripts/tests/test-recommend-issues-bot.js

const { runTestSuite } = require('./test-utils');
const { LABELS, SKILL_HIERARCHY, MAINTAINER_TEAM } = require('../helpers/constants');
const {
    handleRecommendIssues,
    getIssueSkillLevel,
    getNextLevel,
    getFallbackLevel,
    getRecommendedIssues,
} = require('../commands/recommend-issues');

// =============================================================================
// MOCK FACTORY
// =============================================================================

function createMockBotContext(overrides = {}) {
    const calls = { comments: [] };

    // searchItems: null  → simulate API failure
    // searchItems: []    → API succeeds, no results
    // searchItems: [...] → API succeeds with items
    const searchItems = overrides.searchItems !== undefined ? overrides.searchItems : [];

    return {
        botContext: {
            github: {
                rest: {
                    issues: {
                        createComment: async (params) => {
                            calls.comments.push(params.body);
                        },
                    },
                    search: {
                        issuesAndPullRequests: async () => {
                            if (searchItems === null) throw new Error('Search API error');
                            return { data: { items: searchItems } };
                        },
                    },
                },
            },
            owner: 'test',
            repo: 'repo',
            number: 99,
            sender: overrides.sender !== undefined ? overrides.sender : { login: 'contributor' },
            issue: overrides.issue !== undefined ? overrides.issue : null,
        },
        calls,
    };
}

function makeIssue(labels, number = 1) {
    return {
        number,
        title: `Issue ${number}`,
        html_url: `https://github.com/test/repo/issues/${number}`,
        labels: labels.map((name) => ({ name })),
    };
}

// Convenience: lowest and commonly used levels
const GFI      = SKILL_HIERARCHY[0]; // good first issue (or equivalent)
const BEGINNER = SKILL_HIERARCHY[1];
const MID      = SKILL_HIERARCHY[2];
const TOP      = SKILL_HIERARCHY[SKILL_HIERARCHY.length - 1];

// =============================================================================
// UNIT TESTS
// =============================================================================

const unitTests = [

    // ---------------------------------------------------------------------------
    // getIssueSkillLevel
    // ---------------------------------------------------------------------------
    {
        name: 'getIssueSkillLevel: issue with one skill label → returns that level',
        test: () => {
            const issue = makeIssue([BEGINNER, LABELS.READY_FOR_DEV]);
            return getIssueSkillLevel(issue) === BEGINNER;
        },
    },
    {
        name: 'getIssueSkillLevel: issue with no skill labels → returns null',
        test: () => {
            const issue = makeIssue(['bug', 'enhancement']);
            return getIssueSkillLevel(issue) === null;
        },
    },
    {
        name: 'getIssueSkillLevel: issue with multiple skill labels → returns highest',
        test: () => {
            // recommend-issues iterates reversed, so highest wins
            const issue = makeIssue([GFI, BEGINNER, MID]);
            return getIssueSkillLevel(issue) === MID;
        },
    },
    {
        name: 'getIssueSkillLevel: issue with empty labels → returns null',
        test: () => {
            return getIssueSkillLevel(makeIssue([])) === null;
        },
    },

    // ---------------------------------------------------------------------------
    // getNextLevel
    // ---------------------------------------------------------------------------

    {
        name: 'getNextLevel: mid-hierarchy level → returns next level up',
        test: () => {
            return getNextLevel(BEGINNER) === MID;
        },
    },
    {
        name: 'getNextLevel: top-level → returns same level (no higher)',
        test: () => {
            return getNextLevel(TOP) === TOP;
        },
    },
    {
        name: 'getNextLevel: invalid level → returns null',
        test: () => {
            return getNextLevel('not-a-real-level') === null;
        },
    },
    {
        name: 'getNextLevel: lowest level → returns second level',
        test: () => {
            return getNextLevel(GFI) === BEGINNER;
        },
    },

    // ---------------------------------------------------------------------------
    // getFallbackLevel
    // ---------------------------------------------------------------------------
    {
        name: 'getFallbackLevel: mid-hierarchy level → returns one level down',
        test: () => {
            return getFallbackLevel(BEGINNER) === GFI;
        },
    },
    {
        name: 'getFallbackLevel: lowest level (GFI) → returns null (no lower)',
        test: () => {
            return getFallbackLevel(GFI) === null;
        },
    },
    {
        name: 'getFallbackLevel: invalid level → returns null',
        test: () => {
            return getFallbackLevel('not-a-real-level') === null;
        },
    },
    {
        name: 'getFallbackLevel: top level → returns second-highest',
        test: () => {
            const secondTop = SKILL_HIERARCHY[SKILL_HIERARCHY.length - 2];
            return getFallbackLevel(TOP) === secondTop;
        },
    },

    // ---------------------------------------------------------------------------
    // getRecommendedIssues — priority order
    // ---------------------------------------------------------------------------
    {
        name: 'getRecommendedIssues: next-level issues exist → prefers next',
        test: async () => {
            const { botContext } = createMockBotContext({
                sender: { login: 'contributor' },
                searchItems: [
                    makeIssue([BEGINNER, LABELS.READY_FOR_DEV], 1),
                    makeIssue([MID,      LABELS.READY_FOR_DEV], 2),
                ],
            });
            const result = await getRecommendedIssues(botContext, 'contributor', BEGINNER);
            return result !== null && result.every(i => i.number === 2);
        },
    },
    {
        name: 'getRecommendedIssues: no next-level issues → falls back to same level',
        test: async () => {
            const { botContext } = createMockBotContext({
                searchItems: [
                    makeIssue([BEGINNER, LABELS.READY_FOR_DEV], 5),
                ],
            });
            const result = await getRecommendedIssues(botContext, 'contributor', BEGINNER);
            return result !== null && result.length === 1 && result[0].number === 5;
        },
    },
    {
        name: 'getRecommendedIssues: no next or same → falls back to lower level',
        test: async () => {
            const { botContext } = createMockBotContext({
                searchItems: [
                    makeIssue([BEGINNER, LABELS.READY_FOR_DEV], 7),
                ],
            });
            const result = await getRecommendedIssues(botContext, 'contributor', MID);
            return result !== null && result.length === 1 && result[0].number === 7;
        },
    },
    {
        name: 'getRecommendedIssues: BEGINNER skill level → GFI not included in fallback',
        test: async () => {
            const { botContext } = createMockBotContext({
                searchItems: [
                    makeIssue([GFI, LABELS.READY_FOR_DEV], 3), // only GFI available, should be ignored
                ],
            });
            const result = await getRecommendedIssues(botContext, 'contributor', BEGINNER);
            return result !== null && result.length === 0;
        },
    },
    {
        name: 'getRecommendedIssues: API failure → posts error comment, returns null',
        test: async () => {
            const { botContext, calls } = createMockBotContext({
                sender: { login: 'user' },
                issue: makeIssue([BEGINNER]),
                searchItems: null,
            });
            const result = await getRecommendedIssues(botContext, 'user', BEGINNER);
            const expected = [
                '👋 Hi @user!',
                '',
                'I ran into an issue while generating recommendations for you.',
                '',
                `${MAINTAINER_TEAM} — could you please take a look?`,
                '',
                'Sorry for the inconvenience — feel free to explore open issues in the meantime!',
            ].join('\n');
            return (
                result === null &&
                calls.comments.length === 1 &&
                calls.comments[0] === expected
            );
        },
    },
    {
        name: 'getRecommendedIssues: caps results at 5',
        test: async () => {
            const { botContext } = createMockBotContext({
                searchItems: Array.from({ length: 10 }, (_, i) =>
                    makeIssue([BEGINNER, LABELS.READY_FOR_DEV], i + 1)
                ),
            });
            const result = await getRecommendedIssues(botContext, 'contributor', BEGINNER);
            return result !== null && result.length <= 5;
        },
    },

    // ---------------------------------------------------------------------------
    // handleRecommendIssues — short-circuit cases
    // ---------------------------------------------------------------------------
    {
        name: 'handleRecommendIssues: missing sender → skips silently',
        test: async () => {
            const { botContext, calls } = createMockBotContext({ sender: null });
            await handleRecommendIssues(botContext);
            return calls.comments.length === 0;
        },
    },
    {
        name: 'handleRecommendIssues: missing issue → skips silently',
        test: async () => {
            const { botContext, calls } = createMockBotContext({ issue: null });
            await handleRecommendIssues(botContext);
            return calls.comments.length === 0;
        },
    },
    {
        name: 'handleRecommendIssues: issue has no skill label → skips silently',
        test: async () => {
            const { botContext, calls } = createMockBotContext({
                issue: makeIssue(['bug', 'enhancement']),
            });
            await handleRecommendIssues(botContext);
            return calls.comments.length === 0;
        },
    },
    {
        name: 'handleRecommendIssues: no matching issues at any level → no comment',
        test: async () => {
            const { botContext, calls } = createMockBotContext({
                issue: makeIssue([BEGINNER, LABELS.READY_FOR_DEV]),
                searchItems: [],
            });
            await handleRecommendIssues(botContext);
            return calls.comments.length === 0;
        },
    },
    {
        name: 'handleRecommendIssues: valid context → posts recommendation comment',
        test: async () => {
            const { botContext, calls } = createMockBotContext({
                sender: { login: 'user' },
                issue: makeIssue([BEGINNER, LABELS.READY_FOR_DEV]),
                searchItems: [makeIssue([MID, LABELS.READY_FOR_DEV], 1)],
            });
            await handleRecommendIssues(botContext);
            const expected = [
                '👋 Hi @user! Great work on your recent contribution! 🎉',
                '',
                'Here are some issues you might want to explore next:',
                '',
                '- [Issue 1](https://github.com/test/repo/issues/1)',
                '',
                'Happy coding! 🚀',
            ].join('\n');
            return calls.comments[0] === expected;
        },
    },
];

// =============================================================================
// TEST RUNNER
// =============================================================================

async function runUnitTests() {
    console.log('🔬 UNIT TESTS (recommend-issues)');
    console.log('='.repeat(70));
    let passed = 0;
    let failed = 0;
    for (const test of unitTests) {
        try {
            const result = await Promise.resolve(test.test());
            if (result) {
                console.log(`✅ ${test.name}`);
                passed++;
            } else {
                console.log(`❌ ${test.name}`);
                failed++;
            }
        } catch (error) {
            console.log(`❌ ${test.name} - Error: ${error.message}`);
            failed++;
        }
    }
    console.log('\n' + '-'.repeat(70));
    console.log(`Unit Tests: ${passed} passed, ${failed} failed`);
    return { total: unitTests.length, passed, failed };
}

runTestSuite('RECOMMEND ISSUES TEST SUITE', [], async () => true, [
    { label: 'Unit Tests', run: runUnitTests },
]);