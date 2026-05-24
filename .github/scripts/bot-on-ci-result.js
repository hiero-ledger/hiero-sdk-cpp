// SPDX-License-Identifier: Apache-2.0
//
// bot-on-ci-result.js
//
// Runs on a cron schedule. For each open PR:
//   1. Look up the latest "PR Checks" workflow run by head SHA.
//   2. If completed, classify the result (lint / build / tests / pass).
//   3. Re-render the dashboard comment with the CI section.
//   4. On failure, force-swap the status label to needs-revision and post a
//      one-time @mention ping if the prior dashboard didn't already show a
//      failure (transition into failure).
//
// This driver runs entirely in the base-repo context, so fork PRs are handled
// without any input from the fork — replacing the previous workflow_run-based
// driver that GitHub flagged as an unsafe trigger pattern.

const {
  createLogger,
  swapStatusLabel,
  runAllChecksAndComment,
  getBotComment,
  postComment,
  buildCIFailureNotificationComment,
  getLatestPRChecksRun,
  classifyWorkflowFailure,
  hadPriorCIFailure,
  MARKER,
} = require('./helpers');

const logger = createLogger('on-ci-result');

/**
 * Lists every open PR in the repository, paginating through results.
 */
async function listAllOpenPRs(github, owner, repo) {
  const prs = [];
  let page = 1;
  const perPage = 100;

  while (true) {
    const { data } = await github.rest.pulls.list({
      owner,
      repo,
      state: 'open',
      per_page: perPage,
      page,
    });

    prs.push(...data);

    if (data.length < perPage) break;
    page++;
  }

  return prs;
}

/**
 * Reconciles a single PR's CI dashboard / label / ping state.
 * Returns one of: 'no-run', 'no-change', 'success', 'failure'.
 */
async function reconcilePR({ github, owner, repo, pr }) {
  const prNumber = pr.number;
  const headSha = pr.head?.sha;

  if (!headSha) {
    logger.log(`PR #${prNumber}: missing head SHA, skipping`);
    return 'no-run';
  }

  const workflowRun = await getLatestPRChecksRun(github, owner, repo, headSha);
  if (!workflowRun) {
    logger.log(`PR #${prNumber}: no completed PR Checks run for ${headSha}, skipping`);
    return 'no-run';
  }

  const ciResult = await classifyWorkflowFailure(github, workflowRun, owner, repo);
  logger.log(`PR #${prNumber}: failed=${ciResult.failed}, check=${ciResult.check}`);

  const botContext = {
    github,
    owner,
    repo,
    number: prNumber,
    pr,
    eventType: 'schedule',
  };

  // Read prior dashboard before re-rendering, so we can detect a clean→failure
  // transition (only point where we ping the author).
  const priorComment = await getBotComment(botContext, MARKER);
  const hadPriorFailure = hadPriorCIFailure(priorComment?.body);

  const ci = ciResult.failed
    ? { passed: false, check: ciResult.check, runUrl: ciResult.runUrl }
    : { passed: true };

  await runAllChecksAndComment(botContext, { ci });

  if (!ciResult.failed) {
    // On CI success the success-path label management is owned by
    // bot-on-pr-update; we leave the label untouched here.
    return 'success';
  }

  // Force-swap to needs-revision on any CI failure, even if no prior label
  // was present.
  const swapResult = await swapStatusLabel(botContext, false, { force: true });
  if (!swapResult.success) {
    logger.error(`PR #${prNumber}: failed to swap status label: ${swapResult.errorDetails}`);
  }

  // Only ping on the transition into failure — repeat failures stay quiet.
  if (!hadPriorFailure) {
    const prAuthor = pr.user?.login;
    if (prAuthor) {
      const body = buildCIFailureNotificationComment(prAuthor, ciResult.runUrl);
      const postResult = await postComment(botContext, body);
      if (!postResult.success) {
        logger.error(`PR #${prNumber}: failed to post notification: ${postResult.error}`);
      } else {
        logger.log(`PR #${prNumber}: posted CI failure notification`);
      }
    }
  } else {
    logger.log(`PR #${prNumber}: prior CI failure detected, skipping notification`);
  }

  return 'failure';
}

module.exports = async ({ github, context }) => {
  try {
    const owner = context.repo.owner;
    const repo = context.repo.repo;

    const allOpenPRs = await listAllOpenPRs(github, owner, repo);
    logger.log(`Found ${allOpenPRs.length} open PR(s)`);

    const counts = { processed: 0, noRun: 0, success: 0, failure: 0, skipped: 0, errors: 0 };

    for (const pr of allOpenPRs) {
      // Skip drafts — they aren't ready for CI feedback yet.
      if (pr.draft) {
        counts.skipped++;
        continue;
      }
      // Skip bot-authored PRs (matches the on-pr-update bot behaviour).
      if (pr.user?.type === 'Bot') {
        counts.skipped++;
        continue;
      }

      try {
        const outcome = await reconcilePR({ github, owner, repo, pr });
        counts.processed++;
        if (outcome === 'no-run') counts.noRun++;
        else if (outcome === 'success') counts.success++;
        else if (outcome === 'failure') counts.failure++;
      } catch (err) {
        counts.errors++;
        logger.error(`PR #${pr.number}: reconcile failed: ${err.message}`);
        // Continue with the next PR; one bad PR shouldn't kill the tick.
      }
    }

    logger.log(
      `On-CI-result tick complete: processed=${counts.processed}, ` +
      `noRun=${counts.noRun}, success=${counts.success}, failure=${counts.failure}, ` +
      `skipped=${counts.skipped}, errors=${counts.errors}`
    );

    return counts;
  } catch (error) {
    logger.error('Error:', { message: error.message });
    throw error;
  }
};
