// SPDX-License-Identifier: Apache-2.0
//
// test-assign-bot.js
//
// Local test script for bot-assign-on-comment.js
// Run with: node .github/scripts/test-assign-bot.js
//
// This script mocks the GitHub API and runs various test scenarios
// to verify the bot behaves correctly without making real API calls.

const script = require('./bot-assign-on-comment.js');

// =============================================================================
// MOCK GITHUB API
// =============================================================================

function createMockGithub(options = {}) {
  const {
    completedIssueCount = 0,
    graphqlShouldFail = false,
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
  };

  return {
    calls,
    rest: {
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

      if (graphqlShouldFail) {
        throw new Error('Simulated GraphQL failure');
      }

      console.log(`   → Returning count: ${completedIssueCount}`);
      return { search: { issueCount: completedIssueCount } };
    },
  };
}

// =============================================================================
// TEST SCENARIOS
// =============================================================================

const scenarios = [
  {
    name: 'Happy Path - Good First Issue',
    description: 'User requests assignment on a properly labeled GFI',
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
          body: '/assign',
          user: { login: 'new-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: 'new-contributor',
    expectedCommentContains: 'welcome to the Hiero C++ SDK community',
  },

  {
    name: 'Happy Path - Beginner Issue (Prerequisites Met)',
    description: 'User with 2 completed GFIs requests beginner issue',
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
          body: 'I would like to work on this. /assign please',
          user: { login: 'experienced-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 2 },
    expectedAssignee: 'experienced-contributor',
    expectedCommentContains: 'thanks for continuing to contribute',
  },

  {
    name: 'Failure - Prerequisites Not Met',
    description: 'User with 0 GFIs tries to take a beginner issue',
    context: {
      payload: {
        issue: {
          number: 102,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: beginner' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'eager-newbie', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 0 },
    expectedAssignee: null,
    expectedCommentContains: 'complete at least **2 Good First Issues**',
  },

  {
    name: 'Failure - Already Assigned to Someone Else',
    description: 'User tries to assign an issue already taken',
    context: {
      payload: {
        issue: {
          number: 103,
          assignees: [{ login: 'other-user' }],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'late-arrival', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedCommentContains: 'already assigned to @other-user',
  },

  {
    name: 'Info - Already Assigned to Self',
    description: 'User who is already assigned comments /assign again',
    context: {
      payload: {
        issue: {
          number: 104,
          assignees: [{ login: 'forgetful-user' }],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'forgetful-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedCommentContains: "You're already assigned to this issue",
  },

  {
    name: 'Failure - Not Ready for Dev',
    description: 'User tries to assign an issue without ready for dev label',
    context: {
      payload: {
        issue: {
          number: 105,
          assignees: [],
          labels: [
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'eager-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedCommentContains: 'not ready for development yet',
  },

  {
    name: 'Failure - No Skill Level Label',
    description: 'Issue has ready for dev but no skill level',
    context: {
      payload: {
        issue: {
          number: 106,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'confused-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedCommentContains: "doesn't have a skill level label yet",
  },

  {
    name: 'No Action - Comment Without /assign',
    description: 'Regular comment without assignment command',
    context: {
      payload: {
        issue: {
          number: 107,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          body: 'This looks interesting, can someone help me understand it?',
          user: { login: 'curious-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedCommentContains: null, // No comment expected
  },

  {
    name: 'No Action - Bot Comment',
    description: 'Bot comments /assign (should be ignored)',
    context: {
      payload: {
        issue: {
          number: 108,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: good first issue' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'github-actions[bot]', type: 'Bot' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {},
    expectedAssignee: null,
    expectedCommentContains: null,
  },

  {
    name: 'Error Handling - GraphQL API Failure',
    description: 'API fails when checking prerequisites, tags maintainers',
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
          body: '/assign',
          user: { login: 'unlucky-user', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { graphqlShouldFail: true },
    expectedAssignee: null,
    expectedCommentContains: 'encountered an error while trying to verify',
  },

  {
    name: 'Error Handling - Assignment API Failure',
    description: 'Assignment fails, tags maintainers',
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
          body: '/assign',
          user: { login: 'unlucky-user-2', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { assignShouldFail: true },
    expectedAssignee: null,
    expectedCommentContains: 'tried to assign you to this issue, but encountered an error',
  },

  {
    name: 'Error Handling - Label Update Failure',
    description: 'Assignment succeeds but label update fails',
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
          body: '/assign',
          user: { login: 'partially-lucky', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { removeLabelShouldFail: true, addLabelShouldFail: true },
    expectedAssignee: 'partially-lucky',
    expectedCommentContains: 'encountered an error updating the labels',
  },

  {
    name: 'Happy Path - Intermediate Issue',
    description: 'User with 3 completed Beginner issues requests intermediate',
    context: {
      payload: {
        issue: {
          number: 112,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: intermediate' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'growing-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 3 },
    expectedAssignee: 'growing-contributor',
    expectedCommentContains: 'Intermediate',
  },

  {
    name: 'Happy Path - Advanced Issue',
    description: 'User with 3 completed Intermediate issues requests advanced',
    context: {
      payload: {
        issue: {
          number: 113,
          assignees: [],
          labels: [
            { name: 'status: ready for dev' },
            { name: 'skill: advanced' },
          ],
        },
        comment: {
          body: '/assign',
          user: { login: 'senior-contributor', type: 'User' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: { completedIssueCount: 3 },
    expectedAssignee: 'senior-contributor',
    expectedCommentContains: 'Advanced',
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

  // Check comment
  if (scenario.expectedCommentContains) {
    const hasExpectedContent = mockGithub.calls.comments.some(c => c.includes(scenario.expectedCommentContains));
    if (hasExpectedContent) {
      results.details.push(`✅ Comment contains expected text`);
    } else {
      results.passed = false;
      results.details.push(`❌ Comment should contain "${scenario.expectedCommentContains}"`);
    }
  } else if (scenario.expectedCommentContains === null) {
    if (mockGithub.calls.comments.length === 0) {
      results.details.push('✅ Correctly posted no comment');
    }
  }

  console.log('\n📊 RESULT:');
  results.details.forEach(d => console.log(`   ${d}`));

  return results.passed;
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

  console.log('\n' + '='.repeat(70));
  console.log('📈 SUMMARY');
  console.log('='.repeat(70));
  console.log(`   Total:  ${scenarios.length}`);
  console.log(`   Passed: ${passed} ✅`);
  console.log(`   Failed: ${failed} ${failed > 0 ? '❌' : ''}`);
  console.log('='.repeat(70));

  process.exit(failed > 0 ? 1 : 0);
}

// Run specific test by index, or all tests
const testIndex = process.argv[2];
if (testIndex !== undefined) {
  const index = parseInt(testIndex, 10);
  if (index >= 0 && index < scenarios.length) {
    runTest(scenarios[index], index);
  } else {
    console.log(`Invalid test index. Available: 0-${scenarios.length - 1}`);
    console.log('\nAvailable tests:');
    scenarios.forEach((s, i) => console.log(`  ${i}: ${s.name}`));
  }
} else {
  runAllTests();
}
