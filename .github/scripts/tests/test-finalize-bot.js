// SPDX-License-Identifier: Apache-2.0
//
// tests/test-finalize-bot.js
//
// Local test script for the /finalize command (bot-on-comment.js → commands/finalize.js).
// Run with: node .github/scripts/tests/test-finalize-bot.js
//
// Mocks the GitHub API and runs scenarios to verify /finalize behaves correctly
// without making real API calls.

const { LABELS } = require('../helpers');
const script = require('../bot-on-comment.js');

// =============================================================================
// MOCK GITHUB API
// =============================================================================

function createMockGithub(options = {}) {
  const {
    roleName = 'triage',            // role_name returned by getCollaboratorPermissionLevel
    permissionShouldFail = false,   // throw HTTP 500 on permission check
    permissionNotFound = false,     // throw HTTP 404 (non-collaborator)
    updateShouldFail = false,       // throw on issues.update
    removeLabelShouldFail = false,
    addLabelShouldFail = false,
  } = options;

  const calls = {
    comments: [],
    reactions: [],
    labelsAdded: [],
    labelsRemoved: [],
    issueUpdates: [],
    permissionChecks: [],
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
      repos: {
        getCollaboratorPermissionLevel: async (params) => {
          calls.permissionChecks.push(params.username);
          console.log(`\n🔐 PERMISSION CHECK: @${params.username}`);
          if (permissionNotFound) {
            const err = new Error('Not Found');
            err.status = 404;
            throw err;
          }
          if (permissionShouldFail) {
            const err = new Error('Simulated permission check failure');
            err.status = 500;
            throw err;
          }
          console.log(`   → role_name: ${roleName}`);
          return { data: { role_name: roleName, permission: roleName } };
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
        update: async (params) => {
          if (updateShouldFail) {
            throw new Error('Simulated issue update failure');
          }
          calls.issueUpdates.push({ title: params.title, body: params.body });
          console.log(`\n✏️  ISSUE UPDATED: title="${params.title}"`);
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
  };
}

// =============================================================================
// HELPERS
// =============================================================================

function makeIssue(overrides = {}) {
  return {
    number: 42,
    title: 'Fix something',
    state: 'open',
    body: '### 👾 Description of the Issue\n\nThis thing is broken.\n\n### ✔️ Acceptance Criteria\n\n- [ ] Fixed',
    labels: [
      { name: LABELS.AWAITING_TRIAGE },
      { name: LABELS.BEGINNER },
      { name: 'priority: medium' },
      { name: 'kind: maintenance' },
    ],
    type: { name: 'Task' },
    assignees: [],
    ...overrides,
  };
}

function makeContext(issue, commentBody = '/finalize', commenter = 'maintainer') {
  return {
    eventName: 'issue_comment',
    payload: {
      issue,
      comment: {
        id: 9001,
        body: commentBody,
        user: { login: commenter, type: 'User' },
      },
    },
    repo: { owner: 'hiero-ledger', repo: 'hiero-sdk-cpp' },
  };
}

async function runScenario(scenario) {
  console.log('\n' + '='.repeat(70));
  console.log(`TEST: ${scenario.name}`);
  console.log(`DESC: ${scenario.description}`);
  console.log('='.repeat(70));

  const github = createMockGithub(scenario.githubOptions || {});
  const context = scenario.context;

  let threw = null;
  try {
    await script({ github, context });
  } catch (e) {
    threw = e;
  }

  // Run assertions
  let passed = true;
  const failures = [];

  for (const assertion of scenario.assertions || []) {
    const result = assertion(github.calls, threw);
    if (result !== true) {
      passed = false;
      failures.push(result);
    }
  }

  if (passed) {
    console.log('\n✅ PASSED\n');
  } else {
    console.log('\n❌ FAILED');
    for (const f of failures) console.log('  -', f);
    console.log();
  }

  return passed;
}

// =============================================================================
// ASSERTION HELPERS
// =============================================================================

const assert = {
  commentContains: (text) => (calls) => {
    const found = calls.comments.some((c) => c.includes(text));
    return found || `Expected a comment containing: "${text}"`;
  },
  noComments: () => (calls) => calls.comments.length === 0 || `Expected no comments, got ${calls.comments.length}`,
  labelAdded: (label) => (calls) => calls.labelsAdded.includes(label) || `Expected label added: "${label}"`,
  labelRemoved: (label) => (calls) => calls.labelsRemoved.includes(label) || `Expected label removed: "${label}"`,
  noLabelsAdded: () => (calls) => calls.labelsAdded.length === 0 || `Expected no labels added, got: ${calls.labelsAdded}`,
  noLabelsRemoved: () => (calls) => calls.labelsRemoved.length === 0 || `Expected no labels removed, got: ${calls.labelsRemoved}`,
  noIssueUpdate: () => (calls) => calls.issueUpdates.length === 0 || `Expected no issue update, got ${calls.issueUpdates.length}`,
  issueUpdated: () => (calls) => calls.issueUpdates.length > 0 || 'Expected issue to be updated',
  titleContains: (text) => (calls) => {
    const found = calls.issueUpdates.some((u) => u.title && u.title.includes(text));
    return found || `Expected updated title to contain: "${text}"`;
  },
  bodyContains: (text) => (calls) => {
    const found = calls.issueUpdates.some((u) => u.body && u.body.includes(text));
    return found || `Expected updated body to contain: "${text}"`;
  },
  reactionAdded: () => (calls) => calls.reactions.length > 0 || 'Expected thumbs-up reaction to be added',
};

// =============================================================================
// SCENARIOS
// =============================================================================

const scenarios = [
  // ---------------------------------------------------------------------------
  // AUTHORIZATION
  // ---------------------------------------------------------------------------

  {
    name: 'Unauthorized — read-only collaborator',
    description: 'A collaborator with "read" role is rejected',
    context: makeContext(makeIssue()),
    githubOptions: { roleName: 'read' },
    assertions: [
      assert.reactionAdded(),
      assert.commentContains('reserved for maintainers'),
      assert.noIssueUpdate(),
      assert.noLabelsAdded(),
    ],
  },

  {
    name: 'Unauthorized — non-collaborator (404)',
    description: 'A user who is not a repo collaborator is rejected',
    context: makeContext(makeIssue()),
    githubOptions: { permissionNotFound: true },
    assertions: [
      assert.reactionAdded(),
      assert.commentContains('reserved for maintainers'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Permission check API error',
    description: 'When the permission API fails, posts an error comment and tags maintainers',
    context: makeContext(makeIssue()),
    githubOptions: { permissionShouldFail: true },
    assertions: [
      assert.reactionAdded(),
      assert.commentContains('encountered an error while trying to verify your permissions'),
      assert.noIssueUpdate(),
    ],
  },

  // ---------------------------------------------------------------------------
  // LABEL VALIDATION
  // ---------------------------------------------------------------------------

  {
    name: 'Validation — missing status: awaiting triage',
    description: 'Issue has a different status label — should fail validation',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.READY_FOR_DEV },
        { name: LABELS.BEGINNER },
        { name: 'priority: medium' },
        { name: 'kind: maintenance' },
      ],
      type: { name: 'Task' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('status: awaiting triage'),
      assert.noIssueUpdate(),
      assert.noLabelsAdded(),
    ],
  },

  {
    name: 'Validation — no skill label',
    description: 'Issue is missing a skill: label',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: 'priority: medium' },
        { name: 'kind: maintenance' },
      ],
      type: { name: 'Task' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('skill:'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Validation — multiple skill labels',
    description: 'Issue has two skill: labels — exactly one is required',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.BEGINNER },
        { name: LABELS.INTERMEDIATE },
        { name: 'priority: medium' },
        { name: 'kind: maintenance' },
      ],
      type: { name: 'Task' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('skill:'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Validation — no priority label',
    description: 'Issue is missing a priority: label',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.BEGINNER },
        { name: 'kind: maintenance' },
      ],
      type: { name: 'Task' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('priority:'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Validation — Task missing kind label',
    description: 'Task issues require exactly one kind: label',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.BEGINNER },
        { name: 'priority: medium' },
      ],
      type: { name: 'Task' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('kind:'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Validation — Bug missing kind label',
    description: 'Bug issues require exactly one kind: label',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.BEGINNER },
        { name: 'priority: medium' },
      ],
      type: { name: 'Bug' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('kind:'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Validation — Feature with kind label',
    description: 'Feature issues must NOT have a kind: label',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.BEGINNER },
        { name: 'priority: medium' },
        { name: 'kind: enhancement' },
      ],
      type: { name: 'Feature' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('Feature issues should not have a `kind:`'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Validation — Multiple violations listed in one comment',
    description: 'All violations (no skill, no priority, wrong kind for feature) reported together',
    context: makeContext(makeIssue({
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: 'kind: maintenance' },
      ],
      type: { name: 'Feature' },
    })),
    githubOptions: { roleName: 'admin' },
    assertions: [
      assert.commentContains('skill:'),
      assert.commentContains('priority:'),
      assert.commentContains('Feature issues should not'),
      assert.noIssueUpdate(),
    ],
  },

  {
    name: 'Validation — Unknown issue type',
    description: 'Issue created without a recognized type triggers a validation error',
    context: makeContext(makeIssue({ type: null })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.commentContains('issue type'),
      assert.noIssueUpdate(),
    ],
  },

  // ---------------------------------------------------------------------------
  // HAPPY PATHS
  // ---------------------------------------------------------------------------

  {
    name: 'Happy Path — Good First Issue (Feature)',
    description: 'Valid GFI feature issue is finalized successfully',
    context: makeContext(makeIssue({
      title: 'Add batch query support',
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.GOOD_FIRST_ISSUE },
        { name: 'priority: low' },
      ],
      type: { name: 'Feature' },
    })),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.reactionAdded(),
      assert.issueUpdated(),
      assert.titleContains('[Good First Issue]:'),
      assert.bodyContains('First-Time Friendly'),
      assert.bodyContains('About Good First Issues'),
      assert.bodyContains('Step-by-Step Contribution Guide'),
      assert.labelAdded(LABELS.READY_FOR_DEV),
      assert.labelRemoved(LABELS.AWAITING_TRIAGE),
      assert.commentContains('finalized by @maintainer'),
    ],
  },

  {
    name: 'Happy Path — Beginner Task',
    description: 'Valid beginner task is finalized; title prefix added, body reconstructed',
    context: makeContext(makeIssue()),
    githubOptions: { roleName: 'triage' },
    assertions: [
      assert.reactionAdded(),
      assert.issueUpdated(),
      assert.titleContains('[Beginner]: Fix something'),
      assert.bodyContains('Beginner Friendly'),
      assert.bodyContains('About Beginner Issues'),
      assert.bodyContains('Step-by-Step Contribution Guide'),
      assert.labelAdded(LABELS.READY_FOR_DEV),
      assert.labelRemoved(LABELS.AWAITING_TRIAGE),
      assert.commentContains('finalized by @maintainer'),
    ],
  },

  {
    name: 'Happy Path — Intermediate Bug',
    description: 'Valid intermediate bug report is finalized',
    context: makeContext(makeIssue({
      title: 'Client fee cap silently ignored',
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.INTERMEDIATE },
        { name: 'priority: high' },
        { name: 'kind: security' },
      ],
      type: { name: 'Bug' },
    })),
    githubOptions: { roleName: 'write' },
    assertions: [
      assert.issueUpdated(),
      assert.titleContains('[Intermediate]:'),
      assert.bodyContains('Intermediate Friendly'),
      assert.bodyContains('About Intermediate Issues'),
      assert.bodyContains('Step-by-Step Contribution Guide'),
      assert.labelAdded(LABELS.READY_FOR_DEV),
      assert.commentContains('finalized by @maintainer'),
    ],
  },

  {
    name: 'Happy Path — Advanced Task',
    description: 'Valid advanced task is finalized',
    context: makeContext(makeIssue({
      title: 'Improve issue triage workflow',
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.ADVANCED },
        { name: 'priority: medium' },
        { name: 'kind: maintenance' },
        { name: 'scope: ci' },
      ],
      type: { name: 'Task' },
    })),
    githubOptions: { roleName: 'admin' },
    assertions: [
      assert.issueUpdated(),
      assert.titleContains('[Advanced]:'),
      assert.bodyContains('🧠 Advanced'),
      assert.bodyContains('About Advanced Issues'),
      assert.bodyContains('Step-by-Step Contribution Guide'),
      assert.labelAdded(LABELS.READY_FOR_DEV),
      assert.commentContains('finalized by @maintainer'),
    ],
  },

  {
    name: 'Happy Path — existing prefix is replaced',
    description: 'An issue that was already finalized once gets the correct prefix when re-finalized',
    context: makeContext(makeIssue({
      title: '[Beginner]: Fix something',
      labels: [
        { name: LABELS.AWAITING_TRIAGE },
        { name: LABELS.ADVANCED },
        { name: 'priority: medium' },
        { name: 'kind: maintenance' },
      ],
      type: { name: 'Task' },
    })),
    githubOptions: { roleName: 'maintain' },
    assertions: [
      assert.titleContains('[Advanced]: Fix something'),
      assert.issueUpdated(),
    ],
  },

  // ---------------------------------------------------------------------------
  // API FAILURE PATHS
  // ---------------------------------------------------------------------------

  {
    name: 'API failure — issue update fails',
    description: 'When issues.update throws, a failure comment is posted and labels are NOT swapped',
    context: makeContext(makeIssue()),
    githubOptions: { roleName: 'triage', updateShouldFail: true },
    assertions: [
      assert.commentContains('encountered an error while trying to update'),
      assert.noLabelsAdded(),
      assert.noLabelsRemoved(),
    ],
  },

  {
    name: 'API failure — label swap fails after successful update',
    description: 'When removeLabel throws after a successful update, maintainers are tagged',
    context: makeContext(makeIssue()),
    githubOptions: { roleName: 'triage', removeLabelShouldFail: true },
    assertions: [
      assert.issueUpdated(),
      assert.commentContains('encountered an error swapping the status labels'),
    ],
  },
];

// =============================================================================
// RUNNER
// =============================================================================

(async () => {
  let passed = 0;
  let failed = 0;

  for (const scenario of scenarios) {
    const ok = await runScenario(scenario);
    if (ok) passed++;
    else failed++;
  }

  console.log('\n' + '='.repeat(70));
  console.log(`RESULTS: ${passed} passed, ${failed} failed`);
  console.log('='.repeat(70) + '\n');

  if (failed > 0) process.exit(1);
})();
