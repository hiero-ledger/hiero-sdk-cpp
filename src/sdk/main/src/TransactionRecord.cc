// SPDX-License-Identifier: Apache-2.0
#include "TransactionRecord.h"
#include "impl/HexConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <transaction_get_record.pb.h>
#include <transaction_record.pb.h>

namespace Hiero
{
//-----
TransactionRecord TransactionRecord::fromProtobuf(const proto::TransactionGetRecordResponse& proto)
{
  TransactionRecord record = TransactionRecord::fromProtobuf(proto.transactionrecord());

  for (int i = 0; i < proto.duplicatetransactionrecords_size(); ++i)
  {
    record.mDuplicates.push_back(TransactionRecord::fromProtobuf(proto.duplicatetransactionrecords(i)));
  }

  for (int i = 0; i < proto.child_transaction_records_size(); ++i)
  {
    record.mChildren.push_back(TransactionRecord::fromProtobuf(proto.child_transaction_records(i)));
  }

  return record;
}

//-----
TransactionRecord TransactionRecord::fromProtobuf(const proto::TransactionRecord& proto)
{
  TransactionRecord transactionRecord;

  if (proto.has_transactionid())
  {
    transactionRecord.mTransactionId = TransactionId::fromProtobuf(proto.transactionid());

    if (proto.has_receipt())
    {
      transactionRecord.mReceipt =
        TransactionReceipt::fromProtobuf(proto.receipt(), transactionRecord.mTransactionId.value());
    }
  }
  else
  {
    transactionRecord.mReceipt = TransactionReceipt::fromProtobuf(proto.receipt());
  }

  transactionRecord.mTransactionHash = internal::Utilities::stringToByteVector(proto.transactionhash());

  if (proto.has_consensustimestamp())
  {
    transactionRecord.mConsensusTimestamp = internal::TimestampConverter::fromProtobuf(proto.consensustimestamp());
  }

  transactionRecord.mMemo = proto.memo();
  transactionRecord.mTransactionFee = proto.transactionfee();

  if (proto.has_contractcallresult())
  {
    transactionRecord.mContractFunctionResult = ContractFunctionResult::fromProtobuf(proto.contractcallresult());
  }
  else if (proto.has_contractcreateresult())
  {
    transactionRecord.mContractFunctionResult = ContractFunctionResult::fromProtobuf(proto.contractcreateresult());
  }

  if (proto.has_transferlist())
  {
    for (int i = 0; i < proto.transferlist().accountamounts_size(); ++i)
    {
      transactionRecord.mHbarTransferList.push_back(HbarTransfer::fromProtobuf(proto.transferlist().accountamounts(i)));
    }
  }

  for (int i = 0; i < proto.tokentransferlists_size(); ++i)
  {
    const proto::TokenTransferList& list = proto.tokentransferlists(i);
    const TokenId tokenId = TokenId::fromProtobuf(list.token());

    // Fungible token
    for (int j = 0; j < list.transfers_size(); ++j)
    {
      transactionRecord.mTokenTransferList.push_back(TokenTransfer::fromProtobuf(
        list.transfers(j), tokenId, list.has_expected_decimals() ? list.expected_decimals().value() : 0U));
    }

    // NFT
    for (int j = 0; j < list.nfttransfers_size(); ++j)
    {
      transactionRecord.mNftTransferList.push_back(TokenNftTransfer::fromProtobuf(list.nfttransfers(j), tokenId));
    }
  }

  if (proto.has_scheduleref())
  {
    transactionRecord.mScheduleRef = ScheduleId::fromProtobuf(proto.scheduleref());
  }

  for (int i = 0; i < proto.assessed_custom_fees_size(); ++i)
  {
    transactionRecord.mAssessedCustomFees.push_back(AssessedCustomFee::fromProtobuf(proto.assessed_custom_fees(i)));
  }

  for (int i = 0; i < proto.automatic_token_associations_size(); ++i)
  {
    transactionRecord.mAutomaticTokenAssociations.push_back(
      TokenAssociation::fromProtobuf(proto.automatic_token_associations(i)));
  }

  if (proto.has_parent_consensus_timestamp())
  {
    transactionRecord.mParentConsensusTimestamp =
      internal::TimestampConverter::fromProtobuf(proto.parent_consensus_timestamp());
  }

  if (!proto.alias().empty())
  {
    transactionRecord.mAlias = PublicKey::fromAliasBytes(internal::Utilities::stringToByteVector(proto.alias()));
  }

  if (!proto.ethereum_hash().empty())
  {
    transactionRecord.mEthereumHash = internal::Utilities::stringToByteVector(proto.ethereum_hash());
  }

  for (int i = 0; i < proto.paid_staking_rewards_size(); ++i)
  {
    transactionRecord.mPaidStakingRewards.push_back(HbarTransfer::fromProtobuf(proto.paid_staking_rewards(i)));
  }

  if (proto.has_prng_bytes())
  {
    transactionRecord.mPrngBytes = internal::Utilities::stringToByteVector(proto.prng_bytes());
  }
  else if (proto.has_prng_number())
  {
    transactionRecord.mPrngNumber = proto.prng_number();
  }

  if (!proto.evm_address().empty())
  {
    transactionRecord.mEvmAddress = EvmAddress::fromBytes(internal::Utilities::stringToByteVector(proto.evm_address()));
  }

  for (int i = 0; i < proto.new_pending_airdrops_size(); i++)
  {
    transactionRecord.mPendingAirdropRecords.push_back(
      PendingAirdropRecord::fromProtobuf(proto.new_pending_airdrops(i)));
  }

  return transactionRecord;
}

//-----
TransactionRecord TransactionRecord::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TransactionRecord proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::TransactionRecord> TransactionRecord::toProtobuf() const
{
  auto proto = std::make_unique<proto::TransactionRecord>();

  if (mReceipt.has_value())
  {
    proto->set_allocated_receipt(mReceipt->toProtobuf().release());
  }

  proto->set_transactionhash(internal::Utilities::byteVectorToString(mTransactionHash));

  if (mConsensusTimestamp.has_value())
  {
    proto->set_allocated_consensustimestamp(internal::TimestampConverter::toProtobuf(mConsensusTimestamp.value()));
  }

  if (mTransactionId.has_value())
  {
    proto->set_allocated_transactionid(mTransactionId->toProtobuf().release());
  }

  proto->set_memo(mMemo);
  proto->set_transactionfee(mTransactionFee);

  if (mContractFunctionResult.has_value())
  {
    proto->set_allocated_contractcallresult(mContractFunctionResult->toProtobuf().release());
  }

  std::for_each(mHbarTransferList.cbegin(),
                mHbarTransferList.cend(),
                [&proto](const HbarTransfer& transfer)
                { *proto->mutable_transferlist()->add_accountamounts() = *transfer.toProtobuf(); });
  std::for_each(mTokenTransferList.cbegin(),
                mTokenTransferList.cend(),
                [&proto](const TokenTransfer& transfer)
                {
                  // Check if this token ID already has an entry.
                  bool found = false;
                  for (int i = 0; i < proto->tokentransferlists_size(); ++i)
                  {
                    if (transfer.mTokenId == TokenId::fromProtobuf(proto->tokentransferlists(i).token()))
                    {
                      // Add the transfer to this transfer.
                      *proto->mutable_tokentransferlists(i)->add_transfers() = *transfer.toProtobuf();
                      found = true;
                      break;
                    }
                  }

                  if (!found)
                  {
                    proto::TokenTransferList* protoTokenTransferList = proto->add_tokentransferlists();
                    protoTokenTransferList->set_allocated_token(transfer.mTokenId.toProtobuf().release());
                    protoTokenTransferList->mutable_expected_decimals()->set_value(transfer.mExpectedDecimals);
                    *protoTokenTransferList->add_transfers() = *transfer.toProtobuf();
                  }
                });
  std::for_each(mNftTransferList.cbegin(),
                mNftTransferList.cend(),
                [&proto](const TokenNftTransfer& transfer)
                {
                  // Check if this token ID already has an entry.
                  bool found = false;
                  for (int i = 0; i < proto->tokentransferlists_size(); ++i)
                  {
                    if (transfer.mNftId.mTokenId == TokenId::fromProtobuf(proto->tokentransferlists(i).token()))
                    {
                      // Add the transfer to this transfer.
                      *proto->mutable_tokentransferlists(i)->add_nfttransfers() = *transfer.toProtobuf();
                      found = true;
                      break;
                    }
                  }

                  if (!found)
                  {
                    proto::TokenTransferList* protoTokenTransferList = proto->add_tokentransferlists();
                    protoTokenTransferList->set_allocated_token(transfer.mNftId.mTokenId.toProtobuf().release());
                    *protoTokenTransferList->add_nfttransfers() = *transfer.toProtobuf();
                  }
                });

  if (mScheduleRef.has_value())
  {
    proto->set_allocated_scheduleref(mScheduleRef->toProtobuf().release());
  }

  std::for_each(mAssessedCustomFees.cbegin(),
                mAssessedCustomFees.cend(),
                [&proto](const AssessedCustomFee& fee) { *proto->add_assessed_custom_fees() = *fee.toProtobuf(); });
  std::for_each(mAutomaticTokenAssociations.cbegin(),
                mAutomaticTokenAssociations.cend(),
                [&proto](const TokenAssociation& association)
                { *proto->add_automatic_token_associations() = *association.toProtobuf(); });

  if (mParentConsensusTimestamp.has_value())
  {
    proto->set_allocated_parent_consensus_timestamp(
      internal::TimestampConverter::toProtobuf(mParentConsensusTimestamp.value()));
  }

  if (mAlias)
  {
    proto->set_alias(mAlias->toProtobufKey()->SerializeAsString());
  }

  if (mEthereumHash.has_value())
  {
    proto->set_ethereum_hash(internal::Utilities::byteVectorToString(mEthereumHash.value()));
  }

  std::for_each(mPaidStakingRewards.cbegin(),
                mPaidStakingRewards.cend(),
                [&proto](const HbarTransfer& reward) { *proto->add_paid_staking_rewards() = *reward.toProtobuf(); });

  if (mPrngNumber.has_value())
  {
    proto->set_prng_number(mPrngNumber.value());
  }
  else
  {
    proto->set_prng_bytes(internal::Utilities::byteVectorToString(mPrngBytes));
  }

  if (mEvmAddress.has_value())
  {
    proto->set_evm_address(internal::Utilities::byteVectorToString(mEvmAddress->toBytes()));
  }

  for (int i = 0; i < mPendingAirdropRecords.size(); i++)
  {
    auto pr = std::make_unique<proto::PendingAirdropRecord>();
    pr->set_allocated_pending_airdrop_id(mPendingAirdropRecords[i].mPendingAirdropId.toProtobuf().release());
    auto value = std::make_unique<proto::PendingAirdropValue>();
    value->set_amount(mPendingAirdropRecords[i].mAmount);
    pr->set_allocated_pending_airdrop_value(value.release());
  }

  return proto;
}

//-----
std::vector<std::byte> TransactionRecord::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TransactionRecord::toString() const
{
  nlohmann::json json;

  if (mReceipt.has_value())
  {
    json["mReceipt"] = mReceipt->toString();
  }

  json["mTransactionHash"] = internal::HexConverter::bytesToHex(mTransactionHash);

  if (mConsensusTimestamp.has_value())
  {
    json["mConsensusTimestamp"] = internal::TimestampConverter::toString(mConsensusTimestamp.value());
  }

  if (mTransactionId.has_value())
  {
    json["mTransactionId"] = mTransactionId->toString();
  }

  json["mMemo"] = mMemo;
  json["mTransactionFee"] = mTransactionFee;

  if (mContractFunctionResult.has_value())
  {
    json["mContractFunctionResult"] = mContractFunctionResult->toString();
  }

  std::for_each(mHbarTransferList.cbegin(),
                mHbarTransferList.cend(),
                [&json](const HbarTransfer& transfer) { json["mHbarTransferList"].push_back(transfer.toString()); });
  std::for_each(mTokenTransferList.cbegin(),
                mTokenTransferList.cend(),
                [&json](const TokenTransfer& transfer) { json["mTokenTransferList"].push_back(transfer.toString()); });
  std::for_each(mNftTransferList.cbegin(),
                mNftTransferList.cend(),
                [&json](const TokenNftTransfer& transfer) { json["mNftTransferList"].push_back(transfer.toString()); });

  if (mScheduleRef.has_value())
  {
    json["mScheduleRef"] = mScheduleRef->toString();
  }

  std::for_each(mAssessedCustomFees.cbegin(),
                mAssessedCustomFees.cend(),
                [&json](const AssessedCustomFee& fee) { json["mAssessedCustomFees"].push_back(fee.toString()); });
  std::for_each(mAutomaticTokenAssociations.cbegin(),
                mAutomaticTokenAssociations.cend(),
                [&json](const TokenAssociation& association)
                { json["mAutomaticTokenAssociations"].push_back(association.toString()); });

  if (mParentConsensusTimestamp.has_value())
  {
    json["mParentConsensusTimestamp"] = internal::TimestampConverter::toString(mParentConsensusTimestamp.value());
  }

  if (mAlias)
  {
    json["mAlias"] = mAlias->toStringDer();
  }

  if (mEthereumHash.has_value())
  {
    json["mEthereumHash"] = mEthereumHash.value();
  }

  std::for_each(mPaidStakingRewards.cbegin(),
                mPaidStakingRewards.cend(),
                [&json](const HbarTransfer& reward) { json["mPaidStakingRewards"].push_back(reward.toString()); });

  if (mPrngNumber.has_value())
  {
    json["mPrngNumber"] = mPrngNumber.value();
  }
  else
  {
    json["mPrngBytes"] = internal::HexConverter::bytesToHex(mPrngBytes);
  }

  if (mEvmAddress.has_value())
  {
    json["mEvmAddress"] = mEvmAddress->toString();
  }

  std::for_each(mPendingAirdropRecords.cbegin(),
                mPendingAirdropRecords.cend(),
                [&json](const PendingAirdropRecord& record)
                { json["mPendingAirdropRecords"].push_back(record.toString()); });

  std::for_each(mChildren.cbegin(),
                mChildren.cend(),
                [&json](const TransactionRecord& record) { json["mChildren"].push_back(record.toString()); });
  std::for_each(mDuplicates.cbegin(),
                mDuplicates.cend(),
                [&json](const TransactionRecord& record) { json["mDuplicates"].push_back(record.toString()); });

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TransactionRecord& record)
{
  os << record.toString();
  return os;
}

} // namespace Hiero
