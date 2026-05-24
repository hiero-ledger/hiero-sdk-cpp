// SPDX-License-Identifier: Apache-2.0
//
// helpers/ci.js
//
// CI-related helpers for the cron-driven on-ci-result bot. The bot iterates
// open PRs and reconciles each one's dashboard against the latest "PR Checks"
// run for that PR's head SHA. These helpers do the lookup and classification.

const { getLogger } = require('./logger');
const { requireObject, requireNonEmptyString } = require('./validation');

const PR_CHECKS_WORKFLOW_NAME = 'PR Checks';

/**
 * Looks up the latest "PR Checks" workflow run for a given head SHA.
 * Returns null if no run has been recorded yet, or if the latest run is still
 * in progress / queued (the caller should skip those PRs until the run
 * finishes — we don't want to flap the dashboard mid-run).
 *
 * Runs in the base-repository context, so this works for fork PRs without
 * needing any input from the fork.
 *
 * @param {object} github - Octokit GitHub API client.
 * @param {string} owner - Base-repo owner.
 * @param {string} repo - Base-repo name.
 * @param {string} headSha - PR head commit SHA.
 * @returns {Promise<object|null>} - Latest completed workflow run, or null.
 */
async function getLatestPRChecksRun(github, owner, repo, headSha) {
  try {
    requireNonEmptyString(owner, 'owner');
    requireNonEmptyString(repo, 'repo');
    requireNonEmptyString(headSha, 'headSha');

    const { data } = await github.rest.actions.listWorkflowRunsForRepo({
      owner,
      repo,
      head_sha: headSha,
      per_page: 100,
    });

    const runs = (data.workflow_runs || []).filter(
      run => run.name === PR_CHECKS_WORKFLOW_NAME
    );

    if (runs.length === 0) {
      return null;
    }

    // listWorkflowRunsForRepo returns newest-first by default, but be explicit.
    runs.sort((a, b) => new Date(b.run_started_at || b.created_at) - new Date(a.run_started_at || a.created_at));
    const latest = runs[0];

    if (latest.status !== 'completed') {
      getLogger().log(`Latest PR Checks run for ${headSha} is ${latest.status}, skipping`);
      return null;
    }

    return latest;
  } catch (error) {
    getLogger().error(`Failed to look up PR Checks run for ${headSha}: ${error.message}`);
    return null;
  }
}

/**
 * Classifies a workflow run's failure by inspecting job and step results.
 * Returns the type of failure (lint, build, or tests) and the run URL.
 *
 * @param {object} github - Octokit GitHub API client.
 * @param {object} workflowRun - Workflow run object (from listWorkflowRunsForRepo).
 * @param {string} owner - Repository owner.
 * @param {string} repo - Repository name.
 * @returns {Promise<{ failed: boolean, check: string|null, runUrl: string }>}
 */
async function classifyWorkflowFailure(github, workflowRun, owner, repo) {
  const runUrl = workflowRun.html_url;

  try {
    requireObject(workflowRun, 'workflowRun');
    requireNonEmptyString(owner, 'owner');
    requireNonEmptyString(repo, 'repo');

    if (workflowRun.conclusion === 'success') {
      return { failed: false, check: null, runUrl };
    }

    if (workflowRun.conclusion === 'cancelled' || workflowRun.conclusion === 'skipped') {
      return { failed: false, check: null, runUrl };
    }

    const { data: jobsData } = await github.rest.actions.listJobsForWorkflowRun({
      owner,
      repo,
      run_id: workflowRun.id,
      per_page: 100,
    });

    const jobs = jobsData.jobs || [];
    getLogger().log(`Found ${jobs.length} jobs for workflow run ${workflowRun.id}`);

    // Lint job is its own job, named "Lint" in zxc-build-library.yaml.
    const lintJob = jobs.find(job => job.name && job.name.toLowerCase().includes('lint'));
    if (lintJob && lintJob.conclusion === 'failure') {
      getLogger().log('Classified failure as: lint');
      return { failed: true, check: 'lint', runUrl };
    }

    // Build job runs CMake then CTest. Distinguish build vs tests by which
    // step failed.
    const buildJob = jobs.find(job =>
      job.name && (
        job.name.toLowerCase().includes('build') ||
        job.name.toLowerCase().includes('code')
      )
    );

    if (buildJob && buildJob.conclusion === 'failure') {
      const buildSteps = buildJob.steps || [];

      const buildStepFailed = buildSteps.some(step =>
        step.name &&
        (step.name.toLowerCase().includes('cmake build') ||
         step.name.toLowerCase().includes('build project')) &&
        step.conclusion === 'failure'
      );

      if (buildStepFailed) {
        getLogger().log('Classified failure as: build');
        return { failed: true, check: 'build', runUrl };
      }

      const testStepFailed = buildSteps.some(step =>
        step.name &&
        (step.name.toLowerCase().includes('ctest') ||
         step.name.toLowerCase().includes('test')) &&
        step.conclusion === 'failure'
      );

      if (testStepFailed) {
        getLogger().log('Classified failure as: tests');
        return { failed: true, check: 'tests', runUrl };
      }

      getLogger().log('Build job failed but step unclear, defaulting to: build');
      return { failed: true, check: 'build', runUrl };
    }

    getLogger().log('Could not classify failure type, returning generic failure');
    return { failed: true, check: null, runUrl };
  } catch (error) {
    getLogger().error(`Failed to classify workflow failure: ${error.message}`);
    return { failed: true, check: null, runUrl };
  }
}

/**
 * Checks if the prior dashboard comment already shows a CI failure. Used to
 * detect state transitions (no failure → failure) so we only post the
 * @mention notification once per failure cycle.
 *
 * @param {string|null} priorCommentBody - Body of the existing dashboard comment, or null.
 * @returns {boolean}
 */
function hadPriorCIFailure(priorCommentBody) {
  if (!priorCommentBody) return false;

  // Match either ":x: **CI Checks**" or "**CI Checks** ... :x:"
  const ciFailurePattern = /:x:.*?\*\*CI Checks\*\*|\*\*CI Checks\*\*.*?:x:/s;
  return ciFailurePattern.test(priorCommentBody);
}

module.exports = {
  PR_CHECKS_WORKFLOW_NAME,
  getLatestPRChecksRun,
  classifyWorkflowFailure,
  hadPriorCIFailure,
};
