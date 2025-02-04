// SPDX-License-Identifier: Apache-2.0
#include "ContractCreateTransaction.h"
#include "Client.h"
#include "impl/DurationConverter.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <contract_create.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_response.pb.h>

namespace Hiero
{
//-----
ContractCreateTransaction::ContractCreateTransaction()
  : Transaction<ContractCreateTransaction>()
{
  setDefaultMaxTransactionFee(Hbar(20LL));
}

//-----
ContractCreateTransaction::ContractCreateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<ContractCreateTransaction>(transactionBody)
{
  setDefaultMaxTransactionFee(Hbar(20LL));
  initFromSourceTransactionBody();
}

//-----
ContractCreateTransaction::ContractCreateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<ContractCreateTransaction>(transactions)
{
  setDefaultMaxTransactionFee(Hbar(20LL));
  initFromSourceTransactionBody();
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setBytecodeFileId(const FileId& fileId)
{
  requireNotFrozen();
  mBytecodeFileId = fileId;
  mBytecode.reset();
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setBytecode(const std::vector<std::byte>& initCode)
{
  requireNotFrozen();
  mBytecode = initCode;
  mBytecodeFileId.reset();
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setGas(const uint64_t& gas)
{
  requireNotFrozen();
  mGas = gas;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setInitialBalance(const Hbar& initialBalance)
{
  requireNotFrozen();
  mInitialBalance = initialBalance;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setAutoRenewPeriod(
  const std::chrono::system_clock::duration& autoRenewPeriod)
{
  requireNotFrozen();
  mAutoRenewPeriod = autoRenewPeriod;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setConstructorParameters(
  const std::vector<std::byte>& constructorParameters)
{
  requireNotFrozen();
  mConstructorParameters = constructorParameters;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setMemo(std::string_view memo)
{
  requireNotFrozen();
  mMemo = memo;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setMaxAutomaticTokenAssociations(int32_t associations)
{
  requireNotFrozen();
  mMaxAutomaticTokenAssociations = associations;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setAutoRenewAccountId(const AccountId& autoRenewAccountId)
{
  requireNotFrozen();
  mAutoRenewAccountId = autoRenewAccountId;
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setStakedAccountId(const AccountId& stakedAccountId)
{
  requireNotFrozen();
  mStakedAccountId = stakedAccountId;
  mStakedNodeId.reset();
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setStakedNodeId(const uint64_t& stakedNodeId)
{
  requireNotFrozen();
  mStakedNodeId = stakedNodeId;
  mStakedAccountId.reset();
  return *this;
}

//-----
ContractCreateTransaction& ContractCreateTransaction::setDeclineStakingReward(bool declineReward)
{
  requireNotFrozen();
  mDeclineStakingReward = declineReward;
  return *this;
}

//-----
grpc::Status ContractCreateTransaction::submitRequest(const proto::Transaction& request,
                                                      const std::shared_ptr<internal::Node>& node,
                                                      const std::chrono::system_clock::time_point& deadline,
                                                      proto::TransactionResponse* response) const
{
  return node->submitTransaction(
    proto::TransactionBody::DataCase::kContractCreateInstance, request, deadline, response);
}

//-----
void ContractCreateTransaction::validateChecksums(const Client& client) const
{
  if (mBytecodeFileId.has_value())
  {
    mBytecodeFileId->validateChecksum(client);
  }

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
void ContractCreateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_contractcreateinstance(build());
}

//-----
void ContractCreateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_contractcreateinstance())
  {
    throw std::invalid_argument("Transaction body doesn't contain ContractCreateInstance data");
  }

  const proto::ContractCreateTransactionBody& body = transactionBody.contractcreateinstance();

  if (body.has_fileid())
  {
    mBytecodeFileId = FileId::fromProtobuf(body.fileid());
  }

  if (body.has_initcode())
  {
    mBytecode = internal::Utilities::stringToByteVector(body.initcode());
  }

  if (body.has_adminkey())
  {
    mAdminKey = Key::fromProtobuf(body.adminkey());
  }

  mGas = static_cast<uint64_t>(body.gas());
  mInitialBalance = Hbar(body.initialbalance(), HbarUnit::TINYBAR());

  if (body.has_autorenewperiod())
  {
    mAutoRenewPeriod = internal::DurationConverter::fromProtobuf(body.autorenewperiod());
  }

  mConstructorParameters = internal::Utilities::stringToByteVector(body.constructorparameters());
  mMemo = body.memo();
  mMaxAutomaticTokenAssociations = body.max_automatic_token_associations();

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

  mDeclineStakingReward = body.decline_reward();
}

//-----
proto::ContractCreateTransactionBody* ContractCreateTransaction::build() const
{
  auto body = std::make_unique<proto::ContractCreateTransactionBody>();

  if (mBytecodeFileId.has_value())
  {
    body->set_allocated_fileid(mBytecodeFileId->toProtobuf().release());
  }

  else if (mBytecode.has_value())
  {
    body->set_initcode(internal::Utilities::byteVectorToString(*mBytecode));
  }

  if (mAdminKey)
  {
    body->set_allocated_adminkey(mAdminKey->toProtobufKey().release());
  }

  body->set_gas(static_cast<int64_t>(mGas));
  body->set_initialbalance(mInitialBalance.toTinybars());
  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(mAutoRenewPeriod));
  body->set_constructorparameters(internal::Utilities::byteVectorToString(mConstructorParameters));
  body->set_memo(mMemo);
  body->set_max_automatic_token_associations(mMaxAutomaticTokenAssociations);

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
    body->set_staked_node_id(static_cast<int64_t>(*mStakedNodeId));
  }

  body->set_decline_reward(mDeclineStakingReward);

  return body.release();
}

} // namespace Hiero
