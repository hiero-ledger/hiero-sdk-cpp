// SPDX-License-Identifier: Apache-2.0
//
// bot-on-pr-converted-to-draft.js
//
// Runs when a PR is converted to draft.
// Removes review-related status labels.

const {
  createLogger,
  buildBotContext,
  hasLabel,
  removeLabel,
  LABELS,
} = require('./helpers');

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

    for (const label of labelsToRemove) {
      if (!hasLabel(botContext.pr, label)) {
        continue;
      }

      const result = await removeLabel(botContext, label);

      if (!result.success) {
        logger.error(
          `Failed to remove '${label}' from #${botContext.number}: ${result.error}`
        );
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
