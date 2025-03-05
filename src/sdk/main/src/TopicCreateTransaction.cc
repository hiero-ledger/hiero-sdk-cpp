// SPDX-License-Identifier: Apache-2.0
#include "TopicCreateTransaction.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"

#include <consensus_create_topic.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>

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
TopicCreateTransaction& TopicCreateTransaction::setFeeScheduleKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFeeScheduleKey = key;
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::setFeeExemptKeys(const std::vector<std::shared_ptr<Key>>& keys)
{
  requireNotFrozen();
  mFeeExemptKeys = keys;
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::addFeeExemptKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mFeeExemptKeys.push_back(key);
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::setCustomFixedFees(const std::vector<CustomFixedFee>& fees)
{
  requireNotFrozen();
  mCustomFixedFees = fees;
  return *this;
}

//-----
TopicCreateTransaction& TopicCreateTransaction::addCustomFixedFee(const CustomFixedFee& fee)
{
  requireNotFrozen();
  mCustomFixedFees.push_back(fee);
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

  if (body.has_transactionid() && !body.consensuscreatetopic().has_autorenewaccount())
  {
    std::unique_ptr<proto::AccountID> accountId = std::make_unique<proto::AccountID>(body.transactionid().accountid());
    body.mutable_consensuscreatetopic()->set_allocated_autorenewaccount(accountId.release());
  }
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

  if (body.has_fee_schedule_key())
  {
    mFeeScheduleKey = Key::fromProtobuf(body.fee_schedule_key());
  }

  for (const auto& key : body.fee_exempt_key_list())
  {
    mFeeExemptKeys.push_back(Key::fromProtobuf(key));
  }

  for (const auto& fee : body.custom_fees())
  {
    mCustomFixedFees.push_back(CustomFixedFee::fromProtobuf(fee.fixed_fee())
                                 .setFeeCollectorAccountId(AccountId::fromProtobuf(fee.fee_collector_account_id())));
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

  if (mFeeScheduleKey)
  {
    body->set_allocated_fee_schedule_key(mFeeScheduleKey->toProtobufKey().release());
  }

  for (const auto& key : mFeeExemptKeys)
  {
    body->mutable_fee_exempt_key_list()->AddAllocated(key->toProtobufKey().release());
  }

  for (const auto& fee : mCustomFixedFees)
  {
    std::unique_ptr<proto::FixedCustomFee> fixedCustomFee = std::make_unique<proto::FixedCustomFee>();
    fixedCustomFee->set_allocated_fixed_fee(fee.toFixedFeeProtobuf().release());
    fixedCustomFee->set_allocated_fee_collector_account_id(fee.getFeeCollectorAccountId().toProtobuf().release());
    body->mutable_custom_fees()->AddAllocated(fixedCustomFee.release());
  }

  return body.release();
}

} // namespace Hiero
