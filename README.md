# Hiero C++ SDK

![](https://img.shields://img.shields.io/badge/cmakegithub.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/flow-pull-request-checksgithub.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/flow-pull-request-checkscard](https://api.scorecard.dev/projects/github.com/hiero-ledger/hiero-sdk-cpp/badgeer/?uri=github.com/hiero-ledger Practices](https://bestpractices.coreinfrastructure.org://bestpractices.coreinfrastructure](https://img.shields.io/badge/license-apache2-blue.svg++ SDK for interacting with a Hiero network. This SDK provides a robust interface for building applications on the Hiero distributed ledger, supporting features like transactions, queries, and consensus services.

## Prerequisites

Ensure your system meets the following requirements before building the SDK.

### macOS and Linux
- **Ninja**: Install via Homebrew on macOS (`brew install ninja`) or apt on Linux (`sudo apt-get install ninja-build`).
- **pkg-config**: Install via Homebrew on macOS (`brew install pkg-config`) or apt on Linux (`sudo apt-get install pkg-config`).
- **CMake**: Version 3.24 or higher. Install via Homebrew on macOS (`brew install cmake`) or apt on Linux (`sudo apt-get install cmake`).

**Note**: Install all three tools (`ninja`, `pkg-config`, and `cmake`) to avoid configuration errors. The process may take a few minutes.

### Windows
- **Visual Studio**: 2019 or 2022 (Community or Pro edition) with Universal Windows Platform Development Tools.
- **Perl**: Download from [Strawberry Perl](http://strawberryperl.com/) and add `perl.exe` to your PATH.
- **NASM**: Download from [NASM](https://www.nasm.us) and add `nasm.exe` to your PATH.

## Building the SDK

This project uses CMake Presets for simplified setup and vcpkg for dependency management. Follow these steps to build:

1. Clone the repository:
   ```sh
   git clone https://github.com/hiero-ledger/hiero-sdk-cpp.git
   ```

2. Navigate to the project directory:
   ```sh
   cd hiero-sdk-cpp
   ```

3. Initialize the vcpkg submodule:
   ```sh
   git submodule update --init
   ```

4. Configure and build using a CMake preset. Optionally include components like TCK or tests with flags:
   - **Windows (x64)**:
     ```sh
     cmake --preset windows-x64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
     cmake --build --preset windows-x64-release
     ```
   - **Linux (x64)**:
     ```sh
     cmake --preset linux-x64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
     cmake --build --preset linux-x64-release
     ```
   - **macOS (Intel x64)**:
     ```sh
     cmake --preset macos-x64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
     cmake --build --preset macos-x64-release
     ```
   - **macOS (ARM64)**:
     ```sh
     cmake --preset macos-arm64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON
     cmake --build --preset macos-arm64-release
     ```

### Optional Build Flags
- **BUILD_TCK** (default: OFF): Enables TCK tests. Use `-DBUILD_TCK=ON`.
- **BUILD_TESTS** (default: OFF): Enables the test suite. Use `-DBUILD_TESTS=ON`.
- **BUILD_EXAMPLES** (default: OFF): Builds user examples. Use `-DBUILD_EXAMPLES=ON`.

## Testing

Run tests from the build directory using CTest. Specify the build configuration (Release or Debug) as needed.

- Run all SDK tests:
  ```sh
  ctest -C [Release|Debug] --test-dir build/
  ```

- Run unit tests and test vectors:
  ```sh
  ctest -C [Release|Debug] --test-dir build/ -R "TestVectors|UnitTests"
  ```

- Run integration tests:
  ```sh
  ctest -C [Release|Debug] --test-dir build/ -R "IntegrationTests"
  ```

- Run a specific test:
  ```sh
  ctest -C [Release|Debug] --test-dir build/ -R 
  ```

### Running Integration Tests
Integration tests require a running [Hedera Local Node](https://github.com/hashgraph/hedera-local-node) (transitioning to Hiero). Provide a configuration JSON file with network details, such as:
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
Use this config (e.g., from `config/local_node.json`) when running tests.

## Examples

Examples demonstrate SDK usage and must be run from the project root to access `addressbook/` and `config/` directories. Populate a `.env` file with:
- `OPERATOR_ID`: Operator account ID.
- `OPERATOR_KEY`: DER-encoded hex private key.
- `HIERO_NETWORK`: Network name (`mainnet`, `testnet`, or `previewnet`).
- `PASSPHRASE` (optional): For mnemonic-based key generation examples.

Run an example:
```sh
build//sdk/examples/[Release|Debug]/
```
- ``: Your build preset (e.g., `linux-x64-release`).
- `[Release|Debug]`: Matches your build mode.
- ``: The example binary (e.g., `hiero-sdk-cpp-generate-private-key-from-mnemonic-example`).

For release artifacts, navigate to the OS/architecture folder (e.g., `[Release|Debug]/Linux/x86_64`), copy your `.env` file there, and run `examples/`.

Alternatively, use the `run_examples` scripts (`.sh` for macOS/Linux, `.bat` for Windows) from the root. Set `EXECUTABLES_DIRECTORY` in the script to your build folder.

## Contributing

Contributions are welcome for bug fixes, features, or documentation improvements. Review the [C++ Contributing Guide](CONTRIBUTING.md) and the [Global Contributing Guide](https://github.com/hiero-ledger/.github/blob/main/CONTRIBUTING.md) for details.

## Community and Support

- Join discussions on [Discord](https://discord.lfdecentralizedtrust.org/).

## About Users and Maintainers

Guidelines for users and maintainers are in [Hiero Ledger's roles and groups document](https://github.com/hiero-ledger/governance/blob/main/roles-and-groups.md#maintainers).

## Code of Conduct

Hiero follows the Linux Foundation Decentralized Trust [Code of Conduct](https://www.lfdecentralizedtrust.org/code-of-conduct).

## License

[Apache License 2.0](LICENSE)

[1] https://img.shields.io/badge/c++-17-blue
[2] https://img.shields.io/badge/cmake-3.24-blue
[3] https://github.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/flow-pull-request-checks.yaml/badge.svg
[4] https://github.com/hiero-ledger