FetchContent_Declare(
        log4cxx
        GIT_REPOSITORY https://github.com/apache/logging-log4cxx.git
        GIT_TAG rel/v1.1.0
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")

set(BUILD_SHARED_LIBS OFF)
set(BUILD_TESTING OFF)
set(APR_STATIC ON)
set(APU_STATIC ON)

if (WIN32)
    set(APR_INCLUDE_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include/)
    set(APR_UTIL_INCLUDE_DIR ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/include/)
    set(APR_LIBRARIES ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/lib/libapr-1.lib)
    set(APR_UTIL_LIBRARIES ${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/lib/libaprutil-1.lib)
endif ()

FetchContent_MakeAvailable(log4cxx)

add_library(${PROJECT_NAME} STATIC
        src/AccountAllowanceApproveTransaction.cc
        src/AccountAllowanceDeleteTransaction.cc
        src/AccountBalance.cc
        src/AccountBalanceQuery.cc
        src/AccountCreateTransaction.cc
        src/AccountDeleteTransaction.cc
        src/AccountId.cc
        src/AccountInfo.cc
        src/AccountInfoQuery.cc
        src/AccountRecords.cc
        src/AccountRecordsQuery.cc
        src/AccountStakersQuery.cc
        src/AccountUpdateTransaction.cc
        src/AddressBookQuery.cc
        src/AssessedCustomFee.cc
        src/ChunkedTransaction.cc
        src/Client.cc
        src/ContractId.cc
        src/ContractByteCodeQuery.cc
        src/ContractCallQuery.cc
        src/ContractCreateFlow.cc
        src/ContractCreateTransaction.cc
        src/ContractDeleteTransaction.cc
        src/ContractExecuteTransaction.cc
        src/ContractFunctionParameters.cc
        src/ContractFunctionResult.cc
        src/ContractFunctionSelector.cc
        src/ContractInfo.cc
        src/ContractInfoQuery.cc
        src/ContractLogInfo.cc
        src/ContractNonceInfo.cc
        src/ContractUpdateTransaction.cc
        src/CustomFee.cc
        src/CustomFeeBase.cc
        src/CustomFixedFee.cc
        src/CustomFractionalFee.cc
        src/CustomRoyaltyFee.cc
        src/DelegateContractId.cc
        src/ECDSAsecp256k1PrivateKey.cc
        src/ECDSAsecp256k1PublicKey.cc
        src/ED25519PrivateKey.cc
        src/ED25519PublicKey.cc
        src/Endpoint.cc
        src/EthereumFlow.cc
        src/EthereumTransaction.cc
        src/EthereumTransactionData.cc
        src/EthereumTransactionDataEip1559.cc
        src/EthereumTransactionDataLegacy.cc
        src/EvmAddress.cc
        src/ExchangeRate.cc
        src/ExchangeRates.cc
        src/Executable.cc
        src/FeeComponents.cc
        src/FeeData.cc
        src/FeeDataType.cc
        src/FeeSchedule.cc
        src/FeeSchedules.cc
        src/FileAppendTransaction.cc
        src/FileContentsQuery.cc
        src/FileCreateTransaction.cc
        src/FileDeleteTransaction.cc
        src/FileId.cc
        src/FileInfo.cc
        src/FileInfoQuery.cc
        src/FileUpdateTransaction.cc
        src/FreezeTransaction.cc
        src/FreezeType.cc
        src/Hbar.cc
        src/HbarAllowance.cc
        src/HbarTransfer.cc
        src/IPv4Address.cc
        src/Key.cc
        src/KeyList.cc
        src/LedgerId.cc
        src/Logger.cc
        src/Mnemonic.cc
        src/MnemonicBIP39.cc
        src/NetworkVersionInfo.cc
        src/NetworkVersionInfoQuery.cc
        src/NftId.cc
        src/NodeAddress.cc
        src/NodeAddressBook.cc
        src/NodeCreateTransaction.cc
        src/NodeDeleteTransaction.cc
        src/NodeUpdateTransaction.cc
        src/PendingAirdropId.cc
        src/PendingAirdropRecord.cc
        src/PrivateKey.cc
        src/PrngTransaction.cc
        src/ProxyStaker.cc
        src/PublicKey.cc
        src/Query.cc
        src/RequestType.cc
        src/ScheduleCreateTransaction.cc
        src/ScheduleDeleteTransaction.cc
        src/ScheduleId.cc
        src/ScheduleInfo.cc
        src/ScheduleInfoQuery.cc
        src/ScheduleSignTransaction.cc
        src/SemanticVersion.cc
        src/StakingInfo.cc
        src/Status.cc
        src/SubscriptionHandle.cc
        src/SystemDeleteTransaction.cc
        src/SystemUndeleteTransaction.cc
        src/TokenAirdropTransaction.cc
        src/TokenAllowance.cc
        src/TokenAssociateTransaction.cc
        src/TokenAssociation.cc
        src/TokenBurnTransaction.cc
        src/TokenCancelAirdropTransaction.cc
        src/TokenClaimAirdropTransaction.cc
        src/TokenCreateTransaction.cc
        src/TokenDeleteTransaction.cc
        src/TokenDissociateTransaction.cc
        src/TokenFeeScheduleUpdateTransaction.cc
        src/TokenFreezeTransaction.cc
        src/TokenGrantKycTransaction.cc
        src/TokenId.cc
        src/TokenInfo.cc
        src/TokenInfoQuery.cc
        src/TokenKeyValidation.cc
        src/TokenMintTransaction.cc
        src/TokenNftAllowance.cc
        src/TokenNftInfo.cc
        src/TokenNftInfoQuery.cc
        src/TokenNftTransfer.cc
        src/TokenPauseTransaction.cc
        src/TokenRejectFlow.cc
        src/TokenRejectTransaction.cc
        src/TokenRelationship.cc
        src/TokenRevokeKycTransaction.cc
        src/TokenSupplyType.cc
        src/TokenTransfer.cc
        src/TokenType.cc
        src/TokenUnfreezeTransaction.cc
        src/TokenUnpauseTransaction.cc
        src/TokenUpdateNftsTransaction.cc
        src/TokenUpdateTransaction.cc
        src/TokenWipeTransaction.cc
        src/TopicCreateTransaction.cc
        src/TopicDeleteTransaction.cc
        src/TopicId.cc
        src/TopicInfo.cc
        src/TopicInfoQuery.cc
        src/TopicMessage.cc
        src/TopicMessageChunk.cc
        src/TopicMessageQuery.cc
        src/TopicMessageSubmitTransaction.cc
        src/TopicUpdateTransaction.cc
        src/Transaction.cc
        src/TransactionFeeSchedule.cc
        src/TransactionId.cc
        src/TransactionReceipt.cc
        src/TransactionReceiptQuery.cc
        src/TransactionRecord.cc
        src/TransactionRecordQuery.cc
        src/TransactionResponse.cc
        src/TransferTransaction.cc
        src/WrappedTransaction.cc

        src/impl/ASN1ECKey.cc
        src/impl/ASN1ECPrivateKey.cc
        src/impl/ASN1ECPublicKey.cc
        src/impl/ASN1ED25519PrivateKey.cc
        src/impl/ASN1ED25519PublicKey.cc
        src/impl/BaseNetwork.cc
        src/impl/BaseNode.cc
        src/impl/BaseNodeAddress.cc
        src/impl/DerivationPathUtils.cc
        src/impl/DurationConverter.cc
        src/impl/EntityIdHelper.cc
        src/impl/HederaCertificateVerifier.cc
        src/impl/HexConverter.cc
        src/impl/HttpClient.cc
        src/impl/MirrorNetwork.cc
        src/impl/MirrorNode.cc
        src/impl/MirrorNodeGateway.cc
        src/impl/MirrorNodeRouter.cc
        src/impl/Network.cc
        src/impl/Node.cc
        src/impl/OpenSSLUtils.cc
        src/impl/RLPItem.cc
        src/impl/TimestampConverter.cc
        src/impl/Utilities.cc)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC ${HAPI_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${HAPI_INCLUDE_DIR}/proto)
target_include_directories(${PROJECT_NAME} PUBLIC ${ZLIB_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${OPENSSL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${log4cxx_SOURCE_DIR}/src/main/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${log4cxx_BINARY_DIR}/src/main/include)

if (APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OSX_CORE_FOUNDATION})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OSX_CF_NETWORK})
    target_link_libraries(${PROJECT_NAME} PRIVATE aprutil-1 apr-1)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE hapi::proto)
