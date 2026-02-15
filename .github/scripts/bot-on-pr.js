// SPDX-License-Identifier: Apache-2.0
//
// bot-on-pr.js
//
// Runs after bot-on-commit.js in the PR Automation workflow (opened, reopened, ready_for_review).
// Reads DCO_PASSED, GPG_PASSED, and MERGE_CONFLICT from the previous step's outputs (via env). Then:
//   1. Auto-assigns the PR author
//   2. Applies "needs review" only when merge clear and DCO & GPG passed; else "needs revision"
//

const {
  MAINTAINER_TEAM,
  LABELS,
  createLogger,
  buildBotContext,
  addLabels,
  addAssignees,
  postComment,
  requireSafeUsername,
} = require('./helpers');

const logger = createLogger('on-pr');

/**
 * Posts a comment on the PR when adding a label fails (e.g. label missing in repo).
 * Tags MAINTAINER_TEAM and asks for manual addition.
 * @param {{ github: object, owner: string, repo: string, number: number }} botContext - Bot context from buildBotContext.
 * @param {string} labelName - The label that could not be added.
 * @param {string} errorMessage - The error message from addLabels.
 * @returns {Promise<void>}
 */
async function postLabelFailureComment(botContext, labelName, errorMessage) {
  const comment = [
    `⚠️ **PR Automation Bot Error**`,
    '',
    `${MAINTAINER_TEAM} — I was unable to add the \`${labelName}\` label to this PR.`,
    '',
    `**Error:** ${errorMessage}`,
    '',
    `Please add the label manually or check that it exists in the repository.`,
  ].join('\n');

  await postComment(botContext, comment);
}

/**
 * Assigns the PR author to the PR if not already assigned.
 * No-op when author is missing or already in assignees.
 * @param {{ github: object, owner: string, repo: string, number: number, pr?: { user?: { login?: string }, assignees?: Array<{ login?: string }> } }} botContext - Bot context (must include pr from buildBotContext).
 * @returns {Promise<void>}
 */
async function autoAssignAuthor(botContext) {
  const prAuthor = botContext.pr?.user?.login;

  if (!prAuthor) {
    logger.log('Exit: missing pull request author');
    return;
  }
  try {
    requireSafeUsername(prAuthor, 'pr.author');
  } catch (err) {
    logger.log('Exit: invalid pr.author', err.message);
    return;
  }

  logger.log(`Processing PR #${botContext.number} by ${prAuthor}`);

  const currentAssignees = botContext.pr?.assignees || [];
  const isAlreadyAssigned = currentAssignees.some(
    (a) => (a?.login || '').toLowerCase() === prAuthor.toLowerCase()
  );
  if (isAlreadyAssigned) {
    logger.log(`Author ${prAuthor} is already assigned`);
    return;
  }
  await addAssignees(botContext, [prAuthor]);
}

/**
 * Main handler invoked by the workflow. Assigns the PR author and sets NEEDS_REVIEW or NEEDS_REVISION from env.
 * Expects pull_request or pull_request_target; reads DCO_PASSED, GPG_PASSED, MERGE_CONFLICT from the previous step's outputs.
 * Adds NEEDS_REVIEW only when all three are 'success'; otherwise adds NEEDS_REVISION.
 * @param {{ github: object, context: object }} args - Arguments from actions/github-script (github API, context with event payload).
 * @returns {Promise<void>}
 */
module.exports = async ({ github, context }) => {
  try {
    const botContext = buildBotContext({ github, context });

    await autoAssignAuthor(botContext);

    const dcoPassed = process.env.DCO_PASSED === 'success';
    const gpgPassed = process.env.GPG_PASSED === 'success';
    const noConflicts = process.env.MERGE_CONFLICT === 'success';

    logger.log('DCO_PASSED:', process.env.DCO_PASSED, '→', dcoPassed);
    logger.log('GPG_PASSED:', process.env.GPG_PASSED, '→', gpgPassed);
    logger.log('MERGE_CONFLICT:', process.env.MERGE_CONFLICT, '→ noConflicts:', noConflicts);

    const labelToAdd = dcoPassed && gpgPassed && noConflicts ? LABELS.NEEDS_REVIEW : LABELS.NEEDS_REVISION;
    const result = await addLabels(botContext, [labelToAdd]);
    if (!result.success) {
      await postLabelFailureComment(botContext, labelToAdd, result.error);
    }

    logger.log('On-PR bot completed');
  } catch (error) {
    logger.error('Error:', {
      message: error.message,
      number: context?.payload?.pull_request?.number,
    });
    throw error;
  }
};

module.exports.LABELS = LABELS;
