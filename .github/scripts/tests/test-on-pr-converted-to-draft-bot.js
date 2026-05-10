// SPDX-License-Identifier: Apache-2.0
//
// tests/test-on-pr-converted-to-draft-bot.js
//
// Integration tests for bot-on-pr-converted-to-draft.js

const { runTestSuite } = require('./test-utils');
const script = require('../bot-on-pr-converted-to-draft.js');
const { LABELS } = require('../helpers/constants');

function defaultContext(overrides = {}) {
  return {
    eventName: 'pull_request_target',
    payload: {
      action: 'converted_to_draft',
      pull_request: {
        number: 1,
        user: { login: 'contributor', type: 'User' },
        labels: [],
      },
    },
    repo: { owner: 'test', repo: 'repo' },
    ...overrides,
  };
}

function createMockGithub() {
  const calls = {
    labelsRemoved: [],
  };

  return {
    rest: {
      issues: {
        removeLabel: async ({ name }) => {
          calls.labelsRemoved.push(name);
          return {};
        },
      },
    },

    calls,
  };
}

const scenarios = [
  {
    name: 'Remove needs-review label',
    context: defaultContext({
      payload: {
        action: 'converted_to_draft',
        pull_request: {
          number: 1,
          user: { login: 'alice', type: 'User' },
          labels: [{ name: LABELS.NEEDS_REVIEW }],
        },
      },
    }),

    expect: {
      labelsRemoved: [LABELS.NEEDS_REVIEW],
    },
  },

  {
    name: 'Remove needs-revision label',
    context: defaultContext({
      payload: {
        action: 'converted_to_draft',
        pull_request: {
          number: 1,
          user: { login: 'bob', type: 'User' },
          labels: [{ name: LABELS.NEEDS_REVISION }],
        },
      },
    }),

    expect: {
      labelsRemoved: [LABELS.NEEDS_REVISION],
    },
  },

  {
    name: 'Remove both labels',
    context: defaultContext({
      payload: {
        action: 'converted_to_draft',
        pull_request: {
          number: 1,
          user: { login: 'carol', type: 'User' },
          labels: [
            { name: LABELS.NEEDS_REVIEW },
            { name: LABELS.NEEDS_REVISION },
          ],
        },
      },
    }),

    expect: {
      labelsRemoved: [
        LABELS.NEEDS_REVIEW,
        LABELS.NEEDS_REVISION,
      ],
    },
  },

  {
    name: 'No labels present',
    context: defaultContext(),

    expect: {
      labelsRemoved: [],
    },
  },

  {
    name: 'Bot-authored PR skipped',
    context: defaultContext({
      payload: {
        action: 'converted_to_draft',
        pull_request: {
          number: 1,
          user: { login: 'dependabot', type: 'Bot' },
          labels: [
            { name: LABELS.NEEDS_REVIEW },
          ],
        },
      },
    }),

    expect: {
      labelsRemoved: [],
    },
  },
];

async function runTest(scenario, index) {
  console.log('\\n' + '='.repeat(70));
  console.log(`TEST ${index + 1}: ${scenario.name}`);
  console.log('='.repeat(70));

  const github = createMockGithub();

  try {
    await script({
      github,
      context: scenario.context,
    });
  } catch (error) {
    console.log(`\\n❌ SCRIPT THREW ERROR: ${error.message}`);

    if (error.stack) {
      console.log(error.stack);
    }

    return false;
  }

  let passed = true;

  const expected = scenario.expect.labelsRemoved || [];
  const actual = github.calls.labelsRemoved;

  if (
    expected.length !== actual.length ||
    expected.some((label, i) => label !== actual[i])
  ) {
    console.log(
      `\\n❌ labelsRemoved: expected [${expected.join(', ')}], got [${actual.join(', ')}]`
    );

    passed = false;
  } else {
    console.log(`\\n✅ labelsRemoved: [${actual.join(', ')}]`);
  }

  if (passed) {
    console.log('\\n✅ PASSED');
  }

  return passed;
}

runTestSuite(
  'ON-PR-CONVERTED-TO-DRAFT BOT TEST SUITE',
  scenarios,
  runTest
);