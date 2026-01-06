# Advanced Issue Guidelines — Hiero C++ SDK

Advanced Issues represent **high-impact, high-responsibility work**.

They are intended for contributors with deep SDK knowledge who can design, evaluate trade-offs, and own long-term consequences.

---

## Assumptions

Advanced Issues assume contributors:

- Have deep SDK and domain expertise
- Understand performance, concurrency, and ABI concerns
- Can propose and defend designs
- Are comfortable with breaking-change discussions

---

## Characteristics

Advanced Issues:

- Are design-heavy
- Often affect multiple parts of the SDK
- Have long-term maintenance impact
- Require discussion and iteration

---

## Allowed Work

### ✅ Allowed
- Major architectural refactors
- New subsystems or abstractions
- Significant performance rework
- API changes with migration plans
- Protocol-level or cross-SDK alignment work

---

## Time & Scope

- ⏱ Estimated time: **3+ days**
- 📄 Scope: **Multi-module or repository-wide**

Advanced Issues are expected to evolve through discussion and may require
multiple review cycles.

---

## Example: Well-Formed Advanced Issue
**Title:** Implement HIP-1261 fee estimate query support in the C++ SDK

**Description:**  
The Hiero C++ SDK does not currently support fee estimate queries as defined in
HIP-1261. This limits the ability for developers to programmatically estimate
transaction fees prior to execution.

This issue involves **designing and implementing full SDK support** for
HIP-1261, including public APIs, internal handling, tests, and examples.

The implementation must align with the HIP specification and remain consistent
with patterns used across other SDKs.

Reference design document:  
https://github.com/hiero-ledger/sdk-collaboration-hub/blob/main/proposals/hips/hip-1261.md

---

## What to Do

1. Review HIP-1261 in full and understand the intended behavior and constraints.
2. Design the C++ SDK API surface needed to expose fee estimate queries.
3. Implement the feature across the SDK, including:
   - Public-facing query or transaction classes
   - Internal request/response handling
   - Validation and error handling
4. Add unit and integration tests covering expected behavior.
5. Provide at least one example demonstrating correct usage.


## Maintainer Guidance

Label an issue as **Advanced** if it:
- Requires system-level thinking
- Has long-term impact
- Needs experienced review and iteration
