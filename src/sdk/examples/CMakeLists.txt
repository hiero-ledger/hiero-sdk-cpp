FetchContent_Declare(
        dotenv-cpp
        GIT_REPOSITORY https://github.com/laserpants/dotenv-cpp.git
        GIT_TAG master
)
FetchContent_MakeAvailable(dotenv-cpp)
include_directories(${dotenv-cpp_SOURCE_DIR}/include/laserpants/dotenv)

set(ACCOUNT_ALIAS_EXAMPLE_NAME ${PROJECT_NAME}-account-alias-example)
set(ACCOUNT_ALLOWANCE_EXAMPLE_NAME ${PROJECT_NAME}-account-allowance-example)
set(ACCOUNT_ALLOWANCE_SERIALIZED_EXAMPLE_NAME ${PROJECT_NAME}-account-allowance-serialized-example)
set(ACCOUNT_CREATE_WITH_HTS_EXAMPLE_NAME ${PROJECT_NAME}-account-create-with-hts-example)
set(ACCOUNT_CREATION_WAYS_EXAMPLE_NAME ${PROJECT_NAME}-account-creation-ways-example)
set(AUTO_CREATE_ACCOUNT_TRANSFER_TRANSACTION_EXAMPLE_NAME ${PROJECT_NAME}-auto-create-account-transfer-transaction-example)
set(CONSENSUS_PUB_SUB_EXAMPLE_NAME ${PROJECT_NAME}-consensus-pub-sub-example)
set(CONSENSUS_PUB_SUB_CHUNKED_EXAMPLE_NAME ${PROJECT_NAME}-consensus-pub-sub-chunked-example)
set(CONSENSUS_PUB_SUB_WITH_SUBMIT_KEY_EXAMPLE_NAME ${PROJECT_NAME}-consensus-pub-sub-with-submit-key-example)
set(CONSTRUCT_CLIENT_EXAMPLE_NAME ${PROJECT_NAME}-construct-client-example)
set(CREATE_ACCOUNT_EXAMPLE_NAME ${PROJECT_NAME}-create-account-example)
set(CREATE_FILE_EXAMPLE_NAME ${PROJECT_NAME}-create-file-example)
set(CREATE_ACCOUNT_THRESHOLD_KEY_EXAMPLE_NAME ${PROJECT_NAME}-create-account-threshold-key-example)
set(CREATE_SIMPLE_CONTRACT_EXAMPLE_NAME ${PROJECT_NAME}-create-simple-contract-example)
set(CREATE_STATEFUL_CONTRACT_EXAMPLE_NAME ${PROJECT_NAME}-create-stateful-contract-example)
set(CREATE_TOPIC_EXAMPLE_NAME ${PROJECT_NAME}-create-topic-example)
set(CONTRACT_NONCES_EXAMPLE_NAME ${PROJECT_NAME}-contract-nonces-example)
set(CUSTOM_FEES_EXAMPLE_NAME ${PROJECT_NAME}-custom-fees-example)
set(DELETE_ACCOUNT_EXAMPLE_NAME ${PROJECT_NAME}-delete-account-example)
set(DELETE_FILE_EXAMPLE_NAME ${PROJECT_NAME}-delete-file-example)
set(EXEMPT_CUSTOM_FEES_EXAMPLE_NAME ${PROJECT_NAME}-exempt-custom-fees-example)
set(FILE_APPEND_CHUNKED_EXAMPLE_NAME ${PROJECT_NAME}-file-append-chunked-example)
set(GENERATE_KEY_EXAMPLE_NAME ${PROJECT_NAME}-generate-key-example)
set(GENERATE_PRIVATE_KEY_FROM_MNEMONIC_EXAMPLE_NAME ${PROJECT_NAME}-generate-private-key-from-mnemonic-example)
set(GENERATE_PRIVATE_KEY_FROM_SPECIFIC_MNEMONIC_EXAMPLE_NAME ${PROJECT_NAME}-generate-private-key-from-specific-mnemonic-example)
set(GET_ACCOUNT_BALANCE_EXAMPLE_NAME ${PROJECT_NAME}-get-account-balance-example)
set(GET_ACCOUNT_INFO_EXAMPLE_NAME ${PROJECT_NAME}-get-account-info-example)
set(GET_ADDRESS_BOOK_EXAMPLE_NAME ${PROJECT_NAME}-get-address-book-example)
set(GET_EXCHANGE_RATES_EXAMPLE_NAME ${PROJECT_NAME}-get-exchange-rates-example)
set(GET_FILE_CONTENTS_EXAMPLE_NAME ${PROJECT_NAME}-get-file-contents-example)
set(INITIALIZE_CLIENT_WITH_MIRROR_NODE_ADDRESS_BOOK_EXAMPLE_NAME
        ${PROJECT_NAME}-initialize-client-with-mirror-node-address-book-example)
