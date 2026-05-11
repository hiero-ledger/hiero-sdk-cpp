// SPDX-License-Identifier: Apache-2.0
//
// tests/test-on-ci-result-bot.js
//
// Integration tests for bot-on-ci-result.js.
// Run with: node .github/scripts/tests/test-on-ci-result-bot.js

const { runTestSuite, commitDCOAndGPG } = require('./test-utils');
const script = require('../bot-on-ci-result.js');
const { LABELS } = require('../helpers/constants');
const { MARKER } = require('../helpers/comments');

function createMockGithub({
  pr = defaultPR(),
  existingComments = [],
  workflowJobs = [],
  closingIssueNumbers = [42],
} = {}) {
  const calls = {
    commentsCreated: [],
    commentsUpdated: [],
    labelsAdded: [],
    labelsRemoved: [],
  };

  return {
    calls,
    rest: {
      actions: {
        listJobsForWorkflowRun: async () => ({ data: { jobs: workflowJobs } }),
      },
      pulls: {
        get: async () => ({
          data: {
            ...pr,
            mergeable: true,
            mergeable_state: 'clean',
          },
        }),
        list: async () => ({ data: [pr] }),
        listCommits: async () => ({
          data: [commitDCOAndGPG('abc123', 'feat: test')],
        }),
      },
      issues: {
        listComments: async () => ({ data: existingComments }),
        createComment: async ({ body }) => {
          calls.commentsCreated.push(body);
          return {};
        },
        updateComment: async ({ body }) => {
          calls.commentsUpdated.push(body);
          return {};
        },
        addLabels: async ({ labels }) => {
          calls.labelsAdded.push(...labels);
          return {};
        },
        removeLabel: async ({ name }) => {
          calls.labelsRemoved.push(name);
          return {};
        },
        get: async () => ({
          data: {
            number: 42,
            assignees: [{ login: pr.user.login }],
          },
        }),
      },
    },
    graphql: async () => ({
      repository: {
        pullRequest: {
          closingIssuesReferences: {
            nodes: closingIssueNumbers.map(number => ({ number })),
          },
        },
      },
    }),
  };
}

function defaultPR(overrides = {}) {
  return {
    number: 100,
    draft: false,
    user: { login: 'contributor', type: 'User' },
    labels: [{ name: LABELS.NEEDS_REVIEW }],
    head: { sha: 'abc123' },
    ...overrides,
  };
}

function defaultContext(overrides = {}) {
  return {
    eventName: 'workflow_run',
    repo: { owner: 'test-owner', repo: 'test-repo' },
    payload: {
      workflow_run: {
        id: 123,
        conclusion: 'failure',
        html_url: 'https://github.com/test-owner/test-repo/actions/runs/123',
        pull_requests: [{ number: 100 }],
        head_branch: 'feature/ci',
        head_sha: 'abc123',
        head_repository: { owner: { login: 'contributor' } },
      },
    },
    ...overrides,
  };
}

function dashboardBody(ciLine) {
  return [MARKER, 'Existing dashboard', ciLine].join('\n');
}

const scenarios = [
  {
    name: 'CI failure updates dashboard, flips label, and pings on transition',
    run: async () => {
      const github = createMockGithub({
        existingComments: [{ id: 1, body: dashboardBody(':white_check_mark: **CI Checks** -- All CI checks passed.') }],
        workflowJobs: [
          { name: 'Code / Lint', conclusion: 'failure', steps: [{ name: 'Run Clang-Format', conclusion: 'failure' }] },
        ],
      });

      await script({ github, context: defaultContext() });

      return (
        github.calls.commentsUpdated.length === 1 &&
        github.calls.commentsUpdated[0].includes(':x: **CI Checks**') &&
        github.calls.labelsRemoved.includes(LABELS.NEEDS_REVIEW) &&
        github.calls.labelsAdded.includes(LABELS.NEEDS_REVISION) &&
        github.calls.commentsCreated.length === 1 &&
        github.calls.commentsCreated[0].includes('@contributor')
      );
    },
  },
  {
    name: 'Repeated CI failure does not post another ping',
    run: async () => {
      const github = createMockGithub({
        existingComments: [{ id: 1, body: dashboardBody(':x: **CI Checks** -- Tests failed.') }],
        workflowJobs: [
          { name: 'Code / Build', conclusion: 'failure', steps: [{ name: 'Start CTest suite (Debug)', conclusion: 'failure' }] },
        ],
      });

      await script({ github, context: defaultContext() });

      return (
        github.calls.commentsUpdated.length === 1 &&
        github.calls.commentsUpdated[0].includes('Tests failed') &&
        github.calls.commentsCreated.length === 0
      );
    },
  },
  {
    name: 'CI success updates dashboard but leaves status label alone',
    run: async () => {
      const github = createMockGithub({
        existingComments: [{ id: 1, body: dashboardBody(':x: **CI Checks** -- Build failed.') }],
      });
      const context = defaultContext({
        payload: {
          workflow_run: {
            id: 124,
            conclusion: 'success',
            html_url: 'https://github.com/test-owner/test-repo/actions/runs/124',
            pull_requests: [{ number: 100 }],
            head_branch: 'feature/ci',
            head_sha: 'abc123',
            head_repository: { owner: { login: 'contributor' } },
          },
        },
      });

      await script({ github, context });

      return (
        github.calls.commentsUpdated.length === 1 &&
        github.calls.commentsUpdated[0].includes(':white_check_mark: **CI Checks**') &&
        github.calls.labelsAdded.length === 0 &&
        github.calls.labelsRemoved.length === 0 &&
        github.calls.commentsCreated.length === 0
      );
    },
  },
  {
    name: 'Fork PR resolves through head branch fallback',
    run: async () => {
      const github = createMockGithub({
        pr: defaultPR({ number: 101 }),
        workflowJobs: [
          { name: 'Code / Build', conclusion: 'failure', steps: [{ name: 'CMake Build (Debug)', conclusion: 'failure' }] },
        ],
      });
      const context = defaultContext({
        payload: {
          workflow_run: {
            id: 125,
            conclusion: 'failure',
            html_url: 'https://github.com/test-owner/test-repo/actions/runs/125',
            pull_requests: [],
            head_branch: 'feature/fork-ci',
            head_sha: 'abc123',
            head_repository: { owner: { login: 'fork-user' } },
          },
        },
      });

      await script({ github, context });

      return (
        github.calls.commentsUpdated.length === 0 &&
        github.calls.commentsCreated.length === 2 &&
        github.calls.commentsCreated[0].includes(':x: **CI Checks**') &&
        github.calls.commentsCreated[1].includes('@contributor') &&
        github.calls.labelsAdded.includes(LABELS.NEEDS_REVISION)
      );
    },
  },
];

async function runScenario(scenario, index) {
  console.log(`\nScenario ${index}: ${scenario.name}`);
  try {
    const passed = await scenario.run();
    console.log(passed ? 'Passed' : 'Failed');
    return passed;
  } catch (error) {
    console.log(`Failed with error: ${error.message}`);
    console.log(error.stack);
    return false;
  }
}

runTestSuite('ON-CI-RESULT BOT TEST SUITE', scenarios, runScenario);
