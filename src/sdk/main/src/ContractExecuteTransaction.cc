// SPDX-License-Identifier: Apache-2.0
#include "ContractExecuteTransaction.h"
#include "Client.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <contract_call.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_response.pb.h>

namespace Hiero
{
//-----
ContractExecuteTransaction::ContractExecuteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<ContractExecuteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
ContractExecuteTransaction::ContractExecuteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<ContractExecuteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
ContractExecuteTransaction& ContractExecuteTransaction::setContractId(const ContractId& contractId)
{
  requireNotFrozen();
  mContractId = contractId;
  return *this;
}

//-----
ContractExecuteTransaction& ContractExecuteTransaction::setGas(const uint64_t& gas)
{
  requireNotFrozen();
  mGas = gas;
  return *this;
}

//-----
ContractExecuteTransaction& ContractExecuteTransaction::setPayableAmount(const Hbar& amount)
{
  requireNotFrozen();
  mPayableAmount = amount;
  return *this;
}

//-----
ContractExecuteTransaction& ContractExecuteTransaction::setFunctionParameters(const std::vector<std::byte>& parameters)
{
  requireNotFrozen();
  mFunctionParameters = parameters;
  return *this;
}

//-----
ContractExecuteTransaction& ContractExecuteTransaction::setFunction(std::string_view name,
                                                                    const ContractFunctionParameters& parameters)
{
  requireNotFrozen();
  mFunctionParameters = parameters.toBytes(name);
  return *this;
}

//-----
grpc::Status ContractExecuteTransaction::submitRequest(const proto::Transaction& request,
                                                       const std::shared_ptr<internal::Node>& node,
                                                       const std::chrono::system_clock::time_point& deadline,
                                                       proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kContractCall, request, deadline, response);
}

//-----
void ContractExecuteTransaction::validateChecksums(const Client& client) const
{
  mContractId.validateChecksum(client);
}

//-----
void ContractExecuteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_contractcall(build());
}

//-----
void ContractExecuteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_contractcall())
  {
    throw std::invalid_argument("Transaction body doesn't contain ContractCall data");
  }

  const proto::ContractCallTransactionBody& body = transactionBody.contractcall();

  if (body.has_contractid())
  {
    mContractId = ContractId::fromProtobuf(body.contractid());
  }

  mGas = static_cast<uint64_t>(body.gas());
  mPayableAmount = Hbar(body.amount(), HbarUnit::TINYBAR());
  mFunctionParameters = internal::Utilities::stringToByteVector(body.functionparameters());
}

//-----
proto::ContractCallTransactionBody* ContractExecuteTransaction::build() const
{
  auto body = std::make_unique<proto::ContractCallTransactionBody>();
  body->set_allocated_contractid(mContractId.toProtobuf().release());
  body->set_gas(static_cast<int64_t>(mGas));
  body->set_amount(mPayableAmount.toTinybars());
  body->set_functionparameters(internal::Utilities::byteVectorToString(mFunctionParameters));
  return body.release();
}

} // namespace Hiero
