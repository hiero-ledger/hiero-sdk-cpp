// SPDX-License-Identifier: Apache-2.0
//
// tests/test-assign-decision-layer.js
//
// Unit tests for the assignment decision layer.
// These tests verify that eligibility decisions can be tested independently
// of GitHub mutations.
//
// Run with: node .github/scripts/tests/test-assign-decision-layer.js

const { LABELS, ISSUE_STATE } = require("../helpers");
const { 
  determineAssignmentEligibility, 
  AssignmentOutcome 
} = require("../commands/assign");

// =============================================================================
// MOCK GITHUB API FOR DECISION LAYER TESTING
// =============================================================================

function createDecisionMockGithub(options = {}) {
  const {
    openAssignmentCount = 0,
    completedGfiCount = 0,
    completedPrereqCount = 0,
    freshIssueAssignees = [],
    freshIssueLabels = null,
    apiShouldFail = false,
    hasNeedsReviewPRs = false,
  } = options;

  return {
    rest: {
      issues: {
        get: async () => {
          if (apiShouldFail) {
            throw new Error("Simulated API failure");
          }
          return {
            data: {
              assignees: freshIssueAssignees.map(login => ({ login })),
              labels: freshIssueLabels || [
                { name: LABELS.READY_FOR_DEV },
                { name: LABELS.GOOD_FIRST_ISSUE },
              ],
            },
          };
        },
        listForRepo: async (params) => {
          if (apiShouldFail) {
            throw new Error("Simulated API failure");
          }
          
          if (params.state === "open") {
            const issues = [];
            for (let i = 0; i < openAssignmentCount; i++) {
              issues.push({ number: 1000 + i, labels: [] });
            }
            return { data: issues };
          }
          
          if (params.state === "closed") {
            if (params.labels === LABELS.GOOD_FIRST_ISSUE) {
              const issues = [];
              for (let i = 0; i < completedGfiCount; i++) {
                issues.push({ number: 2000 + i, labels: [{ name: LABELS.GOOD_FIRST_ISSUE }] });
              }
              return { data: issues };
            }
            if (params.labels) {
              const issues = [];
              for (let i = 0; i < completedPrereqCount; i++) {
                issues.push({ number: 3000 + i, labels: [{ name: params.labels }] });
              }
              return { data: issues };
            }
          }
          
          return { data: [] };
        },
      },
    },
    graphql: async (query) => {
      if (apiShouldFail) {
        throw new Error("Simulated GraphQL failure");
      }
      
      // Mock needs-review PR check
      if (query.includes("closedByPullRequestsReferences")) {
        if (hasNeedsReviewPRs) {
          return {
            repository: {
              issue: {
                closedByPullRequestsReferences: {
                  nodes: [{
                    state: "OPEN",
                    author: { login: "test-user" },
                    labels: { nodes: [{ name: LABELS.NEEDS_REVIEW }] }
                  }]
                }
              }
            }
          };
        }
        return {
          repository: {
            issue: {
              closedByPullRequestsReferences: {
                nodes: []
              }
            }
          }
        };
      }
      
      return { search: { issueCount: 0 } };
    },
  };
}

// =============================================================================
// TEST SCENARIOS
// =============================================================================

