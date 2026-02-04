// SPDX-License-Identifier: Apache-2.0
//
// test-pr-automation.js
//
// Local test script for bot-pr-automation.js
// Run with: node .github/scripts/test-pr-automation.js
//
// This script mocks the GitHub API and runs various test scenarios
// to verify the bot behaves correctly without making real API calls.

const script = require('./bot-pr-automation.js');
const {
  hasDCOSignoff,
  hasVerifiedGPGSignature,
  verifyDCOSignoffs,
  verifyGPGSignatures,
  LABELS,
} = script;

// =============================================================================
// UNIT TESTS FOR HELPER FUNCTIONS
// =============================================================================

const unitTests = [
  // ---------------------------------------------------------------------------
  // DCO Sign-off Detection Tests
  // ---------------------------------------------------------------------------
  {
    name: 'hasDCOSignoff - valid sign-off',
    test: () => {
      const message = 'feat: add new feature\n\nSigned-off-by: John Doe <john@example.com>';
      return hasDCOSignoff(message) === true;
    },
  },
  {
    name: 'hasDCOSignoff - missing sign-off',
    test: () => {
      const message = 'feat: add new feature\n\nNo signature here';
      return hasDCOSignoff(message) === false;
    },
  },
  {
    name: 'hasDCOSignoff - sign-off without email',
    test: () => {
      const message = 'feat: add new feature\n\nSigned-off-by: John Doe';
      return hasDCOSignoff(message) === false;
    },
  },
  {
    name: 'hasDCOSignoff - case insensitive',
    test: () => {
      const message = 'feat: add new feature\n\nsigned-off-by: John Doe <john@example.com>';
      return hasDCOSignoff(message) === true;
    },
  },
  {
    name: 'hasDCOSignoff - null message',
    test: () => {
      return hasDCOSignoff(null) === false;
    },
  },
  {
    name: 'hasDCOSignoff - empty message',
    test: () => {
      return hasDCOSignoff('') === false;
    },
  },

  // ---------------------------------------------------------------------------
  // GPG Signature Detection Tests
  // ---------------------------------------------------------------------------
  {
    name: 'hasVerifiedGPGSignature - verified true',
    test: () => {
      const commit = { commit: { verification: { verified: true } } };
      return hasVerifiedGPGSignature(commit) === true;
    },
  },
  {
    name: 'hasVerifiedGPGSignature - verified false',
    test: () => {
      const commit = { commit: { verification: { verified: false } } };
      return hasVerifiedGPGSignature(commit) === false;
    },
  },
  {
    name: 'hasVerifiedGPGSignature - no verification object',
    test: () => {
      const commit = { commit: {} };
      return hasVerifiedGPGSignature(commit) === false;
    },
  },
  {
    name: 'hasVerifiedGPGSignature - null commit',
    test: () => {
      return hasVerifiedGPGSignature(null) === false;
    },
  },

  // ---------------------------------------------------------------------------
  // Batch DCO Verification Tests
  // ---------------------------------------------------------------------------
  {
    name: 'verifyDCOSignoffs - all pass',
    test: () => {
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one\n\nSigned-off-by: A <a@a.com>' } },
        { sha: 'def1234567890', commit: { message: 'feat: two\n\nSigned-off-by: B <b@b.com>' } },
      ];
      return verifyDCOSignoffs(commits) === true;
    },
  },
  {
    name: 'verifyDCOSignoffs - one failure',
    test: () => {
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one\n\nSigned-off-by: A <a@a.com>' } },
        { sha: 'def1234567890', commit: { message: 'feat: two - no signoff' } },
      ];
      return verifyDCOSignoffs(commits) === false;
    },
  },
  {
    name: 'verifyDCOSignoffs - all fail',
    test: () => {
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one - no signoff' } },
        { sha: 'def1234567890', commit: { message: 'feat: two - no signoff' } },
      ];
      return verifyDCOSignoffs(commits) === false;
    },
  },

  // ---------------------------------------------------------------------------
  // Batch GPG Verification Tests
  // ---------------------------------------------------------------------------
  {
    name: 'verifyGPGSignatures - all pass',
    test: () => {
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one', verification: { verified: true } } },
        { sha: 'def1234567890', commit: { message: 'feat: two', verification: { verified: true } } },
      ];
      return verifyGPGSignatures(commits) === true;
    },
  },
  {
    name: 'verifyGPGSignatures - one failure',
    test: () => {
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one', verification: { verified: true } } },
        { sha: 'def1234567890', commit: { message: 'feat: two', verification: { verified: false } } },
      ];
      return verifyGPGSignatures(commits) === false;
    },
  },
];

