// SPDX-License-Identifier: Apache-2.0
//
// test-on-pr-bot.js
//
// Local test script for bot-on-pr.js (assign author, set needs review/revision from env).
// Run with: node .github/scripts/test-on-pr-bot.js
//
// Mocks the GitHub API and runs scenarios to verify the on-PR bot without real API calls.

const script = require('./bot-on-pr.js');
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
    labelsRemoved: [],
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
        removeLabel: async (params) => {
          calls.labelsRemoved.push(params.name);
          console.log(`\n🏷️  LABEL REMOVED: ${params.name}`);
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
  // HAPPY PATH: DCO, GPG, no conflicts → needs review
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
    env: { DCO_PASSED: 'success', GPG_PASSED: 'success', MERGE_CONFLICT: 'success' },
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
    env: { DCO_PASSED: 'success', GPG_PASSED: 'success', MERGE_CONFLICT: 'failure' },
    githubOptions: {
      mergeable: false,
      mergeableState: 'dirty',
    },
    expectedAssignee: 'conflicted',
    expectedLabel: LABELS.NEEDS_REVISION,
    expectedComments: [],
  },

  // ---------------------------------------------------------------------------
  // FAILURE: DCO or GPG check failed → needs revision
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
    env: { DCO_PASSED: 'failure', GPG_PASSED: 'success', MERGE_CONFLICT: 'success' },
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
    env: { DCO_PASSED: 'success', GPG_PASSED: 'success', MERGE_CONFLICT: 'success' },
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

const { verifyComments, runTestSuite } = require('./helpers/test-utils');

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

  const results = {
    passed: true,
    details: [],
  };

  if (scenario.expectedAssignee) {
    if (mockGithub.calls.assignees.includes(scenario.expectedAssignee)) {
      results.details.push(`✅ Correctly assigned to ${scenario.expectedAssignee}`);
    } else {
      results.passed = false;
      results.details.push(`❌ Expected assignee ${scenario.expectedAssignee}, got: ${mockGithub.calls.assignees.join(', ') || 'none'}`);
    }
  }

  if (scenario.expectedLabel) {
    if (mockGithub.calls.labelsAdded.includes(scenario.expectedLabel)) {
      results.details.push(`✅ Added label: ${scenario.expectedLabel}`);
    } else {
      results.passed = false;
      results.details.push(`❌ Expected label "${scenario.expectedLabel}", got: ${mockGithub.calls.labelsAdded.join(', ') || 'none'}`);
    }
  }

  const commentResult = verifyComments(scenario.expectedComments || [], mockGithub.calls.comments);
  if (!commentResult.passed) results.passed = false;
  results.details.push(...commentResult.details);

  console.log('\n📊 RESULT:');
  results.details.forEach(d => console.log(`   ${d}`));

  return results.passed;
}

runTestSuite('ON-PR BOT TEST SUITE', scenarios, runIntegrationTest);
