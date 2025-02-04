// SPDX-License-Identifier: Apache-2.0
#include "TransactionReceipt.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/HexConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <transaction_get_receipt.pb.h>
#include <transaction_receipt.pb.h>

namespace Hiero
{
//-----
TransactionReceipt TransactionReceipt::fromProtobuf(const proto::TransactionGetReceiptResponse& proto,
                                                    const TransactionId& transactionId)
{
  TransactionReceipt receipt = TransactionReceipt::fromProtobuf(proto.receipt());
  receipt.mTransactionId = transactionId;

  for (int i = 0; i < proto.duplicatetransactionreceipts_size(); ++i)
  {
    receipt.mDuplicates.push_back(TransactionReceipt::fromProtobuf(proto.duplicatetransactionreceipts(i)));
  }

  for (int i = 0; i < proto.child_transaction_receipts_size(); ++i)
  {
    receipt.mChildren.push_back(TransactionReceipt::fromProtobuf(proto.child_transaction_receipts(i)));
  }

  return receipt;
}

//-----
TransactionReceipt TransactionReceipt::fromProtobuf(const proto::TransactionReceipt& proto,
                                                    const TransactionId& transactionId)
{
  TransactionReceipt receipt;
  receipt.mTransactionId = transactionId;
  receipt.mStatus = gProtobufResponseCodeToStatus.at(proto.status());

  if (proto.has_accountid())
  {
    receipt.mAccountId = AccountId::fromProtobuf(proto.accountid());
  }

  if (proto.has_fileid())
  {
    receipt.mFileId = FileId::fromProtobuf(proto.fileid());
  }

  if (proto.has_contractid())
  {
    receipt.mContractId = ContractId::fromProtobuf(proto.contractid());
  }

  if (proto.has_exchangerate())
  {
    receipt.mExchangeRates = ExchangeRates::fromProtobuf(proto.exchangerate());
  }

  if (proto.has_topicid())
  {
    receipt.mTopicId = TopicId::fromProtobuf(proto.topicid());
  }

  if (proto.topicsequencenumber() != 0ULL)
  {
    receipt.mTopicSequenceNumber = proto.topicsequencenumber();
  }

  if (!proto.topicrunninghash().empty())
  {
    receipt.mTopicRunningHash = internal::Utilities::stringToByteVector(proto.topicrunninghash());
  }

  if (proto.topicrunninghashversion() != 0ULL)
  {
    receipt.mTopicRunningHashVersion = proto.topicrunninghashversion();
  }

  if (proto.has_tokenid())
  {
    receipt.mTokenId = TokenId::fromProtobuf(proto.tokenid());
  }

  receipt.mNewTotalSupply = proto.newtotalsupply();

  if (proto.has_scheduleid())
  {
    receipt.mScheduleId = ScheduleId::fromProtobuf(proto.scheduleid());
  }

  if (proto.has_scheduledtransactionid())
  {
    receipt.mScheduledTransactionId = TransactionId::fromProtobuf(proto.scheduledtransactionid());
  }

  for (int i = 0; i < proto.serialnumbers_size(); ++i)
  {
    receipt.mSerialNumbers.push_back(static_cast<uint64_t>(proto.serialnumbers(i)));
  }

  receipt.mNodeId = proto.node_id();

  return receipt;
}

//-----
TransactionReceipt TransactionReceipt::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TransactionReceipt proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::TransactionReceipt> TransactionReceipt::toProtobuf() const
{
  auto proto = std::make_unique<proto::TransactionReceipt>();
  proto->set_status(gStatusToProtobufResponseCode.at(mStatus));

  if (mAccountId.has_value())
  {
    proto->set_allocated_accountid(mAccountId->toProtobuf().release());
  }

  if (mFileId.has_value())
  {
    proto->set_allocated_fileid(mFileId->toProtobuf().release());
  }

  if (mContractId.has_value())
  {
    proto->set_allocated_contractid(mContractId->toProtobuf().release());
  }

  proto->mutable_exchangerate()->mutable_currentrate()->set_hbarequiv(mExchangeRates.mCurrentRate.mHbars);
  proto->mutable_exchangerate()->mutable_currentrate()->set_centequiv(mExchangeRates.mCurrentRate.mCents);
  proto->mutable_exchangerate()->mutable_currentrate()->set_allocated_expirationtime(
    internal::TimestampConverter::toSecondsProtobuf(mExchangeRates.mCurrentRate.mExpirationTime));

  if (mTopicId.has_value())
  {
    proto->set_allocated_topicid(mTopicId->toProtobuf().release());
  }

  if (mTopicSequenceNumber.has_value())
  {
    proto->set_topicsequencenumber(mTopicSequenceNumber.value());
  }

  if (mTopicRunningHash.has_value())
  {
    proto->set_topicrunninghash(internal::Utilities::byteVectorToString(mTopicRunningHash.value()));
  }

  if (mTopicRunningHashVersion.has_value())
  {
    proto->set_topicrunninghashversion(mTopicRunningHashVersion.value());
  }

  if (mNewTotalSupply.has_value())
  {
    proto->set_newtotalsupply(mNewTotalSupply.value());
  }

  if (mScheduleId.has_value())
  {
    proto->set_allocated_scheduleid(mScheduleId->toProtobuf().release());
  }

  if (mScheduledTransactionId.has_value())
  {
    proto->set_allocated_scheduledtransactionid(mScheduledTransactionId->toProtobuf().release());
  }

  std::for_each(mSerialNumbers.cbegin(),
                mSerialNumbers.cend(),
                [&proto](uint64_t num) { proto->add_serialnumbers(static_cast<int64_t>(num)); });

  if (mNodeId.has_value())
  {
    proto->set_node_id(mNodeId.value());
  }

  return proto;
}

//-----
std::vector<std::byte> TransactionReceipt::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TransactionReceipt::toString() const
{
  nlohmann::json json;
  json["mTransactionId"] = mTransactionId.toString();
  json["mStatus"] = gStatusToString.at(mStatus);

  if (mAccountId.has_value())
  {
    json["mAccountId"] = mAccountId->toString();
  }

  if (mFileId.has_value())
  {
    json["mFileId"] = mFileId->toString();
  }

  if (mContractId.has_value())
  {
    json["mContractId"] = mContractId->toString();
  }

  json["mExchangeRates"] = mExchangeRates.toString();

  if (mTopicId.has_value())
  {
    json["mTopicId"] = mTopicId->toString();
  }

  if (mTopicSequenceNumber.has_value())
  {
    json["mTopicSequenceNumber"] = mTopicSequenceNumber.value();
  }

  if (mTopicRunningHash.has_value())
  {
    json["mTopicRunningHash"] = internal::HexConverter::bytesToHex(mTopicRunningHash.value());
  }

  if (mTopicRunningHashVersion.has_value())
  {
    json["mTopicRunningHashVersion"] = mTopicRunningHashVersion.value();
  }

  if (mTokenId.has_value())
  {
    json["mTokenId"] = mTokenId->toString();
  }

  if (mNewTotalSupply.has_value())
  {
    json["mNewTotalSupply"] = mNewTotalSupply.value();
  }

  if (mScheduleId.has_value())
  {
    json["mScheduleId"] = mScheduleId->toString();
  }

  if (mScheduledTransactionId.has_value())
  {
    json["mScheduledTransactionId"] = mScheduledTransactionId->toString();
  }

  std::for_each(mSerialNumbers.cbegin(),
                mSerialNumbers.cend(),
                [&json](uint64_t serialNumber) { json["mSerialNumbers"].push_back(serialNumber); });
  std::for_each(mDuplicates.cbegin(),
                mDuplicates.cend(),
                [&json](const TransactionReceipt& receipt) { json["mDuplicates"].push_back(receipt.toString()); });
  std::for_each(mChildren.cbegin(),
                mChildren.cend(),
                [&json](const TransactionReceipt& receipt) { json["mChildren"].push_back(receipt.toString()); });

  if (mNodeId.has_value())
  {
    json["mNodeId"] = mNodeId.value();
  }

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TransactionReceipt& receipt)
{
  os << receipt.toString();
  return os;
}

//-----
void TransactionReceipt::validateStatus() const
{
  if (mStatus != Status::SUCCESS)
  {
    throw ReceiptStatusException(mTransactionId, mStatus);
  }
}

} // namespace Hiero