set(LONG_TERM_SCHEDULED_TRANSACTION_EXAMPLE_NAME ${PROJECT_NAME}-long-term-scheduled-transaction-example)
set(MULTI_APP_TRANSFER_EXAMPLE_NAME ${PROJECT_NAME}-multi-app-transfer-example)
set(MULTI_SIG_OFFLINE_EXAMPLE_NAME ${PROJECT_NAME}-multi-sig-offline-example)
set(NFT_ADD_REMOVE_ALLOWANCES_EXAMPLE_NAME ${PROJECT_NAME}-nft-add-remove-allowances-example)
set(PRNG_EXAMPLE_NAME ${PROJECT_NAME}-prng-example)
set(SCHEDULE_EXAMPLE_NAME ${PROJECT_NAME}-schedule-example)
set(SCHEDULE_IDENTICAL_TRANSACTION_EXAMPLE_NAME ${PROJECT_NAME}-schedule-identical-transaction-example)
set(SCHEDULE_MULTI_SIG_TRANSACTION_EXAMPLE_NAME ${PROJECT_NAME}-schedule-multisig-transaction-example)
set(SCHEDULE_TRANSACTION_MULTI_SIG_TRANSACTION_EXAMPLE_NAME ${PROJECT_NAME}-schedule-transaction-multisig-threshold-example)
set(SCHEDULE_TRANSFER_EXAMPLE_NAME ${PROJECT_NAME}-schedule-transfer-example)
set(SIGN_TRANSACTION_EXAMPLE_NAME ${PROJECT_NAME}-sign-transaction-example)
set(SOLIDITY_PRECOMPILE_EXAMPLE_NAME ${PROJECT_NAME}-solidity-precompile-example)
set(STAKING_EXAMPLE_NAME ${PROJECT_NAME}-staking-example)
set(STAKING_WITH_UPDATE_EXAMPLE_NAME ${PROJECT_NAME}-staking-with-update-example)
set(TOKEN_AIRDROP_EXAMPLE_NAME ${PROJECT_NAME}-token-airdrop-example)
set(TOKEN_CREATE_FREEZE_DELETE_EXAMPLE_NAME ${PROJECT_NAME}-token-create-freeze-delete-example)
set(TOKEN_METADATA_EXAMPLE_NAME ${PROJECT_NAME}-token-metadata-example)
set(TOPIC_WITH_ADMIN_KEY_EXAMPLE_NAME ${PROJECT_NAME}-topic-with-admin-key-example)
set(TRANSFER_CRYPTO_EXAMPLE_NAME ${PROJECT_NAME}-transfer-crypto-example)
set(TRANSFER_TOKENS_EXAMPLE_NAME ${PROJECT_NAME}-transfer-tokens-example)
set(TRANSFER_USING_EVM_ADDRESS_EXAMPLE_NAME ${PROJECT_NAME}-transfer-using-evm-address-example)
set(UPDATE_ACCOUNT_PUBLIC_KEY_EXAMPLE_NAME ${PROJECT_NAME}-update-account-public-key-example)
set(VALIDATE_CHECKSUM_EXAMPLE_NAME ${PROJECT_NAME}-validate-checksum-example)
set(ZERO_TOKEN_OPERATIONS_EXAMPLE_NAME ${PROJECT_NAME}-zero-token-operations-example)

