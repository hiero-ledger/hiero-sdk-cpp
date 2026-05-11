#!/usr/bin/env bash
# SPDX-License-Identifier: Apache-2.0
#
# run_examples.sh – Run compiled SDK examples and report pass/fail.
#
# Usage:
#   ./run_examples.sh [OPTIONS]
#
# Options:
#   -b, --build-dir DIR       Path to the preset build directory
#                             Default: build/linux-x64-debug
#   -c, --config CFG          CMake build configuration (Debug | Release)
#                             Default: Debug
#   -C, --category CAT        Which category to run: offline | network | all
#                             Default: all
#   -t, --timeout SECONDS     Per-example timeout in seconds
#                             Default: 120
#   -e, --env-file FILE       .env file to source before running network examples
#                             Default: .env (project root)
#   -h, --help                Show this help message
#
# Exit code:
#   0  – all examples passed
#   1  – one or more examples failed
#
# Examples:
#   ./run_examples.sh --category offline
#   ./run_examples.sh --category network --env-file /tmp/solo.env
#   ./run_examples.sh --build-dir build/linux-x64-release --config Release

set -euo pipefail

# ── Defaults ──────────────────────────────────────────────────────────────────
BUILD_DIR="build/linux-x64-debug"
CONFIG="Debug"
CATEGORY="all"
TIMEOUT_S=120
ENV_FILE=".env"

# ── Argument parsing ───────────────────────────────────────────────────────────
while [[ $# -gt 0 ]]; do
  case "$1" in
    -b|--build-dir)  BUILD_DIR="$2";  shift 2 ;;
    -c|--config)     CONFIG="$2";     shift 2 ;;
    -C|--category)   CATEGORY="$2";   shift 2 ;;
    -t|--timeout)    TIMEOUT_S="$2";  shift 2 ;;
    -e|--env-file)   ENV_FILE="$2";   shift 2 ;;
    -h|--help)
      sed -n '4,30p' "$0"
      exit 0
      ;;
    *) echo "Unknown option: $1" >&2; exit 1 ;;
  esac
done

# ── Validate ───────────────────────────────────────────────────────────────────
EXAMPLES_BIN_DIR="${BUILD_DIR}/src/sdk/examples/${CONFIG}"

if [[ ! -d "${EXAMPLES_BIN_DIR}" ]]; then
  echo "ERROR: Examples binary directory not found: ${EXAMPLES_BIN_DIR}" >&2
  echo "       Build the project first with: cmake --build --preset <preset>" >&2
  exit 1
fi

if [[ "${CATEGORY}" != "offline" && "${CATEGORY}" != "network" && "${CATEGORY}" != "all" ]]; then
  echo "ERROR: --category must be one of: offline | network | all" >&2
  exit 1
fi

# ── Example lists ──────────────────────────────────────────────────────────────
# These must match the CTest registrations in src/sdk/examples/CMakeLists.txt.

OFFLINE_EXAMPLES=(
  "hiero-sdk-cpp-account-creation-ways-example"
  "hiero-sdk-cpp-generate-key-example"
  "hiero-sdk-cpp-generate-private-key-from-mnemonic-example"
  "hiero-sdk-cpp-generate-private-key-from-specific-mnemonic-example"
  "hiero-sdk-cpp-version-example"
  "hiero-sdk-cpp-construct-client-example"
)

