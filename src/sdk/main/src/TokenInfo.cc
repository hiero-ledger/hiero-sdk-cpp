// SPDX-License-Identifier: Apache-2.0
#include "TokenInfo.h"
#include "impl/DurationConverter.h"
#include "impl/HexConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>
#include <token_get_info.pb.h>

namespace Hiero
{
//-----
TokenInfo TokenInfo::fromProtobuf(const proto::TokenInfo& proto)
{
  TokenInfo tokenInfo;

  if (proto.has_tokenid())
  {
    tokenInfo.mTokenId = TokenId::fromProtobuf(proto.tokenid());
  }

  tokenInfo.mTokenName = proto.name();
  tokenInfo.mTokenSymbol = proto.symbol();
  tokenInfo.mDecimals = proto.decimals();
  tokenInfo.mTotalSupply = proto.totalsupply();

  if (proto.has_treasury())
  {
    tokenInfo.mTreasuryAccountId = AccountId::fromProtobuf(proto.treasury());
  }

  if (proto.has_adminkey())
  {
    tokenInfo.mAdminKey = Key::fromProtobuf(proto.adminkey());
  }

  if (proto.has_kyckey())
  {
    tokenInfo.mKycKey = Key::fromProtobuf(proto.kyckey());
  }

  if (proto.has_freezekey())
  {
    tokenInfo.mFreezeKey = Key::fromProtobuf(proto.freezekey());
  }

  if (proto.has_wipekey())
  {
    tokenInfo.mWipeKey = Key::fromProtobuf(proto.wipekey());
  }

  if (proto.has_supplykey())
  {
    tokenInfo.mSupplyKey = Key::fromProtobuf(proto.supplykey());
  }

  if (proto.defaultfreezestatus() != proto::TokenFreezeStatus::FreezeNotApplicable)
  {
    tokenInfo.mDefaultFreezeStatus = proto.defaultfreezestatus() == proto::TokenFreezeStatus::Frozen;
  }

  if (proto.defaultkycstatus() != proto::TokenKycStatus::KycNotApplicable)
  {
    tokenInfo.mDefaultKycStatus = proto.defaultkycstatus() == proto::TokenKycStatus::Granted;
  }

  tokenInfo.mIsDeleted = proto.deleted();

  if (proto.has_autorenewaccount())
  {
    tokenInfo.mAutoRenewAccountId = AccountId::fromProtobuf(proto.autorenewaccount());
  }

  if (proto.has_autorenewperiod())
  {
    tokenInfo.mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(proto.autorenewperiod());
  }

  if (proto.has_expiry())
  {
    tokenInfo.mExpirationTime = internal::TimestampConverter::fromProtobuf(proto.expiry());
  }

  tokenInfo.mTokenMemo = proto.memo();
  tokenInfo.mTokenType = gProtobufTokenTypeToTokenType.at(proto.tokentype());
  tokenInfo.mSupplyType = gProtobufTokenSupplyTypeToTokenSupplyType.at(proto.supplytype());
  tokenInfo.mMaxSupply = static_cast<uint64_t>(proto.maxsupply());

  if (proto.has_fee_schedule_key())
  {
    tokenInfo.mFeeScheduleKey = Key::fromProtobuf(proto.fee_schedule_key());
  }

  for (int i = 0; i < proto.custom_fees_size(); ++i)
  {
    tokenInfo.mCustomFees.push_back(CustomFee::fromProtobuf(proto.custom_fees(i)));
  }

  if (proto.has_pause_key())
  {
    tokenInfo.mPauseKey = Key::fromProtobuf(proto.pause_key());
  }

  if (proto.pause_status() != proto::TokenPauseStatus::PauseNotApplicable)
  {
    tokenInfo.mPauseStatus = proto.pause_status() == proto::TokenPauseStatus::Paused;
  }

  tokenInfo.mLedgerId = LedgerId(internal::Utilities::stringToByteVector(proto.ledger_id()));

  tokenInfo.mMetadata = internal::Utilities::stringToByteVector(proto.metadata());

  if (proto.has_metadata_key())
  {
    tokenInfo.mMetadataKey = Key::fromProtobuf(proto.metadata_key());
  }

  return tokenInfo;
}

//-----
TokenInfo TokenInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TokenInfo proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::TokenInfo> TokenInfo::toProtobuf() const
{
  auto protoTokenInfo = std::make_unique<proto::TokenInfo>();
  protoTokenInfo->set_allocated_tokenid(mTokenId.toProtobuf().release());
  protoTokenInfo->set_name(mTokenName);
  protoTokenInfo->set_symbol(mTokenSymbol);
  protoTokenInfo->set_decimals(mDecimals);
  protoTokenInfo->set_totalsupply(mTotalSupply);
  protoTokenInfo->set_allocated_treasury(mTreasuryAccountId.toProtobuf().release());
  protoTokenInfo->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  protoTokenInfo->set_allocated_kyckey(mKycKey->toProtobufKey().release());
  protoTokenInfo->set_allocated_freezekey(mFreezeKey->toProtobufKey().release());
  protoTokenInfo->set_allocated_wipekey(mWipeKey->toProtobufKey().release());
  protoTokenInfo->set_allocated_supplykey(mSupplyKey->toProtobufKey().release());

  if (mDefaultFreezeStatus.has_value())
  {
    protoTokenInfo->set_defaultfreezestatus(mDefaultFreezeStatus.value() ? proto::TokenFreezeStatus::Frozen
                                                                         : proto::TokenFreezeStatus::Unfrozen);
  }
  else
  {
    protoTokenInfo->set_defaultfreezestatus(proto::TokenFreezeStatus::FreezeNotApplicable);
  }

  if (mDefaultKycStatus.has_value())
  {
    protoTokenInfo->set_defaultkycstatus(mDefaultKycStatus.value() ? proto::TokenKycStatus::Granted
                                                                   : proto::TokenKycStatus::Revoked);
  }
  else
  {
    protoTokenInfo->set_defaultkycstatus(proto::TokenKycStatus::KycNotApplicable);
  }

  protoTokenInfo->set_deleted(mIsDeleted);
  protoTokenInfo->set_allocated_autorenewaccount(mAutoRenewAccountId.toProtobuf().release());
  protoTokenInfo->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod));
  protoTokenInfo->set_allocated_expiry(internal::TimestampConverter::toProtobuf(mExpirationTime));
  protoTokenInfo->set_memo(mTokenMemo);
  protoTokenInfo->set_tokentype(gTokenTypeToProtobufTokenType.at(mTokenType));
  protoTokenInfo->set_supplytype(gTokenSupplyTypeToProtobufTokenSupplyType.at(mSupplyType));
  protoTokenInfo->set_maxsupply(static_cast<int64_t>(mMaxSupply));
  protoTokenInfo->set_allocated_fee_schedule_key(mFeeScheduleKey->toProtobufKey().release());

  for (const auto& fee : mCustomFees)
  {
    *protoTokenInfo->add_custom_fees() = *fee->toProtobuf();
  }

  protoTokenInfo->set_allocated_pause_key(mPauseKey->toProtobufKey().release());

  if (mPauseStatus.has_value())
  {
    protoTokenInfo->set_pause_status(mPauseStatus.value() ? proto::TokenPauseStatus::Paused
                                                          : proto::TokenPauseStatus::Unpaused);
  }
  else
  {
    protoTokenInfo->set_pause_status(proto::TokenPauseStatus::PauseNotApplicable);
  }

  protoTokenInfo->set_ledger_id(internal::Utilities::byteVectorToString(mLedgerId.toBytes()));
  protoTokenInfo->set_metadata(internal::Utilities::byteVectorToString(mMetadata));
  protoTokenInfo->set_allocated_metadata_key(mMetadataKey->toProtobufKey().release());

  return protoTokenInfo;
}

