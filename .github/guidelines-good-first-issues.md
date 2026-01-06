# Good First Issue Guidelines — Hiero C++ SDK

Good First Issues (GFIs) exist to provide a **safe, confidence-building first pull request** for new contributors.

They are designed to teach contributors how to:
- Get Started
- Navigate the repository
- Open a successful pull request

They are **not** designed to test problem-solving or architectural understanding.

---

## Assumptions

A Good First Issue must assume the contributor has:

- Basic C++ knowledge
- Basic Git/GitHub familiarity
- **No prior Hiero or SDK knowledge**
- **No understanding of SDK architecture**
- **No context beyond what the issue explicitly provides**

---

## GFI Requirements

A Good First Issue will **generally** be:

- ✅ Fully scripted (exact changes specified)
- ✅ Mechanical (minimal judgment or decisions)
- ✅ Narrow in scope (usually one file)
- ✅ Solvable without architectural understanding

> **Rule of thumb**  
> If the contributor must decide *what* to change or *how* it should work, it is **not** a Good First Issue.

---

## Allowed Work

### ✅ Allowed
- Fixing typos in comments, docs, or error messages
- Renaming variables, methods, or files when exact names are specified
- Small test fixes with explicit expected behavior
- Formatting fixes enforced by tooling
- Updating documentation links with exact URLs provided
- Improving error messaging

### ❌ Not Allowed
- Designing APIs
- Investigating bugs
- Refactoring for clarity or style
- Performance changes

---

## Time & Scope

- ⏱ Estimated time: **1–4 hours** (most is setting up)
- 📄 Scope: **Single file or tightly scoped location**

---
## Example: Well-Formed Good First Issue

**Title:** Fix typo in README (“mantainer” → “maintainer”)

**Description:**  
In `.README.md` at line 96, there is a spelling mistake.

Current text:
> `mantainer guide`

Should read:
> `maintainer guide`

This issue is to resolve the spelling mistake.
This change is a **direct text replacement only**. No other edits or tests are required.

**Files to Change**
- `.README.md`

**Acceptance Criteria**
- [ ] The text `mantainer guide` is replaced with `maintainer guide`
- [ ] No other files or lines are modified

---

## Support & Mentorship

Good First Issues are supported by maintainers and the
**Good First Issue support team**.

Support is intended to help contributors:
- Navigate the repository
- Understand the contribution workflow
- Confirm that changes meet the stated acceptance criteria

Support **does not** include:
- Designing solutions
- Deciding what should be changed
- Filling in missing requirements or ambiguity

A Good First Issue must be solvable **using only the information in the issue description**.

---

## Maintainer Rule

> A mislabeled Good First Issue harms contributor experience.

If in doubt, label as **[Good First Issue Candidate](guidelines-good-first-issue-candidates.md)`** instead.
