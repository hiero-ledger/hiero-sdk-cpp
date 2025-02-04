// SPDX-License-Identifier: Apache-2.0
#include "TokenCreateTransaction.h"
#include "Hbar.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_create.pb.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
TokenCreateTransaction::TokenCreateTransaction()
{
  setDefaultMaxTransactionFee(Hbar(40LL));
}

//-----
TokenCreateTransaction::TokenCreateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenCreateTransaction>(transactionBody)
{
  setDefaultMaxTransactionFee(Hbar(40LL));
  initFromSourceTransactionBody();
}

//-----
TokenCreateTransaction::TokenCreateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenCreateTransaction>(transactions)
{
  setDefaultMaxTransactionFee(Hbar(40LL));
  initFromSourceTransactionBody();
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setTokenName(std::string_view name)
{
  requireNotFrozen();
  mTokenName = name;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setTokenSymbol(std::string_view symbol)
{
  requireNotFrozen();
  mTokenSymbol = symbol;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setDecimals(uint32_t decimals)
{
  requireNotFrozen();
  mDecimals = decimals;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setInitialSupply(const uint64_t& amount)
{
  requireNotFrozen();
  mInitialSupply = amount;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setTreasuryAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mTreasuryAccountId = accountId;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setKycKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mKycKey = key;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setFreezeKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFreezeKey = key;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setWipeKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mWipeKey = key;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setSupplyKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mSupplyKey = key;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setFreezeDefault(bool freeze)
{
  requireNotFrozen();
  mFreezeDefault = freeze;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setExpirationTime(
  const std::chrono::system_clock::time_point& expiration)
{
  requireNotFrozen();
  mExpirationTime = expiration;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setAutoRenewAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAutoRenewAccountId = accountId;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setAutoRenewPeriod(const std::chrono::system_clock::duration& period)
{
  requireNotFrozen();
  mAutoRenewPeriod = period;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setTokenMemo(std::string_view memo)
{
  requireNotFrozen();
  mTokenMemo = memo;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setTokenType(TokenType type)
{
  requireNotFrozen();
  mTokenType = type;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setSupplyType(TokenSupplyType type)
{
  requireNotFrozen();
  mSupplyType = type;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setMaxSupply(const uint64_t& max)
{
  requireNotFrozen();
  mMaxSupply = max;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setFeeScheduleKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFeeScheduleKey = key;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setCustomFees(const std::vector<std::shared_ptr<CustomFee>>& fees)
{
  requireNotFrozen();
  mCustomFees = fees;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setPauseKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mPauseKey = key;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setMetadata(const std::vector<std::byte>& metadata)
{
  requireNotFrozen();
  mMetadata = metadata;
  return *this;
}

//-----
TokenCreateTransaction& TokenCreateTransaction::setMetadataKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mMetadataKey = key;
  return *this;
}

//-----
grpc::Status TokenCreateTransaction::submitRequest(const proto::Transaction& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenCreation, request, deadline, response);
}

//-----
void TokenCreateTransaction::validateChecksums(const Client& client) const
{
  if (mTreasuryAccountId.has_value())
  {
    mTreasuryAccountId->validateChecksum(client);
  }

  if (mAutoRenewAccountId.has_value())
  {
    mAutoRenewAccountId->validateChecksum(client);
  }

  std::for_each(mCustomFees.cbegin(),
                mCustomFees.cend(),
                [&client](const std::shared_ptr<CustomFee>& fee) { fee->validateChecksums(client); });
}

//-----
void TokenCreateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokencreation(build());
}

//-----
void TokenCreateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokencreation())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenCreate data");
  }

  const proto::TokenCreateTransactionBody& body = transactionBody.tokencreation();

  mTokenName = body.name();
  mTokenSymbol = body.symbol();
  mDecimals = body.decimals();
  mInitialSupply = body.initialsupply();

  if (body.has_treasury())
  {
    mTreasuryAccountId = AccountId::fromProtobuf(body.treasury());
  }

  if (body.has_adminkey())
  {
    mAdminKey = Key::fromProtobuf(body.adminkey());
  }

  if (body.has_kyckey())
  {
    mKycKey = Key::fromProtobuf(body.kyckey());
  }

  if (body.has_freezekey())
  {
    mFreezeKey = Key::fromProtobuf(body.freezekey());
  }

  if (body.has_wipekey())
  {
    mWipeKey = Key::fromProtobuf(body.wipekey());
  }

  if (body.has_supplykey())
  {
    mSupplyKey = Key::fromProtobuf(body.supplykey());
  }

  mFreezeDefault = body.freezedefault();

  if (body.has_expiry())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expiry());
  }

  if (body.has_autorenewaccount())
  {
    mAutoRenewAccountId = AccountId::fromProtobuf(body.autorenewaccount());
  }

  if (body.has_autorenewperiod())
  {
    mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(body.autorenewperiod());
  }

  mTokenMemo = body.memo();
  mTokenType = gProtobufTokenTypeToTokenType.at(body.tokentype());
  mSupplyType = gProtobufTokenSupplyTypeToTokenSupplyType.at(body.supplytype());
  mMaxSupply = static_cast<uint64_t>(body.maxsupply());

  if (body.has_fee_schedule_key())
  {
    mFeeScheduleKey = Key::fromProtobuf(body.fee_schedule_key());
  }

  for (int i = 0; i < body.custom_fees_size(); ++i)
  {
    mCustomFees.push_back(CustomFee::fromProtobuf(body.custom_fees(i)));
  }

  if (body.has_pause_key())
  {
    mPauseKey = Key::fromProtobuf(body.pause_key());
  }

  mMetadata = internal::Utilities::stringToByteVector(body.metadata());

  if (body.has_metadata_key())
  {
    mMetadataKey = Key::fromProtobuf(body.metadata_key());
  }
}

//-----
proto::TokenCreateTransactionBody* TokenCreateTransaction::build() const
{
  auto body = std::make_unique<proto::TokenCreateTransactionBody>();
  body->set_name(mTokenName);
  body->set_symbol(mTokenSymbol);
  body->set_decimals(mDecimals);
  body->set_initialsupply(mInitialSupply);

  if (mTreasuryAccountId.has_value())
  {
    body->set_allocated_treasury(mTreasuryAccountId->toProtobuf().release());
  }

  if (mAdminKey)
  {
    body->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  if (mKycKey)
  {
    body->set_allocated_kyckey(mKycKey->toProtobufKey().release());
  }

  if (mFreezeKey)
  {
    body->set_allocated_freezekey(mFreezeKey->toProtobufKey().release());
  }

  if (mWipeKey)
  {
    body->set_allocated_wipekey(mWipeKey->toProtobufKey().release());
  }

  if (mSupplyKey)
  {
    body->set_allocated_supplykey(mSupplyKey->toProtobufKey().release());
  }

  body->set_freezedefault(mFreezeDefault);
  body->set_allocated_expiry(internal::TimestampConverter::toProtobuf(mExpirationTime));

  if (mAutoRenewAccountId.has_value())
  {
    body->set_allocated_autorenewaccount(mAutoRenewAccountId->toProtobuf().release());
  }

  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod));
  body->set_memo(mTokenMemo);
  body->set_tokentype(gTokenTypeToProtobufTokenType.at(mTokenType));
  body->set_supplytype(gTokenSupplyTypeToProtobufTokenSupplyType.at(mSupplyType));
  body->set_maxsupply(static_cast<int64_t>(mMaxSupply));

  if (mFeeScheduleKey)
  {
    body->set_allocated_fee_schedule_key(mFeeScheduleKey->toProtobufKey().release());
  }

  for (const auto& fee : mCustomFees)
  {
    *body->add_custom_fees() = *fee->toProtobuf();
  }

  if (mPauseKey)
  {
    body->set_allocated_pause_key(mPauseKey->toProtobufKey().release());
  }

  body->set_metadata(internal::Utilities::byteVectorToString(mMetadata));

  if (mMetadataKey)
  {
    body->set_allocated_metadata_key(mMetadataKey->toProtobufKey().release());
  }

  return body.release();
}

} // namespace Hiero
