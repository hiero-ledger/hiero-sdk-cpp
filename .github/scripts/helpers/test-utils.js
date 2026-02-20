// SPDX-License-Identifier: Apache-2.0
//
// helpers/test-utils.js
//
// Shared test utilities for bot script test suites. Provides comment snapshot
// verification, result checking, summary printing, and CLI argument handling.

/**
 * Compares actual comments against expected snapshots. Returns an array of
 * { passed: boolean, detail: string } results and logs mismatches.
 *
 * @param {string[]} expectedComments - Expected comment bodies.
 * @param {string[]} actualComments - Actual comment bodies captured during the test.
 * @returns {{ passed: boolean, details: string[] }}
 */
function verifyComments(expectedComments, actualComments) {
  const results = { passed: true, details: [] };

  if (expectedComments.length === 0 && actualComments.length === 0) {
    results.details.push('✅ Correctly posted no comments');
    return results;
  }

  if (expectedComments.length !== actualComments.length) {
    results.passed = false;
    results.details.push(`❌ Expected ${expectedComments.length} comment(s), got ${actualComments.length}`);
    return results;
  }

  for (let i = 0; i < expectedComments.length; i++) {
    if (actualComments[i] === expectedComments[i]) {
      results.details.push(`✅ Comment ${i + 1} matches snapshot`);
    } else {
      results.passed = false;
      results.details.push(`❌ Comment ${i + 1} does not match snapshot`);
      console.log('\n📋 EXPECTED:');
      console.log('─'.repeat(60));
      console.log(expectedComments[i]);
      console.log('─'.repeat(60));
      console.log('\n📋 ACTUAL:');
      console.log('─'.repeat(60));
      console.log(actualComments[i]);
      console.log('─'.repeat(60));
    }
  }

  return results;
}

/**
 * Prints a summary table and exits with the appropriate code.
 *
 * @param {{ label: string, total: number, passed: number, failed: number }[]} sections
 *   One entry per test section (e.g. unit tests, integration tests).
 */
function printSummaryAndExit(sections) {
  console.log('\n' + '='.repeat(70));
  console.log('📈 SUMMARY');
  console.log('='.repeat(70));

  let anyFailed = false;
  for (const { label, total, passed, failed } of sections) {
    if (failed > 0) anyFailed = true;
    console.log(`   ${label}: ${total} total, ${passed} passed${failed > 0 ? `, ${failed} failed ❌` : ' ✅'}`);
  }

  console.log('='.repeat(70));
  process.exit(anyFailed ? 1 : 0);
}

/**
 * Parses the optional test-index CLI argument and either runs a single
 * scenario or all scenarios, then prints a summary and exits.
 *
 * @param {string} suiteName - Display name (e.g. "ON-COMMIT BOT TEST SUITE").
 * @param {object[]} scenarios - Array of scenario objects.
 * @param {function(object, number): Promise<boolean>} runScenario
 *   Async function that runs one scenario and returns true if it passed.
 * @param {{ label: string, run: () => Promise<{ total: number, passed: number, failed: number }> }[]} [extraSections]
 *   Optional extra sections (e.g. unit tests) to run before scenarios.
 */
async function runTestSuite(suiteName, scenarios, runScenario, extraSections = []) {
  const testIndex = process.argv[2];

  if (testIndex !== undefined) {
    const index = parseInt(testIndex, 10);
    if (index >= 0 && index < scenarios.length) {
      const ok = await runScenario(scenarios[index], index);
      printSummaryAndExit([{ label: 'Single Test', total: 1, passed: ok ? 1 : 0, failed: ok ? 0 : 1 }]);
    } else {
      console.log(`Invalid test index. Available: 0-${scenarios.length - 1}`);
      console.log('\nAvailable scenarios:');
      scenarios.forEach((s, i) => console.log(`  ${i}: ${s.name}`));
      process.exit(1);
    }
    return;
  }

  console.log(`🧪 ${suiteName}`);
  console.log('='.repeat(suiteName.length + 3) + '\n');

  const summaries = [];

  for (const section of extraSections) {
    const result = await section.run();
    summaries.push({ label: section.label, ...result });
  }

  console.log('\n\n🔗 INTEGRATION TESTS');
  console.log('='.repeat(70));

  let passed = 0;
  let failed = 0;
  for (let i = 0; i < scenarios.length; i++) {
    const ok = await runScenario(scenarios[i], i);
    if (ok) passed++;
    else failed++;
  }
  summaries.push({ label: 'Integration Tests', total: scenarios.length, passed, failed });

  printSummaryAndExit(summaries);
}

module.exports = {
  verifyComments,
  printSummaryAndExit,
  runTestSuite,
};
