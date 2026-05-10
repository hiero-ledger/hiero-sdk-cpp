// SPDX-License-Identifier: Apache-2.0
//
// tests/test-config-loader.js
//
// Unit tests for helpers/config-loader.js
// Run with: node .github/scripts/tests/test-config-loader.js
//
// Tests cover:
//   - Valid config loading and constant building
//   - Missing config file error
//   - Malformed JSON error
//   - Validation errors for every required field and cross-reference

const fs = require('fs');
const path = require('path');
const { runTestSuite } = require('./test-utils');
const { loadAutomationConfig, buildConstants, DEFAULT_CONFIG_PATH } = require('../helpers/config-loader');

// =============================================================================
// HELPERS
// =============================================================================

const SCRATCH_DIR = path.resolve(__dirname, '.config-loader-scratch');

/**
 * Returns a deep clone of the production config for use as a valid baseline.
 * Tests mutate this clone to create invalid configs without touching the real file.
 */
function getValidConfig() {
  const raw = fs.readFileSync(DEFAULT_CONFIG_PATH, 'utf8');
  return JSON.parse(raw);
}

/**
 * Writes a config object to a temporary file and returns the path.
 * @param {string} name - Filename (no directory).
 * @param {object|string} content - Object (JSON-serialized) or raw string.
 * @returns {string} Absolute path to the temp file.
 */
function writeTempConfig(name, content) {
  if (!fs.existsSync(SCRATCH_DIR)) {
    fs.mkdirSync(SCRATCH_DIR, { recursive: true });
  }
  const filePath = path.join(SCRATCH_DIR, name);
  const data = typeof content === 'string' ? content : JSON.stringify(content, null, 2);
  fs.writeFileSync(filePath, data, 'utf8');
  return filePath;
}

/**
 * Removes the scratch directory and all temp files.
 */
function cleanupScratch() {
  if (fs.existsSync(SCRATCH_DIR)) {
    for (const file of fs.readdirSync(SCRATCH_DIR)) {
      fs.unlinkSync(path.join(SCRATCH_DIR, file));
    }
    fs.rmdirSync(SCRATCH_DIR);
  }
}

/**
 * Asserts that loadAutomationConfig throws an error containing the expected substring.
 * @param {string} configPath - Path to the temp config file.
 * @param {string} expectedSubstring - Expected substring in the error message.
 * @returns {boolean}
 */
function expectLoadError(configPath, expectedSubstring) {
  try {
    loadAutomationConfig(configPath);
    return false;
  } catch (err) {
    if (!err.message.includes(expectedSubstring)) {
      console.log(`    Expected error containing: "${expectedSubstring}"`);
      console.log(`    Got: "${err.message}"`);
      return false;
    }
    return true;
  }
}

// =============================================================================
// UNIT TESTS
// =============================================================================

