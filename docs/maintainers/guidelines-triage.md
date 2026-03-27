# Issue Triage Guide — Hiero C++ SDK

## Overview

This guide explains how to triage newly submitted issues and use the `/finalize` command to prepare them for contributors.

Issues submitted via the **Bug**, **Feature**, or **Task** templates are automatically labeled `status: awaiting triage`. The triage process turns a raw submission into a well-labeled, clearly structured issue that contributors can confidently pick up.

---

## Finding Issues to Triage

Filter the issue list by `status: awaiting triage` to find issues that need attention:

[Browse issues awaiting triage](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue+is%3Aopen+label%3A%22status%3A+awaiting+triage%22)

---

## The Triage Checklist

Before running `/finalize`, apply all required labels and verify the issue is complete:

### 1. Review the issue

- Does the description make sense?
- Is there enough information to act on it?
- If it's a bug, is it reproducible? If not, apply `status: needs repro` or `status: needs info` and ask for more details.
- Is it a duplicate? If so, apply `resolution: duplicate`, link the original, and close it.

### 2. Apply labels

All of the following must be applied before `/finalize` will succeed:

| Label group | Rule | Examples |
|---|---|---|
| `skill:` | **Exactly 1** — how hard is this to implement? | `skill: beginner`, `skill: advanced` |
| `priority:` | **Exactly 1** — how urgently should this be done? | `priority: medium`, `priority: high` |
| `kind:` | **Exactly 1** for Bug and Task; **0** for Feature | `kind: maintenance`, `kind: refactor` |
| `scope:` | **0 or more** — which part of the SDK is affected? | `scope: api`, `scope: grpc` |

> The `status:` label is managed automatically by the bot and should not be changed manually unless something goes wrong.

#### Choosing a skill level

See the skill level guidelines for detailed guidance:
- [Good First Issue Guidelines](./guidelines-good-first-issues.md)
- [Beginner Issue Guidelines](./guidelines-beginner-issues.md)
- [Intermediate Issue Guidelines](./guidelines-intermediate-issues.md)
- [Advanced Issue Guidelines](./guidelines-advanced-issues.md)
- [Difficulty Overview](./guidelines-difficulty-overview.md)

As a quick reference:

| Level | Expected time | Who it's for |
|---|---|---|
| `skill: good first issue` | 1–4 hours | First-time contributors |
| `skill: beginner` | 4–8 hours | Contributors with 2 completed GFIs |
| `skill: intermediate` | 1–3 days | Contributors familiar with the codebase |
| `skill: advanced` | 3+ days | Experienced contributors |

> **Note:** Time estimates are rough orientation guides, not hard targets — a change's true complexity depends on the contributor's familiarity and the specifics of the issue. See the linked guidelines above for the full criteria used to assign each level.

#### Choosing a kind label (for bugs and tasks)

| Kind | When to use |
|---|---|
| `kind: enhancement` | Improving existing functionality without adding new public API |
| `kind: documentation` | README, guides, API doc comments |
| `kind: refactor` | Code restructuring without behavior change |
| `kind: security` | Security-related improvements or hardening |
| `kind: testing` | Adding or improving tests |
| `kind: maintenance` | Dependencies, build system, CI/CD |

### 3. Edit the issue body if needed

Before running `/finalize`, ensure the issue body is complete and actionable. For complex issues, it's fine to add implementation guidance, point to relevant files, or clarify the scope directly in the issue.

`/finalize` will preserve all the existing content and add skill-level context and a contribution guide around it.

---

## Running `/finalize`

Once all labels are applied and the issue looks good, comment `/finalize` on the issue.

```
/finalize
```

The bot will:

1. Verify you have **triage or above** repository permissions
2. Check that all required labels are correctly applied — if not, it will post a comment listing every violation
3. Update the **issue title** to include the skill-level prefix (e.g. `[Beginner]: Fix thing`)
4. Rebuild the **issue body** in the standard skill-level template format:
   - Prepends the skill-level intro block and `[!IMPORTANT]` callout
   - Preserves all of the submitter's original content
   - Appends a standard contribution guide and additional information section
5. Swap the status label: `status: awaiting triage` → `status: ready for dev`
6. Post a success comment confirming the finalization

The issue is now ready for contributors to pick up via `/assign`.

---

## Handling Validation Errors

If `/finalize` responds with a validation error comment, read the listed violations carefully. Each one explains exactly what label is missing, duplicated, or incorrectly applied.

Common fixes:

| Error | Fix |
|---|---|
| No `skill:` label | Apply one of the four skill labels |
| Multiple `skill:` labels | Remove all but one |
| No `priority:` label | Apply a priority label |
| Bug/Task missing `kind:` label | Apply the appropriate kind label |
| Feature has a `kind:` label | Remove the kind label |
| `status: awaiting triage` not present | Check that the issue hasn't already been finalized |

After fixing the labels, comment `/finalize` again.

---

## Edge Cases

### Issue already finalized (re-finalized)

Running `/finalize` on an already-finalized issue (one with `status: ready for dev`) will fail validation because `status: awaiting triage` is no longer present. To re-finalize, manually swap the status label back to `status: awaiting triage` first, then run `/finalize` again.

If you only need to update the title prefix (e.g. because you changed the skill label), you can do that manually rather than going through the full re-finalize process.

### Blank issues

Issues created without a template won't have a recognized type (Bug, Feature, or Task). `/finalize` will report this as a validation error. Either ask the submitter to re-submit using a template, or manually set the GitHub issue type before running `/finalize`.

### API errors

If the bot reports an API error (permission check failure, issue update failure, or label swap failure), it will tag `@hiero-ledger/hiero-sdk-cpp-maintainers` in the comment with instructions for completing the finalization manually.

---

## Why This Process Exists

Previously, issue submitters chose a skill-level template themselves. This led to mislabeled issues: contributors had no way to assess implementation complexity, so difficulty labels were inconsistently applied and the triage step was unclear.

The new process separates concerns:
- **Submitters** describe the problem (Bug, Feature, or Task)
- **Maintainers** assess complexity and apply the skill label, then run `/finalize`
- **Contributors** pick up well-structured, clearly labeled issues via `/assign`

This makes the contribution experience more predictable for everyone.
