// SPDX-License-Identifier: Apache-2.0
//
// bot-pr-automation.js
//
// Runs after the DCO & GPG job in the PR Automation workflow (opened, reopened, ready_for_review).
// Reads DCO_GPG_PASSED from the previous job's output (via env). Then:
//   1. Auto-assigns the PR author
//   2. Checks for merge conflicts
//   3. Applies "needs review" only when merge clear and DCO & GPG passed; else "needs revision"
//

const {
  MAINTAINER_TEAM,
  LABELS,
  createLogger,
  addLabels,
  addAssignees,
  postComment,
} = require('./bot-helpers');

const logger = createLogger('pr-bot');

async function checkMergeConflicts(github, owner, repo, pullNumber) {
  const maxAttempts = 5;
  const delayMs = 2000;

  for (let attempt = 1; attempt <= maxAttempts; attempt++) {
    const { data: pr } = await github.rest.pulls.get({
      owner,
      repo,
      pull_number: pullNumber,
    });

    if (pr.mergeable !== null) {
      logger.log(`Merge conflict check: mergeable=${pr.mergeable}, state=${pr.mergeable_state}`);
      return pr.mergeable === true;
    }

    if (attempt < maxAttempts) {
      logger.log(`Mergeable state not ready, waiting ${delayMs}ms (attempt ${attempt}/${maxAttempts})`);
      await new Promise(resolve => setTimeout(resolve, delayMs));
    }
  }

  logger.log('Could not determine mergeable state, assuming no conflicts');
  return true;
}

async function postLabelFailureComment(github, owner, repo, pullNumber, labelName, errorMessage) {
  const comment = [
    `⚠️ **PR Automation Bot Error**`,
    '',
    `${MAINTAINER_TEAM} — I was unable to add the \`${labelName}\` label to this PR.`,
    '',
    `**Error:** ${errorMessage}`,
    '',
    `Please add the label manually or check that it exists in the repository.`,
  ].join('\n');

  await postComment(github, owner, repo, pullNumber, comment, logger);
}

async function updateLabels(github, owner, repo, pullNumber, allPassed) {
  const labelToAdd = allPassed ? LABELS.NEEDS_REVIEW : LABELS.NEEDS_REVISION;
  const result = await addLabels(github, owner, repo, pullNumber, [labelToAdd], logger);
  if (!result.success) {
    await postLabelFailureComment(github, owner, repo, pullNumber, labelToAdd, result.error);
  }
}

async function autoAssignAuthor(github, owner, repo, pullNumber, author, currentAssignees) {
  const isAlreadyAssigned = currentAssignees.some(
    a => a.login.toLowerCase() === author.toLowerCase()
  );
  if (isAlreadyAssigned) {
    logger.log(`Author ${author} is already assigned`);
    return;
  }
  await addAssignees(github, owner, repo, pullNumber, [author], logger);
}

/**
 * Main handler. Expects pull_request event and DCO_GPG_PASSED env (from previous job output).
 */
module.exports = async ({ github, context }) => {
  try {
    if (context.eventName !== 'pull_request') {
      logger.log('Exit: expected pull_request event, got', context.eventName);
      return;
    }

    const pr = context.payload.pull_request;
    const { owner, repo } = context.repo;

    logger.log('Payload snapshot:', {
      pullNumber: pr?.number,
      author: pr?.user?.login,
      action: context.payload.action,
    });

    if (!pr?.number || !pr?.user?.login) {
      logger.log('Exit: missing pull request number or author');
      return;
    }

    const pullNumber = pr.number;
    const prAuthor = pr.user.login;
    const currentAssignees = pr.assignees || [];

    const dcoGpgPassedEnv = process.env.DCO_GPG_PASSED || '';
    const dcoGpgPass = dcoGpgPassedEnv === 'success';
    logger.log('DCO_GPG_PASSED:', dcoGpgPassedEnv, '→ dcoGpgPass:', dcoGpgPass);

    logger.log(`Processing PR #${pullNumber} by ${prAuthor}`);

    await autoAssignAuthor(github, owner, repo, pullNumber, prAuthor, currentAssignees);

    const noConflicts = await checkMergeConflicts(github, owner, repo, pullNumber);

    const allPassed = noConflicts && dcoGpgPass;

    logger.log('Check results:', { noConflicts, dcoGpgPass, allPassed });

    await updateLabels(github, owner, repo, pullNumber, allPassed);

    logger.log('PR automation completed successfully');
  } catch (error) {
    logger.error('Error:', {
      message: error.message,
      status: error.status,
      pullNumber: context.payload.pull_request?.number,
    });
    throw error;
  }
};

module.exports.LABELS = LABELS;
