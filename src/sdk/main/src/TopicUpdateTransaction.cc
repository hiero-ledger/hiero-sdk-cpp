// SPDX-License-Identifier: Apache-2.0
#include "TopicUpdateTransaction.h"
#include "KeyList.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"

#include <services/consensus_update_topic.pb.h>
#include <grpcpp/client_context.h>
#include <services/transaction.pb.h>

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
TopicUpdateTransaction& TopicUpdateTransaction::setFeeScheduleKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFeeScheduleKey = key;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setFeeExemptKeys(const std::vector<std::shared_ptr<Key>>& keys)
{
  requireNotFrozen();
  mFeeExemptKeys = keys;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::addFeeExemptKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFeeExemptKeys.push_back(key);
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::setCustomFixedFees(const std::vector<CustomFixedFee>& fees)
{
  requireNotFrozen();
  mCustomFixedFees = fees;
  return *this;
}

//-----
TopicUpdateTransaction& TopicUpdateTransaction::addCustomFixedFee(const CustomFixedFee& fee)
{
  requireNotFrozen();
  mCustomFixedFees.push_back(fee);
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

  if (body.has_fee_schedule_key())
  {
    mFeeScheduleKey = Key::fromProtobuf(body.fee_schedule_key());
  }

  for (const auto& key : body.fee_exempt_key_list().keys())
  {
    mFeeExemptKeys.push_back(Key::fromProtobuf(key));
  }

  for (const auto& fee : body.custom_fees().fees())
  {
    mCustomFixedFees.push_back(CustomFixedFee::fromProtobuf(fee.fixed_fee())
                                 .setFeeCollectorAccountId(AccountId::fromProtobuf(fee.fee_collector_account_id())));
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

  if (mFeeScheduleKey)
  {
    body->set_allocated_fee_schedule_key(mFeeScheduleKey->toProtobufKey().release());
  }

  for (const auto& key : mFeeExemptKeys)
  {
    body->mutable_fee_exempt_key_list()->mutable_keys()->AddAllocated(key->toProtobufKey().release());
  }

  for (const auto& fee : mCustomFixedFees)
  {
    std::unique_ptr<proto::FixedCustomFee> fixedCustomFee = std::make_unique<proto::FixedCustomFee>();
    fixedCustomFee->set_allocated_fixed_fee(fee.toFixedFeeProtobuf().release());
    body->mutable_custom_fees()->mutable_fees()->AddAllocated(fixedCustomFee.release());
  }

  return body.release();
}

} // namespace Hiero
