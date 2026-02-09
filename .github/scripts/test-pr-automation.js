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
const { LABELS } = script;

// =============================================================================
// MOCK GITHUB API
// =============================================================================

function createMockGithub(options = {}) {
  const {
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

function pullRequestContext(pr) {
  const fullPr = { ...pr, head: pr.head || { sha: 'headsha' } };
  return {
    eventName: 'pull_request',
    payload: {
      action: 'opened',
      pull_request: fullPr,
    },
    repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
  };
}

// =============================================================================
// INTEGRATION TEST SCENARIOS
// =============================================================================

const scenarios = [
  // ---------------------------------------------------------------------------
  // HAPPY PATH: DCO_GPG_PASSED=success, no conflicts → needs review
  // ---------------------------------------------------------------------------
  {
    name: 'Happy Path - All Checks Pass',
    description: 'DCO & GPG passed and no merge conflicts',
    context: pullRequestContext({
      number: 100,
      user: { login: 'contributor' },
      assignees: [],
      base: { ref: 'main' },
      head: { ref: 'feature-branch' },
    }),
    env: { DCO_GPG_PASSED: 'success' },
    githubOptions: {
      mergeable: true,
      mergeableState: 'clean',
    },
    expectedAssignee: 'contributor',
    expectedLabel: LABELS.NEEDS_REVIEW,
    expectedComments: [],
  },

  // ---------------------------------------------------------------------------
  // FAILURE: Merge conflicts → needs revision
  // ---------------------------------------------------------------------------
  {
    name: 'Failure - Merge Conflicts',
    description: 'PR with merge conflicts (DCO & GPG passed)',
    context: pullRequestContext({
      number: 202,
      user: { login: 'conflicted' },
      assignees: [],
      base: { ref: 'main' },
      head: { ref: 'conflict-branch' },
    }),
    env: { DCO_GPG_PASSED: 'success' },
    githubOptions: {
      mergeable: false,
      mergeableState: 'dirty',
    },
    expectedAssignee: 'conflicted',
    expectedLabel: LABELS.NEEDS_REVISION,
    expectedComments: [],
  },

  // ---------------------------------------------------------------------------
  // FAILURE: DCO & GPG check failed → needs revision
  // ---------------------------------------------------------------------------
  {
    name: 'Failure - DCO & GPG Check Failed',
    description: 'DCO or GPG check failed; no merge conflicts',
    context: pullRequestContext({
      number: 203,
      user: { login: 'unsigned' },
      assignees: [],
      base: { ref: 'main' },
      head: { ref: 'unsigned-branch' },
    }),
    env: { DCO_GPG_PASSED: 'failure' },
    githubOptions: {
      mergeable: true,
      mergeableState: 'clean',
    },
    expectedAssignee: 'unsigned',
    expectedLabel: LABELS.NEEDS_REVISION,
    expectedComments: [],
  },

  // ---------------------------------------------------------------------------
  // ERROR: Label addition fails → comment posted
  // ---------------------------------------------------------------------------
  {
    name: 'Error - Label Addition Fails',
    description: 'Bot posts comment when label cannot be added',
    context: pullRequestContext({
      number: 300,
      user: { login: 'unlucky' },
      assignees: [],
      base: { ref: 'main' },
      head: { ref: 'unlucky-branch' },
    }),
    env: { DCO_GPG_PASSED: 'success' },
    githubOptions: {
      mergeable: true,
      addLabelShouldFail: true,
    },
    expectedAssignee: 'unlucky',
    expectedLabel: null,
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
  // PR automation no longer has DCO/GPG logic; unit tests for those live in test-dco-gpg-bot.js
  console.log('🔬 UNIT TESTS (PR automation)');
  console.log('='.repeat(70));
  console.log('   (No unit tests in this file; DCO/GPG tests are in test-dco-gpg-bot.js)');
  return true;
}

async function runIntegrationTest(scenario, index) {
  console.log('\n' + '='.repeat(70));
  console.log(`SCENARIO ${index + 1}: ${scenario.name}`);
  console.log(`Description: ${scenario.description}`);
  console.log('='.repeat(70));

  const mockGithub = createMockGithub(scenario.githubOptions);
  const envBackup = { ...process.env };
  if (scenario.env) {
    Object.entries(scenario.env).forEach(([k, v]) => {
      process.env[k] = v;
    });
  }

  try {
    await script({ github: mockGithub, context: scenario.context });
  } catch (error) {
    console.log(`\n❌ SCRIPT THREW ERROR: ${error.message}`);
    return false;
  } finally {
    if (scenario.env) {
      Object.keys(scenario.env).forEach(k => {
        if (envBackup[k] !== undefined) process.env[k] = envBackup[k];
        else delete process.env[k];
      });
    }
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
  console.log(`   Integration Tests: ${scenarios.length} total`);
  console.log(`   Passed: ${integrationPassed} ✅`);
  console.log(`   Failed: ${integrationFailed} ${integrationFailed > 0 ? '❌' : ''}`);
  console.log('='.repeat(70));

  const allPassed = integrationFailed === 0;
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
