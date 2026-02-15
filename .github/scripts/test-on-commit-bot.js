// SPDX-License-Identifier: Apache-2.0
//
// test-on-commit-bot.js
//
// Local test script for bot-on-commit.js (DCO, GPG, merge-conflict checks).
// Run with: node .github/scripts/test-on-commit-bot.js
//
// Tests on-commit bot helpers and full handler without making real API calls.

const fs = require('fs');
const os = require('os');
const path = require('path');

const script = require('./bot-on-commit.js');
const {
  hasDCOSignoff,
  hasVerifiedGPGSignature,
  runDCOCheck,
  runGPGCheck,
  runMergeConflictCheck,
} = script;

/** Creates mock bot context with captured comment bodies for testing runDCOCheck/runGPGCheck. */
function createMockBotContext(comments = []) {
  return {
    github: {
      rest: {
        issues: { createComment: async (params) => { comments.push(params.body); } },
      },
    },
    owner: 'test',
    repo: 'repo',
    number: 1,
  };
}

/** Creates mock bot context with pulls.get for testing runMergeConflictCheck. */
function createMockBotContextWithMergeable(mergeable, comments = []) {
  return {
    github: {
      rest: {
        pulls: {
          get: async () => ({ data: { mergeable, mergeable_state: mergeable ? 'clean' : 'dirty' } }),
        },
        issues: { createComment: async (params) => { comments.push(params.body); } },
      },
    },
    owner: 'test',
    repo: 'repo',
    number: 1,
  };
}

// =============================================================================
// UNIT TESTS — DCO & GPG HELPERS
// =============================================================================

const unitTests = [
  // ---------------------------------------------------------------------------
  // DCO Sign-off Detection (hasDCOSignoff)
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
    test: () => hasDCOSignoff(null) === false,
  },
  {
    name: 'hasDCOSignoff - empty message',
    test: () => hasDCOSignoff('') === false,
  },

  // ---------------------------------------------------------------------------
  // GPG Signature Detection (hasVerifiedGPGSignature)
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
    test: () => hasVerifiedGPGSignature(null) === false,
  },

  // ---------------------------------------------------------------------------
  // runDCOCheck
  // ---------------------------------------------------------------------------
  {
    name: 'runDCOCheck - all pass',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one\n\nSigned-off-by: A <a@a.com>', verification: { verified: true } } },
        { sha: 'def1234567890', commit: { message: 'feat: two\n\nSigned-off-by: B <b@b.com>', verification: { verified: true } } },
      ];
      const result = await runDCOCheck(botContext, commits);
      return result === 'success' && comments.length === 0;
    },
  },
  {
    name: 'runDCOCheck - one failure',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one\n\nSigned-off-by: A <a@a.com>' } },
        { sha: 'def1234567890', commit: { message: 'feat: two - no signoff' } },
      ];
      const result = await runDCOCheck(botContext, commits);
      return result === 'failure' && comments.length === 1 && comments[0].includes('def1234');
    },
  },
  {
    name: 'runDCOCheck - all fail',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one - no signoff' } },
        { sha: 'def1234567890', commit: { message: 'feat: two - no signoff' } },
      ];
      const result = await runDCOCheck(botContext, commits);
      return result === 'failure' && comments.length === 1 && comments[0].includes('abc1234') && comments[0].includes('def1234');
    },
  },
  {
    name: 'runDCOCheck - empty commits (no failures, no comment)',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const result = await runDCOCheck(botContext, []);
      return result === 'success' && comments.length === 0;
    },
  },

  // ---------------------------------------------------------------------------
  // runGPGCheck
  // ---------------------------------------------------------------------------
  {
    name: 'runGPGCheck - all pass',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one', verification: { verified: true } } },
        { sha: 'def1234567890', commit: { message: 'feat: two', verification: { verified: true } } },
      ];
      const result = await runGPGCheck(botContext, commits);
      return result === 'success' && comments.length === 0;
    },
  },
  {
    name: 'runGPGCheck - one failure',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: one', verification: { verified: true } } },
        { sha: 'def1234567890', commit: { message: 'feat: two', verification: { verified: false } } },
      ];
      const result = await runGPGCheck(botContext, commits);
      return result === 'failure' && comments.length === 1 && comments[0].includes('def1234');
    },
  },
  {
    name: 'runGPGCheck - empty commits (no failures, no comment)',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const result = await runGPGCheck(botContext, []);
      return result === 'success' && comments.length === 0;
    },
  },
  {
    name: 'runDCOCheck and runGPGCheck - independent comments',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContext(comments);
      const commits = [
        { sha: 'abc1234567890', commit: { message: 'feat: no signoff', verification: { verified: true } } },
        { sha: 'def1234567890', commit: { message: 'fix: has signoff\n\nSigned-off-by: B <b@b.com>', verification: { verified: false } } },
      ];
      await runDCOCheck(botContext, commits);
      await runGPGCheck(botContext, commits);
      return comments.length === 2 && comments[0].includes('abc1234') && comments[1].includes('def1234');
    },
  },

  // ---------------------------------------------------------------------------
  // runMergeConflictCheck
  // ---------------------------------------------------------------------------
  {
    name: 'runMergeConflictCheck - no conflict (mergeable true)',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContextWithMergeable(true, comments);
      const result = await runMergeConflictCheck(botContext);
      return result === 'success' && comments.length === 0;
    },
  },
  {
    name: 'runMergeConflictCheck - has conflict (mergeable false)',
    test: async () => {
      const comments = [];
      const botContext = createMockBotContextWithMergeable(false, comments);
      const result = await runMergeConflictCheck(botContext);
      return result === 'failure' && comments.length === 1 && comments[0].includes('merge conflict');
    },
  },
];

