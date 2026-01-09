# Contributing to the Hiero SDK for C++

Thank you for your interest in contributing the Hiero SDK for C++!

We appreciate your interest in helping us and the rest of our community. We welcome bug reports, feature requests, and
code contributions.

**Jump To:**
- [Code Contributions](#code-contributions)
- [Bug Reports](#bug-reports)
- [Feature Requests](#feature-requests)
- [Release New Version](#release-new-version)
- [Blog Posts](#blog-posts)


## Code Contributions

1. Get assigned to an [Open C++ SDK Issue](https://github.com/hiero-ledger/hiero-sdk-cpp/issues?q=is%3Aissue%20state%3Aopen%20no%3Aassignee)
2. Solve the issue and create a pull request following the [Issue Workflow Guide](training/workflow.md)

Note:
- The SDK is released under the [Apache 2.0 License][license]. Any code you submit will be released under this license.

## Feature Requests

**NOTE:** If you intend to implement a feature request, please submit the feature request _before_ working on any code
changes and ask to get assigned.

1. Visit [C++ SDK Issue](https://github.com/hiero-ledger/hiero-sdk-cpp/issues)
2. Verify the Feature Request is not already proposed.
2. Click 'New Issue' and click the Feature Request template.
**Ensure** the [New Feature][label-new-feature] or [Feature Enhancements][label-feature-enhancement] label is attached.

### Submitting a Feature Request

Open an [issue][issues] with the following:

- A short, descriptive title. Other community members should be able to understand the nature of the issue by reading
  this title.
- A detailed description of the the proposed feature. Explain why you believe it should be added to the SDK.
  Illustrative example code may also be provided to help explain how the feature should work.
- [Markdown][markdown] formatting as appropriate to make the request easier to read.
- If you plan to implement this feature yourself, please let us know that you'd like to the issue to be assigned to you.

## Bug Reports

⚠️ **Ensure you are using the latest release of the SDK**.

It's possible the bug is already fixed. We will do our utmost to maintain backwards compatibility between patch version releases, so that you can be
   confident that your application will continue to work as expected with the newer version.

1. Visit [C++ SDK Issue Page](https://github.com/hiero-ledger/hiero-sdk-cpp/issues)
2. ⚠️ **Check the Bug is not Already Reported**. If it is, comment to confirm you are also experiencing this bug.
3. Click 'New Issue' and choose the `Bug Report` template

**Ensure** the [bug][label-bug] label is attached.

Please ensure that your bug report contains the following:

- A short, descriptive title. Other community members should be able to understand the nature of the issue by reading
  this title.
- A succinct, detailed description of the problem you're experiencing. This should include:
  - Expected behavior of the SDK and the actual behavior exhibited.
  - Any details of your application development environment that may be relevant.
  - If applicable, the exception stack-trace.
  - If you are able to create one, include a [Minimal Working Example][mwe] that reproduces the issue.
- [Markdown][markdown] formatting as appropriate to make the report easier to read; for example use code blocks when
  pasting a code snippet or exception stack-trace.

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
for example: [TransferTokensExample.cc](<(https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/src/sdk/examples/TransferTokensExample.cc)>). It's a good idea also to add prefix with ➡️ before
each line.

#### Notes

Section to write any kind of important notes. For example, if there are any files which are not included in the build
and should not be used, then it's better all of them to be listed here.

#### Added

Here should be listed all of the Pull Requests (PRs) which are part of the release. Use the following format

> (pull-request-title) by (@author) in (#link)

[issues]: https://github.com/hiero-ledger/hiero-sdk-cpp/issues
[label-bug]: https://github.com/hiero-ledger/hiero-sdk-cpp/labels/bug
[mwe]: https://en.wikipedia.org/wiki/Minimal_Working_Example
[markdown]: https://guides.github.com/features/mastering-markdown/
[label-feature-enhancement]: https://github.com/hiero-ledger/hiero-sdk-cpp/labels/Feature%20Enhancement
[label-new-feature]: https://github.com/hiero-ledger/hiero-sdk-cpp/labels/New%20Feature
[pull-requests]: https://github.com/hiero-ledger/hiero-sdk-cpp/pulls
[license]: https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/LICENSE
[discord]: https://hedera.com/discord

## Blog Posts

We welcome blog posts about the C++ SDK

For example:
- Documenting New Releases and Functionality
- Documenting Contributor Activity
- Documenting Development Use-cases

[Guide on How to Create Blog Posts](training/blogs.md)
