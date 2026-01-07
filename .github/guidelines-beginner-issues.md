# Beginner Issue Guidelines — Hiero C++ SDK

Beginner Issues are the **next step after
[Good First Issues](./guidelines-good-first-issues.md)**.

They are intended for contributors who are comfortable with the basic
contribution workflow and are ready to **read existing code and make small,
safe decisions**.

Beginner Issues remain low risk and localized, but are **not fully scripted**.

---

## Assumptions

Beginner Issues assume contributors:

- Can build the SDK locally
- Understand the basic repository structure
- Are comfortable reading existing C++ code and familiar with concepts such as `std::async`, `std::future`, and `std::promise`
- Have completed (or could complete) a Good First Issue

Beginner Issues do **not** assume:

- Deep SDK architecture knowledge
- API or system design experience
- Performance, concurrency, or ABI expertise

---

## Characteristics

Beginner Issues:

- Require **some interpretation**, unlike fully scripted
  [Good First Issues](./guidelines-good-first-issues.md)
- Require reading existing code to understand local context
- Allow **small, safe decisions** by the contributor
- Are broader than GFIs, but still well-scoped and localized
- Are low to moderate risk

> **Rule of thumb**  
> If a contributor must decide *how* to make a change, but not *what the system
> should be*, it is a Beginner Issue.

---

## Allowed Work

### ✅ Allowed

- Small bug fixes with clear reproduction steps
- Adding or adjusting specified unit tests that have similar examples
- Improving error messages or documentation wording
- Small feature additions with clearly defined behavior
- Cleaning up localized inconsistencies in a small area of the codebase

### ❌ Not Allowed

- API redesigns or breaking changes
- Cross-cutting or multi-system refactors
- Performance optimization or tuning
- Architectural or design-heavy changes

---

## Time & Scope

- ⏱ **Estimated time:** 4–8 hours  
- 📄 **Scope:** 1–3 related files  
- 🧠 **Challenge:** Local reasoning and small judgment calls  

If an issue is expected to require investigation across multiple subsystems
or significantly longer than a day, it is **not** a Beginner Issue.

---

## Example: Well-Formed Beginner Issue

**Title:** Improve documentation clarity for `FileUpdateTransaction` setters

**Description:**  
The documentation for several setter methods in
`FileUpdateTransaction.h` is slightly inconsistent with other transaction
headers.

Review the documentation for:
- `setContents`
- `setKeys`
- `setFileMemo`

Update comments for clarity and consistency without changing method
signatures or behavior.

**Scope**
- `src/sdk/main/include/FileUpdateTransaction.h`

**Acceptance Criteria**
- Documentation matches the style used in similar transaction headers
- No functional or API changes are made
- No new includes or dependencies are introduced

### Why this is a Beginner Issue

This issue requires:
- Reading existing code and related transaction headers
- Making small judgment calls about documentation consistency
- Preserving existing behavior and API surface

It does **not** require architectural knowledge, API design, or cross-module
changes.

---

## Support

Beginner Issues are supported through:

- Community discussion (issues and PR comments)
- Maintainer guidance when clarification is needed

Support is intended to:

- Clarify intent
- Confirm understanding of existing behavior
- Review proposed changes

Support does **not** include:

- Designing solutions
- Making decisions on behalf of the contributor
- Defining scope that is not already stated

---

## Maintainer Guidance

Label an issue as **Beginner** if it:

- Builds naturally on Good First Issues
- Requires reading existing code to understand context
- Allows small, low-risk decisions
- Has clear intent and well-defined boundaries
