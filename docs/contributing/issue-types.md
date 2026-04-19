# Issue Types — Hiero C++ SDK

## Overview

When submitting an issue to the Hiero C++ SDK, you'll be asked to choose one of three issue types:

| Type | Use when... |
|---|---|
| **Bug** | Something isn't working correctly |
| **Feature** | You want a capability that doesn't exist yet |
| **Task** | Maintenance, improvement, or operational work |

Each type has its own submission template to help you provide the right level of detail.

> **Note:** You do not need to assess how difficult an issue is to implement — that's determined by maintainers during triage. Simply choose the type that best describes the work.

---

## Bug

A **bug** is an unintended behavior, crash, incorrect output, or regression in the SDK.

Use the **Bug** template when the SDK does something it shouldn't do, or fails to do something it should.

### Examples of bugs

- A method returns an incorrect value or throws unexpectedly
- A transaction executes successfully when it should have been rejected
- A query returns stale or malformed data
- The SDK crashes or throws an unhandled exception
- A previously working feature stopped working after an update
- The build fails on a supported platform
- A test fails in CI without any relevant code change

### Not a bug

- A request for behavior the SDK has never supported → that's a **Feature**
- A desire to improve performance without a regression → that's a **Task**
- Something you'd like to work differently as a matter of preference → that might be a **Feature** or **Task**

### What makes a good bug report

A useful bug report includes:
- A clear description of what went wrong
- Steps that reliably reproduce the issue
- The expected vs. actual behavior
- Environment details (SDK version, OS, compiler)

The more reproducible the report, the faster a fix can land.

---

## Feature

A **feature** is a new capability for **consumers of the SDK** — developers building applications on Hiero — that does not currently exist.

Use the **Feature** template when you want the SDK to do something it has never done before, and that something is user-facing: it would show up in the SDK's public API documentation or release notes as something an application developer can now do.

A useful test: *Would this change appear in the SDK's public API documentation or release notes as a user-visible capability?* If yes, it's likely a Feature. If the change improves the SDK's development process, tooling, or contributor experience rather than what application developers can do with it, it's a **Task**.

### Examples of features

- A new transaction type not yet implemented (e.g. a new HIP)
- A new query type or query mode
- A new public API method or configuration option
- A new example program demonstrating an SDK capability
- Support for a new network or environment type

### Not a feature

- An improvement to an existing feature → that's a **Task** (`kind: enhancement`)
- A fix for broken existing behavior → that's a **Bug**
- A refactor that doesn't change the public API → that's a **Task** (`kind: refactor`)
- A new bot command, CI workflow, issue template, or contributor tooling addition → that's a **Task** (`kind: maintenance`), even if it's genuinely "new" — it improves the development process, not the SDK's public capabilities

### What makes a good feature request

A useful feature request focuses on the problem being solved, not just the solution. Ask yourself:

- Why does this capability need to exist?
- Who benefits, and how?
- Is there a workaround today, and why is it insufficient?

Large or protocol-level feature requests may benefit from a
[Hiero Improvement Proposal (HIP)](https://github.com/hiero-ledger/hiero-improvement-proposals)
or a [GitHub Discussion](https://github.com/hiero-ledger/hiero-sdk-cpp/discussions) before a formal issue.

---

## Task

A **task** is maintenance, improvement, or operational work that keeps the SDK healthy and easy to contribute to.

Use the **Task** template when the work isn't a bug fix and doesn't add new public API surface — it improves the existing codebase.

### Examples of tasks

| What you want to do | `kind:` label to use |
|---|---|
| Improve or clarify documentation or code comments | `kind: documentation` |
| Refactor existing code without changing behavior | `kind: refactor` |
| Update a dependency or build tooling | `kind: maintenance` |
| Improve CI/CD pipelines or GitHub Actions workflows | `kind: maintenance` |
| Add or improve tests | `kind: testing` |
| Improve performance of existing functionality | `kind: enhancement` |
| Improve an existing feature without adding new API | `kind: enhancement` |
| Address a security concern that isn't a regression | `kind: security` |

> **Note:** The `kind:` label is applied by maintainers during triage — you don't need to choose it when submitting. Just describe what needs to be done and why.

### Not a task

- Something that is broken → that's a **Bug**
- A new capability that doesn't currently exist → that's a **Feature**

### What makes a good task report

A useful task report includes:
- A clear description of the current state and why it should change
- A concrete proposed approach
- Step-by-step implementation guidance (if you know it)

Even if you don't know the full implementation, describing the problem clearly is enough to get started.

---

## After Submitting

When you submit an issue using any of the three templates, it is automatically labeled `status: awaiting triage`. This signals to maintainers that the issue needs review.

A maintainer will then:

1. Review the issue for completeness
2. Apply the appropriate **skill level**, **priority**, **kind** (for bugs and tasks), and **scope** labels
3. Run `/finalize`, which updates the issue title and description to the standard format and marks it `status: ready for dev`

Once an issue is `status: ready for dev`, any contributor meeting the skill prerequisites can comment `/assign` to pick it up.

For more on how contribution works, see the [Workflow Guide](../training/workflow.md).
