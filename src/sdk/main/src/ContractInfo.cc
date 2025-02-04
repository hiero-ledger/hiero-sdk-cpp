// SPDX-License-Identifier: Apache-2.0
#include "ContractInfo.h"
#include "impl/DurationConverter.h"
#include "impl/HexConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <contract_get_info.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
ContractInfo ContractInfo::fromProtobuf(const proto::ContractGetInfoResponse_ContractInfo& proto)
{
  ContractInfo contractInfo;

  if (proto.has_contractid())
  {
    contractInfo.mContractId = ContractId::fromProtobuf(proto.contractid());
  }

  if (proto.has_accountid())
  {
    contractInfo.mAccountId = AccountId::fromProtobuf(proto.accountid());
  }

  contractInfo.mContractAccountId = proto.contractaccountid();

  if (proto.has_adminkey())
  {
    contractInfo.mAdminKey = Key::fromProtobuf(proto.adminkey());
  }

  if (proto.has_expirationtime())
  {
    contractInfo.mExpirationTime = internal::TimestampConverter::fromProtobuf(proto.expirationtime());
  }

  if (proto.has_autorenewperiod())
  {
    contractInfo.mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(proto.autorenewperiod());
  }

  contractInfo.mStorage = proto.storage();
  contractInfo.mMemo = proto.memo();
  contractInfo.mBalance = Hbar(static_cast<int64_t>(proto.balance()), HbarUnit::TINYBAR());
  contractInfo.mIsDeleted = proto.deleted();
  contractInfo.mLedgerId = LedgerId(internal::Utilities::stringToByteVector(proto.ledger_id()));

  if (proto.has_auto_renew_account_id())
  {
    contractInfo.mAutoRenewAccountId = AccountId::fromProtobuf(proto.auto_renew_account_id());
  }

  contractInfo.mMaxAutomaticTokenAssociations = proto.max_automatic_token_associations();

  if (proto.has_staking_info())
  {
    contractInfo.mStakingInfo = StakingInfo::fromProtobuf(proto.staking_info());
  }

  for (auto it = proto.tokenrelationships().begin(); it != proto.tokenrelationships().end(); it++)
  {
    contractInfo.mTokenRelationships.insert(
      { TokenId::fromProtobuf(it->tokenid()), TokenRelationship::fromProtobuf(*it) });
  }

  return contractInfo;
}

//-----
ContractInfo ContractInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ContractGetInfoResponse_ContractInfo proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ContractGetInfoResponse_ContractInfo> ContractInfo::toProtobuf() const
{
  auto proto = std::make_unique<proto::ContractGetInfoResponse_ContractInfo>();
  proto->set_allocated_contractid(mContractId.toProtobuf().release());
  proto->set_allocated_accountid(mAccountId.toProtobuf().release());
  proto->set_contractaccountid(mContractAccountId);

  if (mAdminKey)
  {
    proto->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  proto->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(mExpirationTime));
  proto->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod));
  proto->set_storage(static_cast<int64_t>(mStorage));
  proto->set_memo(mMemo);
  proto->set_balance(mBalance.toTinybars());
  proto->set_deleted(mIsDeleted);
  proto->set_ledger_id(internal::Utilities::byteVectorToString(mLedgerId.toBytes()));

  if (mAutoRenewAccountId.has_value())
  {
    proto->set_allocated_auto_renew_account_id(mAutoRenewAccountId->toProtobuf().release());
  }

  proto->set_max_automatic_token_associations(mMaxAutomaticTokenAssociations);
  proto->set_allocated_staking_info(mStakingInfo.toProtobuf().release());

  for (auto tr : mTokenRelationships)
  {
    proto->mutable_tokenrelationships()->AddAllocated(tr.second.toProtobuf().release());
  }

  return proto;
}

//-----
std::vector<std::byte> ContractInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ContractInfo::toString() const
{
  nlohmann::json json;
  json["mContractId"] = mContractId.toString();
  json["mAccountId"] = mAccountId.toString();
  json["mContractAccountId"] = mContractAccountId;

  if (mAdminKey)
  {
    json["mAdminKey"] = internal::HexConverter::bytesToHex(mAdminKey->toBytes());
  }

  json["mExpirationTime"] = internal::TimestampConverter::toString(mExpirationTime);
  json["mAutoRenewPeriod"] = std::to_string(mAutoRenewPeriod.count());
  json["mStorage"] = mStorage;
  json["mMemo"] = mMemo;
  json["mBalance"] = mBalance.toString();
  json["mIsDeleted"] = mIsDeleted;
  json["mLedgerId"] = mLedgerId.toString();

  if (mAutoRenewAccountId.has_value())
  {
    json["mAutoRenewAccountId"] = mAutoRenewAccountId->toString();
  }

  json["mMaxAutomaticTokenAssociations"] = mMaxAutomaticTokenAssociations;
  json["mStakingInfo"] = mStakingInfo.toString();
  return json;
}

//-----
std::ostream& operator<<(std::ostream& os, const ContractInfo& info)
{
  os << info.toString();
  return os;
}

} // namespace Hiero
