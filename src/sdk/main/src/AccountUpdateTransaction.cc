// SPDX-License-Identifier: Apache-2.0
#include "AccountUpdateTransaction.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"

#include <crypto_update.pb.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
AccountUpdateTransaction::AccountUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<AccountUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
AccountUpdateTransaction::AccountUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<AccountUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();

  mAccountId = accountId;
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();

  mKey = key;
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setReceiverSignatureRequired(bool receiveSignatureRequired)
{
  requireNotFrozen();

  mReceiverSignatureRequired = receiveSignatureRequired;
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setAutoRenewPeriod(
  const std::chrono::system_clock::duration& autoRenewPeriod)
{
  requireNotFrozen();

  mAutoRenewPeriod = autoRenewPeriod;
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setExpirationTime(
  const std::chrono::system_clock::time_point& expiration)
{
  requireNotFrozen();

  mExpirationTime = expiration;
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setAccountMemo(std::string_view memo)
{
  requireNotFrozen();

  mAccountMemo = memo;
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::clearAccountMemo()
{
  requireNotFrozen();
  mAccountMemo = "";
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setMaxAutomaticTokenAssociations(int32_t associations)
{
  requireNotFrozen();

  mMaxAutomaticTokenAssociations = associations;
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setStakedAccountId(const AccountId& stakedAccountId)
{
  requireNotFrozen();

  mStakedAccountId = stakedAccountId;
  mStakedNodeId.reset();
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::clearStakedAccountId()
{
  requireNotFrozen();
  mStakedAccountId = AccountId(0ULL);
  mStakedNodeId.reset();
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setStakedNodeId(const uint64_t& stakedNodeId)
{
  requireNotFrozen();

  mStakedNodeId = stakedNodeId;
  mStakedAccountId.reset();
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::clearStakedNodeId()
{
  requireNotFrozen();

  mStakedNodeId = -1LL;
  mStakedAccountId.reset();
  return *this;
}

//-----
AccountUpdateTransaction& AccountUpdateTransaction::setDeclineStakingReward(bool declineReward)
{
  requireNotFrozen();

  mDeclineStakingReward = declineReward;
  return *this;
}

//-----
grpc::Status AccountUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                     const std::shared_ptr<internal::Node>& node,
                                                     const std::chrono::system_clock::time_point& deadline,
                                                     proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kCryptoUpdateAccount, request, deadline, response);
}

//-----
void AccountUpdateTransaction::validateChecksums(const Client& client) const
{
  mAccountId.validateChecksum(client);

  if (mStakedAccountId.has_value())
  {
    mStakedAccountId->validateChecksum(client);
  }
}

//-----
void AccountUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_cryptoupdateaccount(build());
}

//-----
void AccountUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_cryptoupdateaccount())
  {
    throw std::invalid_argument("Transaction body doesn't contain CryptoUpdate data");
  }

  const proto::CryptoUpdateTransactionBody& body = transactionBody.cryptoupdateaccount();

  if (body.has_accountidtoupdate())
  {
    mAccountId = AccountId::fromProtobuf(body.accountidtoupdate());
  }

  if (body.has_key())
  {
    mKey = Key::fromProtobuf(body.key());
  }

  if (body.has_receiversigrequiredwrapper())
  {
    mReceiverSignatureRequired = body.receiversigrequiredwrapper().value();
  }

  if (body.has_autorenewperiod())
  {
    mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(body.autorenewperiod());
  }

  if (body.has_expirationtime())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expirationtime());
  }

  if (body.has_memo())
  {
    mAccountMemo = body.memo().value();
  }

  if (body.has_max_automatic_token_associations())
  {
    mMaxAutomaticTokenAssociations = body.max_automatic_token_associations().value();
  }

  if (body.has_staked_account_id())
  {
    mStakedAccountId = AccountId::fromProtobuf(body.staked_account_id());
  }

  if (body.has_staked_node_id())
  {
    mStakedNodeId = static_cast<uint64_t>(body.staked_node_id());
  }

  if (body.has_decline_reward())
  {
    mDeclineStakingReward = body.decline_reward().value();
  }
}

//-----
proto::CryptoUpdateTransactionBody* AccountUpdateTransaction::build() const
{
  auto body = std::make_unique<proto::CryptoUpdateTransactionBody>();

  if (!(mAccountId == AccountId()))
  {
    body->set_allocated_accountidtoupdate(mAccountId.toProtobuf().release());
  }

  if (mKey)
  {
    body->set_allocated_key(mKey->toProtobufKey().release());
  }

  if (mReceiverSignatureRequired.has_value())
  {
    auto value = std::make_unique<google::protobuf::BoolValue>();
    value->set_value(mReceiverSignatureRequired.value());
    body->set_allocated_receiversigrequiredwrapper(value.release());
  }

  if (mAutoRenewPeriod.has_value())
  {
    body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod.value()));
  }

  if (mExpirationTime.has_value())
  {
    body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(mExpirationTime.value()));
  }

  if (mAccountMemo.has_value())
  {
    auto value = std::make_unique<google::protobuf::StringValue>();
    value->set_value(mAccountMemo.value());
    body->set_allocated_memo(value.release());
  }

  if (mMaxAutomaticTokenAssociations.has_value())
  {
    auto value = std::make_unique<google::protobuf::Int32Value>();
    value->set_value(mMaxAutomaticTokenAssociations.value());
    body->set_allocated_max_automatic_token_associations(value.release());
  }

  if (mStakedAccountId.has_value())
  {
    body->set_allocated_staked_account_id(mStakedAccountId->toProtobuf().release());
  }

  if (mStakedNodeId.has_value())
  {
    body->set_staked_node_id(static_cast<int64_t>(mStakedNodeId.value()));
  }

  if (mDeclineStakingReward.has_value())
  {
    auto value = std::make_unique<google::protobuf::BoolValue>();
    value->set_value(mDeclineStakingReward.value());
    body->set_allocated_decline_reward(value.release());
  }

  return body.release();
}

} // namespace Hiero