target_link_libraries(${PROJECT_NAME} PRIVATE log4cxx)

if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE sys::resolv)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenSSL::SSL
        OpenSSL::Crypto)

target_link_libraries(${PROJECT_NAME} PRIVATE
        gRPC::address_sorting
        gRPC::gpr
        gRPC::grpc
        gRPC::grpc_plugin_support
        gRPC::grpc_unsecure
        protobuf::libprotobuf)

target_link_libraries(${PROJECT_NAME} PRIVATE
        gRPC::grpc++
        gRPC::grpc++_alts
        gRPC::grpc++_error_details
        gRPC::grpc++_unsecure)

if (TARGET gRPC::grpc++_reflection)
    target_link_libraries(${PROJECT_NAME} PRIVATE gRPC::grpc++_reflection)
endif ()

if (TARGET gRPC::grpcpp_channelz)
    target_link_libraries(${PROJECT_NAME} PRIVATE gRPC::grpcpp_channelz)
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        absl::bad_any_cast_impl
        absl::bad_optional_access
        absl::bad_variant_access
        absl::base
        absl::city
        absl::civil_time
        absl::cord
        absl::cord_internal
        absl::cordz_functions
        absl::cordz_handle
        absl::cordz_info
        absl::cordz_sample_token
        absl::debugging_internal
        absl::demangle_internal
        absl::examine_stack
        absl::exponential_biased
        absl::failure_signal_handler
        absl::flags
        absl::flags_commandlineflag
        absl::flags_commandlineflag_internal
        absl::flags_config
        absl::flags_internal
        absl::flags_marshalling
        absl::flags_parse
        absl::flags_private_handle_accessor
        absl::flags_program_name
        absl::flags_reflection
        absl::flags_usage
        absl::flags_usage_internal
        absl::graphcycles_internal
        absl::hash
        absl::hashtablez_sampler
        absl::int128
        absl::leak_check
        absl::log_severity
        absl::low_level_hash
        absl::malloc_internal
        absl::periodic_sampler
        absl::random_distributions
        absl::random_internal_distribution_test_util
        absl::random_internal_platform
        absl::random_internal_pool_urbg
        absl::random_internal_randen
        absl::random_internal_randen_hwaes
        absl::random_internal_randen_hwaes_impl
        absl::random_internal_randen_slow
        absl::random_internal_seed_material
        absl::random_seed_gen_exception
        absl::random_seed_sequences
        absl::raw_hash_set
        absl::raw_logging_internal
        absl::scoped_set_env
        absl::spinlock_wait
        absl::stacktrace
        absl::status
        absl::statusor
        absl::str_format_internal
        absl::strerror
        absl::strings
        absl::strings_internal
        absl::symbolize
        absl::synchronization
        absl::throw_delegate
        absl::time
        absl::time_zone)