const unitTests = [

  // ---------------------------------------------------------------------------
  // Valid config loading
  // ---------------------------------------------------------------------------
  {
    name: 'loadAutomationConfig: loads production config without error',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      return config !== null && typeof config === 'object';
    },
  },
  {
    name: 'loadAutomationConfig: returns a frozen object',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      return Object.isFrozen(config);
    },
  },
  {
    name: 'buildConstants: produces MAINTAINER_TEAM string',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const derived = buildConstants(config);
      return derived.MAINTAINER_TEAM === '@hiero-ledger/hiero-sdk-cpp-maintainers';
    },
  },
  {
    name: 'buildConstants: produces GFI_SUPPORT_TEAM string',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const derived = buildConstants(config);
      return derived.GFI_SUPPORT_TEAM === '@hiero-ledger/hiero-sdk-good-first-issue-support';
    },
  },
  {
    name: 'buildConstants: LABELS has all expected keys',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const { LABELS } = buildConstants(config);
      const expectedKeys = [
        'AWAITING_TRIAGE', 'READY_FOR_DEV', 'IN_PROGRESS', 'BLOCKED',
        'NEEDS_REVIEW', 'NEEDS_REVISION',
        'GOOD_FIRST_ISSUE', 'BEGINNER', 'INTERMEDIATE', 'ADVANCED',
        'PRIORITY_CRITICAL', 'PRIORITY_HIGH', 'PRIORITY_MEDIUM', 'PRIORITY_LOW',
      ];
      return expectedKeys.every(k => typeof LABELS[k] === 'string' && LABELS[k].length > 0);
    },
  },
  {
    name: 'buildConstants: LABELS is frozen',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const { LABELS } = buildConstants(config);
      return Object.isFrozen(LABELS);
    },
  },
  {
    name: 'buildConstants: SKILL_HIERARCHY matches config order',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const { SKILL_HIERARCHY, LABELS } = buildConstants(config);
      return (
        SKILL_HIERARCHY.length === 4 &&
        SKILL_HIERARCHY[0] === LABELS.GOOD_FIRST_ISSUE &&
        SKILL_HIERARCHY[3] === LABELS.ADVANCED
      );
    },
  },
  {
    name: 'buildConstants: PRIORITY_HIERARCHY matches config order',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const { PRIORITY_HIERARCHY, LABELS } = buildConstants(config);
      return (
        PRIORITY_HIERARCHY.length === 4 &&
        PRIORITY_HIERARCHY[0] === LABELS.PRIORITY_CRITICAL &&
        PRIORITY_HIERARCHY[3] === LABELS.PRIORITY_LOW
      );
    },
  },
  {
    name: 'buildConstants: SKILL_PREREQUISITES has correct progression',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const { SKILL_PREREQUISITES, LABELS } = buildConstants(config);
      return (
        SKILL_PREREQUISITES[LABELS.GOOD_FIRST_ISSUE].requiredLabel === null &&
        SKILL_PREREQUISITES[LABELS.BEGINNER].requiredLabel === LABELS.GOOD_FIRST_ISSUE &&
        SKILL_PREREQUISITES[LABELS.BEGINNER].requiredCount === 2 &&
        SKILL_PREREQUISITES[LABELS.INTERMEDIATE].requiredLabel === LABELS.BEGINNER &&
        SKILL_PREREQUISITES[LABELS.INTERMEDIATE].requiredCount === 3 &&
        SKILL_PREREQUISITES[LABELS.ADVANCED].requiredLabel === LABELS.INTERMEDIATE &&
        SKILL_PREREQUISITES[LABELS.ADVANCED].requiredCount === 3
      );
    },
  },
  {
    name: 'buildConstants: DOCUMENTATION has all expected keys',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const { DOCUMENTATION } = buildConstants(config);
      return (
        typeof DOCUMENTATION.workflowGuide === 'string' &&
        typeof DOCUMENTATION.readme === 'string' &&
        typeof DOCUMENTATION.signingGuide === 'string' &&
        typeof DOCUMENTATION.mergeConflictsGuide === 'string'
      );
    },
  },
  {
    name: 'buildConstants: COMMUNITY has discordChannel',
    test: () => {
      const config = loadAutomationConfig(DEFAULT_CONFIG_PATH);
      const { COMMUNITY } = buildConstants(config);
      return typeof COMMUNITY.discordChannel === 'string' && COMMUNITY.discordChannel.length > 0;
    },
  },

  // ---------------------------------------------------------------------------
  // File-level errors
  // ---------------------------------------------------------------------------
  {
    name: 'loadAutomationConfig: missing file → clear error',
    test: () => {
      return expectLoadError('/nonexistent/path/config.json', 'Failed to read automation config');
    },
  },
  {
    name: 'loadAutomationConfig: malformed JSON → clear error',
    test: () => {
      const p = writeTempConfig('malformed.json', '{ broken json!!!');
      return expectLoadError(p, 'Failed to parse automation config');
    },
  },

  // ---------------------------------------------------------------------------
  // Validation: teams
  // ---------------------------------------------------------------------------
  {
    name: 'validation: missing maintainerTeam → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.maintainerTeam;
      const p = writeTempConfig('no-team.json', cfg);
      return expectLoadError(p, 'maintainerTeam must be a non-empty string');
    },
  },
  {
    name: 'validation: empty goodFirstIssueSupportTeam → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.goodFirstIssueSupportTeam = '';
      const p = writeTempConfig('empty-gfi-team.json', cfg);
      return expectLoadError(p, 'goodFirstIssueSupportTeam must be a non-empty string');
    },
  },

  // ---------------------------------------------------------------------------
  // Validation: labels
  // ---------------------------------------------------------------------------
  {
    name: 'validation: missing labels → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.labels;
      const p = writeTempConfig('no-labels.json', cfg);
      return expectLoadError(p, 'labels must be an object');
    },
  },
  {
    name: 'validation: missing labels.skill → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.labels.skill;
      const p = writeTempConfig('no-skill-labels.json', cfg);
      return expectLoadError(p, 'labels.skill must be an object');
    },
  },
  {
    name: 'validation: empty label value → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.labels.status.awaitingTriage = '';
      const p = writeTempConfig('empty-label.json', cfg);
      return expectLoadError(p, 'labels.status.awaitingTriage is required and must be a non-empty string');
    },
  },
  {
    name: 'validation: missing required status key (blocked) → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.labels.status.blocked;
      const p = writeTempConfig('no-blocked.json', cfg);
      return expectLoadError(p, 'labels.status.blocked is required and must be a non-empty string');
    },
  },
  {
    name: 'validation: missing required skill key (beginner) → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.labels.skill.beginner;
      const p = writeTempConfig('no-beginner.json', cfg);
      return expectLoadError(p, 'labels.skill.beginner is required and must be a non-empty string');
    },
  },
  {
    name: 'validation: missing required priority key (high) → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.labels.priority.high;
      const p = writeTempConfig('no-high.json', cfg);
      return expectLoadError(p, 'labels.priority.high is required and must be a non-empty string');
    },
  },

  // ---------------------------------------------------------------------------
  // Validation: hierarchies
  // ---------------------------------------------------------------------------
  {
    name: 'validation: empty skillHierarchy → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.skillHierarchy = [];
      const p = writeTempConfig('empty-skill-hier.json', cfg);
      return expectLoadError(p, 'skillHierarchy must be a non-empty array');
    },
  },
  {
    name: 'validation: skillHierarchy entry not in labels.skill → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.skillHierarchy.push('skill: nonexistent');
      const p = writeTempConfig('bad-skill-hier.json', cfg);
      return expectLoadError(p, 'skillHierarchy entry "skill: nonexistent" not found in labels.skill values');
    },
  },
  {
    name: 'validation: priorityHierarchy entry not in labels.priority → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.priorityHierarchy.push('priority: ultra');
      const p = writeTempConfig('bad-prio-hier.json', cfg);
      return expectLoadError(p, 'priorityHierarchy entry "priority: ultra" not found in labels.priority values');
    },
  },

  // ---------------------------------------------------------------------------
  // Validation: skill prerequisites
  // ---------------------------------------------------------------------------
  {
    name: 'validation: skillPrerequisites key not in skillHierarchy → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.skillPrerequisites['skill: phantom'] = { requiredLabel: null, requiredCount: 0, displayName: 'Phantom' };
      const p = writeTempConfig('bad-prereq-key.json', cfg);
      return expectLoadError(p, 'skillPrerequisites key "skill: phantom" not found in skillHierarchy');
    },
  },
  {
    name: 'validation: non-null requiredLabel not in skillHierarchy → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.skillPrerequisites['skill: beginner'].requiredLabel = 'skill: imaginary';
      const p = writeTempConfig('bad-prereq-label.json', cfg);
      return expectLoadError(p, 'requiredLabel "skill: imaginary" not found in skillHierarchy');
    },
  },
  {
    name: 'validation: skillHierarchy entry missing from skillPrerequisites → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.skillPrerequisites['skill: intermediate'];
      const p = writeTempConfig('missing-prereq-entry.json', cfg);
      return expectLoadError(p, 'skillPrerequisites is missing entry for skillHierarchy value "skill: intermediate"');
    },
  },
  {
    name: 'validation: prerequisite missing requiredLabel → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.skillPrerequisites['skill: good first issue'].requiredLabel;
      const p = writeTempConfig('no-req-label.json', cfg);
      return expectLoadError(p, 'requiredLabel is required');
    },
  },
  {
    name: 'validation: prerequisite missing requiredCount → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.skillPrerequisites['skill: beginner'].requiredCount;
      const p = writeTempConfig('no-req-count.json', cfg);
      return expectLoadError(p, 'requiredCount must be a non-negative integer');
    },
  },
  {
    name: 'validation: prerequisite missing displayName → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.skillPrerequisites['skill: advanced'].displayName;
      const p = writeTempConfig('no-display-name.json', cfg);
      return expectLoadError(p, 'displayName is required and must be a non-empty string');
    },
  },
  {
    name: 'validation: prerequisite missing prerequisiteDisplayName when requiredLabel is not null → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.skillPrerequisites['skill: intermediate'].prerequisiteDisplayName;
      const p = writeTempConfig('no-prereq-display.json', cfg);
      return expectLoadError(p, 'prerequisiteDisplayName is required when requiredLabel is not null');
    },
  },

  // ---------------------------------------------------------------------------
  // Validation: assignment limits
  // ---------------------------------------------------------------------------
  {
    name: 'validation: missing assignmentLimits → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.assignmentLimits;
      const p = writeTempConfig('no-limits.json', cfg);
      return expectLoadError(p, 'assignmentLimits must be an object');
    },
  },
  {
    name: 'validation: maxOpenAssignments = 0 → error (must be positive)',
    test: () => {
      const cfg = getValidConfig();
      cfg.assignmentLimits.maxOpenAssignments = 0;
      const p = writeTempConfig('zero-limit.json', cfg);
      return expectLoadError(p, 'maxOpenAssignments must be a positive integer');
    },
  },
  {
    name: 'validation: maxGfiCompletions = -1 → error (must be positive)',
    test: () => {
      const cfg = getValidConfig();
      cfg.assignmentLimits.maxGfiCompletions = -1;
      const p = writeTempConfig('neg-gfi.json', cfg);
      return expectLoadError(p, 'maxGfiCompletions must be a positive integer');
    },
  },
  {
    name: 'validation: maxOpenAssignments = 1.5 → error (must be integer)',
    test: () => {
      const cfg = getValidConfig();
      cfg.assignmentLimits.maxOpenAssignments = 1.5;
      const p = writeTempConfig('float-limit.json', cfg);
      return expectLoadError(p, 'maxOpenAssignments must be a positive integer');
    },
  },

  // ---------------------------------------------------------------------------
  // Validation: documentation
  // ---------------------------------------------------------------------------
  {
    name: 'validation: missing documentation → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.documentation;
      const p = writeTempConfig('no-docs.json', cfg);
      return expectLoadError(p, 'documentation must be an object');
    },
  },
  {
    name: 'validation: empty documentation value → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.documentation.signingGuide = '';
      const p = writeTempConfig('empty-doc.json', cfg);
      return expectLoadError(p, 'documentation.signingGuide is required and must be a non-empty string');
    },
  },
  {
    name: 'validation: missing required documentation key (readme) → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.documentation.readme;
      const p = writeTempConfig('no-readme-doc.json', cfg);
      return expectLoadError(p, 'documentation.readme is required and must be a non-empty string');
    },
  },

  // ---------------------------------------------------------------------------
  // Validation: community
  // ---------------------------------------------------------------------------
  {
    name: 'validation: missing community → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.community;
      const p = writeTempConfig('no-community.json', cfg);
      return expectLoadError(p, 'community must be an object');
    },
  },
  {
    name: 'validation: empty community value → error',
    test: () => {
      const cfg = getValidConfig();
      cfg.community.discordChannel = '   ';
      const p = writeTempConfig('empty-discord.json', cfg);
      return expectLoadError(p, 'community.discordChannel is required and must be a non-empty string');
    },
  },
  {
    name: 'validation: missing required community key (discordChannel) → error',
    test: () => {
      const cfg = getValidConfig();
      delete cfg.community.discordChannel;
      const p = writeTempConfig('no-discord.json', cfg);
      return expectLoadError(p, 'community.discordChannel is required and must be a non-empty string');
    },
  },

  // ---------------------------------------------------------------------------
  // Custom config with different values
  // ---------------------------------------------------------------------------
  {
    name: 'loadAutomationConfig: loads a custom config with alternate values',
    test: () => {
      const cfg = getValidConfig();
      cfg.maintainerTeam = '@my-org/my-team';
      cfg.assignmentLimits.maxOpenAssignments = 5;
      const p = writeTempConfig('custom.json', cfg);
      const config = loadAutomationConfig(p);
      const derived = buildConstants(config);
      return (
        derived.MAINTAINER_TEAM === '@my-org/my-team' &&
        config.assignmentLimits.maxOpenAssignments === 5
      );
    },
  },
];

// =============================================================================
// TEST RUNNER
// =============================================================================

async function runUnitTests() {
  console.log('🔬 UNIT TESTS (config-loader)');
  console.log('='.repeat(70));
  let passed = 0;
  let failed = 0;
  for (const test of unitTests) {
    try {
      const result = await Promise.resolve(test.test());
      if (result) {
        console.log(`✅ ${test.name}`);
        passed++;
      } else {
        console.log(`❌ ${test.name}`);
        failed++;
      }
    } catch (error) {
      console.log(`❌ ${test.name} - Error: ${error.message}`);
      failed++;
    }
  }
  console.log('\n' + '-'.repeat(70));
  console.log(`Unit Tests: ${passed} passed, ${failed} failed`);

  // Cleanup scratch files
  cleanupScratch();

  return { total: unitTests.length, passed, failed };
}

runTestSuite('CONFIG LOADER TEST SUITE', [], async () => true, [
  { label: 'Unit Tests', run: runUnitTests },
]);
