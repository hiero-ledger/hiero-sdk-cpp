// SPDX-License-Identifier: Apache-2.0
//
// bot-pr-automation.js
//
// Automates PR workflow when a pull request is opened.
// Performs the following:
//   1. Auto-assigns the PR author
//   2. Verifies DCO sign-off on all commits
//   3. Verifies GPG signatures on all commits
//   4. Checks for merge conflicts
//   5. Applies appropriate status label based on results

const {
  MAINTAINER_TEAM,
  LABELS,
  createLogger,
  addLabels,
  addAssignees,
  postComment,
} = require('./bot-helpers');

// Create logger for this bot
const logger = createLogger('pr-bot');

/**
 * Fetches all commits for a pull request.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} pullNumber - The pull request number.
 * @returns {Promise<Array>} - Array of commit objects.
 */
async function fetchPRCommits(github, owner, repo, pullNumber) {
  const commits = [];
  let page = 1;
  const perPage = 100;

  while (true) {
    const response = await github.rest.pulls.listCommits({
      owner,
      repo,
      pull_number: pullNumber,
      per_page: perPage,
      page,
    });

    commits.push(...response.data);

    if (response.data.length < perPage) {
      break;
    }
    page++;
  }

  logger.log(`Fetched ${commits.length} commits for PR #${pullNumber}`);
  return commits;
}

/**
 * Checks if a commit message has DCO sign-off.
 * @param {string} message - The commit message.
 * @returns {boolean} - True if DCO sign-off is present.
 */
function hasDCOSignoff(message) {
  if (!message) return false;
  // DCO sign-off line format: "Signed-off-by: Name <email>"
  return /^Signed-off-by:\s+.+\s+<.+>/mi.test(message);
}

/**
 * Checks if a commit has a verified GPG signature.
 * @param {object} commit - The commit object from GitHub API.
 * @returns {boolean} - True if GPG signature is verified.
 */
function hasVerifiedGPGSignature(commit) {
  return commit?.commit?.verification?.verified === true;
}

/**
 * Verifies all commits for DCO sign-off.
 * @param {Array} commits - Array of commit objects.
 * @returns {boolean} - True if all commits have DCO sign-off.
 */
function verifyDCOSignoffs(commits) {
  let passed = 0;

  for (const commit of commits) {
    const message = commit.commit?.message || '';
    if (hasDCOSignoff(message)) {
      passed++;
    }
  }

  logger.log(`DCO check: ${passed}/${commits.length} passed`);
  return passed === commits.length;
}

/**
 * Verifies all commits for GPG signatures.
 * @param {Array} commits - Array of commit objects.
 * @returns {boolean} - True if all commits have verified GPG signatures.
 */
function verifyGPGSignatures(commits) {
  let passed = 0;

  for (const commit of commits) {
    if (hasVerifiedGPGSignature(commit)) {
      passed++;
    }
  }

  logger.log(`GPG check: ${passed}/${commits.length} passed`);
  return passed === commits.length;
}

/**
 * Checks if a PR has merge conflicts.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} pullNumber - The pull request number.
 * @returns {Promise<boolean>} - True if PR has no merge conflicts.
 */
async function checkMergeConflicts(github, owner, repo, pullNumber) {
  // GitHub may need time to compute mergeable state
  // We'll try a few times with delays (5 attempts x 2s = 10s max)
  const maxAttempts = 5;
  const delayMs = 2000;

  for (let attempt = 1; attempt <= maxAttempts; attempt++) {
    const { data: pr } = await github.rest.pulls.get({
      owner,
      repo,
      pull_number: pullNumber,
    });

    // mergeable can be true, false, or null (still computing)
    if (pr.mergeable !== null) {
      logger.log(`Merge conflict check: mergeable=${pr.mergeable}, state=${pr.mergeable_state}`);
      return pr.mergeable === true;
    }

    if (attempt < maxAttempts) {
      logger.log(`Mergeable state not ready, waiting ${delayMs}ms (attempt ${attempt}/${maxAttempts})`);
      await new Promise(resolve => setTimeout(resolve, delayMs));
    }
  }

  // If we couldn't determine, assume no conflicts to avoid blocking
  logger.log('Could not determine mergeable state, assuming no conflicts');
  return true;
}

