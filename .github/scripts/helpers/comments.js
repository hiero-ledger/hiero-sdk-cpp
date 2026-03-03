// SPDX-License-Identifier: Apache-2.0
//
// helpers/comments.js
//
// Builds the unified PR Helper Bot dashboard comment. Designed with a layered
// structure so future sections (commands, instructions) can be added alongside
// checks without changing the overall shape.

const { MAINTAINER_TEAM } = require('./constants');

const MARKER = '<!-- bot:pr-helper -->';

const SIGNING_GUIDE = 'https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/signing.md';
const MERGE_CONFLICTS_GUIDE = 'https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/merge-conflicts.md';

/**
 * Renders a single check result into a markdown section.
 * Supports three states: passed, failed, or error.
 * @param {{ passed?: boolean, error?: boolean }} result
 * @returns {'pass'|'fail'|'error'}
 */
function checkState(result) {
  if (result.error) return 'error';
  return result.passed ? 'pass' : 'fail';
}

/**
 * @param {{ passed: boolean, failures?: Array<{ sha: string, message: string }>, error?: boolean, errorMessage?: string }} dco
 * @returns {string}
 */
function buildDCOSection(dco) {
  const state = checkState(dco);
  if (state === 'error') {
    return [
      `:warning: **DCO Sign-off** -- This check encountered an internal error. ${MAINTAINER_TEAM} please review manually.`,
      '',
      `Error: ${dco.errorMessage || 'Unknown error'}`,
    ].join('\n');
  }
  if (state === 'pass') {
    return ':white_check_mark: **DCO Sign-off** -- All commits have valid sign-offs.';
  }
  const failList = (dco.failures || []).map(f => `- \`${f.sha}\` ${f.message}`).join('\n');
  return [
    ':x: **DCO Sign-off** -- The following commits are missing the required DCO sign-off:',
    failList,
    '',
    `Please add \`Signed-off-by: Your Name <email>\` to each commit (e.g. \`git commit -s\`). See the [Signing Guide](${SIGNING_GUIDE}).`,
  ].join('\n');
}

/**
 * @param {{ passed: boolean, failures?: Array<{ sha: string, message: string }>, error?: boolean, errorMessage?: string }} gpg
 * @returns {string}
 */
function buildGPGSection(gpg) {
  const state = checkState(gpg);
  if (state === 'error') {
    return [
      `:warning: **GPG Signature** -- This check encountered an internal error. ${MAINTAINER_TEAM} please review manually.`,
      '',
      `Error: ${gpg.errorMessage || 'Unknown error'}`,
    ].join('\n');
  }
  if (state === 'pass') {
    return ':white_check_mark: **GPG Signature** -- All commits have verified GPG signatures.';
  }
  const failList = (gpg.failures || []).map(f => `- \`${f.sha}\` ${f.message}`).join('\n');
  return [
    ':x: **GPG Signature** -- The following commits don\'t have a verified GPG signature:',
    failList,
    '',
    `Please sign your commits with GPG (e.g. \`git commit -S\`). See the [Signing Guide](${SIGNING_GUIDE}).`,
  ].join('\n');
}

/**
 * @param {{ passed: boolean, error?: boolean, errorMessage?: string }} merge
 * @returns {string}
 */
function buildMergeSection(merge) {
  const state = checkState(merge);
  if (state === 'error') {
    return [
      `:warning: **Merge Conflicts** -- This check encountered an internal error. ${MAINTAINER_TEAM} please review manually.`,
      '',
      `Error: ${merge.errorMessage || 'Unknown error'}`,
    ].join('\n');
  }
  if (state === 'pass') {
    return ':white_check_mark: **Merge Conflicts** -- No merge conflicts detected.';
  }
  return [
    ':x: **Merge Conflicts** -- This PR has merge conflicts with the base branch.',
    '',
    `Please update your branch (e.g. rebase or merge from base) and push. See the [Merge Conflicts Guide](${MERGE_CONFLICTS_GUIDE}).`,
  ].join('\n');
}

/**
 * @param {{ passed: boolean, reason?: string, issues?: Array<{ number: number, title: string, isAssigned: boolean }>, error?: boolean, errorMessage?: string }} issueLink
 * @returns {string}
 */
function buildIssueLinkSection(issueLink) {
  const state = checkState(issueLink);
  if (state === 'error') {
    return [
      `:warning: **Issue Link** -- This check encountered an internal error. ${MAINTAINER_TEAM} please review manually.`,
      '',
      `Error: ${issueLink.errorMessage || 'Unknown error'}`,
    ].join('\n');
  }
  if (state === 'pass') {
    const linked = (issueLink.issues || [])
      .filter(i => i.isAssigned)
      .map(i => `#${i.number}`)
      .join(', ');
    return `:white_check_mark: **Issue Link** -- Linked to ${linked} (assigned to you).`;
  }
  if (issueLink.reason === 'not_assigned') {
    const unassigned = (issueLink.issues || []).filter(i => !i.isAssigned).map(i => `#${i.number}`).join(', ');
    return [
      `:x: **Issue Link** -- You are not assigned to the following linked issues: ${unassigned}.`,
      '',
      'Please ensure you are assigned to all linked issues before opening a PR.',
    ].join('\n');
  }
  return [
    ':x: **Issue Link** -- This PR is not linked to any issue.',
    '',
    'Please reference an issue using a closing keyword (e.g. `Fixes #123`) and ensure the issue is assigned to you.',
  ].join('\n');
}

/**
 * Builds the ### PR Checks section of the dashboard comment.
 * @param {{ dco: object, gpg: object, merge: object, issueLink: object }} results
 * @returns {string}
 */
function buildChecksSection({ dco, gpg, merge, issueLink }) {
  return [
    '### PR Checks',
    '',
    buildDCOSection(dco),
    '',
    '---',
    '',
    buildGPGSection(gpg),
    '',
    '---',
    '',
    buildMergeSection(merge),
    '',
    '---',
    '',
    buildIssueLinkSection(issueLink),
  ].join('\n');
}

/**
 * Determines whether all checks passed (errors count as not passed).
 * @param {{ dco: object, gpg: object, merge: object, issueLink: object }} results
 * @returns {boolean}
 */
function allChecksPassed({ dco, gpg, merge, issueLink }) {
  return (
    !dco.error && dco.passed &&
    !gpg.error && gpg.passed &&
    !merge.error && merge.passed &&
    !issueLink.error && issueLink.passed
  );
}

/**
 * Builds the full unified bot comment.
 * @param {{ prAuthor: string, dco: object, gpg: object, merge: object, issueLink: object }} params
 * @returns {{ marker: string, body: string, allPassed: boolean }}
 */
function buildBotComment({ prAuthor, dco, gpg, merge, issueLink }) {
  const greeting = [
    `Hi @${prAuthor} :wave:, thank you for submitting this PR!`,
    "I'm your **PR Helper Bot** and I'll be keeping track of your PR's",
    'status to help you get it approved and merged.',
    '',
    'This comment will stay updated as you make changes.',
    "Here's where things stand:",
  ].join('\n');

  const checksSection = buildChecksSection({ dco, gpg, merge, issueLink });
  const passed = allChecksPassed({ dco, gpg, merge, issueLink });

  const footer = passed
    ? ':tada: *All checks passed! Your PR is ready for review.*'
    : '*All checks must pass before this PR can be reviewed.*';

  const body = [MARKER, greeting, '', '---', '', checksSection, '', '---', '', footer].join('\n');
  return { marker: MARKER, body, allPassed: passed };
}

module.exports = {
  MARKER,
  buildBotComment,
  buildChecksSection,
  allChecksPassed,
};
