// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransaction.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <ethereum_transaction.pb.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
EthereumTransaction::EthereumTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<EthereumTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
EthereumTransaction::EthereumTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<EthereumTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
EthereumTransaction& EthereumTransaction::setEthereumData(const std::vector<std::byte>& ethereumData)
{
  requireNotFrozen();
  mEthereumData = ethereumData;
  return *this;
}

//-----
EthereumTransaction& EthereumTransaction::setCallDataFileId(const FileId& fileId)
{
  requireNotFrozen();
  mCallDataFileId = fileId;
  return *this;
}

//-----
EthereumTransaction& EthereumTransaction::setMaxGasAllowance(const Hbar& maxGasAllowance)
{
  requireNotFrozen();
  mMaxGasAllowance = maxGasAllowance;
  return *this;
}

//-----
grpc::Status EthereumTransaction::submitRequest(const proto::Transaction& request,
                                                const std::shared_ptr<internal::Node>& node,
                                                const std::chrono::system_clock::time_point& deadline,
                                                proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kEthereumTransaction, request, deadline, response);
}

//-----
void EthereumTransaction::validateChecksums(const Client& client) const
{
  if (mCallDataFileId.has_value())
  {
    mCallDataFileId->validateChecksum(client);
  }
}

//-----
void EthereumTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_ethereumtransaction(build());
}

//-----
void EthereumTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_ethereumtransaction())
  {
    throw std::invalid_argument("Transaction body doesn't contain EthereumTransaction data");
  }

  const proto::EthereumTransactionBody& body = transactionBody.ethereumtransaction();

  mEthereumData = internal::Utilities::stringToByteVector(body.ethereum_data());

  if (body.has_call_data())
  {
    mCallDataFileId = FileId::fromProtobuf(body.call_data());
  }

  mMaxGasAllowance = Hbar(body.max_gas_allowance(), HbarUnit::TINYBAR());
}

//-----
proto::EthereumTransactionBody* EthereumTransaction::build() const
{
  auto body = std::make_unique<proto::EthereumTransactionBody>();

  body->set_ethereum_data(internal::Utilities::byteVectorToString(mEthereumData));

  if (mCallDataFileId.has_value())
  {
    body->set_allocated_call_data(mCallDataFileId->toProtobuf().release());
  }

  body->set_max_gas_allowance(mMaxGasAllowance.toTinybars());

  return body.release();
}

} // namespace Hiero
