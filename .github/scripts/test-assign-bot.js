// SPDX-License-Identifier: Apache-2.0
//
// test-assign-bot.js
//
// Local test script for bot-assign-on-comment.js
// Run with: node .github/scripts/test-assign-bot.js
//
// This script mocks the GitHub API and runs various test scenarios
// to verify the bot behaves correctly without making real API calls.

const { LABELS } = require('./bot-helpers.js');
const script = require('./bot-assign-on-comment.js');

// =============================================================================
// GRAPHQL QUERY KIND (for mock)
// =============================================================================
// Derives the kind of count query from the search string the bot builds.
// Update this if the bot's query format changes so the mock stays in sync.

/**
 * @param {string} searchQuery - The GraphQL search query string.
 * @returns {'openExcludingBlocked'|'openWithLabelBlocked'|'closedWithLabel'|'open'|'unknown'}
 */
function getGraphQLQueryKind(searchQuery) {
  if (typeof searchQuery !== 'string') return 'unknown';
  if (searchQuery.includes('is:closed')) return 'closedWithLabel';
  if (searchQuery.includes('is:open')) {
    if (searchQuery.includes(`-label:"${LABELS.BLOCKED}"`)) return 'openExcludingBlocked';
    if (searchQuery.includes(`label:"${LABELS.BLOCKED}"`)) return 'openWithLabelBlocked';
    return 'open';
  }
  return 'unknown';
}

// =============================================================================
// MOCK GITHUB API
// =============================================================================

function createMockGithub(options = {}) {
  const {
    completedIssueCount = 0,
    openAssignmentCount = 0,
    openAssignmentCountExcludingBlocked = openAssignmentCount,
    blockedIssueCount = 0,
    graphqlShouldFail = false,
    graphqlOpenAssignmentsShouldFail = false,
    assignShouldFail = false,
    removeLabelShouldFail = false,
    addLabelShouldFail = false,
  } = options;

  const calls = {
    comments: [],
    assignees: [],
    labelsAdded: [],
    labelsRemoved: [],
    graphqlCalls: [],
    reactions: [],
  };

  return {
    calls,
    rest: {
      reactions: {
        createForIssueComment: async (params) => {
          calls.reactions.push({ commentId: params.comment_id, content: params.content });
          console.log(`\n👍 REACTION ADDED: ${params.content}`);
        },
      },
      issues: {
        createComment: async (params) => {
          calls.comments.push(params.body);
          console.log('\n📝 COMMENT POSTED:');
          console.log('─'.repeat(60));
          console.log(params.body);
          console.log('─'.repeat(60));
        },
        addAssignees: async (params) => {
          if (assignShouldFail) {
            throw new Error('Simulated assignment failure');
          }
          calls.assignees.push(...params.assignees);
          console.log(`\n✅ ASSIGNED: ${params.assignees.join(', ')}`);
        },
        addLabels: async (params) => {
          if (addLabelShouldFail) {
            throw new Error('Simulated add label failure');
          }
          calls.labelsAdded.push(...params.labels);
          console.log(`\n🏷️  LABEL ADDED: ${params.labels.join(', ')}`);
        },
        removeLabel: async (params) => {
          if (removeLabelShouldFail) {
            throw new Error('Simulated remove label failure');
          }
          calls.labelsRemoved.push(params.name);
          console.log(`\n🏷️  LABEL REMOVED: ${params.name}`);
        },
      },
    },
    graphql: async (query, variables) => {
      calls.graphqlCalls.push(variables.searchQuery);
      console.log(`\n🔍 GRAPHQL QUERY: ${variables.searchQuery}`);

      const kind = getGraphQLQueryKind(variables.searchQuery);

      if (kind === 'openExcludingBlocked' || kind === 'openWithLabelBlocked' || kind === 'open') {
        if (graphqlOpenAssignmentsShouldFail) {
          throw new Error('Simulated GraphQL failure for open assignments');
        }
        const count =
          kind === 'openExcludingBlocked'
            ? openAssignmentCountExcludingBlocked
            : kind === 'openWithLabelBlocked'
              ? blockedIssueCount
              : openAssignmentCount;
        console.log(`   → Returning ${kind} count: ${count}`);
        return { search: { issueCount: count } };
      }

      if (kind === 'closedWithLabel') {
        if (graphqlShouldFail) {
          throw new Error('Simulated GraphQL failure');
        }
        console.log(`   → Returning completed count: ${completedIssueCount}`);
        return { search: { issueCount: completedIssueCount } };
      }

      console.log(`   → Unknown query kind, returning 0`);
      return { search: { issueCount: 0 } };
    },
  };
}

