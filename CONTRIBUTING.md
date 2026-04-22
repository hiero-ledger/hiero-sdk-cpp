# Contributing to the Hiero SDK for C++

Thank you for your interest in contributing the Hiero SDK for C++!

We appreciate your interest in helping us and the rest of our community. We welcome bug reports, feature requests, and
code contributions.

**Jump To:**
- [Code Contributions](#code-contributions)
- [Submitting an Issue](#submitting-an-issue)
- [Blog Posts](#blog-posts)


## Code Contributions

### Finding an Issue

Browse [open, unassigned issues](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue+is%3Aopen+no%3Aassignee+label%3A%22status%3A+ready+for+dev%22) to find one that matches your skill level:

| Skill Level | Description | Prerequisite |
|-------------|-------------|--------------|
| **Good First Issue** | Step-by-step tasks for newcomers | None |
| **Beginner** | Small improvements with some exploration | 2 completed Good First Issues |
| **Intermediate** | Multi-file changes with design decisions | 3 completed Beginner Issues |
| **Advanced** | Architecture and design-heavy work | 3 completed Intermediate Issues |

Look for issues with the `status: ready for dev` label — these have been triaged and are ready to be worked on.

### Getting Assigned

To claim an issue, comment on it with:

```
/assign
```

The bot will automatically:
1. Verify you meet the skill-level prerequisites
2. Assign you to the issue
3. Update the issue labels
4. Post a welcome message

If you don't meet the prerequisites, the bot will show your progress and link to issues you can work on first.

### Submitting Your Work

Once assigned, follow the [Issue Workflow Guide](docs/training/workflow.md) to:
1. Fork the repository and create a branch
2. Make your changes
3. Sign your commits (`-s -S`)
4. Open a pull request

Note:
- The SDK is released under the [Apache 2.0 License][license]. Any code you submit will be released under this license.
- Pull requests **cannot be merged** without signed commits. See the [Signing Guide](docs/training/signing.md).

## Submitting an Issue

Not sure which issue type to use? See the [Issue Types Guide](docs/contributing/issue-types.md) for a full breakdown
with examples of what belongs in each category.

### Bug Reports

⚠️ **Ensure you are using the latest release of the SDK** — it's possible the bug is already fixed.

1. Visit the [C++ SDK Issues page][issues]
2. ⚠️ **Check the bug is not already reported.** If it is, comment to confirm you're also experiencing it.
3. Click **New Issue** and choose the **Bug Report** template.

The template will guide you through providing a description, steps to reproduce, expected vs. actual behavior, and
environment details. The more reproducible the report, the faster a fix can land.

Security vulnerabilities should be disclosed responsibly via our [bug bounty program](https://hedera.com/bounty)
rather than as a public issue.

### Feature Requests

**Note:** If you intend to implement a feature yourself, please submit the request _before_ writing any code and
ask to be assigned. Features are for user-facing SDK capabilities — new transactions, queries, API methods, etc.
Improvements to tooling, CI, or the contribution process are [Tasks](#tasks) instead.

1. Visit the [C++ SDK Issues page][issues]
2. Verify the feature has not already been proposed.
3. Click **New Issue** and choose the **Feature Request** template.

For large or protocol-level changes, consider opening a
[Hiero Improvement Proposal](https://github.com/hiero-ledger/hiero-improvement-proposals) or a
[GitHub Discussion](https://github.com/hiero-ledger/hiero-sdk-cpp/discussions) first.

### Tasks

Use a Task for maintenance, improvement, or operational work: refactoring, dependency updates, documentation
improvements, CI/CD changes, test coverage, or enhancements to existing features.

1. Visit the [C++ SDK Issues page][issues]
2. Verify the work has not already been proposed.
3. Click **New Issue** and choose the **Task** template.

### After Submitting

All three templates automatically apply `status: awaiting triage`. A maintainer will review the issue, apply the
appropriate skill level and priority labels, and run `/finalize` to prepare it for contributors. Once finalized,
the issue will have `status: ready for dev` and can be claimed via `/assign`.

[issues]: https://github.com/hiero-ledger/hiero-sdk-cpp/issues
[mwe]: https://en.wikipedia.org/wiki/Minimal_Working_Example
[markdown]: https://guides.github.com/features/mastering-markdown/
[pull-requests]: https://github.com/hiero-ledger/hiero-sdk-cpp/pulls
[license]: https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/LICENSE
[discord]: https://hedera.com/discord

## Blog Posts

We welcome blog posts about the C++ SDK 

For example:
- Documenting New Releases and Functionality
- Documenting Contributor Activity
- Documenting Development Use-cases

[Guide on How to Create Blog Posts](docs/training/blogs.md)
