// SPDX-License-Identifier: Apache-2.0
#include "BatchTransaction.h"
#include "WrappedTransaction.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <services/transaction.pb.h>
#include <services/transaction_contents.pb.h>

#include <stdexcept>

namespace Hiero
{
//-----
BatchTransaction::BatchTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<BatchTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
BatchTransaction::BatchTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<BatchTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
BatchTransaction& BatchTransaction::addInnerTransaction(const WrappedTransaction& transaction)
{
  requireNotFrozen();
  mInnerTransactions.push_back(transaction);
  return *this;
}

//-----
BatchTransaction& BatchTransaction::setInnerTransactions(const std::vector<WrappedTransaction>& transactions)
{
  requireNotFrozen();
  mInnerTransactions = transactions;
  return *this;
}

//-----
grpc::Status BatchTransaction::submitRequest(const proto::Transaction& request,
                                             const std::shared_ptr<internal::Node>& node,
                                             const std::chrono::system_clock::time_point& deadline,
                                             proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kAtomicBatch, request, deadline, response);
}

//-----
void BatchTransaction::validateChecksums(const Client& client) const {}

//-----
void BatchTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_atomic_batch(build());
}

//-----
void BatchTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_atomic_batch())
  {
    throw std::invalid_argument("Transaction body doesn't contain Atomic Batch data");
  }

  const proto::AtomicBatchTransactionBody& body = transactionBody.atomic_batch();

  for (int i = 0; i < body.transactions_size(); ++i)
  {
    std::string signedTransactionBytes = body.transactions(i);
    mInnerTransactions.emplace_back(
      Transaction::fromBytes(internal::Utilities::stringToByteVector(signedTransactionBytes)));
  }
}

//-----
proto::AtomicBatchTransactionBody* BatchTransaction::build() const
{
  auto body = std::make_unique<proto::AtomicBatchTransactionBody>();

  for (const auto& transaction : mInnerTransactions)
  {
    std::unique_ptr<proto::TransactionBody> transactionProto = transaction.toProtobuf();

    proto::SignedTransaction signedTx;
    signedTx.set_bodybytes(transactionProto->SerializeAsString());

    proto::Transaction tx;
    tx.set_signedtransactionbytes(signedTx.SerializeAsString());

    body->add_transactions(tx.signedtransactionbytes());
  }

  return body.release();
}

} // namespace Hiero