//-----
std::vector<std::byte> TokenInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TokenInfo::toString() const
{
  nlohmann::json json;
  json["mTokenId"] = mTokenId.toString();
  json["mTokenName"] = mTokenName;
  json["mTokenSymbol"] = mTokenSymbol;
  json["mDecimals"] = mDecimals;
  json["mTotalSupply"] = mTotalSupply;
  json["mTreasuryAccountId"] = mTreasuryAccountId.toString();

  if (mAdminKey)
  {
    json["mAdminKey"] = internal::HexConverter::bytesToHex(mAdminKey->toBytes());
  }

  if (mKycKey)
  {
    json["mKycKey"] = internal::HexConverter::bytesToHex(mKycKey->toBytes());
  }

  if (mFreezeKey)
  {
    json["mFreezeKey"] = internal::HexConverter::bytesToHex(mFreezeKey->toBytes());
  }

  if (mWipeKey)
  {
    json["mWipeKey"] = internal::HexConverter::bytesToHex(mWipeKey->toBytes());
  }

  if (mSupplyKey)
  {
    json["mSupplyKey"] = internal::HexConverter::bytesToHex(mSupplyKey->toBytes());
  }

  if (mDefaultFreezeStatus.has_value())
  {
    json["mDefaultFreezeStatus"] = mDefaultFreezeStatus.value();
  }

  if (mDefaultKycStatus.has_value())
  {
    json["mDefaultKycStatus"] = mDefaultKycStatus.value();
  }

  json["mIsDeleted"] = mIsDeleted;
  json["mAutoRenewAccountId"] = mAutoRenewAccountId.toString();
  json["mAutoRenewPeriod"] = std::to_string(mAutoRenewPeriod.count());
  json["mExpirationTime"] = internal::TimestampConverter::toString(mExpirationTime);
  json["mTokenMemo"] = mTokenMemo;
  json["mTokenType"] = gTokenTypeToString.at(mTokenType);
  json["mSupplyType"] = gTokenSupplyTypeToString.at(mSupplyType);
  json["mMaxSupply"] = mMaxSupply;

  if (mFeeScheduleKey)
  {
    json["mFeeScheduleKey"] = internal::HexConverter::bytesToHex(mFeeScheduleKey->toBytes());
  }

  std::for_each(mCustomFees.cbegin(),
                mCustomFees.cend(),
                [&json](const std::shared_ptr<CustomFee>& fee) { json["mCustomFees"].push_back(fee->toString()); });

  if (mPauseKey)
  {
    json["mPauseKey"] = internal::HexConverter::bytesToHex(mPauseKey->toBytes());
  }

  if (mPauseStatus.has_value())
  {
    json["mPauseStatus"] = mPauseStatus.value();
  }

  json["mLedgerId"] = mLedgerId.toString();

  if (!mMetadata.empty())
  {
    json["mMetadata"] = internal::HexConverter::bytesToHex(mMetadata);
  }

  if (mMetadataKey)
  {
    json["mMetadataKey"] = internal::HexConverter::bytesToHex(mMetadataKey->toBytes());
  }

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TokenInfo& info)
{
  os << info.toString();
  return os;
}

} // namespace Hiero