add_executable(${ACCOUNT_ALIAS_EXAMPLE_NAME} AccountAliasExample.cc)
add_executable(${ACCOUNT_ALLOWANCE_EXAMPLE_NAME} AccountAllowanceExample.cc)
add_executable(${ACCOUNT_ALLOWANCE_SERIALIZED_EXAMPLE_NAME} AccountAllowanceSerializedExample.cc)
add_executable(${ACCOUNT_CREATE_WITH_HTS_EXAMPLE_NAME} AccountCreateWithHtsExample.cc)
add_executable(${ACCOUNT_CREATION_WAYS_EXAMPLE_NAME} AccountCreationWaysExample.cc)
add_executable(${AUTO_CREATE_ACCOUNT_TRANSFER_TRANSACTION_EXAMPLE_NAME} AutoCreateAccountTransferTransactionExample.cc)
add_executable(${CONSENSUS_PUB_SUB_EXAMPLE_NAME} ConsensusPubSubExample.cc)
add_executable(${CONSENSUS_PUB_SUB_CHUNKED_EXAMPLE_NAME} ConsensusPubSubChunkedExample.cc)
add_executable(${CONSENSUS_PUB_SUB_WITH_SUBMIT_KEY_EXAMPLE_NAME} ConsensusPubSubWithSubmitKeyExample.cc)
add_executable(${CONSTRUCT_CLIENT_EXAMPLE_NAME} ConstructClientExample.cc)
add_executable(${CREATE_ACCOUNT_EXAMPLE_NAME} CreateAccountExample.cc)
add_executable(${CREATE_FILE_EXAMPLE_NAME} CreateFileExample.cc)
add_executable(${CREATE_ACCOUNT_THRESHOLD_KEY_EXAMPLE_NAME} CreateAccountThresholdKeyExample.cc)
add_executable(${CREATE_SIMPLE_CONTRACT_EXAMPLE_NAME} CreateSimpleContractExample.cc)
add_executable(${CREATE_STATEFUL_CONTRACT_EXAMPLE_NAME} CreateStatefulContractExample.cc)
add_executable(${CREATE_TOPIC_EXAMPLE_NAME} CreateTopicExample.cc)
add_executable(${CONTRACT_NONCES_EXAMPLE_NAME} ContractNoncesExample.cc)
add_executable(${CUSTOM_FEES_EXAMPLE_NAME} CustomFeesExample.cc)
add_executable(${DELETE_ACCOUNT_EXAMPLE_NAME} DeleteAccountExample.cc)
add_executable(${DELETE_FILE_EXAMPLE_NAME} DeleteFileExample.cc)
add_executable(${EXEMPT_CUSTOM_FEES_EXAMPLE_NAME} ExemptCustomFeesExample.cc)
add_executable(${FILE_APPEND_CHUNKED_EXAMPLE_NAME} FileAppendChunkedExample.cc)
add_executable(${GENERATE_KEY_EXAMPLE_NAME} GenerateKeyExample.cc)
add_executable(${GENERATE_PRIVATE_KEY_FROM_MNEMONIC_EXAMPLE_NAME} GeneratePrivateKeyFromMnemonicExample.cc)
add_executable(${GENERATE_PRIVATE_KEY_FROM_SPECIFIC_MNEMONIC_EXAMPLE_NAME} GeneratePrivateKeyFromSpecificMnemonicExample.cc)
add_executable(${GET_ACCOUNT_BALANCE_EXAMPLE_NAME} GetAccountBalanceExample.cc)
add_executable(${GET_ACCOUNT_INFO_EXAMPLE_NAME} GetAccountInfoExample.cc)
add_executable(${GET_ADDRESS_BOOK_EXAMPLE_NAME} GetAddressBookExample.cc)
add_executable(${GET_EXCHANGE_RATES_EXAMPLE_NAME} GetExchangeRatesExample.cc)
add_executable(${GET_FILE_CONTENTS_EXAMPLE_NAME} GetFileContentsExample.cc)
add_executable(${INITIALIZE_CLIENT_WITH_MIRROR_NODE_ADDRESS_BOOK_EXAMPLE_NAME} InitializeClientWithMirrorNodeAddressBookExample.cc)
add_executable(${LONG_TERM_SCHEDULED_TRANSACTION_EXAMPLE_NAME} LongTermScheduledTransactionExample.cc)
add_executable(${MULTI_APP_TRANSFER_EXAMPLE_NAME} MultiAppTransferExample.cc)
add_executable(${MULTI_SIG_OFFLINE_EXAMPLE_NAME} MultiSigOfflineExample.cc)
add_executable(${NFT_ADD_REMOVE_ALLOWANCES_EXAMPLE_NAME} NftAddRemoveAllowancesExample.cc)
add_executable(${PRNG_EXAMPLE_NAME} PrngExample.cc)
add_executable(${SCHEDULE_EXAMPLE_NAME} ScheduleExample.cc)
add_executable(${SCHEDULE_IDENTICAL_TRANSACTION_EXAMPLE_NAME} ScheduleIdenticalTransactionExample.cc)
add_executable(${SCHEDULE_MULTI_SIG_TRANSACTION_EXAMPLE_NAME} ScheduleMultiSigTransactionExample.cc)
add_executable(${SCHEDULE_TRANSACTION_MULTI_SIG_TRANSACTION_EXAMPLE_NAME} ScheduleTransactionMultiSigThresholdExample.cc)
add_executable(${SCHEDULE_TRANSFER_EXAMPLE_NAME} ScheduleTransferExample.cc)
add_executable(${SIGN_TRANSACTION_EXAMPLE_NAME} SignTransactionExample.cc)
add_executable(${SOLIDITY_PRECOMPILE_EXAMPLE_NAME} SolidityPrecompileExample.cc)
add_executable(${STAKING_EXAMPLE_NAME} StakingExample.cc)
add_executable(${STAKING_WITH_UPDATE_EXAMPLE_NAME} StakingWithUpdateExample.cc)
add_executable(${TOKEN_AIRDROP_EXAMPLE_NAME} TokenAirdropExample.cc)
add_executable(${TOKEN_CREATE_FREEZE_DELETE_EXAMPLE_NAME} TokenCreateFreezeDeleteExample.cc)
add_executable(${TOKEN_METADATA_EXAMPLE_NAME} TokenMetadataExample.cc)
add_executable(${TOPIC_WITH_ADMIN_KEY_EXAMPLE_NAME} TopicWithAdminKeyExample.cc)
add_executable(${TRANSFER_CRYPTO_EXAMPLE_NAME} TransferCryptoExample.cc)
add_executable(${TRANSFER_TOKENS_EXAMPLE_NAME} TransferTokensExample.cc)
add_executable(${TRANSFER_USING_EVM_ADDRESS_EXAMPLE_NAME} TransferUsingEvmAddressExample.cc)
add_executable(${UPDATE_ACCOUNT_PUBLIC_KEY_EXAMPLE_NAME} UpdateAccountPublicKeyExample.cc)
add_executable(${VALIDATE_CHECKSUM_EXAMPLE_NAME} ValidateChecksumExample.cc)
add_executable(${ZERO_TOKEN_OPERATIONS_EXAMPLE_NAME} ZeroTokenOperationsExample.cc)