/**
 * Posts a comment on a PR when label assignment fails.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} pullNumber - The pull request number.
 * @param {string} labelName - The label that could not be added.
 * @param {string} errorMessage - The error message.
 */
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

/**
 * Updates labels on a PR based on check results.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} pullNumber - The pull request number.
 * @param {boolean} allPassed - Whether all checks passed.
 */
async function updateLabels(github, owner, repo, pullNumber, allPassed) {
  const labelToAdd = allPassed ? LABELS.NEEDS_REVIEW : LABELS.NEEDS_REVISION;

  const result = await addLabels(github, owner, repo, pullNumber, [labelToAdd], logger);

  if (!result.success) {
    await postLabelFailureComment(github, owner, repo, pullNumber, labelToAdd, result.error);
  }
}

/**
 * Auto-assigns the PR author to the pull request.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - The repository owner.
 * @param {string} repo - The repository name.
 * @param {number} pullNumber - The pull request number.
 * @param {string} author - The PR author's username.
 * @param {Array} currentAssignees - Current assignees on the PR.
 */
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
 * Main handler for PR automation.
 *
 * Flow:
 * 1. Extract PR information from the webhook payload
 * 2. Auto-assign the PR author
 * 3. Fetch all commits on the PR
 * 4. Verify DCO sign-off on all commits
 * 5. Verify GPG signatures on all commits
 * 6. Check for merge conflicts
 * 7. Update labels based on results
 */
module.exports = async ({ github, context }) => {
  try {
    const { pull_request: pr } = context.payload;
    const { owner, repo } = context.repo;

    logger.log('Payload snapshot:', {
      pullNumber: pr?.number,
      author: pr?.user?.login,
      action: context.payload.action,
      baseBranch: pr?.base?.ref,
      headBranch: pr?.head?.ref,
    });

    // =========================================================================
    // VALIDATION: Ensure payload has required data
    // =========================================================================

    if (!pr?.number) {
      logger.log('Exit: missing pull request number');
      return;
    }

    if (!pr?.user?.login) {
      logger.log('Exit: missing PR author');
      return;
    }

    const pullNumber = pr.number;
    const prAuthor = pr.user.login;
    const currentAssignees = pr.assignees || [];

    logger.log(`Processing PR #${pullNumber} by ${prAuthor}`);

    // =========================================================================
    // STEP 1: Auto-assign PR author
    // =========================================================================

    await autoAssignAuthor(github, owner, repo, pullNumber, prAuthor, currentAssignees);

    // =========================================================================
    // STEP 2: Fetch all commits
    // =========================================================================

    const commits = await fetchPRCommits(github, owner, repo, pullNumber);

    if (commits.length === 0) {
      logger.log('Exit: no commits found on PR');
      return;
    }

    // =========================================================================
    // STEP 3: Verify DCO sign-off
    // =========================================================================

    const dcoPass = verifyDCOSignoffs(commits);

    // =========================================================================
    // STEP 4: Verify GPG signatures
    // =========================================================================

    const gpgPass = verifyGPGSignatures(commits);

    // =========================================================================
    // STEP 5: Check for merge conflicts
    // =========================================================================

    const noConflicts = await checkMergeConflicts(github, owner, repo, pullNumber);

    // =========================================================================
    // STEP 6: Determine overall status and update labels
    // =========================================================================

    const allPassed = dcoPass && gpgPass && noConflicts;

    logger.log('Check results:', {
      dcoPass,
      gpgPass,
      noConflicts,
      allPassed,
    });

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

// Export helper functions for testing
module.exports.hasDCOSignoff = hasDCOSignoff;
module.exports.hasVerifiedGPGSignature = hasVerifiedGPGSignature;
module.exports.verifyDCOSignoffs = verifyDCOSignoffs;
module.exports.verifyGPGSignatures = verifyGPGSignatures;
module.exports.LABELS = LABELS;