// =============================================================================
// MOCK GITHUB API FOR INTEGRATION TESTS
// =============================================================================

function createMockGithub(options = {}) {
  const { commits = [], mergeable = true } = options;
  const calls = { comments: [] };

  return {
    calls,
    rest: {
      pulls: {
        listCommits: async () => {
          console.log(`\n📥 FETCHED ${commits.length} commits`);
          return { data: commits };
        },
        get: async () => ({
          data: { mergeable, mergeable_state: mergeable ? 'clean' : 'dirty' },
        }),
      },
      issues: {
        createComment: async (params) => {
          calls.comments.push(params.body);
          console.log('\n📝 COMMENT POSTED');
          return {};
        },
      },
    },
  };
}

/** Parses GITHUB_OUTPUT format (key=value lines) into an object. */
function parseGithubOutput(content) {
  const out = {};
  for (const line of (content || '').split('\n')) {
    const eq = line.indexOf('=');
    if (eq > 0) out[line.slice(0, eq)] = line.slice(eq + 1);
  }
  return out;
}

// =============================================================================
// INTEGRATION TEST SCENARIOS
// =============================================================================

const scenarios = [
  {
    name: 'All checks pass',
    description: 'All commits have DCO and GPG; no conflicts; no comment',
    context: {
      eventName: 'pull_request',
      payload: {
        pull_request: { number: 1, head: { sha: 'headsha123' } },
      },
      repo: { owner: 'test', repo: 'repo' },
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
    },
    expectedCommentCount: 0,
    expectedOutputs: { dco: 'success', gpg: 'success', merge_conflict: 'success' },
  },
  {
    name: 'DCO failure',
    description: 'Commit missing DCO; one comment posted',
    context: {
      eventName: 'pull_request',
      payload: {
        pull_request: { number: 2, head: { sha: 'headsha456' } },
      },
      repo: { owner: 'test', repo: 'repo' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'unsigned123',
          commit: {
            message: 'feat: forgot to sign off',
            verification: { verified: true },
          },
        },
      ],
      mergeable: true,
    },
    expectedCommentCount: 1,
    expectedOutputs: { dco: 'failure', gpg: 'success', merge_conflict: 'success' },
  },
  {
    name: 'GPG failure',
    description: 'Commit missing GPG; one comment posted',
    context: {
      eventName: 'pull_request',
      payload: {
        pull_request: { number: 3, head: { sha: 'headsha789' } },
      },
      repo: { owner: 'test', repo: 'repo' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'nogpg1234567',
          commit: {
            message: 'feat: no gpg\n\nSigned-off-by: Test <test@test.com>',
            verification: { verified: false },
          },
        },
      ],
      mergeable: true,
    },
    expectedCommentCount: 1,
    expectedOutputs: { dco: 'success', gpg: 'failure', merge_conflict: 'success' },
  },
  {
    name: 'Merge conflict',
    description: 'PR has merge conflicts; one comment posted',
    context: {
      eventName: 'pull_request',
      payload: {
        pull_request: { number: 4, head: { sha: 'headsha999' } },
      },
      repo: { owner: 'test', repo: 'repo' },
    },
    githubOptions: {
      commits: [
        {
          sha: 'abc1234567890',
          commit: {
            message: 'feat: ok\n\nSigned-off-by: Test <test@test.com>',
            verification: { verified: true },
          },
        },
      ],
      mergeable: false,
    },
    expectedCommentCount: 1,
    expectedOutputs: { dco: 'success', gpg: 'success', merge_conflict: 'failure' },
  },
];