const decisionTests = [
  {
    name: "Decision: Eligible for Assignment",
    description: "All checks pass, returns ELIGIBLE outcome",
    botContext: {
      github: createDecisionMockGithub(),
      owner: "test-owner",
      repo: "test-repo",
      number: 100,
      issue: {
        assignees: [],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.GOOD_FIRST_ISSUE },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.ELIGIBLE,
  },
  {
    name: "Decision: Already Assigned (Payload)",
    description: "Issue has assignees in payload, returns ALREADY_ASSIGNED",
    botContext: {
      github: createDecisionMockGithub(),
      owner: "test-owner",
      repo: "test-repo",
      number: 101,
      issue: {
        assignees: [{ login: "other-user" }],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.GOOD_FIRST_ISSUE },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.ALREADY_ASSIGNED,
  },
  {
    name: "Decision: Not Ready",
    description: "Missing ready-for-dev label, returns NOT_READY",
    botContext: {
      github: createDecisionMockGithub(),
      owner: "test-owner",
      repo: "test-repo",
      number: 102,
      issue: {
        assignees: [],
        labels: [{ name: LABELS.GOOD_FIRST_ISSUE }],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.NOT_READY,
  },
  {
    name: "Decision: Missing Skill Label",
    description: "No skill level label present, returns MISSING_SKILL_LABEL",
    botContext: {
      github: createDecisionMockGithub(),
      owner: "test-owner",
      repo: "test-repo",
      number: 103,
      issue: {
        assignees: [],
        labels: [{ name: LABELS.READY_FOR_DEV }],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.MISSING_SKILL_LABEL,
  },
  {
    name: "Decision: Assignment Limit Exceeded",
    description: "User at MAX_OPEN_ASSIGNMENTS, returns ASSIGNMENT_LIMIT_EXCEEDED",
    botContext: {
      github: createDecisionMockGithub({ openAssignmentCount: 2 }),
      owner: "test-owner",
      repo: "test-repo",
      number: 104,
      issue: {
        assignees: [],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.GOOD_FIRST_ISSUE },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.ASSIGNMENT_LIMIT_EXCEEDED,
  },
  {
    name: "Decision: GFI Limit Exceeded",
    description: "User completed 5 GFIs, returns GFI_LIMIT_EXCEEDED",
    botContext: {
      github: createDecisionMockGithub({ completedGfiCount: 5 }),
      owner: "test-owner",
      repo: "test-repo",
      number: 105,
      issue: {
        assignees: [],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.GOOD_FIRST_ISSUE },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.GFI_LIMIT_EXCEEDED,
  },
  {
    name: "Decision: Prerequisite Not Met",
    description: "Beginner issue requires 2 GFIs, user has 0, returns PREREQUISITE_NOT_MET",
    botContext: {
      github: createDecisionMockGithub({ 
        completedPrereqCount: 0,
        freshIssueLabels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.BEGINNER },
        ],
      }),
      owner: "test-owner",
      repo: "test-repo",
      number: 106,
      issue: {
        assignees: [],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.BEGINNER },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.PREREQUISITE_NOT_MET,
  },
  {
    name: "Decision: Skill Level Changed",
    description: "Fresh issue has different skill level, returns SKILL_LEVEL_CHANGED",
    botContext: {
      github: createDecisionMockGithub({
        freshIssueLabels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.BEGINNER },
        ],
      }),
      owner: "test-owner",
      repo: "test-repo",
      number: 107,
      issue: {
        assignees: [],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.GOOD_FIRST_ISSUE },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.SKILL_LEVEL_CHANGED,
  },
  {
    name: "Decision: API Error",
    description: "GitHub API fails, returns API_ERROR",
    botContext: {
      github: createDecisionMockGithub({ apiShouldFail: true }),
      owner: "test-owner",
      repo: "test-repo",
      number: 108,
      issue: {
        assignees: [],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.GOOD_FIRST_ISSUE },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.API_ERROR,
  },
  {
    name: "Decision: Already Assigned (Fresh Fetch)",
    description: "Fresh fetch shows user already assigned, returns ALREADY_ASSIGNED",
    botContext: {
      github: createDecisionMockGithub({
        freshIssueAssignees: ["test-user"],
      }),
      owner: "test-owner",
      repo: "test-repo",
      number: 109,
      issue: {
        assignees: [],
        labels: [
          { name: LABELS.READY_FOR_DEV },
          { name: LABELS.GOOD_FIRST_ISSUE },
        ],
      },
    },
    requesterUsername: "test-user",
    expectedOutcome: AssignmentOutcome.ALREADY_ASSIGNED,
  },
];

// =============================================================================
// TEST RUNNER
// =============================================================================

async function runDecisionTests() {
  console.log("🧪 ASSIGNMENT DECISION LAYER TEST SUITE");
  console.log("=" .repeat(70));
  console.log();

  let passed = 0;
  let failed = 0;

  for (const test of decisionTests) {
    console.log("=" .repeat(70));
    console.log(`TEST: ${test.name}`);
    console.log(`Description: ${test.description}`);
    console.log("=" .repeat(70));

    try {
      const decision = await determineAssignmentEligibility(
        test.botContext,
        test.requesterUsername,
      );

      if (decision.outcome === test.expectedOutcome) {
        console.log(`✅ PASS: Got expected outcome "${decision.outcome}"`);
        console.log(`   Metadata:`, JSON.stringify(decision.metadata, null, 2));
        passed++;
      } else {
        console.log(`❌ FAIL: Expected "${test.expectedOutcome}", got "${decision.outcome}"`);
        console.log(`   Decision:`, JSON.stringify(decision, null, 2));
        failed++;
      }
    } catch (error) {
      console.log(`❌ FAIL: Unexpected error: ${error.message}`);
      console.log(error.stack);
      failed++;
    }

    console.log();
  }

  console.log("=" .repeat(70));
  console.log("📈 SUMMARY");
  console.log("=" .repeat(70));
  console.log(`   Decision Layer Tests: ${decisionTests.length} total, ${passed} passed ✅, ${failed} failed ❌`);
  console.log("=" .repeat(70));
  console.log();

  if (failed > 0) {
    process.exit(1);
  }
}

// Run tests
runDecisionTests().catch((error) => {
  console.error("Fatal error running tests:", error);
  process.exit(1);
});
