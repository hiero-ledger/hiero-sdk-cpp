// SPDX-License-Identifier: Apache-2.0
//
// tests/test-on-pr-close-bot.js
//
// Integration tests for bot-on-pr-close.js post-merge automation.
// Run with: node .github/scripts/tests/test-on-pr-close-bot.js

const { runTestSuite } = require('./test-utils');
const script = require('../bot-on-pr-close.js');
const { LABELS, MAINTAINER_TEAM } = require('../helpers/constants');

function createMockGithub({
  closingIssueNumbers = [],
  issues = {},
  milestones = [{ number: 9, title: 'Release', due_on: '2026-06-01T00:00:00Z' }],
} = {}) {
  const calls = {
    commentsCreated: [],
    labelsRemoved: [],
    milestonesUpdated: [],
  };

  return {
    calls,
    rest: {
      issues: {
        listMilestones: async () => ({ data: milestones }),
        get: async ({ issue_number }) => {
          const issue = issues[issue_number];
          if (!issue) throw new Error(`Issue #${issue_number} not found`);
          return { data: issue };
        },
        removeLabel: async ({ issue_number, name }) => {
          calls.labelsRemoved.push({ issue_number, name });
          return {};
        },
        update: async ({ issue_number, milestone }) => {
          calls.milestonesUpdated.push({ issue_number, milestone });
          return {};
        },
        createComment: async ({ issue_number, body }) => {
          calls.commentsCreated.push({ issue_number, body });
          return {};
        },
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

function defaultContext(overrides = {}) {
  return {
    eventName: 'pull_request_target',
    repo: { owner: 'test-owner', repo: 'test-repo' },
    payload: {
      pull_request: {
        number: 100,
        merged: true,
        user: { login: 'contributor', type: 'User' },
        labels: [{ name: LABELS.NEEDS_REVIEW }],
      },
    },
    ...overrides,
  };
}

const scenarios = [
  {
    name: 'Merged PR with linked issue removes status labels and milestones linked issue',
    run: async () => {
      const github = createMockGithub({
        closingIssueNumbers: [42],
        issues: {
          42: {
            number: 42,
            title: 'Linked issue',
            labels: [{ name: LABELS.IN_PROGRESS }, { name: 'area: ci' }],
          },
        },
      });

      await script({ github, context: defaultContext() });

      const removed = github.calls.labelsRemoved;
      const milestones = github.calls.milestonesUpdated;

      return (
        removed.some(call => call.issue_number === 100 && call.name === LABELS.NEEDS_REVIEW) &&
        removed.some(call => call.issue_number === 42 && call.name === LABELS.IN_PROGRESS) &&
        milestones.length === 1 &&
        milestones[0].issue_number === 42 &&
        milestones[0].milestone === 9
      );
    },
  },
  {
    name: 'Merged PR without linked issues milestones the PR itself',
    run: async () => {
      const github = createMockGithub({ closingIssueNumbers: [] });

      await script({ github, context: defaultContext() });

      return (
        github.calls.milestonesUpdated.length === 1 &&
        github.calls.milestonesUpdated[0].issue_number === 100 &&
        github.calls.milestonesUpdated[0].milestone === 9
      );
    },
  },
  {
    name: 'Merged PR without open milestone comments maintainers and stops',
    run: async () => {
      const github = createMockGithub({
        closingIssueNumbers: [42],
        issues: {
          42: { number: 42, title: 'Linked issue', labels: [{ name: LABELS.IN_PROGRESS }] },
        },
        milestones: [],
      });

      await script({ github, context: defaultContext() });

      return (
        github.calls.milestonesUpdated.length === 0 &&
        github.calls.commentsCreated.length === 1 &&
        github.calls.commentsCreated[0].issue_number === 100 &&
        github.calls.commentsCreated[0].body.includes(MAINTAINER_TEAM)
      );
    },
  },
  {
    name: 'Bot-authored merged PR still receives milestone automation',
    run: async () => {
      const github = createMockGithub({ closingIssueNumbers: [] });
      const context = defaultContext({
        payload: {
          pull_request: {
            number: 101,
            merged: true,
            user: { login: 'dependabot[bot]', type: 'Bot' },
            labels: [{ name: LABELS.NEEDS_REVIEW }],
          },
        },
      });

      await script({ github, context });

      return (
        github.calls.labelsRemoved.some(call => call.issue_number === 101 && call.name === LABELS.NEEDS_REVIEW) &&
        github.calls.milestonesUpdated.length === 1 &&
        github.calls.milestonesUpdated[0].issue_number === 101
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

runTestSuite('ON-PR-CLOSE BOT TEST SUITE', scenarios, runScenario);
