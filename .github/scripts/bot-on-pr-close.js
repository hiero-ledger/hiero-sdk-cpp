// SPDX-License-Identifier: Apache-2.0
//
// bot-on-pr-close.js
//
// Handles pull_request close events and triggers post-merge automation.
//
// Purpose:
//   When a PR is closed (and merged), trigger issue recommendation
//   to guide contributors to their next task.
//
// Security:
//   - Only runs on merged PRs
//   - Ignores bot users to prevent loops

const {
    MAINTAINER_TEAM,
    createLogger,
    buildBotContext,
    fetchClosingIssueNumbers,
    fetchIssue,
    fetchLatestMilestone,
    getLabelsByPrefix,
    postComment,
    removeLabel,
    resolveLinkedIssue,
    setMilestone,
} = require('./helpers');
const { handleRecommendIssues } = require('./bot/bot-recommend-issues');

let logger = createLogger('on-pr-close');

function buildMissingMilestoneComment() {
    return [
        `${MAINTAINER_TEAM} a PR was merged, but there are no open milestones available.`,
        '',
        'Please create an open milestone so this merged work can be assigned appropriately.',
    ].join('\n');
}

async function removeStatusLabels(botContext, item) {
    const statusLabels = getLabelsByPrefix(item, 'status:');
    for (const label of statusLabels) {
        const result = await removeLabel(botContext, label);
        if (!result.success) {
            logger.error(`Failed to remove status label "${label}" from #${botContext.number}: ${result.error}`);
        }
    }
}

async function applyMergeMilestoneAutomation(botContext) {
    await removeStatusLabels(botContext, botContext.pr);

    const milestone = await fetchLatestMilestone(botContext);
    if (!milestone) {
        await postComment(botContext, buildMissingMilestoneComment());
        return false;
    }

    const issueNumbers = await fetchClosingIssueNumbers(botContext);
    if (issueNumbers.length === 0) {
        await setMilestone(botContext, botContext.number, milestone.number);
        return true;
    }

    for (const issueNumber of issueNumbers) {
        const issue = await fetchIssue(botContext, issueNumber);
        const issueContext = { ...botContext, number: issueNumber, issue };
        await removeStatusLabels(issueContext, issue);
        await setMilestone(botContext, issueNumber, milestone.number);
    }

    return true;
}

// =============================================================================
// ENTRY POINT
// =============================================================================

/**
 * Entry point for PR close event.
 *
 * Validates:
 *   - PR is merged
 *   - Actor is not a bot
 *
 * Then triggers issue recommendation flow.
 */
module.exports = async ({ github, context }) => {
    try {
        const botContext = buildBotContext({ github, context });

        const pr = botContext.pr;

        if (!pr) {
            logger.log('Exit: no pull_request payload');
            return;
        }

        if (!pr.merged) {
            logger.log('Exit: PR closed but not merged');
            return;
        }

        const milestoneReady = await applyMergeMilestoneAutomation(botContext);
        if (!milestoneReady) {
            logger.log('Exit: no open milestone available');
            return;
        }

        const username = pr.user?.login;

        if (!username) {
            logger.log('Exit: missing PR author');
            return;
        }

        if (pr.user?.type === 'Bot') {
            logger.log('Exit: PR authored by bot');
            return;
        }

        logger.log('Recommendation context:', {
            username,
            prNumber: pr.number,
        });

        const linkedIssue = await resolveLinkedIssue(botContext);

        if (!linkedIssue) {
            logger.log('Skipping recommendation (no resolvable issue)', {
                prNumber: pr.number,
                username,
            });
            return;
        }

        await handleRecommendIssues({
            ...botContext,
            issue: linkedIssue,
            number: pr.number,
            sender: pr.user,
        });

    } catch (error) {
        logger.error('Error:', {
            message: error.message,
            status: error.status,
            pr: context.payload.pull_request?.number,
        });
        throw error;
    }
};
