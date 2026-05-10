// SPDX-License-Identifier: Apache-2.0
//
// bot-on-pr-converted-to-draft.js
//
// Runs when a PR is converted to draft.
// Removes review-related status labels.

const {
  createLogger,
  buildBotContext,
} = require('./helpers');

const { LABELS } = require('./helpers/constants');

const logger = createLogger('on-pr-converted-to-draft');

module.exports = async ({ github, context }) => {
  try {
    const botContext = buildBotContext({ github, context });

    if (botContext.pr?.user?.type === 'Bot') {
      logger.log('Skipping bot-authored PR');
      return;
    }

    const labelsToRemove = [
      LABELS.NEEDS_REVIEW,
      LABELS.NEEDS_REVISION,
    ];

    const existingLabels = botContext.pr.labels.map((label) => label.name);

    for (const label of labelsToRemove) {
      if (existingLabels.includes(label)) {
        await github.rest.issues.removeLabel({
          owner: context.repo.owner,
          repo: context.repo.repo,
          issue_number: botContext.pr.number,
          name: label,
        });

        logger.log(`Removed label: ${label}`);
      }
    }

    logger.log('On-PR-converted-to-draft bot completed');
  } catch (error) {
    logger.error('Error:', {
      message: error.message,
      number: context?.payload?.pull_request?.number,
    });

    throw error;
  }
};