target_link_libraries(${PROJECT_NAME} PRIVATE
        upb::upb
        upb::descriptor_upb_proto
        upb::extension_registry
        upb::fastdecode
        upb::json
        upb::mini_table
        upb::reflection
        upb::textformat
        upb::utf8_range)

target_link_libraries(${PROJECT_NAME} PRIVATE
        c-ares::cares
        re2::re2
        ${ZLIB_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES VERSIONS 0.1.0)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)

# Install Library & Headers
install(TARGETS ${PROJECT_NAME} ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin)
install(DIRECTORY include/ TYPE INCLUDE FILES_MATCHING PATTERN "*.h")
install(DIRECTORY ${HAPI_INCLUDE_DIR}/ TYPE INCLUDE)
install(DIRECTORY ${HAPI_LIB_DIR}/ TYPE LIB)
install(DIRECTORY ${Protobuf_INCLUDE_DIRS} DESTINATION ${CMAKE_INSTALL_PREFIX})

# Install the address books
install(FILES
        ${PROJECT_SOURCE_DIR}/addressbook/mainnet.pb
        ${PROJECT_SOURCE_DIR}/addressbook/previewnet.pb
        ${PROJECT_SOURCE_DIR}/addressbook/testnet.pb
        DESTINATION addressbook)

# Install the configuration JSON files
install(FILES
        ${PROJECT_SOURCE_DIR}/config/hello_world.json
        ${PROJECT_SOURCE_DIR}/config/local_node.json
        ${PROJECT_SOURCE_DIR}/config/stateful.json
        DESTINATION config)