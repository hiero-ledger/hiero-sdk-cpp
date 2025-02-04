// SPDX-License-Identifier: Apache-2.0
#include "TopicCreateTransaction.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"

#include <consensus_create_topic.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
TopicCreateTransaction::TopicCreateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TopicCreateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TopicCreateTransaction::TopicCreateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TopicCreateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TopicCreateTransaction& TopicCreateTransaction::setMemo(std::string_view memo)
{
  requireNotFrozen();
  mMemo = memo;
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::setSubmitKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mSubmitKey = key;
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenew)
{
  requireNotFrozen();
  mAutoRenewPeriod = autoRenew;
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::setAutoRenewAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAutoRenewAccountId = accountId;
  return *this;
}

//-----
grpc::Status TopicCreateTransaction::submitRequest(const proto::Transaction& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kConsensusCreateTopic, request, deadline, response);
}

//-----
void TopicCreateTransaction::validateChecksums(const Client& client) const
{
  if (mAutoRenewAccountId.has_value())
  {
    mAutoRenewAccountId->validateChecksum(client);
  }
}

//-----
void TopicCreateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_consensuscreatetopic(build());
}

//-----
void TopicCreateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_consensuscreatetopic())
  {
    throw std::invalid_argument("Transaction body doesn't contain ConsensusCreateTopic data");
  }

  const proto::ConsensusCreateTopicTransactionBody& body = transactionBody.consensuscreatetopic();

  mMemo = body.memo();

  if (body.has_adminkey())
  {
    mAdminKey = Key::fromProtobuf(body.adminkey());
  }

  if (body.has_submitkey())
  {
    mSubmitKey = Key::fromProtobuf(body.submitkey());
  }

  if (body.has_autorenewperiod())
  {
    mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(body.autorenewperiod());
  }

  if (body.has_autorenewaccount())
  {
    mAutoRenewAccountId = AccountId::fromProtobuf(body.autorenewaccount());
  }
}

//-----
proto::ConsensusCreateTopicTransactionBody* TopicCreateTransaction::build() const
{
  auto body = std::make_unique<proto::ConsensusCreateTopicTransactionBody>();

  body->set_memo(mMemo);

  if (mAdminKey)
  {
    body->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  if (mSubmitKey)
  {
    body->set_allocated_submitkey(mSubmitKey->toProtobufKey().release());
  }

  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod));

  if (mAutoRenewAccountId.has_value())
  {
    body->set_allocated_autorenewaccount(mAutoRenewAccountId->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero
