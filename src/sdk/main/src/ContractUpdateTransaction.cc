// SPDX-License-Identifier: Apache-2.0
#include "ContractUpdateTransaction.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"

#include <contract_update.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
ContractUpdateTransaction::ContractUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<ContractUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
ContractUpdateTransaction::ContractUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<ContractUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setContractId(const ContractId& contractId)
{
  requireNotFrozen();
  mContractId = contractId;
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setExpirationTime(
  const std::chrono::system_clock::time_point& expiration)
{
  requireNotFrozen();
  mExpirationTime = expiration;
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setAdminKey(const std::shared_ptr<Key>& adminKey)
{
  requireNotFrozen();
  mAdminKey = adminKey;
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setAutoRenewPeriod(
  const std::chrono::system_clock::duration& autoRenewPeriod)
{
  requireNotFrozen();
  mAutoRenewPeriod = autoRenewPeriod;
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setContractMemo(std::string_view memo)
{
  requireNotFrozen();

  if (memo.size() > 100)
  {
    throw std::length_error("Contract memo is too large. Must be smaller than 100 bytes");
  }

  mContractMemo = memo;
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setMaxAutomaticTokenAssociations(int32_t associations)
{
  requireNotFrozen();
  mMaxAutomaticTokenAssociations = associations;
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setAutoRenewAccountId(const AccountId& autoRenewAccountId)
{
  requireNotFrozen();
  mAutoRenewAccountId = autoRenewAccountId;
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setStakedAccountId(const AccountId& stakedAccountId)
{
  requireNotFrozen();
  mStakedAccountId = stakedAccountId;
  mStakedNodeId.reset();
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setStakedNodeId(const uint64_t& stakedNodeId)
{
  requireNotFrozen();
  mStakedNodeId = stakedNodeId;
  mStakedAccountId.reset();
  return *this;
}

//-----
ContractUpdateTransaction& ContractUpdateTransaction::setDeclineStakingReward(bool declineReward)
{
  requireNotFrozen();
  mDeclineStakingReward = declineReward;
  return *this;
}

//-----
grpc::Status ContractUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                      const std::shared_ptr<internal::Node>& node,
                                                      const std::chrono::system_clock::time_point& deadline,
                                                      proto::TransactionResponse* response) const
{
  return node->submitTransaction(
    proto::TransactionBody::DataCase::kContractUpdateInstance, request, deadline, response);
}

//-----
void ContractUpdateTransaction::validateChecksums(const Client& client) const
{
  mContractId.validateChecksum(client);

  if (mAutoRenewAccountId.has_value())
  {
    mAutoRenewAccountId->validateChecksum(client);
  }

  if (mStakedAccountId.has_value())
  {
    mStakedAccountId->validateChecksum(client);
  }
}

//-----
void ContractUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_contractupdateinstance(build());
}

//-----
void ContractUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_contractupdateinstance())
  {
    throw std::invalid_argument("Transaction body doesn't contain ContractUpdateInstance data");
  }

  const proto::ContractUpdateTransactionBody& body = transactionBody.contractupdateinstance();

  if (body.has_contractid())
  {
    mContractId = ContractId::fromProtobuf(body.contractid());
  }

  if (body.has_expirationtime())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expirationtime());
  }

  if (body.has_adminkey())
  {
    mAdminKey = Key::fromProtobuf(body.adminkey());
  }

  if (body.has_autorenewperiod())
  {
    mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(body.autorenewperiod());
  }

  if (body.has_memowrapper())
  {
    mContractMemo = body.memowrapper().value();
  }

  if (body.has_max_automatic_token_associations())
  {
    mMaxAutomaticTokenAssociations = body.max_automatic_token_associations().value();
  }

  if (body.has_auto_renew_account_id())
  {
    mAutoRenewAccountId = AccountId::fromProtobuf(body.auto_renew_account_id());
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
proto::ContractUpdateTransactionBody* ContractUpdateTransaction::build() const
{
  auto body = std::make_unique<proto::ContractUpdateTransactionBody>();

  body->set_allocated_contractid(mContractId.toProtobuf().release());

  if (mExpirationTime.has_value())
  {
    body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(mExpirationTime.value()));
  }

  if (mAdminKey)
  {
    body->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  if (mAutoRenewPeriod.has_value())
  {
    body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod.value()));
  }

  if (mContractMemo.has_value())
  {
    auto value = std::make_unique<google::protobuf::StringValue>();
    value->set_value(mContractMemo.value());
    body->set_allocated_memowrapper(value.release());
  }

  if (mMaxAutomaticTokenAssociations.has_value())
  {
    auto value = std::make_unique<google::protobuf::Int32Value>();
    value->set_value(mMaxAutomaticTokenAssociations.value());
    body->set_allocated_max_automatic_token_associations(value.release());
  }

  if (mAutoRenewAccountId.has_value())
  {
    body->set_allocated_auto_renew_account_id(mAutoRenewAccountId->toProtobuf().release());
  }

  if (mStakedAccountId.has_value())
  {
    body->set_allocated_staked_account_id(mStakedAccountId->toProtobuf().release());
  }

  else if (mStakedNodeId.has_value())
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
