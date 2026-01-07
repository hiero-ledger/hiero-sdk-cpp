# Issue Difficulty Overview — Hiero C++ SDK

This document summarizes the **issue difficulty and readiness labeling system**
used in the Hiero C++ SDK.

The system is designed to:
- Protect contributor trust
- Provide clear entry points by skill level
- Establish a safe progression path from first PR to advanced ownership

---

## Difficulty & Readiness Levels

Issues in this repository fall into **one readiness label** and **one difficulty label**.

### Readiness Label (Optional, Temporary)

| Label | Purpose |
|------|--------|
| **Good First Issue: Candidate** | Holding state for issues that *might* become Good First Issues but are not yet fully specified |

> The candidate label is **temporary** and must be resolved before contributor self-assignment.

---

### Difficulty Progression

| Level | Purpose | Decision-Making |
|------|--------|----------------|
| **Good First Issue** | Learn the workflow | None |
| **Beginner** | Learn the codebase | Small, safe |
| **Intermediate** | Own solutions | Moderate |
| **Advanced** | Design systems | High |

---

## How Labels Work Together

- **`good first issue: candidate`**  
  → Indicates *potential*, not readiness  
- **`good first issue`**  
  → Fully specified, mechanical, safe for first-time contributors  
- **Beginner / Intermediate / Advanced**  
  → Indicate increasing levels of investigation, judgment, and ownership

An issue **must not** be labeled **Good First Issue** until it satisfies all
requirements defined in [Good First Issue Guidelines](./guidelines-good-first-issues.md)

---

## Labeling Rules

- Every issue **must** have exactly **one difficulty label**
- The **candidate** label is optional and temporary
- Use `Good First Issue Candidate` label for expected `Good First Issues' only
- Label an issue with one level of difficulty higher if in doubt. For example, an intermediate issue that is will likely take more than a few days to complete, could be more safely categorized as `advanced`.
- Exploratory, investigative, or underspecified issues must **not** be labeled:
  - Good First Issue
  - Beginner

If an issue is not yet ready for new contributors, use **candidate** or a higher difficulty.

---

## Promotion Rule

> The **candidate** label exists to protect the Good First Issue bar.

Issues may only be promoted from **candidate → Good First Issue**
after becoming fully mechanical, fully specified, and judgment-free.

---

## Final Rule

> Difficulty and readiness labels exist to **protect contributors**
> and help them build skills safely and confidently.

**Accuracy matters more than volume.**
