# Contributing to the Hiero SDK for C++

Thank you for your interest in contributing the Hiero SDK for C++!

We appreciate your interest in helping us and the rest of our community. We welcome bug reports, feature requests, and
code contributions.

**Jump To:**
- [Code Contributions](#code-contributions)
- [Submitting an Issue](#submitting-an-issue)
- [Release New Version](#release-new-version)
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

## Release New Version

### Step 1 - Populate New Tag

Prepare a new git `tag` and push it to `origin`.

Add a new git `tag` using the following command in terminal where `v0.3.0` is the name of the newly added tag:

> git tag -s v0.3.0

Then execute:

> git push origin v0.3.0

It will populate the newly added tag in `origin`.

### Step 2 - Draft Release in GitHub

Open [Hiero C++ SDK](https://github.com/hiero-ledger/hiero-sdk-cpp) and search for `Create a new release` link.
Select tag `v0.3.0` from the dropdown list. Enter the Release title to be same as the release number which is been
prepared (for example: `v0.3.0`).

### Step 3 - Notes

It's a good decision to tap directly on button `Generate release notes` to get an automatically generated notes which
can be used as a basis.

#### What's New

This section should be filled with all of the issues which have been completed and merged into `main` branch. Use the
emoji ➡️ followed by a description in plain English for each over-arching new feature and a bug fix.

#### Examples

This section should list all of the newly added examples. Each of them should represent a link to the implementation (
for example: [TransferTokensExample.cpp](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/src/sdk/examples/TransferTokensExample.cpp) ). It's a good idea also to add prefix with ➡️ before
each line.

#### Notes

Section to write any kind of important notes. For example, if there are any files which are not included in the build
and should not be used, then it's better all of them to be listed here.

#### Added

Here should be listed all of the Pull Requests (PRs) which are part of the release. Use the following format

> (pull-request-title) by (@author) in (#link)

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
