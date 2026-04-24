// SPDX-License-Identifier: Apache-2.0
//
// helpers/constants.js
//
// Shared constants for bot scripts: maintainer team, labels, issue state.

/**
 * Team to tag when manual intervention is needed.
 */
const MAINTAINER_TEAM = '@hiero-ledger/hiero-sdk-cpp-maintainers';

/**
 * Common label constants used across bot scripts.
 */
const LABELS = Object.freeze({
  // Status labels
  AWAITING_TRIAGE: 'status: awaiting triage',
  READY_FOR_DEV: 'status: ready for dev',
  IN_PROGRESS: 'status: in progress',
  BLOCKED: 'status: blocked',
  NEEDS_REVIEW: 'status: needs review',
  NEEDS_REVISION: 'status: needs revision',

  // Skill level labels
  GOOD_FIRST_ISSUE: 'skill: good first issue',
  BEGINNER: 'skill: beginner',
  INTERMEDIATE: 'skill: intermediate',
  ADVANCED: 'skill: advanced',

  // Priority labels
  PRIORITY_CRITICAL: 'priority: critical',
  PRIORITY_HIGH: 'priority: high',
  PRIORITY_MEDIUM: 'priority: medium',
  PRIORITY_LOW: 'priority: low',
});

/**
 * Skill hierarchy used to determine progression for recommendations.
 */
const SKILL_HIERARCHY = Object.freeze([
    LABELS.GOOD_FIRST_ISSUE,
    LABELS.BEGINNER,
    LABELS.INTERMEDIATE,
    LABELS.ADVANCED,
]);

/**
 * Priority hierarchy for issue recommendations.
 */
const PRIORITY_HIERARCHY = Object.freeze([
    LABELS.PRIORITY_CRITICAL,
    LABELS.PRIORITY_HIGH,
    LABELS.PRIORITY_MEDIUM,
    LABELS.PRIORITY_LOW,
]);

/**
 * Issue state values for GitHub search queries.
 */
const ISSUE_STATE = Object.freeze({
  OPEN: 'open',
  CLOSED: 'closed',
});

module.exports = {
  MAINTAINER_TEAM,
  LABELS,
  ISSUE_STATE,
  SKILL_HIERARCHY,
  PRIORITY_HIERARCHY,
};
