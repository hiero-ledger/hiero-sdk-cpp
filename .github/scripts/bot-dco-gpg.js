// SPDX-License-Identifier: Apache-2.0
//
// bot-dco-gpg.js
//
// DCO and GPG verification bot. Runs on every new commit in a PR (opened, synchronize).
// Fetches all commits, verifies DCO sign-off and GPG signatures, and reports via
// a GitHub check run and (on failure) a PR comment.

const {
  MAINTAINER_TEAM,
  createLogger,
  postComment,
  fetchPRCommits,
  verifyDCOSignoffs,
  verifyGPGSignatures,
  hasDCOSignoff,
  hasVerifiedGPGSignature,
} = require('./bot-helpers');

const CHECK_NAME = 'DCO & GPG';
const logger = createLogger('dco-gpg-bot');

const fs = require('fs');

/**
 * Writes the DCO/GPG result to GITHUB_OUTPUT when set (e.g. in PR Automation workflow),
 * so later steps can use steps.<id>.outputs.dco_gpg_passed.
 * @param {boolean} passed - Whether DCO & GPG checks passed.
 */
function writeResultToGithubOutput(passed) {
  const path = process.env.GITHUB_OUTPUT;
  if (!path) return;
  try {
    const value = passed ? 'success' : 'failure';
    fs.appendFileSync(path, `dco_gpg_passed=${value}\n`, 'utf8');
    logger.log(`Wrote dco_gpg_passed=${value} to GITHUB_OUTPUT`);
  } catch (err) {
    logger.error('Failed to write GITHUB_OUTPUT:', err.message);
  }
}

/**
 * Builds a list of commits that failed DCO or GPG for the check output and comment.
 * @param {Array} commits - Array of commit objects from GitHub API.
 * @returns {{ dcoFailures: Array<{sha: string, message: string}>, gpgFailures: Array<{sha: string, message: string}> }}
 */
function getFailures(commits) {
  const dcoFailures = [];
  const gpgFailures = [];

  for (const c of commits) {
    const message = c.commit?.message || '';
    const shortSha = (c.sha || '').slice(0, 7);
    const firstLine = message.split('\n')[0] || '(no message)';

    if (!hasDCOSignoff(message)) {
      dcoFailures.push({ sha: shortSha, message: firstLine });
    }
    if (!hasVerifiedGPGSignature(c)) {
      gpgFailures.push({ sha: shortSha, message: firstLine });
    }
  }

  return { dcoFailures, gpgFailures };
}

/**
 * Creates or updates a GitHub check run for DCO & GPG.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - Repository owner.
 * @param {string} repo - Repository name.
 * @param {string} headSha - Commit SHA to attach the check to.
 * @param {boolean} passed - Whether all checks passed.
 * @param {object} failureInfo - { dcoFailures, gpgFailures } from getFailures.
 * @param {number} totalCommits - Total number of commits.
 */
async function createCheckRun(github, owner, repo, headSha, passed, failureInfo, totalCommits) {
  const { dcoFailures, gpgFailures } = failureInfo;
  const conclusion = passed ? 'success' : 'failure';
  const title = passed
    ? `All ${totalCommits} commit(s) have DCO sign-off and verified GPG signatures`
    : 'Some commits are missing DCO sign-off or GPG verification';

  let summary = passed
    ? `Checked ${totalCommits} commit(s). All have required DCO sign-off and verified GPG signatures.`
    : `Checked ${totalCommits} commit(s). `;

  if (dcoFailures.length > 0) {
    summary += `**DCO:** ${dcoFailures.length} commit(s) missing \`Signed-off-by\`. `;
  }
  if (gpgFailures.length > 0) {
    summary += `**GPG:** ${gpgFailures.length} commit(s) without verified GPG signature.`;
  }

  let text = '';
  if (dcoFailures.length > 0) {
    text += '### Commits missing DCO sign-off\n';
    dcoFailures.forEach(({ sha, message }) => {
      text += `- \`${sha}\` ${message}\n`;
    });
    text += '\n';
  }
  if (gpgFailures.length > 0) {
    text += '### Commits without verified GPG signature\n';
    gpgFailures.forEach(({ sha, message }) => {
      text += `- \`${sha}\` ${message}\n`;
    });
  }
  if (text) {
    text += '\nSee the [Signing Guide](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/signing.md) for how to sign commits.';
  }

  try {
    await github.rest.checks.create({
      owner,
      repo,
      name: CHECK_NAME,
      head_sha: headSha,
      status: 'completed',
      conclusion,
      completed_at: new Date().toISOString(),
      output: {
        title,
        summary,
        text: text || undefined,
      },
    });
    logger.log(`Check run created: ${conclusion}`);
  } catch (err) {
    logger.error('Failed to create check run:', err.message);
    throw err;
  }
}

