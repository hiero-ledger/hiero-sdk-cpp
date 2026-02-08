// SPDX-License-Identifier: Apache-2.0
//
// test-dco-gpg-bot.js
//
// Local test script for bot-dco-gpg.js and shared DCO/GPG helpers.
// Run with: node .github/scripts/test-dco-gpg-bot.js
//
// Tests DCO and GPG verification logic and the DCO/GPG bot behavior
// without making real API calls.

const script = require('./bot-dco-gpg.js');
const {
  hasDCOSignoff,
  hasVerifiedGPGSignature,
  verifyDCOSignoffs,
  verifyGPGSignatures,
  getFailures,
} = script;

// =============================================================================
// UNIT TESTS FOR DCO & GPG HELPERS
// =============================================================================

const unitTests = [
  // ---------------------------------------------------------------------------
  // DCO Sign-off Detection
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
  // GPG Signature Detection
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
  // Batch DCO Verification
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
  // Batch GPG Verification
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

  // ---------------------------------------------------------------------------
  // getFailures helper
  // ---------------------------------------------------------------------------
  {
    name: 'getFailures - identifies DCO and GPG failures',
    test: () => {
      const commits = [
        {
          sha: 'abc1234567890',
          commit: { message: 'feat: no signoff', verification: { verified: true } },
        },
        {
          sha: 'def1234567890',
          commit: {
            message: 'fix: has signoff\n\nSigned-off-by: B <b@b.com>',
            verification: { verified: false },
          },
        },
      ];
      const { dcoFailures, gpgFailures } = getFailures(commits);
      return (
        dcoFailures.length === 1 &&
        dcoFailures[0].sha === 'abc1234' &&
        gpgFailures.length === 1 &&
        gpgFailures[0].sha === 'def1234'
      );
    },
  },
  {
    name: 'getFailures - no failures',
    test: () => {
      const commits = [
        {
          sha: 'abc1234567890',
          commit: {
            message: 'feat: one\n\nSigned-off-by: A <a@a.com>',
            verification: { verified: true },
          },
        },
      ];
      const { dcoFailures, gpgFailures } = getFailures(commits);
      return dcoFailures.length === 0 && gpgFailures.length === 0;
    },
  },
];

// =============================================================================
// MOCK GITHUB API FOR INTEGRATION TESTS
// =============================================================================

function createMockGithub(options = {}) {
  const { commits = [], createCheckFails = false } = options;
  const calls = {
    checkRuns: [],
    comments: [],
  };

  return {
    calls,
    rest: {
      pulls: {
        listCommits: async () => {
          console.log(`\n📥 FETCHED ${commits.length} commits`);
          return { data: commits };
        },
      },
      checks: {
        create: async (params) => {
          if (createCheckFails) {
            throw new Error('Simulated check create failure');
          }
          calls.checkRuns.push(params);
          console.log(`\n✅ CHECK RUN: ${params.conclusion} - ${params.output?.title}`);
          return { data: { id: 1 } };
        },
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

// =============================================================================
// INTEGRATION TEST SCENARIOS
// =============================================================================

const scenarios = [
  {
    name: 'All checks pass',
    description: 'All commits have DCO and GPG; check run success, no comment',
    context: {
      payload: {
        pull_request: {
          number: 1,
          head: { sha: 'headsha123' },
        },
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
    },
    expectedCheckConclusion: 'success',
    expectedCommentCount: 0,
  },
  {
    name: 'DCO failure',
    description: 'Commit missing DCO; check run failure, comment posted',
    context: {
      payload: {
        pull_request: {
          number: 2,
          head: { sha: 'headsha456' },
        },
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
    },
    expectedCheckConclusion: 'failure',
    expectedCommentCount: 1,
  },
  {
    name: 'GPG failure',
    description: 'Commit missing GPG; check run failure, comment posted',
    context: {
      payload: {
        pull_request: {
          number: 3,
          head: { sha: 'headsha789' },
        },
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
    },
    expectedCheckConclusion: 'failure',
    expectedCommentCount: 1,
  },
];

// =============================================================================
// TEST RUNNER
// =============================================================================

function runUnitTests() {
  console.log('🔬 UNIT TESTS (DCO & GPG)');
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

  const checkRun = mockGithub.calls.checkRuns[0];
  const commentCount = mockGithub.calls.comments.length;

  let passed = true;
  if (checkRun?.conclusion !== scenario.expectedCheckConclusion) {
    console.log(
      `\n❌ Expected check conclusion "${scenario.expectedCheckConclusion}", got "${checkRun?.conclusion}"`
    );
    passed = false;
  } else {
    console.log(`\n✅ Check conclusion: ${checkRun.conclusion}`);
  }
  if (commentCount !== scenario.expectedCommentCount) {
    console.log(
      `\n❌ Expected ${scenario.expectedCommentCount} comment(s), got ${commentCount}`
    );
    passed = false;
  } else {
    console.log(`✅ Comments: ${commentCount}`);
  }

  return passed;
}

async function runAllTests() {
  console.log('🧪 DCO/GPG BOT TEST SUITE');
  console.log('=========================\n');

  const unitOk = runUnitTests();

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
