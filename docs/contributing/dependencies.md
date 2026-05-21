# Dependency Management

This document describes how third-party dependencies are managed in the Hiero C++ SDK and the processes for keeping them up to date.

## Automated Monitoring (Dependabot)

The following dependency ecosystems are monitored automatically by [GitHub Dependabot](https://docs.github.com/en/code-security/dependabot/dependabot-version-updates) via [`.github/dependabot.yml`](../../.github/dependabot.yml):

| Ecosystem | Manifest file | Update cadence |
|---|---|---|
| GitHub Actions | `.github/workflows/*.yml` | Daily |
| vcpkg | `vcpkg.json` | Weekly |

Dependabot will open pull requests when newer versions are available. For vcpkg, this means updating the `builtin-baseline` commit hash in `vcpkg.json` so that all ports move to a newer curated snapshot.

## Manual Review Required — FetchContent Pins

Some dependencies are fetched at CMake configure time using `FetchContent_Declare` and are **not** monitored by any automated tooling. These pins must be reviewed manually.

### FetchContent dependencies

| Dependency | Declared in | Git repository | Current pin |
|---|---|---|---|
| **log4cxx** | [`src/sdk/main/CMakeLists.txt`](../../src/sdk/main/CMakeLists.txt) | https://github.com/apache/logging-log4cxx.git | `rel/v1.1.0` |
| **HProto** (Hiero API protobufs) | [`HieroApi.cmake`](../../HieroApi.cmake) | https://github.com/hiero-ledger/hiero-consensus-node.git | `v0.73.0` |
| **dotenv-cpp** | [`src/sdk/examples/CMakeLists.txt`](../../src/sdk/examples/CMakeLists.txt) | https://github.com/laserpants/dotenv-cpp.git | `a64ee31a34271f8703b5ad0c4e5417b2cc8bdb94` |

### Recommended review cadence

**Quarterly** (every 3 months) — A maintainer should check each upstream repository's releases page and compare the pinned tag or commit against the latest release. If a newer version is available and does not introduce breaking changes, open a PR to update the pin.

Suggested review months: **January, April, July, October**.

### How to update a FetchContent pin

1. Visit the upstream repository's **Releases** or **Tags** page.
2. Identify the latest stable release tag (or commit hash for `dotenv-cpp`).
3. Update the `GIT_TAG` value in the corresponding CMake file.
4. Build the project locally and run the test suite to verify compatibility.
5. Open a pull request with the updated pin.
