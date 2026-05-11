// SPDX-License-Identifier: Apache-2.0
//
// bot-on-ci-result.js
//
// Runs when the PR Checks workflow completes. Updates the PR Helper Bot
// dashboard with the CI result, flips failed CI back to needs-revision, and
// pings the PR author once when CI transitions into failure.

const {
  MARKER,
  buildCIFailureNotificationComment,
  classifyWorkflowRunCI,
  createLogger,
  dashboardHasCIFailure,
  getBotComment,
  postComment,
  resolveWorkflowRunPR,
  runAllChecksAndComment,
  swapStatusLabel,
} = require('./helpers');

const logger = createLogger('on-ci-result');

function buildWorkflowRunBotContext({ github, context, pr }) {
  return {
    github,
    owner: context.repo.owner,
    repo: context.repo.repo,
    eventType: context.eventName,
    number: pr.number,
    pr,
  };
}

module.exports = async ({ github, context }) => {
  try {
    const workflowRun = context.payload.workflow_run;
    if (!workflowRun) {
      logger.log('Exit: missing workflow_run payload');
      return;
    }

    if (workflowRun.conclusion === 'cancelled') {
      logger.log('Exit: workflow run was cancelled');
      return;
    }

    const baseContext = {
      github,
      owner: context.repo.owner,
      repo: context.repo.repo,
    };
    const pr = await resolveWorkflowRunPR(baseContext, workflowRun);
    if (!pr) {
      logger.log('Exit: no pull request resolved for workflow_run');
      return;
    }

    if (pr.draft) {
      logger.log('Exit: draft PR');
      return;
    }

    const botContext = buildWorkflowRunBotContext({ github, context, pr });
    const existingDashboard = await getBotComment(botContext, MARKER);
    const hadCIFailure = dashboardHasCIFailure(existingDashboard?.body);
    const ci = await classifyWorkflowRunCI(botContext, workflowRun);

    await runAllChecksAndComment(botContext, { ci });

    if (!ci.failed) {
      logger.log('CI did not fail; leaving status label unchanged');
      return;
    }

    const labelResult = await swapStatusLabel(botContext, false, { force: true });
    if (!labelResult.success) {
      logger.error(`Failed to force-swap status label after CI failure: ${labelResult.errorDetails}`);
    }

    if (!hadCIFailure) {
      const prAuthor = pr.user?.login;
      const body = buildCIFailureNotificationComment(prAuthor, ci.runUrl);
      await postComment(botContext, body);
      logger.log('Posted CI failure notification comment');
    }

    logger.log('On-CI-result bot completed');
  } catch (error) {
    logger.error('Error:', {
      message: error.message,
      runId: context?.payload?.workflow_run?.id,
    });
    throw error;
  }
};
