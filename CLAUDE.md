# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

Uses CMake with vcpkg for dependency management. Build presets are defined in `CMakePresets.json`.

**Available presets:** `linux-x64-release`, `linux-x64-debug`, `macos-x64-release`, `macos-x64-debug`, `macos-arm64-release`, `macos-arm64-debug`, `windows-x64-release`, `windows-x64-debug`

```bash
# Configure (add -DBUILD_TESTS=ON to include tests)
cmake --preset linux-x64-debug -DBUILD_TESTS=ON -DBUILD_TCK_TESTS=ON

# Build
cmake --build -j 6 --preset linux-x64-debug
```

**Build flags:**
- `BUILD_TESTS=ON` — unit and integration tests
- `BUILD_TCK=ON` — TCK server
- `BUILD_EXAMPLES=ON` — example programs
- `BUILD_TCK_TESTS=ON` — TCK server unit tests

## Running Tests

```bash
# All tests
ctest -j 6 -C Debug --test-dir build/linux-x64-debug

# Exclude slow integration tests (as CI does)
ctest -j 6 -C Debug --test-dir build/linux-x64-debug -E NodeUpdateTransactionIntegrationTests

# Run a specific test by name
ctest -C Debug --test-dir build/linux-x64-debug -R <TestName>

# Show output on failure
ctest -C Debug --test-dir build/linux-x64-debug --output-on-failure
```

Integration tests require a running Hiero network (Solo). Config lives in `config/local_node.json`.

## Code Style

- **Formatter:** clang-format (v17), config in `.clang-format`
- **Linter:** clang-tidy, config in `.clang-tidy` (CERT, bugprone, modernize, performance, readability checks)
- **Standard:** C++17

## Architecture

The SDK provides a C++ client for the Hiero distributed ledger network.

**Core flow:** Application → `Client` → (gRPC to consensus nodes for transactions, HTTP/REST to mirror nodes for queries) → Hiero Network

**Key abstractions:**

- **`Client`** (`src/sdk/main/include/Client.h`) — Central class. Manages network connections, operator account, fee configuration, and executes all transactions/queries. All requests flow through here.

- **`Transaction<T>` / `Executable<SdkRequestT, ProtoRequestT, ProtoResponseT, SdkResponseT>`** — Base templates for all transaction and query types. Transactions serialize to protobuf, sign with keys, and submit via gRPC. Queries may use either gRPC (consensus nodes) or REST (mirror nodes).

- **`Query<T>`** — Base for all query types. Mirror node queries use HTTP; consensus node queries use gRPC.

**Directory layout:**
```
src/sdk/main/
  include/          # Public headers — entity IDs, transactions, queries, response types
  include/impl/     # Internal headers — network layer, ASN1, OpenSSL wrappers, utilities
  src/              # Implementation (.cc files mirror include/ structure)
src/sdk/tests/
  unit/             # GTest unit tests (no network required)
  integration/      # GTest integration tests (require live Hiero network)
src/sdk/examples/   # ~66 standalone example programs
src/tck/            # Test Compatibility Kit server
proto/              # Protobuf definitions (services, platform, block, mirror)
addressbook/        # Network address books for mainnet/testnet/previewnet
config/             # JSON config files for examples and integration tests
```

**Cryptography:** ED25519 and ECDSAsecp256k1 keys with BIP39 mnemonic and SLIP10 derivation. OpenSSL is wrapped in `src/sdk/main/include/impl/openssl_utils/` and `impl/ASN1*.h`.

**Dependencies** (managed via vcpkg): OpenSSL, gRPC, Protobuf, abseil-cpp, nlohmann_json, log4cxx, re2, ZLIB.

**Protobuf generation:** `HieroApi.cmake` fetches Hiero API `.proto` files at configure time and generates C++ sources into the build directory.
