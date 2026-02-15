// SPDX-License-Identifier: Apache-2.0
//
// bot-on-commit.js
//
// Runs on every new commit pushed to a PR (synchronize) and optionally as the
// first step of PR automation (opened, reopened, ready_for_review).
// Performs three independent checks: DCO sign-off, GPG signatures, merge conflicts.
// Each check logs results, posts a PR comment on failure, and writes its result to GITHUB_OUTPUT.

const {
  createLogger,
  buildBotContext,
  writeGithubOutput,
  postComment,
} = require('./helpers');

const logger = createLogger('on-commit');

/**
 * Converts a boolean pass/fail to the string value written to GITHUB_OUTPUT.
 * @param {boolean} passed - Whether the check passed.
 * @returns {'success'|'failure'} - Value for the workflow output.
 */
function toOutputValue(passed) {
  return passed ? 'success' : 'failure';
}

/**
 * Fetches all commits for a pull request via the GitHub API (paginated).
 * @param {{ github: object, owner: string, repo: string, number: number }} botContext - Bot context from buildBotContext.
 * @returns {Promise<Array<{ sha: string, commit: { message?: string, verification?: { verified?: boolean } } }>>} - Commit objects from the API.
 */
async function fetchPRCommits(botContext) {
  const commits = [];
  let page = 1;
  const perPage = 100;

  while (true) {
    const response = await botContext.github.rest.pulls.listCommits({
      owner: botContext.owner,
      repo: botContext.repo,
      pull_number: botContext.number,
      per_page: perPage,
      page,
    });

    commits.push(...response.data);

    if (response.data.length < perPage) {
      break;
    }
    page++;
  }

  logger.log(`Fetched ${commits.length} commits for PR #${botContext.number}`);
  return commits;
}

/**
 * Checks whether a commit message contains a valid DCO sign-off line.
 * Expects a line matching "Signed-off-by: Name <email>" (case-insensitive).
 * @param {string} message - The commit message (e.g. from commit.commit.message).
 * @returns {boolean} - True if a valid sign-off line is present.
 */
function hasDCOSignoff(message) {
  if (!message) return false;
  // DCO sign-off line format: "Signed-off-by: Name <email>"
  return /^Signed-off-by:\s+.+\s+<.+>/mi.test(message);
}

/**
 * Checks whether a commit has a verified GPG signature according to the GitHub API.
 * @param {{ commit?: { verification?: { verified?: boolean } } }} commit - Commit object from pulls.listCommits (commit.commit.verification.verified).
 * @returns {boolean} - True if the commit is signed and the signature is verified.
 */
function hasVerifiedGPGSignature(commit) {
  return commit?.commit?.verification?.verified === true;
}

/**
 * Runs the DCO (Developer Certificate of Origin) check on all commits.
 * Logs the pass count, posts a PR comment listing failing commits if any, and returns the result for GITHUB_OUTPUT.
 * @param {{ github: object, owner: string, repo: string, number: number }} botContext - Bot context (for API calls and postComment).
 * @param {Array<{ sha?: string, commit?: { message?: string } }>} commits - Commits from fetchPRCommits.
 * @returns {Promise<'success'|'failure'>} - Value to write as the dco output.
 */
async function runDCOCheck(botContext, commits) {
  const dcoFailures = [];
  for (const c of commits) {
    const message = c.commit?.message || '';
    const shortSha = (c.sha || '').slice(0, 7);
    const firstLine = message.split('\n')[0] || '(no message)';
    if (!hasDCOSignoff(message)) {
      dcoFailures.push({ sha: shortSha, message: firstLine });
    }
  }

  const total = commits.length;
  const dcoPass = dcoFailures.length === 0;
  logger.log(`DCO check: ${total - dcoFailures.length}/${total} passed`);
  if (!dcoPass) {
    const lines = [
      'Hi, this is **DCO Bot**! 👋',
      '',
      'I noticed some commits on this PR are missing the required DCO sign-off. Here are the ones that need attention:',
      '',
      ...dcoFailures.map(({ sha, message }) => `- \`${sha}\` ${message}`),
      '',
      'Please add a line `Signed-off-by: Your Name <your.email@example.com>` to each commit (e.g. `git commit -s`). For more info, see the [Signing Guide](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/signing.md).',
    ];
    await postComment(botContext, lines.join('\n'));
  }
  return toOutputValue(dcoPass);
}

/**
 * Runs the GPG signature check on all commits.
 * Logs the pass count, posts a PR comment listing commits without verified GPG if any, and returns the result for GITHUB_OUTPUT.
 * @param {{ github: object, owner: string, repo: string, number: number }} botContext - Bot context (for API calls and postComment).
 * @param {Array<{ sha?: string, commit?: { message?: string, verification?: { verified?: boolean } } }>} commits - Commits from fetchPRCommits.
 * @returns {Promise<'success'|'failure'>} - Value to write as the gpg output.
 */
