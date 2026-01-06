# Intermediate Issue Guidelines — Hiero C++ SDK

Intermediate Issues are for contributors who can **own a solution end-to-end**.

They require investigation, reasoning, and choosing between **multiple valid
implementation approaches**, while still operating within clearly defined goals.

Intermediate Issues are where contributors begin to **own technical decisions**,
not just execute instructions.

---

## Assumptions

Intermediate Issues assume contributors:

- Have intermediate C++ knowledge
- Understand the Hiero SDK architecture at a high level
- Can navigate and modify multiple related modules
- Can justify implementation choices in a pull request
- Are comfortable asking targeted questions and responding to review feedback

Intermediate Issues do **not** assume:

- System or architectural redesign authority
- Deep protocol or ABI expertise
- Unbounded exploratory work

---

## Characteristics

Intermediate Issues:

- Are **not fully scripted**
- Require investigation and understanding of existing behavior
- May span multiple related components or files
- Allow contributor judgment in how goals are achieved
- Require maintainer review of approach, not just correctness
- Are expected to have multiple reasonable solutions

> **Rule of thumb**  
> If a contributor must investigate existing behavior, evaluate trade-offs,
> and choose an implementation approach, it is an Intermediate Issue.

---

## Allowed Work

### ✅ Allowed

- Implementing new SDK features with clearly defined goals
- Refactoring existing code for maintainability or clarity
- Localized performance improvements with stated intent
- Adding new transaction or query types using existing patterns
- Improving async or concurrency behavior with maintainer guidance

### ❌ Not Allowed

- Large-scale or cross-SDK redesigns
- Introducing entirely new subsystems or frameworks
- Open-ended exploratory work without defined goals

---

## Time & Scope

- ⏱ **Estimated time:** 1–3 days  
- 📄 **Scope:** Multiple related files or modules  
- 🧠 **Cognitive load:** Investigation, trade-offs, and implementation ownership  

If an issue is expected to span large portions of the SDK, introduce new
architectural patterns, or require extended design discussion, it is **not**
an Intermediate Issue.

---

## Example: Well-Formed Intermediate Issue

### Consolidate `std::string_view` usage in selected SDK APIs

Some transaction setter methods in the Hiero C++ SDK accept
`const std::string&` even though they only **read** the value and immediately
copy it. In these cases, `std::string_view` is a lighter and more consistent
choice.

This issue focuses on **updating a small, specific set of setters** to improve
API consistency and reduce unnecessary string copies.

1. Start with the following transaction classes:
   - `FileUpdateTransaction`
   - `AccountUpdateTransaction`

2. In these classes, look for setter methods that:
   - Accept `const std::string&`
   - Do **not** store the string by reference
   - Copy the value into an internal field

3. Update those setter method signatures to accept:
   - `std::string_view` instead of `const std::string&`

4. Update the corresponding `.cpp` implementations as needed.

5. Ensure all existing behavior remains unchanged.

---

## Why This Is an Intermediate Issue

This task requires:
- Investigating existing code patterns across multiple files
- Reasoning about string ownership and lifetime
- Evaluating trade-offs between safety and API consistency
- Choosing an implementation approach and justifying it in review

It does **not** require architectural redesign or protocol-level changes.

---

## Maintainer Guidance

Label an issue as **Intermediate** if it:

- Requires investigation and trade-off analysis
- Has multiple reasonable implementation approaches
- Involves ownership of design decisions within defined goals
- Remains reviewable in a single pull request
- Does not introduce new subsystems or architectural patterns