/**
 * Posts a comment on the PR when DCO or GPG verification fails.
 * @param {object} github - The GitHub API client.
 * @param {string} owner - Repository owner.
 * @param {string} repo - Repository name.
 * @param {number} pullNumber - PR number.
 * @param {object} failureInfo - { dcoFailures, gpgFailures } from getFailures.
 */
async function postFailureComment(github, owner, repo, pullNumber, failureInfo) {
  const { dcoFailures, gpgFailures } = failureInfo;
  if (dcoFailures.length === 0 && gpgFailures.length === 0) return;

  const lines = [
    '## DCO & GPG check — action needed',
    '',
    'Some commits on this PR are missing required sign-off or GPG verification.',
    '',
  ];

  if (dcoFailures.length > 0) {
    lines.push('### Commits missing DCO sign-off');
    dcoFailures.forEach(({ sha, message }) => {
      lines.push(`- \`${sha}\` ${message}`);
    });
    lines.push('');
  }

  if (gpgFailures.length > 0) {
    lines.push('### Commits without verified GPG signature');
    gpgFailures.forEach(({ sha, message }) => {
      lines.push(`- \`${sha}\` ${message}`);
    });
    lines.push('');
  }

  lines.push(
    'Please ensure every commit has a line `Signed-off-by: Your Name <your.email@example.com>` and is signed with GPG (`git commit -s -S`).',
    '',
    'See the [Signing Guide](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/signing.md) for details.'
  );

  await postComment(github, owner, repo, pullNumber, lines.join('\n'), logger);
}

/**
 * Main handler. Expects context from pull_request (opened or synchronize).
 */
module.exports = async ({ github, context }) => {
  try {
    const { pull_request: pr } = context.payload;
    const { owner, repo } = context.repo;

    logger.log('Payload snapshot:', {
      pullNumber: pr?.number,
      action: context.payload.action,
      headSha: pr?.head?.sha,
    });

    if (!pr?.number) {
      logger.log('Exit: missing pull request number');
      writeResultToGithubOutput(false);
      return;
    }

    const pullNumber = pr.number;
    const headSha = pr.head?.sha;
    if (!headSha) {
      logger.log('Exit: missing head SHA');
      writeResultToGithubOutput(false);
      return;
    }

    const commits = await fetchPRCommits(github, owner, repo, pullNumber, logger);
    if (commits.length === 0) {
      logger.log('Exit: no commits on PR');
      writeResultToGithubOutput(false);
      return;
    }

    const dcoPass = verifyDCOSignoffs(commits, logger);
    const gpgPass = verifyGPGSignatures(commits, logger);
    const passed = dcoPass && gpgPass;

    logger.log('Results:', { dcoPass, gpgPass, passed });

    const failureInfo = getFailures(commits);
    await createCheckRun(github, owner, repo, headSha, passed, failureInfo, commits.length);

    if (!passed) {
      await postFailureComment(github, owner, repo, pullNumber, failureInfo);
    }

    writeResultToGithubOutput(passed);
    logger.log('DCO/GPG bot completed');
  } catch (error) {
    logger.error('Error:', {
      message: error.message,
      pullNumber: context.payload.pull_request?.number,
    });
    throw error;
  }
};

// Export for tests
module.exports.getFailures = getFailures;
module.exports.hasDCOSignoff = hasDCOSignoff;
module.exports.hasVerifiedGPGSignature = hasVerifiedGPGSignature;
module.exports.verifyDCOSignoffs = verifyDCOSignoffs;
module.exports.verifyGPGSignatures = verifyGPGSignatures;