// =============================================================================
// TEST SCENARIOS
// =============================================================================

const scenarios = [
  // ---------------------------------------------------------------------------
  // HAPPY PATHS (4 tests)
  // Successful assignment for each skill level
  // ---------------------------------------------------------------------------

  {
    name: 'Happy Path - Good First Issue',
    description: 'New contributor successfully assigned to GFI',
    context: {
      payload: {
        issue: {
          number: 100,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1001,
          body: '/assign',
          user: { login: 'new-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: 'new-contributor',
    expectedComments: [
      `👋 Hi @new-contributor, welcome to the Hiero C++ SDK community! Thank you for choosing to contribute — we're thrilled to have you here! 🎉

You've been assigned this **Good First Issue**, and the **Good First Issue Support Team** (@hiero-ledger/hiero-sdk-good-first-issue-support) is ready to help you succeed.

The issue description above has everything you need: implementation steps, contribution workflow, and links to guides. If anything is unclear, just ask — we're happy to help.

Good luck, and welcome aboard! 🚀`,
    ],
  },

  {
    name: 'Happy Path - Beginner Issue',
    description: 'Contributor with 2 completed GFIs assigned to Beginner',
    context: {
      payload: {
        issue: {
          number: 101,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: beginner' },
          ],
        },
        comment: {
          id: 1002,
          body: '/assign',
          user: { login: 'experienced-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 2 },
    expectedAssignee: 'experienced-contributor',
    expectedComments: [
      `👋 Hi @experienced-contributor, thanks for continuing to contribute to the Hiero C++ SDK! You've been assigned this **Beginner** issue. 🙌

If this task involves any design decisions or you'd like early feedback, feel free to share your plan here before diving into the code.

Good luck! 🚀`,
    ],
  },

  {
    name: 'Happy Path - Intermediate Issue',
    description: 'Contributor with 3 completed Beginners assigned to Intermediate',
    context: {
      payload: {
        issue: {
          number: 102,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: intermediate' },
          ],
        },
        comment: {
          id: 1003,
          body: '/assign',
          user: { login: 'growing-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 3 },
    expectedAssignee: 'growing-contributor',
    expectedComments: [
      `👋 Hi @growing-contributor, thanks for continuing to contribute to the Hiero C++ SDK! You've been assigned this **Intermediate** issue. 🙌

If this task involves any design decisions or you'd like early feedback, feel free to share your plan here before diving into the code.

Good luck! 🚀`,
    ],
  },

  {
    name: 'Happy Path - Advanced Issue',
    description: 'Contributor with 3 completed Intermediates assigned to Advanced',
    context: {
      payload: {
        issue: {
          number: 103,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: advanced' },
          ],
        },
        comment: {
          id: 1004,
          body: '/assign',
          user: { login: 'senior-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 3 },
    expectedAssignee: 'senior-contributor',
    expectedComments: [
      `👋 Hi @senior-contributor, thanks for continuing to contribute to the Hiero C++ SDK! You've been assigned this **Advanced** issue. 🙌

If this task involves any design decisions or you'd like early feedback, feel free to share your plan here before diving into the code.

Good luck! 🚀`,
    ],
  },

  // ---------------------------------------------------------------------------
  // VALIDATION FAILURES (9 tests)
  // Bot rejects assignment with helpful message
  // ---------------------------------------------------------------------------

  {
    name: 'Validation - Already Assigned to Someone Else',
    description: 'Issue is taken by another contributor',
    context: {
      payload: {
        issue: {
          number: 104,
          assignees: [{ login: 'other-user' }],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1005,
          body: '/assign',
          user: { login: 'late-arrival', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @late-arrival! This issue is already assigned to @other-user.

👉 **Find another issue to work on:**
[Browse unassigned issues](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue+is%3Aopen+no%3Aassignee+label%3A%22status%3A+ready+for+dev%22)

Once you find one you like, comment \`/assign\` to get started!`,
    ],
  },

  {
    name: 'Validation - Already Assigned to Self',
    description: 'Contributor already owns the issue',
    context: {
      payload: {
        issue: {
          number: 105,
          assignees: [{ login: 'forgetful-user' }],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1006,
          body: '/assign',
          user: { login: 'forgetful-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @forgetful-user! You're already assigned to this issue. You're all set to start working on it!

If you have any questions, feel free to ask here or reach out to the team.`,
    ],
  },

  {
    name: 'Validation - Not Ready for Dev',
    description: 'Issue missing status: ready for dev label',
    context: {
      payload: {
        issue: {
          number: 106,
          assignees: [],
          labels: [
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1007,
          body: '/assign',
          user: { login: 'eager-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @eager-user! This issue is not ready for development yet.

Issues must have the \`status: ready for dev\` label before they can be assigned.

👉 **Find an issue that's ready:**
[Browse ready issues](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue+is%3Aopen+no%3Aassignee+label%3A%22status%3A+ready+for+dev%22)

Once you find one you like, comment \`/assign\` to get started!`,
    ],
  },

  {
    name: 'Validation - No Labels At All',
    description: 'Issue has no labels',
    context: {
      payload: {
        issue: {
          number: 107,
          assignees: [],
          labels: [],
        },
        comment: {
          id: 1007,
          body: '/assign',
          user: { login: 'eager-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @eager-user! This issue is not ready for development yet.

Issues must have the \`status: ready for dev\` label before they can be assigned.

👉 **Find an issue that's ready:**
[Browse ready issues](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue+is%3Aopen+no%3Aassignee+label%3A%22status%3A+ready+for+dev%22)

Once you find one you like, comment \`/assign\` to get started!`,
    ],
  },

  {
    name: 'Validation - No Skill Level Label',
    description: 'Issue missing skill level label',
    context: {
      payload: {
        issue: {
          number: 107,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
          ],
        },
        comment: {
          id: 1008,
          body: '/assign',
          user: { login: 'confused-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @confused-user! This issue doesn't have a skill level label yet.

@hiero-ledger/hiero-sdk-cpp-maintainers — could you please add one of the following labels?
- \`skill: good first issue\`
- \`skill: beginner\`
- \`skill: intermediate\`
- \`skill: advanced\`

@confused-user, once a maintainer adds the label, comment \`/assign\` again to request assignment.`,
    ],
  },

  {
    name: 'Validation - Prerequisites Not Met',
    description: 'Contributor lacks required experience',
    context: {
      payload: {
        issue: {
          number: 108,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: beginner' },
          ],
        },
        comment: {
          id: 1009,
          body: '/assign',
          user: { login: 'eager-newbie', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 0, openAssignmentCount: 0 },
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @eager-newbie! Thanks for your interest in contributing!

This is a **Beginner** issue. Before taking it on, you need to complete at least **2 Good First Issues** to build familiarity with the codebase.

📊 **Your Progress:** You've completed **0** so far.

👉 **Find Good First Issues to work on:**
[Browse available Good First Issues](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue%20is%3Aopen%20no%3Aassignee%20label%3A%22skill%3A%20good%20first%20issue%22%20label%3A%22status%3A%20ready%20for%20dev%22)

Once you've completed 2, come back and we'll be happy to assign this to you! 🎯`,
    ],
  },

  {
    name: 'Validation - Too Many Open Assignments (at limit)',
    description: 'Contributor already has 2 open issues assigned',
    context: {
      payload: {
        issue: {
          number: 114,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1015,
          body: '/assign',
          user: { login: 'busy-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { openAssignmentCount: 2 },
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @busy-contributor! Thanks for your enthusiasm to contribute!

To help contributors stay focused and ensure issues remain available for others, we limit assignments to **2 open issues** at a time. Issues labeled \`status: blocked\` are not counted toward this limit.

📊 **Your Current Assignments:** You're currently assigned to **2** open issues.

👉 **View your assigned issues:**
[Your open assignments](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue%20is%3Aopen%20assignee%3Abusy-contributor%20-label%3A%22status%3A%20blocked%22)

Once you complete or unassign from one of your current issues, come back and we'll be happy to assign this to you! 🎯`,
    ],
  },

  {
    name: 'Validation - Too Many Open Assignments (over limit)',
    description: 'Contributor has more than 2 open issues assigned',
    context: {
      payload: {
        issue: {
          number: 115,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1016,
          body: '/assign',
          user: { login: 'very-busy-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { openAssignmentCount: 5 },
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @very-busy-contributor! Thanks for your enthusiasm to contribute!

To help contributors stay focused and ensure issues remain available for others, we limit assignments to **2 open issues** at a time. Issues labeled \`status: blocked\` are not counted toward this limit.

📊 **Your Current Assignments:** You're currently assigned to **5** open issues.

👉 **View your assigned issues:**
[Your open assignments](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue%20is%3Aopen%20assignee%3Avery-busy-contributor%20-label%3A%22status%3A%20blocked%22)

Once you complete or unassign from one of your current issues, come back and we'll be happy to assign this to you! 🎯`,
    ],
  },

  {
    name: 'Validation - Over Limit After Issues Unblocked',
    description: 'User has 3 open issues; some were blocked when they got the third. Now 3 count (excluding blocked), so over limit and cannot be assigned',
    context: {
      payload: {
        issue: {
          number: 118,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1019,
          body: '/assign',
          user: { login: 'now-over-limit-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      openAssignmentCount: 3,
      openAssignmentCountExcludingBlocked: 3,
    },
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @now-over-limit-user! Thanks for your enthusiasm to contribute!

To help contributors stay focused and ensure issues remain available for others, we limit assignments to **2 open issues** at a time. Issues labeled \`status: blocked\` are not counted toward this limit.

📊 **Your Current Assignments:** You're currently assigned to **3** open issues.

👉 **View your assigned issues:**
[Your open assignments](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue%20is%3Aopen%20assignee%3Anow-over-limit-user%20-label%3A%22status%3A%20blocked%22)

Once you complete or unassign from one of your current issues, come back and we'll be happy to assign this to you! 🎯`,
    ],
  },

  {
    name: 'Validation - At Limit With Blocked Issues (shows blocked link)',
    description: 'User at 2 open (excluding blocked) and has 1 blocked issue; comment includes link to blocked issues',
    context: {
      payload: {
        issue: {
          number: 119,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1020,
          body: '/assign',
          user: { login: 'at-limit-with-blocked', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      openAssignmentCount: 2,
      openAssignmentCountExcludingBlocked: 2,
      blockedIssueCount: 1,
    },
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @at-limit-with-blocked! Thanks for your enthusiasm to contribute!

To help contributors stay focused and ensure issues remain available for others, we limit assignments to **2 open issues** at a time. Issues labeled \`status: blocked\` are not counted toward this limit.

📊 **Your Current Assignments:** You're currently assigned to **2** open issues.

👉 **View your assigned issues:**
[Your open assignments](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue%20is%3Aopen%20assignee%3Aat-limit-with-blocked%20-label%3A%22status%3A%20blocked%22)

👉 **View your blocked issues:**
[Your blocked issues](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue%20is%3Aopen%20assignee%3Aat-limit-with-blocked%20label%3A%22status%3A%20blocked%22)

Once you complete or unassign from one of your current issues, come back and we'll be happy to assign this to you! 🎯`,
    ],
  },

  {
    name: 'Validation - Under Assignment Limit (1 open issue)',
    description: 'Contributor with 1 open issue can take another',
    context: {
      payload: {
        issue: {
          number: 116,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1017,
          body: '/assign',
          user: { login: 'active-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { openAssignmentCount: 1 },
    expectedAssignee: 'active-contributor',
    expectedComments: [
      `👋 Hi @active-contributor, welcome to the Hiero C++ SDK community! Thank you for choosing to contribute — we're thrilled to have you here! 🎉

You've been assigned this **Good First Issue**, and the **Good First Issue Support Team** (@hiero-ledger/hiero-sdk-good-first-issue-support) is ready to help you succeed.

The issue description above has everything you need: implementation steps, contribution workflow, and links to guides. If anything is unclear, just ask — we're happy to help.

Good luck, and welcome aboard! 🚀`,
    ],
  },

  {
    name: 'Validation - Open Assignments Exclude Blocked',
    description: 'Contributor with 2 open issues both status: blocked can be assigned (blocked not counted)',
    context: {
      payload: {
        issue: {
          number: 117,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1018,
          body: '/assign',
          user: { login: 'blocked-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      openAssignmentCount: 2,
      openAssignmentCountExcludingBlocked: 0,
    },
    expectedAssignee: 'blocked-contributor',
    expectedComments: [
      `👋 Hi @blocked-contributor, welcome to the Hiero C++ SDK community! Thank you for choosing to contribute — we're thrilled to have you here! 🎉

You've been assigned this **Good First Issue**, and the **Good First Issue Support Team** (@hiero-ledger/hiero-sdk-good-first-issue-support) is ready to help you succeed.

The issue description above has everything you need: implementation steps, contribution workflow, and links to guides. If anything is unclear, just ask — we're happy to help.

Good luck, and welcome aboard! 🚀`,
    ],
  },

  // ---------------------------------------------------------------------------
  // ERROR HANDLING (5 tests)
  // API failures result in maintainer tagging
  // ---------------------------------------------------------------------------

  {
    name: 'Error - Open Assignments API Failure',
    description: 'Tags maintainers when open assignments check fails',
    context: {
      payload: {
        issue: {
          number: 117,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1018,
          body: '/assign',
          user: { login: 'unlucky-user-3', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { graphqlOpenAssignmentsShouldFail: true },
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @unlucky-user-3! I encountered an error while trying to verify your eligibility for this issue.

@hiero-ledger/hiero-sdk-cpp-maintainers — could you please help with this assignment request?

@unlucky-user-3, a maintainer will review your request and assign you manually if appropriate. Sorry for the inconvenience!`,
    ],
  },

  {
    name: 'Error - GraphQL API Failure',
    description: 'Tags maintainers when prerequisite check fails',
    context: {
      payload: {
        issue: {
          number: 109,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: beginner' },
          ],
        },
        comment: {
          id: 1010,
          body: '/assign',
          user: { login: 'unlucky-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { graphqlShouldFail: true },
    expectedAssignee: null,
    expectedComments: [
      `👋 Hi @unlucky-user! I encountered an error while trying to verify your eligibility for this issue.

@hiero-ledger/hiero-sdk-cpp-maintainers — could you please help with this assignment request?

@unlucky-user, a maintainer will review your request and assign you manually if appropriate. Sorry for the inconvenience!`,
    ],
  },

  {
    name: 'Error - Assignment API Failure',
    description: 'Tags maintainers when assignment fails',
    context: {
      payload: {
        issue: {
          number: 110,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1011,
          body: '/assign',
          user: { login: 'unlucky-user-2', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { assignShouldFail: true },
    expectedAssignee: null,
    expectedComments: [
      `⚠️ Hi @unlucky-user-2! I tried to assign you to this issue, but encountered an error.

@hiero-ledger/hiero-sdk-cpp-maintainers — could you please manually assign @unlucky-user-2 to this issue?

Error details: Simulated assignment failure`,
    ],
  },

  {
    name: 'Error - Label Update Failure',
    description: 'Tags maintainers when labels cannot be updated',
    context: {
      payload: {
        issue: {
          number: 111,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1012,
          body: '/assign',
          user: { login: 'partially-lucky', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { removeLabelShouldFail: true, addLabelShouldFail: true },
    expectedAssignee: 'partially-lucky',
    expectedComments: [
      `👋 Hi @partially-lucky, welcome to the Hiero C++ SDK community! Thank you for choosing to contribute — we're thrilled to have you here! 🎉

You've been assigned this **Good First Issue**, and the **Good First Issue Support Team** (@hiero-ledger/hiero-sdk-good-first-issue-support) is ready to help you succeed.

The issue description above has everything you need: implementation steps, contribution workflow, and links to guides. If anything is unclear, just ask — we're happy to help.

Good luck, and welcome aboard! 🚀`,
      `⚠️ @partially-lucky has been successfully assigned to this issue, but I encountered an error updating the labels.

@hiero-ledger/hiero-sdk-cpp-maintainers — please manually:
- Remove the \`status: ready for dev\` label
- Add the \`status: in progress\` label

Error details: Failed to remove "status: ready for dev" label: Simulated remove label failure; Failed to add "status: in progress" label: Simulated add label failure`,
    ],
  },

  // ---------------------------------------------------------------------------
  // NO ACTION (2 tests)
  // Bot stays silent and takes no action
  // ---------------------------------------------------------------------------

  {
    name: 'No Action - Comment Without /assign',
    description: 'Regular comment ignored',
    context: {
      payload: {
        issue: {
          number: 112,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1013,
          body: 'This looks interesting, can someone help me understand it?',
          user: { login: 'curious-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedComments: [],
  },

  {
    name: 'No Action - Bot Comment',
    description: 'Bot users ignored to prevent loops',
    context: {
      payload: {
        issue: {
          number: 113,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          id: 1014,
          body: '/assign',
          user: { login: 'github-actions[bot]', type: 'Bot' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedComments: [],
  },
];

// =============================================================================
// TEST RUNNER
// =============================================================================

async function runTest(scenario, index) {
  console.log('\n' + '='.repeat(70));
  console.log(`TEST ${index + 1}: ${scenario.name}`);
  console.log(`Description: ${scenario.description}`);
  console.log('='.repeat(70));

  const mockGithub = createMockGithub(scenario.githubOptions);

  try {
    await script({ github: mockGithub, context: scenario.context });
  } catch (error) {
    console.log(`\n❌ SCRIPT THREW ERROR: ${error.message}`);
  }

  // Verify results
  const results = {
    passed: true,
    details: [],
  };

  // Check assignee
  if (scenario.expectedAssignee) {
    if (mockGithub.calls.assignees.includes(scenario.expectedAssignee)) {
      results.details.push(`✅ Correctly assigned to ${scenario.expectedAssignee}`);
    } else {
      results.passed = false;
      results.details.push(`❌ Expected assignee ${scenario.expectedAssignee}, got: ${mockGithub.calls.assignees.join(', ') || 'none'}`);
    }
  } else {
    if (mockGithub.calls.assignees.length === 0) {
      results.details.push('✅ Correctly did not assign anyone');
    } else {
      results.passed = false;
      results.details.push(`❌ Should not have assigned, but assigned: ${mockGithub.calls.assignees.join(', ')}`);
    }
  }

  // Check comments (snapshot comparison)
  const expectedComments = scenario.expectedComments || [];
  const actualComments = mockGithub.calls.comments;

  if (expectedComments.length === 0 && actualComments.length === 0) {
    results.details.push('✅ Correctly posted no comments');
  } else if (expectedComments.length !== actualComments.length) {
    results.passed = false;
    results.details.push(`❌ Expected ${expectedComments.length} comment(s), got ${actualComments.length}`);
  } else {
    for (let i = 0; i < expectedComments.length; i++) {
      if (actualComments[i] === expectedComments[i]) {
        results.details.push(`✅ Comment ${i + 1} matches snapshot`);
      } else {
        results.passed = false;
        results.details.push(`❌ Comment ${i + 1} does not match snapshot`);
        console.log('\n📋 EXPECTED:');
        console.log('─'.repeat(60));
        console.log(expectedComments[i]);
        console.log('─'.repeat(60));
        console.log('\n📋 ACTUAL:');
        console.log('─'.repeat(60));
        console.log(actualComments[i]);
        console.log('─'.repeat(60));
      }
    }
  }

  console.log('\n📊 RESULT:');
  results.details.forEach(d => console.log(`   ${d}`));

  return results.passed;
}

function printSummaryAndExit(total, passed, failed) {
  console.log('\n' + '='.repeat(70));
  console.log('📈 SUMMARY');
  console.log('='.repeat(70));
  console.log(`   Total:  ${total}`);
  console.log(`   Passed: ${passed} ✅`);
  console.log(`   Failed: ${failed} ${failed > 0 ? '❌' : ''}`);
  console.log('='.repeat(70));
  process.exit(failed > 0 ? 1 : 0);
}

async function runAllTests() {
  console.log('🧪 BOT-ASSIGN-ON-COMMENT TEST SUITE');
  console.log('====================================\n');

  let passed = 0;
  let failed = 0;

  for (let i = 0; i < scenarios.length; i++) {
    const success = await runTest(scenarios[i], i);
    if (success) {
      passed++;
    } else {
      failed++;
    }
  }

  printSummaryAndExit(scenarios.length, passed, failed);
}

// Run specific test by index, or all tests
const testIndex = process.argv[2];
if (testIndex !== undefined) {
  const index = parseInt(testIndex, 10);
  if (index >= 0 && index < scenarios.length) {
    (async () => {
      const success = await runTest(scenarios[index], index);
      printSummaryAndExit(1, success ? 1 : 0, success ? 0 : 1);
    })();
  } else {
    console.log(`Invalid test index. Available: 0-${scenarios.length - 1}`);
    console.log('\nAvailable tests:');
    scenarios.forEach((s, i) => console.log(`  ${i}: ${s.name}`));
  }
} else {
  runAllTests();
}
