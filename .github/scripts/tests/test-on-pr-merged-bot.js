// SPDX-License-Identifier: Apache-2.0
//
// tests/test-on-pr-merged-bot.js
//
// Integration tests for the bot-on-pr-merged.js script.
// Verifies that when a PR is merged, sibling conflicts are properly evaluated
// and their components (dashboard comments and labels) updated seamlessly.

const { runTestSuite, createMockGithub } = require('./test-utils');
const onPrMergedBot = require('../bot-on-pr-merged');
const { MARKER } = require('../helpers/comments');

// =============================================================================
// SCENARIOS
// =============================================================================

const scenarios = [
  {
    name: 'Merged PR triggers conflict check on siblings: one clean, one with new conflict',
    run: async () => {
      // Mock GitHub:
      // - PR 10: Just merged
      // - PR 20: Sibling PR, clean
      // - PR 30: Sibling PR, now dirty (shows conflict)
      
      const pullsState = {
        20: { mergeable: true, mergeable_state: 'clean' },
        30: { mergeable: false, mergeable_state: 'dirty' }
      };

      const mockGithub = createMockGithub({
        existingComments: [
          // PR 20 currently clean in its comment
          { id: 201, body: `${MARKER}\nAll good` }
          // PR 30 currently clean in its comment, but API says dirty!
        ],
      });

      // Override pulls.list to return PR 20 and 30
      mockGithub.rest.pulls.list = async () => ({
        data: [
          { number: 20, draft: false },
          { number: 30, draft: false },
          { number: 40, draft: true } // skipped
        ]
      });

      // Override pulls.get to return mergeable state per PR
      mockGithub.rest.pulls.get = async ({ pull_number }) => {
        const state = pullsState[pull_number];
        return { data: { mergeable: state.mergeable, mergeable_state: state.mergeable_state } };
      };

      const context = {
        eventName: 'pull_request_target',
        repo: { owner: 'test', repo: 'repo' },
        payload: {
          action: 'closed',
          pull_request: {
            number: 10,
            merged: true,
            user: { login: 'contributor' }
          }
        }
      };

      // Also need to stub listComments to return comments per PR
      mockGithub.rest.issues.listComments = async ({ issue_number }) => {
        if (issue_number === 20) {
          return { data: [{ id: 201, body: `${MARKER}\nNo conflicts previously` }] };
        }
        if (issue_number === 30) {
          return { data: [{ id: 301, body: `${MARKER}\nNo conflicts previously` }] };
        }
        return { data: [] };
      };

      await onPrMergedBot({ github: mockGithub, context });

      // What should happen?
      // PR 20: mergeable=true. Comment body doesn't say "Merge Conflicts". Match! No update.
      // PR 30: mergeable=false. Comment body doesn't say "Merge Conflicts". Mismatch! Update!
      // Draft PR 40: Skipped.
      
      // So commentsUpdated should have length 1, for PR 30
      if (mockGithub.calls.commentsCreated.length > 0) {
        console.log('Expected no comments created, got:', mockGithub.calls.commentsCreated);
        return false;
      }

      if (mockGithub.calls.commentsUpdated.length !== 1) {
        console.log('Expected 1 comment updated, got:', mockGithub.calls.commentsUpdated.length);
        return false;
      }
      
      const update = mockGithub.calls.commentsUpdated[0];
      if (update.comment_id !== 301) {
        console.log('Expected PR 30 comment (301) to be updated, got:', update.comment_id);
        return false;
      }

      if (!update.body.includes(':x: **Merge Conflicts**')) {
        console.log('Update body should contain Merge Conflicts error');
        return false;
      }

      return true;
    }
  },
  {
    name: 'Merged PR triggers conflict check: PR already showed conflict and still has it (no churn)',
    run: async () => {
      const mockGithub = createMockGithub();
      
      mockGithub.rest.pulls.list = async () => ({
        data: [{ number: 50, draft: false }]
      });

      mockGithub.rest.pulls.get = async () => ({
        data: { mergeable: false, mergeable_state: 'dirty' }
      });

      mockGithub.rest.issues.listComments = async () => {
        return { data: [{ id: 501, body: `${MARKER}\n:x: **Merge Conflicts**` }] };
      };

      const context = {
        eventName: 'pull_request',
        repo: { owner: 'test', repo: 'repo' },
        payload: { pull_request: { number: 10, merged: true, user: { login: 'contributor' } } }
      };

      await onPrMergedBot({ github: mockGithub, context });

      // No creates or updates expected because status didn't change
      if (mockGithub.calls.commentsUpdated.length !== 0 || mockGithub.calls.commentsCreated.length !== 0) {
        console.log('Expected no comments updated/created due to no churn');
        return false;
      }

      return true;
    }
  },
  {
    name: 'Zero open non-draft sibling PRs -> exits cleanly',
    run: async () => {
      const mockGithub = createMockGithub();
      
      mockGithub.rest.pulls.list = async () => ({
        data: [{ number: 10, draft: false }, { number: 40, draft: true }]
      });

      const context = {
        eventName: 'pull_request_target',
        repo: { owner: 'test', repo: 'repo' },
        payload: {
          pull_request: { number: 10, merged: true, user: { login: 'contributor' } }
        }
      };

      await onPrMergedBot({ github: mockGithub, context });

      if (mockGithub.calls.commentsUpdated.length !== 0 || mockGithub.calls.commentsCreated.length !== 0) {
        console.log('Expected no comments updated/created');
        return false;
      }

      return true;
    }
  },
  {
    name: 'mergeable returns null across all retries -> treated as no conflict',
    run: async () => {
      const mockGithub = createMockGithub({
        existingComments: [
          { id: 601, body: `${MARKER}\nAll good` }
        ],
      });
      
      mockGithub.rest.pulls.list = async () => ({
        data: [{ number: 60, draft: false }]
      });

      // API always returns null
      mockGithub.rest.pulls.get = async () => ({
        data: { mergeable: null, mergeable_state: 'unknown' }
      });

      mockGithub.rest.issues.listComments = async () => {
        return { data: [{ id: 601, body: `${MARKER}\nAll good` }] };
      };

      const context = {
        eventName: 'pull_request',
        repo: { owner: 'test', repo: 'repo' },
        payload: { pull_request: { number: 10, merged: true, user: { login: 'contributor' } } }
      };

      await onPrMergedBot({ github: mockGithub, context });

      if (mockGithub.calls.commentsUpdated.length !== 0 || mockGithub.calls.commentsCreated.length !== 0) {
        console.log('Expected no comments updated/created');
        return false;
      }

      return true;
    }
  },
  {
    name: 'dirty -> clean: comment is updated to show no conflict',
    run: async () => {
      const mockGithub = createMockGithub({
        existingComments: [
          { id: 701, body: `${MARKER}\n:x: **Merge Conflicts**` }
        ],
      });
      
      mockGithub.rest.pulls.list = async () => ({
        data: [{ number: 70, draft: false }]
      });

      mockGithub.rest.pulls.get = async () => ({
        data: { mergeable: true, mergeable_state: 'clean' }
      });

      mockGithub.rest.issues.listComments = async () => {
        return { data: [{ id: 701, body: `${MARKER}\n:x: **Merge Conflicts**` }] };
      };

      const context = {
        eventName: 'pull_request',
        repo: { owner: 'test', repo: 'repo' },
        payload: { pull_request: { number: 10, merged: true, user: { login: 'contributor' } } }
      };
      
      await onPrMergedBot({ github: mockGithub, context });

      if (mockGithub.calls.commentsUpdated.length !== 1) {
        console.log('Expected 1 comment updated');
        return false;
      }
      
      const update = mockGithub.calls.commentsUpdated[0];
      if (update.body.includes(':x: **Merge Conflicts**')) {
        console.log('Update body should NOT contain Merge Conflicts error. Got:', update.body);
        return false;
      }

      return true;
    }
  }
];

async function runScenario(scenario, index) {
  console.log(`\n▶️  Scenario ${index}: ${scenario.name}`);
  try {
    const passed = await scenario.run();
    if (passed) {
      console.log('✅ Passed');
      return true;
    } else {
      console.log('❌ Failed');
      return false;
    }
  } catch (err) {
    console.log(`❌ Failed with error: ${err.message}`);
    console.log(err.stack);
    return false;
  }
}

// =============================================================================
// RUN
// =============================================================================

runTestSuite('ON-PR-MERGED BOT TEST SUITE', scenarios, runScenario);
