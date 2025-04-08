# Begin Protobuf Definitions
set(PROTO_FILES
        transaction_list.proto

        mirror/consensus_service.proto
        mirror/mirror_network_service.proto

        platform/event/state_signature_transaction.proto

        services/address_book_service.proto
        services/basic_types.proto
        services/consensus_create_topic.proto
        services/consensus_delete_topic.proto
        services/consensus_get_topic_info.proto
        services/consensus_service.proto
        services/consensus_submit_message.proto
        services/consensus_topic_info.proto
        services/consensus_update_topic.proto
        services/contract_call.proto
        services/contract_call_local.proto
        services/contract_create.proto
        services/contract_delete.proto
        services/contract_get_bytecode.proto
        services/contract_get_info.proto
        services/contract_get_records.proto
        services/contract_update.proto
        services/contract_types.proto
        services/crypto_add_live_hash.proto
        services/crypto_approve_allowance.proto
        services/crypto_create.proto
        services/crypto_delete.proto
        services/crypto_delete_allowance.proto
        services/crypto_delete_live_hash.proto
        services/crypto_get_account_balance.proto
        services/crypto_get_account_records.proto
        services/crypto_get_info.proto
        services/crypto_get_live_hash.proto
        services/crypto_get_stakers.proto
        services/crypto_service.proto
        services/crypto_transfer.proto
        services/crypto_update.proto
        services/custom_fees.proto
        services/duration.proto
        services/ethereum_transaction.proto
        services/exchange_rate.proto
        services/file_append.proto
        services/file_create.proto
        services/file_delete.proto
        services/file_get_contents.proto
        services/file_get_info.proto
        services/file_service.proto
        services/file_update.proto
        services/freeze.proto
        services/freeze_service.proto
        services/freeze_type.proto
        services/get_account_details.proto
        services/get_by_key.proto
        services/get_by_solidity_id.proto
        services/network_get_execution_time.proto
        services/network_get_version_info.proto
        services/network_service.proto
        services/node_create.proto
        services/node_delete.proto
        services/node_stake_update.proto
        services/node_update.proto
        services/query.proto
        services/query_header.proto
        services/response.proto
        services/response_code.proto
        services/response_header.proto
        services/schedulable_transaction_body.proto
        services/schedule_create.proto
        services/schedule_delete.proto
        services/schedule_get_info.proto
        services/schedule_service.proto
        services/schedule_sign.proto
        services/smart_contract_service.proto
        services/system_delete.proto
        services/system_undelete.proto
        services/throttle_definitions.proto
        services/timestamp.proto
        services/token_airdrop.proto
        services/token_associate.proto
        services/token_burn.proto
        services/token_cancel_airdrop.proto
        services/token_claim_airdrop.proto
        services/token_create.proto
        services/token_delete.proto
        services/token_dissociate.proto
        services/token_fee_schedule_update.proto
        services/token_freeze_account.proto
        services/token_get_account_nft_infos.proto
        services/token_get_info.proto
        services/token_get_nft_info.proto
        services/token_get_nft_infos.proto
        services/token_grant_kyc.proto
        services/token_mint.proto
        services/token_pause.proto
        services/token_reject.proto
        services/token_revoke_kyc.proto
        services/token_service.proto
        services/token_unfreeze_account.proto
        services/token_unpause.proto
        services/token_update.proto
        services/token_update_nfts.proto
        services/token_wipe_account.proto
        services/transaction.proto
        services/transaction_contents.proto
        services/transaction_get_fast_record.proto
        services/transaction_get_receipt.proto
        services/transaction_get_record.proto
        services/transaction_receipt.proto
        services/transaction_record.proto
        services/transaction_response.proto
        services/unchecked_submit.proto
        services/util_prng.proto
        services/util_service.proto

        services/auxiliary/hints/crs_publication.proto
        services/auxiliary/hints/hints_key_publication.proto
        services/auxiliary/hints/hints_partial_signature.proto
        services/auxiliary/hints/hints_preprocessing_vote.proto
        services/auxiliary/history/history_proof_signature.proto
        services/auxiliary/history/history_proof_key_publication.proto
        services/auxiliary/history/history_proof_vote.proto
        services/auxiliary/tss/tss_message.proto
        services/auxiliary/tss/tss_vote.proto

        services/state/hints/hints_types.proto
        services/state/history/history_types.proto
)
# End Protobuf Definitions

add_library(hapi STATIC ${PROTO_FILES})
target_link_libraries(hapi PRIVATE protobuf::libprotobuf gRPC::grpc gRPC::grpc++ OpenSSL::SSL OpenSSL::Crypto)
target_include_directories(hapi PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

#
# Compile protobuf and grpc files in the hapi target to cpp
#
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET hapi LANGUAGE cpp IMPORT_DIRS ${PROTO_SRC})
protobuf_generate(TARGET hapi LANGUAGE grpc GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}" IMPORT_DIRS ${PROTO_SRC})
