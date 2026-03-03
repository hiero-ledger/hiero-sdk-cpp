// SPDX-License-Identifier: Apache-2.0
//
// helpers/checks.js
//
// Pure check functions for PR validation: DCO sign-off, GPG signatures,
// merge conflicts, and issue link (with assignment verification).
// Each function returns a structured result object with no side effects.

const { getLogger } = require('./logger');

/**
 * Checks whether a commit message contains a valid DCO sign-off line.
 * Expects a line matching "Signed-off-by: Name <email>" (case-insensitive).
 * @param {string} message - The commit message.
 * @returns {boolean}
 */
function hasDCOSignoff(message) {
  if (!message) return false;
  return /^Signed-off-by:\s+.+\s+<.+>/mi.test(message);
}

/**
 * Checks whether a commit has a verified GPG signature.
 * @param {{ commit?: { verification?: { verified?: boolean } } }} commit
 * @returns {boolean}
 */
function hasVerifiedGPGSignature(commit) {
  return commit?.commit?.verification?.verified === true;
}

/**
 * Checks all commits for DCO sign-off.
 * @param {Array<{ sha?: string, commit?: { message?: string } }>} commits
 * @returns {{ passed: boolean, failures: Array<{ sha: string, message: string }> }}
 */
function checkDCO(commits) {
  const failures = [];
  for (const c of commits) {
    const message = c.commit?.message || '';
    const shortSha = (c.sha || '').slice(0, 7);
    const firstLine = message.split('\n')[0] || '(no message)';
    if (!hasDCOSignoff(message)) {
      failures.push({ sha: shortSha, message: firstLine });
    }
  }
  getLogger().log(`DCO check: ${commits.length - failures.length}/${commits.length} passed`);
  return { passed: failures.length === 0, failures };
}

/**
 * Checks all commits for verified GPG signatures.
 * @param {Array<{ sha?: string, commit?: { message?: string, verification?: { verified?: boolean } } }>} commits
 * @returns {{ passed: boolean, failures: Array<{ sha: string, message: string }> }}
 */
function checkGPG(commits) {
  const failures = [];
  for (const c of commits) {
    const shortSha = (c.sha || '').slice(0, 7);
    const message = c.commit?.message || '';
    const firstLine = message.split('\n')[0] || '(no message)';
    if (!hasVerifiedGPGSignature(c)) {
      failures.push({ sha: shortSha, message: firstLine });
    }
  }
  getLogger().log(`GPG check: ${commits.length - failures.length}/${commits.length} passed`);
  return { passed: failures.length === 0, failures };
}

/**
 * Checks whether the PR has merge conflicts with the base branch.
 * Polls pulls.get for the mergeable state with retries.
 * @param {object} botContext
 * @returns {Promise<{ passed: boolean }>}
 */
async function checkMergeConflict(botContext) {
  const logger = getLogger();
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
  return { passed: !conflicts };
}

/**
 * Extracts issue numbers from a PR body using closing and "related to" keywords.
 * @param {string} body
 * @returns {Set<number>}
 */
function parseIssueNumbers(body) {
  const numbers = new Set();
  const patterns = [
    /(?:close[sd]?|fix(?:e[sd])?|resolve[sd]?)\s+#(\d+)/gi,
    /related\s+to\s+#(\d+)/gi,
  ];
  for (const regex of patterns) {
    let match;
    while ((match = regex.exec(body)) !== null) {
      numbers.add(parseInt(match[1], 10));
    }
  }
  return numbers;
}

/**
 * Fetches each issue by number and checks whether the given author is assigned.
 * Issues that fail to fetch are silently skipped (logged only).
 * @param {object} botContext
 * @param {function} fetchIssue
 * @param {Set<number>} issueNumbers
 * @param {string} prAuthor
 * @returns {Promise<Array<{ number: number, title: string, isAssigned: boolean }>>}
 */
async function fetchAndCheckAssignees(botContext, fetchIssue, issueNumbers, prAuthor) {
  const logger = getLogger();
  const results = [];
  for (const num of issueNumbers) {
    try {
      const issue = await fetchIssue(botContext, num);
      const isAssigned = (issue.assignees || []).some(
        a => a.login.toLowerCase() === prAuthor.toLowerCase()
      );
      results.push({ number: num, title: issue.title, isAssigned });
    } catch (err) {
      logger.log(`Issue link check: could not fetch issue #${num}: ${err.message}`);
    }
  }
  return results;
}

/**
 * Checks whether the PR is linked to an issue and whether the PR author is
 * assigned to that issue.
 *
 * Detection: regex on PR body for closing keywords, then GraphQL
 * closingIssuesReferences as fallback.
 *
 * @param {object} botContext
 * @param {{ fetchIssue: function, fetchClosingIssueNumbers: function }} api
 * @returns {Promise<{ passed: boolean, reason: string|null, issues: Array<{ number: number, title: string, isAssigned: boolean }> }>}
 */
async function checkIssueLink(botContext, { fetchIssue, fetchClosingIssueNumbers }) {
  const logger = getLogger();
  const body = botContext.pr?.body || '';
  const prAuthor = botContext.pr?.user?.login;

  const issueNumbers = parseIssueNumbers(body);

  if (issueNumbers.size === 0) {
    const graphqlIssues = await fetchClosingIssueNumbers(botContext);
    graphqlIssues.forEach(n => issueNumbers.add(n));
  }

  if (issueNumbers.size === 0) {
    logger.log('Issue link check: no linked issues found');
    return { passed: false, reason: 'no_issue_linked', issues: [] };
  }

  const linkedIssues = await fetchAndCheckAssignees(botContext, fetchIssue, issueNumbers, prAuthor);

  if (linkedIssues.length === 0) {
    logger.log('Issue link check: all linked issues returned errors');
    return { passed: false, reason: 'no_issue_linked', issues: [] };
  }

  const allAssigned = linkedIssues.every(i => i.isAssigned);
  if (!allAssigned) {
    const missing = linkedIssues.filter(i => !i.isAssigned).map(i => `#${i.number}`).join(', ');
    logger.log(`Issue link check: author ${prAuthor} not assigned to all linked issues (missing: ${missing})`);
    return { passed: false, reason: 'not_assigned', issues: linkedIssues };
  }

  logger.log(`Issue link check: passed (author assigned to all linked issues)`);
  return { passed: true, reason: null, issues: linkedIssues };
}

module.exports = {
  hasDCOSignoff,
  hasVerifiedGPGSignature,
  checkDCO,
  checkGPG,
  checkMergeConflict,
  checkIssueLink,
};
