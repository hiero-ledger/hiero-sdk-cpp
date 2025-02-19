// SPDX-License-Identifier: Apache-2.0
#include "TopicInfo.h"
#include "impl/DurationConverter.h"
#include "impl/HexConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <consensus_get_topic_info.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
TopicInfo TopicInfo::fromProtobuf(const proto::ConsensusGetTopicInfoResponse& proto)
{
  TopicInfo topicInfo;

  if (proto.has_topicid())
  {
    topicInfo.mTopicId = TopicId::fromProtobuf(proto.topicid());
  }

  const proto::ConsensusTopicInfo& consensusTopicInfo = proto.topicinfo();

  topicInfo.mMemo = consensusTopicInfo.memo();
  topicInfo.mRunningHash = internal::Utilities::stringToByteVector(consensusTopicInfo.runninghash());
  topicInfo.mSequenceNumber = consensusTopicInfo.sequencenumber();

  if (consensusTopicInfo.has_expirationtime())
  {
    topicInfo.mExpirationTime = internal::TimestampConverter::fromProtobuf(consensusTopicInfo.expirationtime());
  }

  if (consensusTopicInfo.has_adminkey())
  {
    topicInfo.mAdminKey = Key::fromProtobuf(consensusTopicInfo.adminkey());
  }

  if (consensusTopicInfo.has_submitkey())
  {
    topicInfo.mSubmitKey = Key::fromProtobuf(consensusTopicInfo.submitkey());
  }

  if (consensusTopicInfo.has_autorenewperiod())
  {
    topicInfo.mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(consensusTopicInfo.autorenewperiod());
  }

  if (consensusTopicInfo.has_autorenewaccount())
  {
    topicInfo.mAutoRenewAccountId = AccountId::fromProtobuf(consensusTopicInfo.autorenewaccount());
  }

  topicInfo.mLedgerId = LedgerId(internal::Utilities::stringToByteVector(consensusTopicInfo.ledger_id()));

  if (consensusTopicInfo.has_fee_schedule_key())
  {
    topicInfo.mFeeScheduleKey = Key::fromProtobuf(consensusTopicInfo.fee_schedule_key());
  }

  for (const auto& key : consensusTopicInfo.fee_exempt_key_list())
  {
    topicInfo.mFeeExemptKeys.push_back(Key::fromProtobuf(key));
  }

  for (const auto& fee : consensusTopicInfo.custom_fees())
  {
    topicInfo.mCustomFixedFees.push_back(
      CustomFixedFee::fromProtobuf(fee.fixed_fee())
        .setFeeCollectorAccountId(AccountId::fromProtobuf(fee.fee_collector_account_id())));
  }

  return topicInfo;
}

//-----
TopicInfo TopicInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ConsensusGetTopicInfoResponse proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ConsensusGetTopicInfoResponse> TopicInfo::toProtobuf() const
{
  auto protoTopicInfo = std::make_unique<proto::ConsensusGetTopicInfoResponse>();
  protoTopicInfo->set_allocated_topicid(mTopicId.toProtobuf().release());
  protoTopicInfo->mutable_topicinfo()->set_memo(mMemo);
  protoTopicInfo->mutable_topicinfo()->set_runninghash(internal::Utilities::byteVectorToString(mRunningHash));
  protoTopicInfo->mutable_topicinfo()->set_sequencenumber(mSequenceNumber);
  protoTopicInfo->mutable_topicinfo()->set_allocated_expirationtime(
    internal::TimestampConverter::toProtobuf(mExpirationTime));

  if (mAdminKey)
  {
    protoTopicInfo->mutable_topicinfo()->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  if (mSubmitKey)
  {
    protoTopicInfo->mutable_topicinfo()->set_allocated_submitkey(mSubmitKey->toProtobufKey().release());
  }

  if (mAutoRenewPeriod.has_value())
  {
    protoTopicInfo->mutable_topicinfo()->set_allocated_autorenewperiod(
      internal::DurationConverter::toProtobuf(mAutoRenewPeriod.value()));
  }

  if (mAutoRenewAccountId.has_value())
  {
    protoTopicInfo->mutable_topicinfo()->set_allocated_autorenewaccount(mAutoRenewAccountId->toProtobuf().release());
  }

  protoTopicInfo->mutable_topicinfo()->set_ledger_id(internal::Utilities::byteVectorToString(mLedgerId.toBytes()));

  if (mFeeScheduleKey)
  {
    protoTopicInfo->mutable_topicinfo()->set_allocated_fee_schedule_key(mFeeScheduleKey->toProtobufKey().release());
  }

  for (const auto& key : mFeeExemptKeys)
  {
    protoTopicInfo->mutable_topicinfo()->mutable_fee_exempt_key_list()->AddAllocated(key->toProtobufKey().release());
  }

  for (const auto& fee : mCustomFixedFees)
  {
    std::unique_ptr<proto::FixedCustomFee> fixedCustomFee = std::make_unique<proto::FixedCustomFee>();
    fixedCustomFee->set_allocated_fixed_fee(fee.toFixedFeeProtobuf().release());
    protoTopicInfo->mutable_topicinfo()->mutable_custom_fees()->AddAllocated(fixedCustomFee.release());
  }

  return protoTopicInfo;
}

//-----
std::vector<std::byte> TopicInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TopicInfo::toString() const
{
  nlohmann::json json;
  json["mTopicId"] = mTopicId.toString();
  json["mMemo"] = mMemo;
  json["mRunningHash"] = internal::HexConverter::bytesToHex(mRunningHash);
  json["mSequenceNumber"] = mSequenceNumber;
  json["mExpirationTime"] = internal::TimestampConverter::toString(mExpirationTime);

  if (mAdminKey)
  {
    json["mAdminKey"] = internal::HexConverter::bytesToHex(mAdminKey->toBytes());
  }

  if (mSubmitKey)
  {
    json["mSubmitKey"] = internal::HexConverter::bytesToHex(mSubmitKey->toBytes());
  }

  if (mAutoRenewPeriod.has_value())
  {
    json["mAutoRenewPeriod"] = std::to_string(mAutoRenewPeriod->count());
  }

  if (mAutoRenewAccountId.has_value())
  {
    json["mAutoRenewAccountId"] = mAutoRenewAccountId->toString();
  }

  json["mLedgerId"] = mLedgerId.toString();

  if (mFeeScheduleKey)
  {
    json["mFeeScheduleKey"] = internal::HexConverter::bytesToHex(mFeeScheduleKey->toBytes());
  }

  int i = 1;
  for (const auto& key : mFeeExemptKeys)
  {
    json["mFeeExemptKey" + std::to_string(i++)] = internal::HexConverter::bytesToHex(key->toBytes());
  }

  i = 1;
  for (const auto& fee : mCustomFixedFees)
  {
    json["mCustomFixedFee" + std::to_string(i++)] = fee.toString();
  }

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TopicInfo& info)
{
  os << info.toString();
  return os;
}

} // namespace Hiero
