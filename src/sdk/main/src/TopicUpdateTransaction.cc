// SPDX-License-Identifier: Apache-2.0
#include "TopicUpdateTransaction.h"
#include "KeyList.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"

#include <consensus_update_topic.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
TopicUpdateTransaction::TopicUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TopicUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TopicUpdateTransaction::TopicUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TopicUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setTopicId(const TopicId& topicId)
{
  requireNotFrozen();
  mTopicId = topicId;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setMemo(std::string_view memo)
{
  requireNotFrozen();
  mMemo = memo;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setExpirationTime(const std::chrono::system_clock::time_point& expiry)
{
  requireNotFrozen();
  mExpirationTime = expiry;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setSubmitKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mSubmitKey = key;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenew)
{
  requireNotFrozen();
  mAutoRenewPeriod = autoRenew;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setAutoRenewAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAutoRenewAccountId = accountId;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::clearTopicMemo()
{
  requireNotFrozen();
  mMemo = "";
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::clearAdminKey()
{
  requireNotFrozen();
  mAdminKey = std::make_unique<KeyList>();
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::clearSubmitKey()
{
  requireNotFrozen();
  mSubmitKey = std::make_unique<KeyList>();
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::clearAutoRenewAccountId()
{
  requireNotFrozen();
  mAutoRenewAccountId = AccountId();
  return *this;
}

//-----
grpc::Status TopicUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kConsensusUpdateTopic, request, deadline, response);
}

//-----
void TopicUpdateTransaction::validateChecksums(const Client& client) const
{
  mTopicId.validateChecksum(client);

  if (mAutoRenewAccountId.has_value())
  {
    mAutoRenewAccountId->validateChecksum(client);
  }
}

//-----
void TopicUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_consensusupdatetopic(build());
}

//-----
void TopicUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_consensusupdatetopic())
  {
    throw std::invalid_argument("Transaction body doesn't contain ConsensusUpdateTopic data");
  }

  const proto::ConsensusUpdateTopicTransactionBody& body = transactionBody.consensusupdatetopic();

  if (body.has_topicid())
  {
    mTopicId = TopicId::fromProtobuf(body.topicid());
  }

  if (body.has_memo())
  {
    mMemo = body.memo().value();
  }

  if (body.has_expirationtime())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expirationtime());
  }

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
proto::ConsensusUpdateTopicTransactionBody* TopicUpdateTransaction::build() const
{
  auto body = std::make_unique<proto::ConsensusUpdateTopicTransactionBody>();

  if (!(mTopicId == TopicId()))
  {
    body->set_allocated_topicid(mTopicId.toProtobuf().release());
  }

  if (mMemo.has_value())
  {
    body->mutable_memo()->set_value(mMemo.value());
  }

  if (mExpirationTime.has_value())
  {
    body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(mExpirationTime.value()));
  }

  if (mAdminKey)
  {
    body->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  if (mSubmitKey)
  {
    body->set_allocated_submitkey(mSubmitKey->toProtobufKey().release());
  }

  if (mAutoRenewPeriod.has_value())
  {
    body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod.value()));
  }

  if (mAutoRenewAccountId.has_value())
  {
    body->set_allocated_autorenewaccount(mAutoRenewAccountId->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero
