// SPDX-License-Identifier: Apache-2.0
#include "TokenUpdateTransaction.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_update.pb.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
TokenUpdateTransaction::TokenUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenUpdateTransaction::TokenUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setTokenName(std::string_view name)
{
  requireNotFrozen();
  mTokenName = name;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setTokenSymbol(std::string_view symbol)
{
  requireNotFrozen();
  mTokenSymbol = symbol;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setTreasuryAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mTreasuryAccountId = accountId;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setKycKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mKycKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setFreezeKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFreezeKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setWipeKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mWipeKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setSupplyKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mSupplyKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setAutoRenewAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAutoRenewAccountId = accountId;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setAutoRenewPeriod(const std::chrono::system_clock::duration& period)
{
  requireNotFrozen();
  mAutoRenewPeriod = period;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setExpirationTime(
  const std::chrono::system_clock::time_point& expiration)
{
  requireNotFrozen();
  mExpirationTime = expiration;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setTokenMemo(std::string_view memo)
{
  requireNotFrozen();
  mTokenMemo = memo;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setFeeScheduleKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFeeScheduleKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setPauseKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mPauseKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setMetadata(const std::vector<std::byte>& metadata)
{
  requireNotFrozen();
  mMetadata = metadata;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setMetadataKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mMetadataKey = key;
  return *this;
}

//-----
TokenUpdateTransaction& TokenUpdateTransaction::setTokenVerificationMode(TokenKeyValidation mode)
{
  requireNotFrozen();
  mKeyVerificationMode = mode;
  return *this;
}

//-----
grpc::Status TokenUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenUpdate, request, deadline, response);
}

//-----
void TokenUpdateTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);

  if (mTreasuryAccountId.has_value())
  {
    mTreasuryAccountId->validateChecksum(client);
  }

  if (mAutoRenewAccountId.has_value())
  {
    mAutoRenewAccountId->validateChecksum(client);
  }
}

//-----
void TokenUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenupdate(build());
}

//-----
void TokenUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenupdate())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenUpdate data");
  }

  const proto::TokenUpdateTransactionBody& body = transactionBody.tokenupdate();

  if (body.has_token())
  {
    mTokenId = TokenId::fromProtobuf(body.token());
  }

  mTokenName = body.name();
  mTokenSymbol = body.symbol();

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

  if (body.has_autorenewaccount())
  {
    mAutoRenewAccountId = AccountId::fromProtobuf(body.autorenewaccount());
  }

  if (body.has_autorenewperiod())
  {
    mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(body.autorenewperiod());
  }

  if (body.has_expiry())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expiry());
  }

  if (body.has_memo())
  {
    mTokenMemo = body.memo().value();
  }

  if (body.has_fee_schedule_key())
  {
    mFeeScheduleKey = Key::fromProtobuf(body.fee_schedule_key());
  }

  if (body.has_pause_key())
  {
    mPauseKey = Key::fromProtobuf(body.pause_key());
  }

  mMetadata = internal::Utilities::stringToByteVector(body.metadata().value());

  if (body.has_metadata_key())
  {
    mMetadataKey = Key::fromProtobuf(body.metadata_key());
  }

  mKeyVerificationMode = gProtobufTokenKeyValidationToTokenKeyValidation.at(body.key_verification_mode());
}

//-----
proto::TokenUpdateTransactionBody* TokenUpdateTransaction::build() const
{
  auto body = std::make_unique<proto::TokenUpdateTransactionBody>();
  body->set_allocated_token(mTokenId.toProtobuf().release());

  if (mTokenName.has_value())
  {
    body->set_name(mTokenName.value());
  }

  if (mTokenSymbol.has_value())
  {
    body->set_symbol(mTokenSymbol.value());
  }

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

  if (mAutoRenewAccountId.has_value())
  {
    body->set_allocated_autorenewaccount(mAutoRenewAccountId->toProtobuf().release());
  }

  if (mAutoRenewPeriod.has_value())
  {
    body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod.value()));
  }

  if (mExpirationTime.has_value())
  {
    body->set_allocated_expiry(internal::TimestampConverter::toProtobuf(mExpirationTime.value()));
  }

  if (mTokenMemo.has_value())
  {
    body->mutable_memo()->set_value(mTokenMemo.value());
  }

  if (mFeeScheduleKey)
  {
    body->set_allocated_fee_schedule_key(mFeeScheduleKey->toProtobufKey().release());
  }

  if (mPauseKey)
  {
    body->set_allocated_pause_key(mPauseKey->toProtobufKey().release());
  }

  if (!mMetadata.empty())
  {
    body->mutable_metadata()->set_value(internal::Utilities::byteVectorToString(mMetadata));
  }

  if (mMetadataKey)
  {
    body->set_allocated_metadata_key(mMetadataKey->toProtobufKey().release());
  }

  body->set_key_verification_mode(gTokenKeyValidationToProtobufTokenKeyValidation.at(mKeyVerificationMode));

  return body.release();
}

} // namespace Hiero
