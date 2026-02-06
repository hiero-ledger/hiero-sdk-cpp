# Hiero C++ SDK

![](https://img.shields.io/badge/c++-17-blue)
![](https://img.shields.io/badge/cmake-3.15+-blue)
[![](https://github.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/flow-pull-request-checks.yaml/badge.svg)](https://github.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/flow-pull-request-checks.yaml)
[![OpenSSF Scorecard](https://api.scorecard.dev/projects/github.com/hiero-ledger/hiero-sdk-cpp/badge)](https://scorecard.dev/viewer/?uri=github.com/hiero-ledger/hiero-sdk-cpp)
[![CII Best Practices](https://bestpractices.coreinfrastructure.org/projects/10697/badge)](https://bestpractices.coreinfrastructure.org/projects/10697)
[![License](https://img.shields.io/badge/license-apache2-blue.svg)](LICENSE)

The C++ SDK for interacting with a [Hiero](https://hiero.org) network.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Build](#build)
- [Testing](#testing)
- [Examples](#examples)
- [Contributing](#contributing)
- [Support](#support)
- [License](#license)

## Prerequisites

### macOS and Linux

- **ninja**
  - macOS: `brew install ninja`
  - Linux: `apt-get install ninja-build`
- **pkg-config**
  - macOS: `brew install pkg-config`
  - Linux: `apt-get install pkg-config`
- **cmake** (3.15 or higher)
  - macOS: `brew install cmake`
  - Linux: `apt-get install cmake`

> **Note**: Ensure you install all three dependencies (`ninja`, `pkg-config`, and `cmake`) to avoid errors in subsequent steps. The installations might take a few minutes.

### Windows

- [Visual Studio 2022 Community/Pro](https://visualstudio.microsoft.com/vs/community/) with the **"Desktop development with C++"** workload installed
- [CMake](https://cmake.org/download/) (select "Add CMake to the system PATH" during installation)
- [Perl](http://strawberryperl.com/) (`perl.exe` must be added to `%PATH%`)
- [NASM](https://www.nasm.us) (`nasm.exe` must be added to `%PATH%`)

#### Run
```powershell
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
```

> [!NOTE]
> Please make sure to run the commands in **x64 Native Tools Command Prompt for VS 2022**

## Build

This project uses CMake Presets which simplify the setup of vcpkg-based dependencies.

### Quick Start

1. Clone the repository

```sh
git clone https://github.com/hiero-ledger/hiero-sdk-cpp.git
cd hiero-sdk-cpp
```

2. Initialize the vcpkg submodule

```sh
git submodule update --init
```

3. Configure and build for your platform

```sh
# Windows (x64)
cmake --preset windows-x64-release
cmake --build --preset windows-x64-release

# Linux (x64)
cmake --preset linux-x64-release
cmake --build --preset linux-x64-release

# macOS (Intel x64)
cmake --preset macos-x64-release
cmake --build --preset macos-x64-release

# macOS (Apple Silicon arm64)
cmake --preset macos-arm64-release
cmake --build --preset macos-arm64-release
```

### Build Options

The following optional flags can be added during configuration:

| Flag | Default | Description |
|------|---------|-------------|
| `BUILD_TESTS` | `OFF` | Include the test suite in the build |
| `BUILD_TCK` | `OFF` | Include TCK tests in the build |
| `BUILD_EXAMPLES` | `OFF` | Include example programs in the build |
| `BUILD_TCK_TESTS`| `OFF` | Include TCK Server unit tests in the build |

Example with all options enabled:

```sh
cmake --preset linux-x64-release -DBUILD_TESTS=ON -DBUILD_TCK=ON -DBUILD_EXAMPLES=ON -DBUILD_TCK_TESTS=ON
cmake --build --preset linux-x64-release
```

## Testing

### Running Tests

To run all SDK tests:

```sh
ctest -C Release --test-dir build/<PRESET>
```

To run only unit tests and test vectors:

```sh
ctest -C Release --test-dir build/<PRESET> -R "TestVectors|UnitTests"
```

To run a specific test:

```sh
ctest -C Release --test-dir build/<PRESET> -R <TEST_NAME>
```

Replace `Release` with `Debug` if you built in debug mode.

### Integration Tests

Integration tests require a running Hiero network. You can use [Solo](https://solo.hiero.org) to spin up a local development network.

#### Setting Up Solo

Solo is a CLI tool for running a local Hiero network for development and testing. For setup instructions, see the [Solo documentation](https://solo.hiero.org) or the [solo repository](https://github.com/hiero-ledger/solo).

#### Configuration

Once your local network is running, verify the configuration in `config/local_node.json` matches your network settings:

```json
{
  "network": {
    "0.0.3": "127.0.0.1:50211"
  },
  "mirrorNetwork": [
    "127.0.0.1:5600"
  ],
  "operator": {
    "accountId": "0.0.2",
    "privateKey": "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"
  }
}
```

To run integration tests:

```sh
ctest -C Release --test-dir build/<PRESET> -R "IntegrationTests"
```

## Examples

Examples demonstrate various SDK features and must be run from the project root directory.

### Environment Setup

Create a `.env` file in the project root with the following variables:

| Variable | Description |
|----------|-------------|
| `OPERATOR_ID` | The ID of the operator account (e.g., `0.0.1234`) |
| `OPERATOR_KEY` | The DER-encoded hex private key of the operator account |
| `HIERO_NETWORK` | Network name: `mainnet`, `testnet`, or `previewnet` |
| `PASSPHRASE` | (Optional) Passphrase for mnemonic-based key generation |

### Running Examples

#### Mac
```C++
package/Release/Darwin/arm64/examples/Release/<EXAMPLE-NAME>
```

#### Windows
```C++
package\Release\Windows\AMD64\examples\Release\<EXAMPLE-NAME>
```

- `<PRESET>`: The CMake preset used during build (e.g., `linux-x64-release`)
- `<EXAMPLE_NAME>`: Name of the example to run

### Running from Release Artifacts

If running examples from release artifacts, first navigate to the appropriate architecture folder:

```sh
cd Release/Linux/x86_64
examples/<EXAMPLE_NAME>
```

> **Note**: Copy your `.env` file into the examples directory.

### Batch Execution

You can run all examples using the provided scripts:

- macOS/Linux: `run_examples.sh`
- Windows: `run_examples.bat`

Before running, update the `EXECUTABLES_DIRECTORY` variable in the script to point to your build output folder.

## Contributing

We welcome contributions! Whether you're fixing bugs, adding features, or improving documentation, your help is appreciated.

- [Contributing Guide](CONTRIBUTING.md)
- [Training Guides](docs/training/)

### Additional Resources

- [Discord Guide](docs/training/discord.md)
- [Blog Post Guide](docs/training/blogs.md)
- [Resolving Merge Conflicts](docs/training/merge-conflicts.md)
- [Signing Commits](docs/training/signing.md)
- [Rebasing Guide](docs/training/rebasing.md)
- [Workflow Guide](docs/training/workflow.md)

### Maintainers

Users and Maintainers guidelines are located in **[Hiero-Ledger's CONTRIBUTING.md file](https://github.com/hiero-ledger/.github/blob/main/CONTRIBUTING.md#about-users-and-maintainers)** under the "About-Users-and-Maintainers" section.

## Support

- [Discord](https://discord.gg/hyperledger)
- [GitHub Issues](https://github.com/hiero-ledger/hiero-sdk-cpp/issues)

## Code of Conduct

Hiero uses the Linux Foundation Decentralised Trust [Code of Conduct](https://www.lfdecentralizedtrust.org/code-of-conduct).

## License

[Apache License 2.0](LICENSE)
