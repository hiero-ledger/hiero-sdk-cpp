// SPDX-License-Identifier: Apache-2.0
#include "ScheduleInfo.h"
#include "impl/HexConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <schedule_get_info.pb.h>

namespace Hiero
{
//-----
ScheduleInfo ScheduleInfo::fromProtobuf(const proto::ScheduleInfo& proto)
{
  ScheduleInfo scheduleInfo;

  if (proto.has_scheduleid())
  {
    scheduleInfo.mScheduleId = ScheduleId::fromProtobuf(proto.scheduleid());
  }

  if (proto.has_execution_time())
  {
    scheduleInfo.mExecutionTime = internal::TimestampConverter::fromProtobuf(proto.execution_time());
  }
  else if (proto.has_deletion_time())
  {
    scheduleInfo.mDeletionTime = internal::TimestampConverter::fromProtobuf(proto.deletion_time());
  }

  if (proto.has_expirationtime())
  {
    scheduleInfo.mExpirationTime = internal::TimestampConverter::fromProtobuf(proto.expirationtime());
  }

  if (proto.has_scheduledtransactionbody())
  {
    scheduleInfo.mScheduledTransaction = WrappedTransaction::fromProtobuf(proto.scheduledtransactionbody());
  }

  scheduleInfo.mMemo = proto.memo();

  if (proto.has_adminkey())
  {
    scheduleInfo.mAdminKey = Key::fromProtobuf(proto.adminkey());
  }

  if (proto.has_signers())
  {
    scheduleInfo.mSignatories = KeyList::fromProtobuf(proto.signers());
  }

  if (proto.has_creatoraccountid())
  {
    scheduleInfo.mCreatorAccountId = AccountId::fromProtobuf(proto.creatoraccountid());
  }

  if (proto.has_payeraccountid())
  {
    scheduleInfo.mPayerAccountId = AccountId::fromProtobuf(proto.payeraccountid());
  }

  if (proto.has_scheduledtransactionid())
  {
    scheduleInfo.mScheduledTransactionId = TransactionId::fromProtobuf(proto.scheduledtransactionid());
  }

  scheduleInfo.mLedgerId = LedgerId(internal::Utilities::stringToByteVector(proto.ledger_id()));
  scheduleInfo.mWaitForExpiry = proto.wait_for_expiry();

  return scheduleInfo;
}

//-----
ScheduleInfo ScheduleInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ScheduleInfo proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ScheduleInfo> ScheduleInfo::toProtobuf() const
{
  auto protoScheduleInfo = std::make_unique<proto::ScheduleInfo>();
  protoScheduleInfo->set_allocated_scheduleid(mScheduleId.toProtobuf().release());

  if (mDeletionTime.has_value())
  {
    protoScheduleInfo->set_allocated_deletion_time(internal::TimestampConverter::toProtobuf(mDeletionTime.value()));
  }
  else if (mExecutionTime.has_value())
  {
    protoScheduleInfo->set_allocated_execution_time(internal::TimestampConverter::toProtobuf(mExecutionTime.value()));
  }

  protoScheduleInfo->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(mExpirationTime));
  protoScheduleInfo->set_allocated_scheduledtransactionbody(mScheduledTransaction.toSchedulableProtobuf().release());
  protoScheduleInfo->set_memo(mMemo);

  if (mAdminKey)
  {
    protoScheduleInfo->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  if (!mSignatories.empty())
  {
    protoScheduleInfo->set_allocated_signers(mSignatories.toProtobuf().release());
  }

  protoScheduleInfo->set_allocated_creatoraccountid(mCreatorAccountId.toProtobuf().release());
  protoScheduleInfo->set_allocated_payeraccountid(mPayerAccountId.toProtobuf().release());
  protoScheduleInfo->set_allocated_scheduledtransactionid(mScheduledTransactionId.toProtobuf().release());
  protoScheduleInfo->set_ledger_id(internal::Utilities::byteVectorToString(mLedgerId.toBytes()));
  protoScheduleInfo->set_wait_for_expiry(mWaitForExpiry);

  return protoScheduleInfo;
}

//-----
std::vector<std::byte> ScheduleInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ScheduleInfo::toString() const
{
  nlohmann::json json;
  json["mScheduleId"] = mScheduleId.toString();

  if (mExecutionTime.has_value())
  {
    json["mExecutionTime"] = internal::TimestampConverter::toString(mExecutionTime.value());
  }

  if (mDeletionTime.has_value())
  {
    json["mDeletionTime"] = internal::TimestampConverter::toString(mDeletionTime.value());
  }

  json["mExpirationTime"] = internal::TimestampConverter::toString(mExpirationTime);
  json["mMemo"] = mMemo;

  if (mAdminKey)
  {
    json["mAdminKey"] = internal::HexConverter::bytesToHex(mAdminKey->toBytes());
  }

  json["mSignatories"] = mSignatories.toString();
  json["mCreatorAccountId"] = mCreatorAccountId.toString();
  json["mPayerAccountId"] = mPayerAccountId.toString();
  json["mScheduledTransactionId"] = mScheduledTransactionId.toString();
  json["mLedgerId"] = mLedgerId.toString();
  json["mWaitForExpiry"] = mWaitForExpiry;
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const ScheduleInfo& info)
{
  os << info.toString();
  return os;
}

} // namespace Hiero