async function runGPGCheck(botContext, commits) {
  const gpgFailures = [];
  for (const c of commits) {
    const shortSha = (c.sha || '').slice(0, 7);
    const message = c.commit?.message || '';
    const firstLine = message.split('\n')[0] || '(no message)';
    if (!hasVerifiedGPGSignature(c)) {
      gpgFailures.push({ sha: shortSha, message: firstLine });
    }
  }

  const total = commits.length;
  const gpgPass = gpgFailures.length === 0;
  logger.log(`GPG check: ${total - gpgFailures.length}/${total} passed`);
  if (!gpgPass) {
    const lines = [
      'Hi, this is **GPG Bot**! 👋',
      '',
      'I noticed some commits on this PR don’t have a verified GPG signature. Here are the ones that need attention:',
      '',
      ...gpgFailures.map(({ sha, message }) => `- \`${sha}\` ${message}`),
      '',
      'Please sign your commits with GPG (e.g. `git commit -S`). For more info, see the [Signing Guide](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/signing.md).',
    ];
    await postComment(botContext, lines.join('\n'));
  }
  return toOutputValue(gpgPass);
}

/**
 * Checks whether the PR has merge conflicts with the base branch.
 * Uses pulls.get mergeable; retries up to 5 times with 2s delay when mergeable is null. Posts a PR comment if conflicts exist.
 * @param {{ github: object, owner: string, repo: string, number: number }} botContext - Bot context (for API calls and postComment).
 * @returns {Promise<'success'|'failure'>} - 'success' when no conflicts (or mergeable never resolved), 'failure' when mergeable is false.
 */
async function runMergeConflictCheck(botContext) {
  const maxAttempts = 5;
  const delayMs = 2000;

  let conflicts = false;
  let mergeableResolved = false;
  for (let attempt = 1; attempt <= maxAttempts; attempt++) {
    const { data: pr } = await botContext.github.rest.pulls.get({
      owner: botContext.owner,
      repo: botContext.repo,
      pull_number: botContext.number,
    });

    if (pr.mergeable !== null) {
      logger.log(`Merge conflict check: mergeable=${pr.mergeable}, state=${pr.mergeable_state}`);
      conflicts = !pr.mergeable;
      mergeableResolved = true;
      break;
    }

    if (attempt < maxAttempts) {
      logger.log(`Mergeable state not ready, waiting ${delayMs}ms (attempt ${attempt}/${maxAttempts})`);
      await new Promise(resolve => setTimeout(resolve, delayMs));
    }
  }

  if (!mergeableResolved) {
    logger.log('Merge conflict check: mergeable never resolved after retries, assuming no conflicts');
  }
  logger.log(`Merge conflict check: ${conflicts ? 'has conflicts' : 'no conflicts'}`);
  if (conflicts) {
    const body = [
      'Hi, this is **Merge Conflict Bot**! 👋',
      '',
      'I noticed this PR has merge conflicts with the base branch. Please update your branch (e.g. rebase or merge from base) and push. See the [Merge conflicts guide](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/merge-conflicts.md) for help.',
    ].join('\n');
    await postComment(botContext, body);
  }
  return toOutputValue(!conflicts);
}

/**
 * Main handler invoked by the workflow. Runs DCO, GPG, and merge-conflict checks, then writes outputs once.
 * Expects pull_request or pull_request_target (opened, reopened, ready_for_review, or synchronize).
 * On success: writes dco, gpg, merge_conflict to GITHUB_OUTPUT once. On error: logs, writes failure defaults, rethrows.
 * @param {{ github: object, context: object }} args - Arguments from actions/github-script (github API, context with event payload).
 * @returns {Promise<void>}
 */
module.exports = async ({ github, context }) => {
  try {
    const botContext = buildBotContext({ github, context });

    const commits = await fetchPRCommits(botContext);

    const dco = await runDCOCheck(botContext, commits);
    const gpg = await runGPGCheck(botContext, commits);
    const merge_conflict = await runMergeConflictCheck(botContext);

    writeGithubOutput({ dco, gpg, merge_conflict });
    logger.log('On-commit bot completed');
  } catch (error) {
    logger.error('Error:', {
      message: error.message,
      number: context?.payload?.pull_request?.number,
    });
    writeGithubOutput({ dco: 'failure', gpg: 'failure', merge_conflict: 'success' });
    throw error;
  }
};

// Export for tests
module.exports.hasDCOSignoff = hasDCOSignoff;
module.exports.hasVerifiedGPGSignature = hasVerifiedGPGSignature;
module.exports.fetchPRCommits = fetchPRCommits;
module.exports.runDCOCheck = runDCOCheck;
module.exports.runGPGCheck = runGPGCheck;
module.exports.runMergeConflictCheck = runMergeConflictCheck;
