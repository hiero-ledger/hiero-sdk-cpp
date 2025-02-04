// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "TransactionResponse.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <crypto_create.pb.h>
#include <grpcpp/client_context.h>
#include <stdexcept>
#include <transaction.pb.h>
#include <transaction_response.pb.h>

namespace Hiero
{
//-----
AccountCreateTransaction::AccountCreateTransaction()
  : Transaction<AccountCreateTransaction>()
{
  setDefaultMaxTransactionFee(Hbar(10000LL));
}

//-----
AccountCreateTransaction::AccountCreateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<AccountCreateTransaction>(transactionBody)
{
  setDefaultMaxTransactionFee(Hbar(10000LL));
  initFromSourceTransactionBody();
}

//-----
AccountCreateTransaction::AccountCreateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<AccountCreateTransaction>(transactions)
{
  setDefaultMaxTransactionFee(Hbar(10000LL));
  initFromSourceTransactionBody();
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();

  mKey = key;
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setInitialBalance(const Hbar& initialBalance)
{
  requireNotFrozen();

  mInitialBalance = initialBalance;
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setReceiverSignatureRequired(bool receiveSignatureRequired)
{
  requireNotFrozen();

  mReceiverSignatureRequired = receiveSignatureRequired;
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setAutoRenewPeriod(
  const std::chrono::system_clock::duration& autoRenewPeriod)
{
  requireNotFrozen();

  mAutoRenewPeriod = autoRenewPeriod;
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setAccountMemo(std::string_view memo)
{
  requireNotFrozen();

  mAccountMemo = memo;
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setMaxAutomaticTokenAssociations(int32_t associations)
{
  requireNotFrozen();

  mMaxAutomaticTokenAssociations = associations;
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setStakedAccountId(const AccountId& stakedAccountId)
{
  requireNotFrozen();

  mStakedAccountId = stakedAccountId;
  mStakedNodeId.reset();
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setStakedNodeId(const uint64_t& stakedNodeId)
{
  requireNotFrozen();

  mStakedNodeId = stakedNodeId;
  mStakedAccountId.reset();
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setDeclineStakingReward(bool declineReward)
{
  requireNotFrozen();

  mDeclineStakingReward = declineReward;
  return *this;
}

//-----
AccountCreateTransaction& AccountCreateTransaction::setAlias(const EvmAddress& address)
{
  requireNotFrozen();

  mAlias = address;
  return *this;
}

//-----
grpc::Status AccountCreateTransaction::submitRequest(const proto::Transaction& request,
                                                     const std::shared_ptr<internal::Node>& node,
                                                     const std::chrono::system_clock::time_point& deadline,
                                                     proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kCryptoCreateAccount, request, deadline, response);
}

//-----
void AccountCreateTransaction::validateChecksums(const Client& client) const
{
  if (mStakedAccountId.has_value())
  {
    mStakedAccountId->validateChecksum(client);
  }
}

//-----
void AccountCreateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_cryptocreateaccount(build());
}

//-----
void AccountCreateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_cryptocreateaccount())
  {
    throw std::invalid_argument("Transaction body doesn't contain CryptoCreateAccount data");
  }

  const proto::CryptoCreateTransactionBody& body = transactionBody.cryptocreateaccount();

  if (body.has_key())
  {
    mKey = Key::fromProtobuf(body.key());
  }

  mInitialBalance = Hbar(static_cast<int64_t>(body.initialbalance()), HbarUnit::TINYBAR());
  mReceiverSignatureRequired = body.receiversigrequired();

  if (body.has_autorenewperiod())
  {
    mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(body.autorenewperiod());
  }

  mAccountMemo = body.memo();
  mMaxAutomaticTokenAssociations = body.max_automatic_token_associations();

  if (body.has_staked_account_id())
  {
    mStakedAccountId = AccountId::fromProtobuf(body.staked_account_id());
  }

  else if (body.has_staked_node_id())
  {
    mStakedNodeId = static_cast<uint64_t>(body.staked_node_id());
  }

  mDeclineStakingReward = body.decline_reward();

  if (!body.alias().empty())
  {
    mAlias = EvmAddress::fromBytes(internal::Utilities::stringToByteVector(body.alias()));
  }
}

//-----
proto::CryptoCreateTransactionBody* AccountCreateTransaction::build() const
{
  auto body = std::make_unique<proto::CryptoCreateTransactionBody>();

  if (mKey)
  {
    body->set_allocated_key(mKey->toProtobufKey().release());
  }

  body->set_initialbalance(mInitialBalance.toTinybars());
  body->set_receiversigrequired(mReceiverSignatureRequired);
  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod));
  body->set_memo(mAccountMemo);
  body->set_max_automatic_token_associations(mMaxAutomaticTokenAssociations);

  if (mStakedAccountId)
  {
    body->set_allocated_staked_account_id(mStakedAccountId->toProtobuf().release());
  }

  else if (mStakedNodeId)
  {
    body->set_staked_node_id(static_cast<int64_t>(*mStakedNodeId));
  }

  body->set_decline_reward(mDeclineStakingReward);

  if (mAlias.has_value())
  {
    body->set_alias(internal::Utilities::byteVectorToString(mAlias->toBytes()));
  }

  return body.release();
}

} // namespace Hiero