file(COPY ${PROJECT_SOURCE_DIR}/addressbook/mainnet.pb
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
file(COPY ${PROJECT_SOURCE_DIR}/addressbook/previewnet.pb
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
file(COPY ${PROJECT_SOURCE_DIR}/addressbook/testnet.pb
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

file(COPY ${PROJECT_SOURCE_DIR}/config/hello_world.json
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
file(COPY ${PROJECT_SOURCE_DIR}/config/local_node.json
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
file(COPY ${PROJECT_SOURCE_DIR}/config/stateful.json
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

file(COPY precompile-example DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})

# Install gRPC's roots.pem file for Windows with the examples so that it can be easily referenced.
if (WIN32)
    file(COPY ${PROJECT_SOURCE_DIR}/vcpkg/packages/grpc_x64-windows/share/grpc/roots.pem
            DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif ()

target_link_libraries(${ACCOUNT_ALIAS_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${ACCOUNT_ALLOWANCE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${ACCOUNT_ALLOWANCE_SERIALIZED_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${ACCOUNT_CREATE_WITH_HTS_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${ACCOUNT_CREATION_WAYS_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${AUTO_CREATE_ACCOUNT_TRANSFER_TRANSACTION_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CONSENSUS_PUB_SUB_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CONSENSUS_PUB_SUB_CHUNKED_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CONSENSUS_PUB_SUB_WITH_SUBMIT_KEY_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CONSTRUCT_CLIENT_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CREATE_ACCOUNT_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CREATE_FILE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CREATE_ACCOUNT_THRESHOLD_KEY_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CREATE_SIMPLE_CONTRACT_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CREATE_STATEFUL_CONTRACT_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CREATE_TOPIC_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CONTRACT_NONCES_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${CUSTOM_FEES_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${DELETE_ACCOUNT_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${DELETE_FILE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${EXEMPT_CUSTOM_FEES_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${FILE_APPEND_CHUNKED_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GENERATE_KEY_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GENERATE_PRIVATE_KEY_FROM_MNEMONIC_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GENERATE_PRIVATE_KEY_FROM_SPECIFIC_MNEMONIC_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GET_ACCOUNT_BALANCE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GET_ACCOUNT_INFO_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GET_ADDRESS_BOOK_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GET_EXCHANGE_RATES_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${GET_FILE_CONTENTS_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${INITIALIZE_CLIENT_WITH_MIRROR_NODE_ADDRESS_BOOK_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${LONG_TERM_SCHEDULED_TRANSACTION_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${MULTI_APP_TRANSFER_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${MULTI_SIG_OFFLINE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${NFT_ADD_REMOVE_ALLOWANCES_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${PRNG_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${SCHEDULE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${SCHEDULE_IDENTICAL_TRANSACTION_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${SCHEDULE_MULTI_SIG_TRANSACTION_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${SCHEDULE_TRANSACTION_MULTI_SIG_TRANSACTION_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${SCHEDULE_TRANSFER_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${SIGN_TRANSACTION_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${SOLIDITY_PRECOMPILE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${STAKING_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${STAKING_WITH_UPDATE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${TOKEN_AIRDROP_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${TOKEN_CREATE_FREEZE_DELETE_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${TOKEN_METADATA_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${TOPIC_WITH_ADMIN_KEY_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${TRANSFER_CRYPTO_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${TRANSFER_TOKENS_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${TRANSFER_USING_EVM_ADDRESS_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${UPDATE_ACCOUNT_PUBLIC_KEY_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${VALIDATE_CHECKSUM_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})
target_link_libraries(${ZERO_TOKEN_OPERATIONS_EXAMPLE_NAME} PUBLIC ${PROJECT_NAME})

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE} DESTINATION examples FILES_MATCHING PATTERN *)

install(TARGETS
        ${ACCOUNT_ALIAS_EXAMPLE_NAME}
        ${ACCOUNT_ALLOWANCE_EXAMPLE_NAME}
        ${ACCOUNT_CREATE_WITH_HTS_EXAMPLE_NAME}
        ${ACCOUNT_CREATION_WAYS_EXAMPLE_NAME}
        ${AUTO_CREATE_ACCOUNT_TRANSFER_TRANSACTION_EXAMPLE_NAME}
        ${CONSENSUS_PUB_SUB_EXAMPLE_NAME}
        ${CONSENSUS_PUB_SUB_CHUNKED_EXAMPLE_NAME}
        ${CONSENSUS_PUB_SUB_WITH_SUBMIT_KEY_EXAMPLE_NAME}
        ${CONSTRUCT_CLIENT_EXAMPLE_NAME}
        ${CREATE_ACCOUNT_EXAMPLE_NAME}
        ${CREATE_FILE_EXAMPLE_NAME}
        ${CREATE_ACCOUNT_THRESHOLD_KEY_EXAMPLE_NAME}
        ${CREATE_SIMPLE_CONTRACT_EXAMPLE_NAME}
        ${CREATE_STATEFUL_CONTRACT_EXAMPLE_NAME}
        ${CREATE_TOPIC_EXAMPLE_NAME}
        ${CONTRACT_NONCES_EXAMPLE_NAME}
        ${CUSTOM_FEES_EXAMPLE_NAME}
        ${DELETE_ACCOUNT_EXAMPLE_NAME}
        ${DELETE_FILE_EXAMPLE_NAME}
        ${EXEMPT_CUSTOM_FEES_EXAMPLE_NAME}
        ${FILE_APPEND_CHUNKED_EXAMPLE_NAME}
        ${GENERATE_KEY_EXAMPLE_NAME}
        ${GENERATE_PRIVATE_KEY_FROM_MNEMONIC_EXAMPLE_NAME}
        ${GENERATE_PRIVATE_KEY_FROM_SPECIFIC_MNEMONIC_EXAMPLE_NAME}
        ${GET_ACCOUNT_BALANCE_EXAMPLE_NAME}
        ${GET_ACCOUNT_INFO_EXAMPLE_NAME}
        ${GET_ADDRESS_BOOK_EXAMPLE_NAME}
        ${GET_EXCHANGE_RATES_EXAMPLE_NAME}
        ${GET_FILE_CONTENTS_EXAMPLE_NAME}
        ${INITIALIZE_CLIENT_WITH_MIRROR_NODE_ADDRESS_BOOK_EXAMPLE_NAME}
        ${LONG_TERM_SCHEDULED_TRANSACTION_EXAMPLE_NAME}
        ${MULTI_APP_TRANSFER_EXAMPLE_NAME}
        ${MULTI_SIG_OFFLINE_EXAMPLE_NAME}
        ${NFT_ADD_REMOVE_ALLOWANCES_EXAMPLE_NAME}
        ${PRNG_EXAMPLE_NAME}
        ${SCHEDULE_EXAMPLE_NAME}
        ${SCHEDULE_IDENTICAL_TRANSACTION_EXAMPLE_NAME}
        ${SCHEDULE_MULTI_SIG_TRANSACTION_EXAMPLE_NAME}
        ${SCHEDULE_TRANSACTION_MULTI_SIG_TRANSACTION_EXAMPLE_NAME}
        ${SCHEDULE_TRANSFER_EXAMPLE_NAME}
        ${SIGN_TRANSACTION_EXAMPLE_NAME}
        ${SOLIDITY_PRECOMPILE_EXAMPLE_NAME}
        ${STAKING_EXAMPLE_NAME}
        ${STAKING_WITH_UPDATE_EXAMPLE_NAME}
        ${TOKEN_AIRDROP_EXAMPLE_NAME}
        ${TOPIC_WITH_ADMIN_KEY_EXAMPLE_NAME}
        ${TRANSFER_CRYPTO_EXAMPLE_NAME}
        ${TRANSFER_TOKENS_EXAMPLE_NAME}
        ${TRANSFER_USING_EVM_ADDRESS_EXAMPLE_NAME}
        ${UPDATE_ACCOUNT_PUBLIC_KEY_EXAMPLE_NAME}
        ${VALIDATE_CHECKSUM_EXAMPLE_NAME}
        ${ZERO_TOKEN_OPERATIONS_EXAMPLE_NAME}
        RUNTIME DESTINATION examples)