NETWORK_EXAMPLES=(
  "hiero-sdk-cpp-account-alias-example"
  "hiero-sdk-cpp-account-allowance-example"
  "hiero-sdk-cpp-account-allowance-serialized-example"
  "hiero-sdk-cpp-account-create-with-hts-example"
  "hiero-sdk-cpp-account-hooks-example"
  "hiero-sdk-cpp-auto-create-account-transfer-transaction-example"
  "hiero-sdk-cpp-batch-transaction-example"
  "hiero-sdk-cpp-contract-hooks-example"
  "hiero-sdk-cpp-contract-nonces-example"
  "hiero-sdk-cpp-create-account-example"
  "hiero-sdk-cpp-create-account-threshold-key-example"
  "hiero-sdk-cpp-create-file-example"
  "hiero-sdk-cpp-create-simple-contract-example"
  "hiero-sdk-cpp-create-stateful-contract-example"
  "hiero-sdk-cpp-create-topic-example"
  "hiero-sdk-cpp-custom-fees-example"
  "hiero-sdk-cpp-delete-account-example"
  "hiero-sdk-cpp-delete-file-example"
  "hiero-sdk-cpp-exempt-custom-fees-example"
  "hiero-sdk-cpp-file-append-chunked-example"
  "hiero-sdk-cpp-get-account-balance-example"
  "hiero-sdk-cpp-get-account-info-example"
  "hiero-sdk-cpp-get-address-book-example"
  "hiero-sdk-cpp-get-exchange-rates-example"
  "hiero-sdk-cpp-get-file-contents-example"
  "hiero-sdk-cpp-grpc-deadline-example"
  "hiero-sdk-cpp-hook-store-example"
  "hiero-sdk-cpp-initialize-client-with-mirror-node-address-book-example"
  "hiero-sdk-cpp-long-term-scheduled-transaction-example"
  "hiero-sdk-cpp-mirror-node-contract-query-example"
  "hiero-sdk-cpp-multi-app-transfer-example"
  "hiero-sdk-cpp-multi-sig-offline-example"
  "hiero-sdk-cpp-nft-add-remove-allowances-example"
  "hiero-sdk-cpp-prng-example"
  "hiero-sdk-cpp-revenue-generating-topics-example"
  "hiero-sdk-cpp-schedule-example"
  "hiero-sdk-cpp-schedule-identical-transaction-example"
  "hiero-sdk-cpp-schedule-multisig-transaction-example"
  "hiero-sdk-cpp-schedule-network-update-example"
  "hiero-sdk-cpp-schedule-transaction-multisig-threshold-example"
  "hiero-sdk-cpp-schedule-transfer-example"
  "hiero-sdk-cpp-sign-transaction-example"
  "hiero-sdk-cpp-solidity-precompile-example"
  "hiero-sdk-cpp-staking-example"
  "hiero-sdk-cpp-staking-with-update-example"
  "hiero-sdk-cpp-token-airdrop-example"
  "hiero-sdk-cpp-token-create-freeze-delete-example"
  "hiero-sdk-cpp-token-metadata-example"
  "hiero-sdk-cpp-topic-with-admin-key-example"
  "hiero-sdk-cpp-transfer-crypto-example"
  "hiero-sdk-cpp-transfer-tokens-example"
  "hiero-sdk-cpp-transfer-transaction-hooks-example"
  "hiero-sdk-cpp-transfer-using-evm-address-example"
  "hiero-sdk-cpp-update-account-public-key-example"
  "hiero-sdk-cpp-zero-token-operations-example"
)

SKIPPED_EXAMPLES=(
  "hiero-sdk-cpp-consensus-pub-sub-example               [infinite loop]"
  "hiero-sdk-cpp-consensus-pub-sub-chunked-example       [infinite loop]"
  "hiero-sdk-cpp-consensus-pub-sub-with-submit-key-example [infinite loop]"
  "hiero-sdk-cpp-validate-checksum-example               [interactive stdin]"
)

# ── Colour helpers ─────────────────────────────────────────────────────────────
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BOLD='\033[1m'
RESET='\033[0m'

# ── Run one example with a timeout ─────────────────────────────────────────────
# Returns 0 (pass), 1 (fail), 2 (timeout).
run_example() {
  local binary="$1"
  local path="${EXAMPLES_BIN_DIR}/${binary}"

  if [[ ! -x "${path}" ]]; then
    echo -e "  ${YELLOW}MISSING${RESET}  ${binary} (binary not found, skipping)"
    return 1
  fi

  # Run from the binary directory so dotenv finds its .env file.
  local output
  if output=$(cd "${EXAMPLES_BIN_DIR}" && timeout "${TIMEOUT_S}" "./${binary}" 2>&1); then
    echo -e "  ${GREEN}PASS${RESET}     ${binary}"
    return 0
  else
    local exit_code=$?
    if [[ ${exit_code} -eq 124 ]]; then
      echo -e "  ${RED}TIMEOUT${RESET}  ${binary} (>${TIMEOUT_S}s)"
      return 2
    fi
    echo -e "  ${RED}FAIL${RESET}     ${binary} (exit ${exit_code})"
    echo "${output}" | sed 's/^/             /'
    return 1
  fi
}