// =============================================================================
// TEST RUNNER
// =============================================================================

async function runUnitTests() {
  console.log('🔬 UNIT TESTS (on-commit bot)');
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
  return failed === 0;
}

async function runIntegrationTest(scenario, index) {
  console.log('\n' + '='.repeat(70));
  console.log(`SCENARIO ${index + 1}: ${scenario.name}`);
  console.log(`Description: ${scenario.description}`);
  console.log('='.repeat(70));

  const mockGithub = createMockGithub(scenario.githubOptions);
  const outputPath = path.join(os.tmpdir(), `github-output-on-commit-${Date.now()}-${index}.txt`);
  const prevGithubOutput = process.env.GITHUB_OUTPUT;
  process.env.GITHUB_OUTPUT = outputPath;

  try {
    await script({ github: mockGithub, context: scenario.context });
  } catch (error) {
    console.log(`\n❌ SCRIPT THREW ERROR: ${error.message}`);
    process.env.GITHUB_OUTPUT = prevGithubOutput;
    try { fs.unlinkSync(outputPath); } catch (_) {}
    return false;
  }

  process.env.GITHUB_OUTPUT = prevGithubOutput;

  const commentCount = mockGithub.calls.comments.length;
  let outputContent = '';
  try {
    outputContent = fs.readFileSync(outputPath, 'utf8');
    fs.unlinkSync(outputPath);
  } catch (_) {}

  const outputs = parseGithubOutput(outputContent);
  let passed = true;

  if (commentCount !== scenario.expectedCommentCount) {
    console.log(`\n❌ Expected ${scenario.expectedCommentCount} comment(s), got ${commentCount}`);
    passed = false;
  } else {
    console.log(`\n✅ Comments: ${commentCount}`);
  }

  if (scenario.expectedOutputs) {
    for (const [key, value] of Object.entries(scenario.expectedOutputs)) {
      if (outputs[key] !== value) {
        console.log(`\n❌ Expected outputs.${key}="${value}", got "${outputs[key]}"`);
        passed = false;
      }
    }
    if (passed && scenario.expectedOutputs) {
      console.log(`✅ GITHUB_OUTPUT: dco=${outputs.dco}, gpg=${outputs.gpg}, merge_conflict=${outputs.merge_conflict}`);
    }
  }

  return passed;
}

async function runAllTests() {
  console.log('🧪 ON-COMMIT BOT TEST SUITE');
  console.log('==========================\n');

  const unitOk = await runUnitTests();

  console.log('\n\n🔗 INTEGRATION TESTS');
  console.log('='.repeat(70));

  let integrationPassed = 0;
  let integrationFailed = 0;

  for (let i = 0; i < scenarios.length; i++) {
    const success = await runIntegrationTest(scenarios[i], i);
    if (success) integrationPassed++;
    else integrationFailed++;
  }

  console.log('\n' + '='.repeat(70));
  console.log('📈 SUMMARY');
  console.log('='.repeat(70));
  console.log(
    `   Unit Tests:        ${unitTests.length} total, ${unitOk ? 'all passed ✅' : 'some failed ❌'}`
  );
  console.log(`   Integration Tests: ${scenarios.length} total, ${integrationPassed} passed, ${integrationFailed} failed`);
  console.log('='.repeat(70));

  const allPassed = unitOk && integrationFailed === 0;
  process.exit(allPassed ? 0 : 1);
}

const testIndex = process.argv[2];
if (testIndex !== undefined) {
  const index = parseInt(testIndex, 10);
  if (index >= 0 && index < scenarios.length) {
    runIntegrationTest(scenarios[index], index).then((ok) => process.exit(ok ? 0 : 1));
  } else {
    console.log(`Invalid test index. Use 0-${scenarios.length - 1}`);
    scenarios.forEach((s, i) => console.log(`  ${i}: ${s.name}`));
  }
} else {
  runAllTests();
}
