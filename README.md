# Hiero C++ SDK

![](https://img.shields.io/badge/c++-17-blue)
![](https://img.shields.io/badge/cmake-3.24-blue)
[![](https://github.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/flow-pull-request-checks.yaml/badge.svg)](https://github.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/flow-pull-request-checks.yaml)

The C++ SDK for interacting with a Hiero network.

## Prerequisites

For MacOS and Linux users:

- ninja
  - MacOS: `brew install ninja`
  - Linux: `apt-get install ninja`
- pkg-config
  - MacOS: `brew install pkg-config`
  - Linux: `apt-get install pkg-config`
- cmake
  - MacOS: `brew install cmake`
  - Linux: `apt-get install cmake`

📣 **Note**: Ensure you install all three `ninja`, `pkg-config`, and `cmake` to avoid errors in subsequent steps. The
installations might take a few minutes.

For Windows users:

- Visual Studio 2019/2022 Community/Pro with Universal Windows Platform Development Tools
- [Perl](http://strawberryperl.com/) (`perl.exe` must be added to `%PATH%`)
- [NASM](https://www.nasm.us) (`nasm.exe` must be added to `%PATH%`)

## Build

This project features CMake Presets which simplify the setup of vcpkg based dependencies. The below commands are
typically all that is required.

1. Clone the project

```sh
git clone https://github.com/hiero-ledger/hiero-sdk-cpp.git
```

2. CD to the project directory

```sh
cd hiero-sdk-cpp
```

3. You can configure and build the project for various platforms using CMake presets. Additionally, optional components like TCK and Tests can be included in the build using flags.

```sh
# Ensure the VCPkg Submodule is initialized
git submodule update --init

# Windows (x64) Build
cmake --preset windows-x64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON
cmake --build --preset windows-x64-release

# Linux (x64) Build
cmake --preset linux-x64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON
cmake --build --preset linux-x64-release

# MacOS (Intel x64) Build
cmake --preset macos-x64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON
cmake --build --preset macos-x64-release

# MacOS (Aarch64) Build
cmake --preset macos-arm64-release -DBUILD_TCK=ON -DBUILD_TESTS=ON
cmake --build --preset macos-arm64-release
```

Optional Build Flags
The project supports the following optional flags to include or exclude specific components:

`BUILD_TCK`

```
Description: Controls whether the TCK tests are included in the build.
Default: OFF
Enable: Add -DBUILD_TCK=ON during configuration.
```

`BUILD_TESTS`

```
Description: Controls whether the test suite is included in the build.
Default: OFF
Enable: Add -DBUILD_TESTS=ON during configuration.
```

`BUILD_EXAMPLES`

```
Description: Controls whether the user examples are included in the build.
Default: OFF
Enable: Add -DBUILD_EXAMPLES=ON during configuration.
```

## Testing

To run all SDK tests (for Release or Debug builds):

```sh
ctest -C [Release|Debug] --test-dir build/<PRESET>
```

To run all SDK unit tests and test vectors:

```sh
ctest -C [Release|Debug] --test-dir build/<PRESET> -R "TestVectors|UnitTests"
```

To run all SDK integration tests:

```sh
ctest -C [Release|Debug] --test-dir build/<PRESET> -R "IntegrationTests"
```

To run a specific test:

```sh
ctest -C [Release|Debug] --test-dir build/<PRESET> -R <NAME OF TEST>
```

### Running Integration Tests

To run the integration tests it's necessary to have a
running [Hedera Local Node](https://github.com/hashgraph/hedera-local-node) (The project will be transfered to Hiero in near future). If the local node is already running, check
the configuration JSON file for the network settings. Ensure the values for `network` tag contains a valid `AccountId`
and a valid `IP address` for an operational node.

Example config file:

```JSON
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

(Source: [config/local_node.json](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/config/local_node.json))

## Examples

Examples must be run from the root directory in order to correctly access the address book and configuration files
located in the `addressbook/` and `config/` directories. Make sure your `.env` file is populated with:

- `OPERATOR_ID`: The ID of the operator account.
- `OPERATOR_KEY`: The DER-encoded hex private key of the operator account.
- `HIERO_NETWORK`: The Hiero network name. Must be one of `mainnet`, `testnet`, or `previewnet`.
- `PASSPHRASE`: Optional variable used by `hiero-sdk-cpp-generate-private-key-from-mnemonic-example` to generate a
  private key from a mnemonic with a passphrase.

The command to run an example looks like:

```shell
build/<PRESET>/sdk/examples/[Release|Debug]/<EXAMPLE_NAME>
```

- `<PRESET>`: the preset that was used to build in Step 3 under [Build](#build).
- `[Release|Debug]`: `Release` if you built in `Release` mode, otherwise `Debug`.
- `<EXAMPLE_NAME>`: The name of the example you are trying to run.

If you're trying to run an example from the release artifacts, you must first `cd` into the architecture folder of the
OS on which you are trying to run the example. For example, if you are running an `x86_64` architecture on `Linux`:

```shell
cd [Release|Debug]/Linux/x86_64
```

From there, you can run:

```shell
examples/<EXAMPLE_NAME>
```

**NOTE:** Make sure you copy your `.env` file with your environment variables into this folder as well.

Additionally, the examples can be run using the `run_examples` scripts(`.sh` for macOS, Linux/ `.bat` for Windows systems) from the project root directory. In the scripts you will find an `EXECUTABLES_DIRECTORY` variable.

```
EXECUTABLES_DIRECTORY = <build_folder_with_exec_binaries>
```

Make sure to set it to the proper build folder of the binaries after building the project.

## Contributing to this Project

Whether you’re fixing bugs, enhancing features, or improving documentation, your contributions are important — let’s build something great together!

For instructions on how to contribute to this repo, please
review the [Contributing Guide for C++](CONTRIBUTING.md).

More instructions for contribution can be found in the
[Global Contributing Guide](https://github.com/hiero-ledger/.github/blob/main/CONTRIBUTING.md).

## Code of Conduct

Hiero uses the Linux Foundation Decentralised Trust [Code of Conduct](https://www.lfdecentralizedtrust.org/code-of-conduct).

## License

[Apache License 2.0](LICENSE)