# ── Main ───────────────────────────────────────────────────────────────────────
echo ""
echo -e "${BOLD}════════════════════════════════════════════════════════${RESET}"
echo -e "${BOLD}  Hiero SDK C++ – Example Runner${RESET}"
echo -e "${BOLD}════════════════════════════════════════════════════════${RESET}"
echo -e "  Build dir : ${CYAN}${EXAMPLES_BIN_DIR}${RESET}"
echo -e "  Category  : ${CYAN}${CATEGORY}${RESET}"
echo -e "  Timeout   : ${CYAN}${TIMEOUT_S}s${RESET} per example"
echo ""

PASS=0
FAIL=0
SKIP=0

# Load .env for network examples if available
if [[ "${CATEGORY}" == "network" || "${CATEGORY}" == "all" ]]; then
  if [[ -f "${ENV_FILE}" ]]; then
    echo -e "${CYAN}Sourcing credentials from ${ENV_FILE}${RESET}"
    # Copy the .env file into the examples binary directory so dotenv picks it up
    cp "${ENV_FILE}" "${EXAMPLES_BIN_DIR}/.env"
  else
    echo -e "${YELLOW}WARNING: .env file not found at '${ENV_FILE}'.${RESET}"
    echo -e "${YELLOW}         Network examples will likely fail without credentials.${RESET}"
  fi
  echo ""
fi

# ── Offline ────────────────────────────────────────────────────────────────────
if [[ "${CATEGORY}" == "offline" || "${CATEGORY}" == "all" ]]; then
  echo -e "${BOLD}── Offline Examples ──────────────────────────────────────${RESET}"
  # Set HIERO_NETWORK for ConstructClientExample
  export HIERO_NETWORK="testnet"
  for binary in "${OFFLINE_EXAMPLES[@]}"; do
    run_example "${binary}" && PASS=$((PASS+1)) || FAIL=$((FAIL+1))
  done
  unset HIERO_NETWORK
  echo ""
fi

# ── Network ────────────────────────────────────────────────────────────────────
if [[ "${CATEGORY}" == "network" || "${CATEGORY}" == "all" ]]; then
  echo -e "${BOLD}── Network Examples ──────────────────────────────────────${RESET}"
  for binary in "${NETWORK_EXAMPLES[@]}"; do
    run_example "${binary}" && PASS=$((PASS+1)) || FAIL=$((FAIL+1))
  done
  echo ""
fi

# ── Skipped ────────────────────────────────────────────────────────────────────
echo -e "${BOLD}── Skipped (not CI-compatible) ───────────────────────────${RESET}"
for entry in "${SKIPPED_EXAMPLES[@]}"; do
  echo -e "  ${YELLOW}SKIP${RESET}     ${entry}"
  SKIP=$((SKIP+1))
done
echo ""

# ── Summary ────────────────────────────────────────────────────────────────────
echo -e "${BOLD}════════════════════════════════════════════════════════${RESET}"
echo -e "${BOLD}  Results${RESET}"
echo -e "  ${GREEN}PASS${RESET}   : ${PASS}"
echo -e "  ${RED}FAIL${RESET}   : ${FAIL}"
echo -e "  ${YELLOW}SKIP${RESET}   : ${SKIP}"
echo -e "${BOLD}════════════════════════════════════════════════════════${RESET}"
echo ""

if [[ ${FAIL} -gt 0 ]]; then
  echo -e "${RED}${BOLD}BUILD SUCCESS but ${FAIL} example(s) FAILED.${RESET}"
  exit 1
else
  echo -e "${GREEN}${BOLD}All examples PASSED.${RESET}"
  exit 0
fi