// =============================================================================
// MOCK GITHUB API
// =============================================================================

function createMockGithub(options = {}) {
  const {
    commits = [],
    mergeable = true,
    mergeableState = 'clean',
    assignShouldFail = false,
    addLabelShouldFail = false,
  } = options;

  const calls = {
    assignees: [],
    labelsAdded: [],
    comments: [],
    prFetches: 0,
  };

  return {
    calls,
    rest: {
      pulls: {
        listCommits: async () => {
          console.log(`\n📥 FETCHED ${commits.length} commits`);
          return { data: commits };
        },
        get: async () => {
          calls.prFetches++;
          console.log(`\n📥 FETCHED PR (mergeable: ${mergeable}, state: ${mergeableState})`);
          return {
            data: {
              mergeable,
              mergeable_state: mergeableState,
            },
          };
        },
      },
      issues: {
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
        createComment: async (params) => {
          calls.comments.push(params.body);
          console.log('\n📝 COMMENT POSTED:');
          console.log('─'.repeat(60));
          console.log(params.body);
          console.log('─'.repeat(60));
        },
      },
    },
  };
}

// =============================================================================
// INTEGRATION TEST SCENARIOS
// =============================================================================

const scenarios = [
  // ---------------------------------------------------------------------------
  // HAPPY PATHS
  // Bot assigns author and adds "needs review" label, no comments
  // ---------------------------------------------------------------------------
  {
    name: 'Happy Path - All Checks Pass',
    description: 'PR with properly signed commits and no conflicts',
    context: {
      payload: {
        action: 'opened',
        pull_request: {
          number: 100,
          user: { login: 'contributor' },
          assignees: [],
          base: { ref: 'main' },
          head: { ref: 'feature-branch' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'abc1234567890',
          commit: {
            message: 'feat: add feature\n\nSigned-off-by: Test <test@test.com>',
            verification: { verified: true },
          },
        },
      ],
      mergeable: true,
      mergeableState: 'clean',
    },
    expectedAssignee: 'contributor',
    expectedLabel: LABELS.NEEDS_REVIEW,
    expectedComments: [],
  },

  // ---------------------------------------------------------------------------
  // FAILURE SCENARIOS
  // Bot assigns author and adds "needs revision" label, no comments
  // ---------------------------------------------------------------------------
  {
    name: 'Failure - Missing DCO Sign-off',
    description: 'PR with commit missing DCO sign-off',
    context: {
      payload: {
        action: 'opened',
        pull_request: {
          number: 200,
          user: { login: 'newbie' },
          assignees: [],
          base: { ref: 'main' },
          head: { ref: 'unsigned-branch' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'unsigned123456',
          commit: {
            message: 'feat: forgot to sign off',
            verification: { verified: true },
          },
        },
      ],
      mergeable: true,
    },
    expectedAssignee: 'newbie',
    expectedLabel: LABELS.NEEDS_REVISION,
    expectedComments: [],
  },

  {
    name: 'Failure - Missing GPG Signature',
    description: 'PR with commit missing GPG signature',
    context: {
      payload: {
        action: 'opened',
        pull_request: {
          number: 201,
          user: { login: 'forgotgpg' },
          assignees: [],
          base: { ref: 'main' },
          head: { ref: 'nogpg-branch' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'nogpg1234567',
          commit: {
            message: 'feat: no gpg\n\nSigned-off-by: Test <test@test.com>',
            verification: { verified: false, reason: 'unsigned' },
          },
        },
      ],
      mergeable: true,
    },
    expectedAssignee: 'forgotgpg',
    expectedLabel: LABELS.NEEDS_REVISION,
    expectedComments: [],
  },

  {
    name: 'Failure - Merge Conflicts',
    description: 'PR with merge conflicts',
    context: {
      payload: {
        action: 'opened',
        pull_request: {
          number: 202,
          user: { login: 'conflicted' },
          assignees: [],
          base: { ref: 'main' },
          head: { ref: 'conflict-branch' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'conflict12345',
          commit: {
            message: 'feat: has conflicts\n\nSigned-off-by: Test <test@test.com>',
            verification: { verified: true },
          },
        },
      ],
      mergeable: false,
      mergeableState: 'dirty',
    },
    expectedAssignee: 'conflicted',
    expectedLabel: LABELS.NEEDS_REVISION,
    expectedComments: [],
  },

  {
    name: 'Failure - Multiple Issues',
    description: 'PR with both DCO and GPG issues',
    context: {
      payload: {
        action: 'opened',
        pull_request: {
          number: 203,
          user: { login: 'multiproblems' },
          assignees: [],
          base: { ref: 'main' },
          head: { ref: 'problems-branch' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'problem123456',
          commit: {
            message: 'feat: no signatures at all',
            verification: { verified: false, reason: 'unsigned' },
          },
        },
      ],
      mergeable: true,
    },
    expectedAssignee: 'multiproblems',
    expectedLabel: LABELS.NEEDS_REVISION,
    expectedComments: [],
  },

  // ---------------------------------------------------------------------------
  // ERROR SCENARIOS
  // Bot posts comment tagging maintainers when it cannot add labels
  // ---------------------------------------------------------------------------
  {
    name: 'Error - Label Addition Fails',
    description: 'Bot posts comment tagging maintainers when label cannot be added',
    context: {
      payload: {
        action: 'opened',
        pull_request: {
          number: 300,
          user: { login: 'unlucky' },
          assignees: [],
          base: { ref: 'main' },
          head: { ref: 'unlucky-branch' },
        },
      },
      repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'unlucky123456',
          commit: {
            message: 'feat: good commit\n\nSigned-off-by: Test <test@test.com>',
            verification: { verified: true },
          },
        },
      ],
      mergeable: true,
      addLabelShouldFail: true,
    },
    expectedAssignee: 'unlucky',
    expectedLabel: null, // Label will fail
    expectedComments: [
      `⚠️ **PR Automation Bot Error**

@hiero-ledger/hiero-sdk-cpp-maintainers — I was unable to add the \`status: needs review\` label to this PR.

**Error:** Simulated add label failure

Please add the label manually or check that it exists in the repository.`,
    ],
  },
];

// =============================================================================
// TEST RUNNER
// =============================================================================

function runUnitTests() {
  console.log('🔬 UNIT TESTS');
  console.log('='.repeat(70));

  let passed = 0;
  let failed = 0;

  for (const test of unitTests) {
    try {
      const result = test.test();
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
  console.log('-'.repeat(70));

  return failed === 0;
}

async function runIntegrationTest(scenario, index) {
  console.log('\n' + '='.repeat(70));
  console.log(`SCENARIO ${index + 1}: ${scenario.name}`);
  console.log(`Description: ${scenario.description}`);
  console.log('='.repeat(70));

  const mockGithub = createMockGithub(scenario.githubOptions);

  try {
    await script({ github: mockGithub, context: scenario.context });
  } catch (error) {
    console.log(`\n❌ SCRIPT THREW ERROR: ${error.message}`);
    return false;
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
  }

  // Check label
  if (scenario.expectedLabel) {
    if (mockGithub.calls.labelsAdded.includes(scenario.expectedLabel)) {
      results.details.push(`✅ Added label: ${scenario.expectedLabel}`);
    } else {
      results.passed = false;
      results.details.push(`❌ Expected label "${scenario.expectedLabel}", got: ${mockGithub.calls.labelsAdded.join(', ') || 'none'}`);
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

async function runAllTests() {
  console.log('🧪 BOT-PR-AUTOMATION TEST SUITE');
  console.log('================================\n');

  // Run unit tests first
  const unitTestsPassed = runUnitTests();

  // Run integration tests
  console.log('\n\n🔗 INTEGRATION TESTS');
  console.log('='.repeat(70));

  let integrationPassed = 0;
  let integrationFailed = 0;

  for (let i = 0; i < scenarios.length; i++) {
    const success = await runIntegrationTest(scenarios[i], i);
    if (success) {
      integrationPassed++;
    } else {
      integrationFailed++;
    }
  }

  // Summary
  console.log('\n' + '='.repeat(70));
  console.log('📈 SUMMARY');
  console.log('='.repeat(70));
  console.log(`   Unit Tests:        ${unitTests.length} total, ${unitTestsPassed ? 'all passed ✅' : 'some failed ❌'}`);
  console.log(`   Integration Tests: ${scenarios.length} total`);
  console.log(`   Passed: ${integrationPassed} ✅`);
  console.log(`   Failed: ${integrationFailed} ${integrationFailed > 0 ? '❌' : ''}`);
  console.log('='.repeat(70));

  const allPassed = unitTestsPassed && integrationFailed === 0;
  process.exit(allPassed ? 0 : 1);
}

// Run specific test by index, or all tests
const testIndex = process.argv[2];
if (testIndex !== undefined) {
  const index = parseInt(testIndex, 10);
  if (index >= 0 && index < scenarios.length) {
    runIntegrationTest(scenarios[index], index);
  } else {
    console.log(`Invalid test index. Available: 0-${scenarios.length - 1}`);
    console.log('\nAvailable scenarios:');
    scenarios.forEach((s, i) => console.log(`  ${i}: ${s.name}`));
  }
} else {
  runAllTests();
}
