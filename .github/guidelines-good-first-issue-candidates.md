# Good First Issue — Candidate Guidelines  
**Hiero C++ SDK (`hiero-sdk-cpp`)**

This document defines the purpose and correct usage of the  
**`good first issue: candidate`** label for the Hiero C++ SDK.

The candidate label exists to **protect the integrity of Good First Issues** by
ensuring issues are fully prepared **before** being advertised to new contributors.

---

## Purpose

The **`good first issue: candidate`** label represents a **temporary holding state**.

It indicates that an issue:

- Appears small and low risk
- Might qualify as a Good First Issue
- **Does not yet meet all Good First Issue requirements**

The label gives maintainers space to refine the issue **without exposing it prematurely**.

> The candidate label is **not** a weaker Good First Issue.  
> It is a **quality-control gate**.

---

## Relationship to Good First Issues

This document **does not redefine** what a Good First Issue is.

All standards are defined in:

[Good First Issue Guidelines](./guidelines-good-first-issues.md)

A candidate issue must be promoted to **Good First Issue** only after it fully satisfies
**every** requirement in that document.

---

## When to Use `good first issue: candidate`

Apply the candidate label when:

- The issue *seems* appropriate for new contributors
- But one or more required GFI elements are missing or unclear

### Common Reasons

- Exact file paths are missing
- Exact code changes are not fully specified
- Acceptance criteria are vague or incomplete
- It is unclear whether the issue is truly mechanical
- Maintainers are unsure whether judgment is required

If there is **any doubt**, use the candidate label.

---

## What Candidate Issues MAY Include

Candidate issues may temporarily include:

- TODOs for maintainers to clarify
- Incomplete acceptance criteria
- Notes like “we should probably…”
- Ambiguity around exact implementation steps

These **must be resolved before promotion**.

---

## What the Candidate Label Is NOT

The candidate label **must not** be used for issues that will **never** qualify
as a Good First Issue.

Do **not** apply the candidate label if the issue:

- Requires deciding *what* behavior should be
- Requires designing APIs or logic
- Requires investigation or debugging
- Involves performance work
- Touches architecture or cross-cutting concerns
- Would still require interpretation even if refined

If an issue cannot become fully mechanical, it should be labeled:
**Beginner**, **Intermediate**, or **Advanced** instead.

---

## Promotion Criteria (Candidate → Good First Issue)

A candidate issue may be promoted **only when all of the following are true**:

- [ ] Exact files to modify are listed
- [ ] Exact code or text changes are specified
- [ ] No interpretation or design decisions are required
- [ ] Scope is limited to a single file or tight location
- [ ] Acceptance criteria are objective and testable
- [ ] A new contributor could complete it without SDK context

If **any** box is unchecked, the issue stays a candidate.

---

## Promotion Process

1. Review the issue against `guidelines_good_first_issues.md`
2. Add missing details, steps, or acceptance criteria
3. Confirm the issue is purely mechanical
4. Remove the `good first issue: candidate` label
5. Apply the **Good First Issue** label

---

## Maintainer Rule

> The candidate label exists to **protect contributor trust**.

It is always better to delay a Good First Issue  
than to publish one that turns out to be misleading.

---

## Final Rule

> If answering contributor questions would require explaining  
> *what to change*, the issue is **not ready** to be a Good First Issue.
