// SPDX-License-Identifier: Apache-2.0
#include "AccountDeleteTransaction.h"
#include "impl/Node.h"

#include <crypto_delete.pb.h>
#include <grpcpp/client_context.h>
#include <stdexcept>
#include <transaction.pb.h>

namespace Hiero
{
//-----
AccountDeleteTransaction::AccountDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<AccountDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
AccountDeleteTransaction::AccountDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<AccountDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
AccountDeleteTransaction& AccountDeleteTransaction::setDeleteAccountId(const AccountId& accountId)
{
  requireNotFrozen();

  mDeleteAccountId = accountId;
  return *this;
}

//-----
AccountDeleteTransaction& AccountDeleteTransaction::setTransferAccountId(const AccountId& accountId)
{
  requireNotFrozen();

  mTransferAccountId = accountId;
  return *this;
}

//-----
grpc::Status AccountDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                     const std::shared_ptr<internal::Node>& node,
                                                     const std::chrono::system_clock::time_point& deadline,
                                                     proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kCryptoDelete, request, deadline, response);
}

//-----
void AccountDeleteTransaction::validateChecksums(const Client& client) const
{
  if (mDeleteAccountId.has_value())
  {
    mDeleteAccountId->validateChecksum(client);
  }

  if (mTransferAccountId.has_value())
  {
    mTransferAccountId->validateChecksum(client);
  }
}

//-----
void AccountDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_cryptodelete(build());
}

//-----
void AccountDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_cryptodelete())
  {
    throw std::invalid_argument("Transaction body doesn't contain CryptoDelete data");
  }

  const proto::CryptoDeleteTransactionBody& body = transactionBody.cryptodelete();

  if (body.has_deleteaccountid())
  {
    mDeleteAccountId = AccountId::fromProtobuf(body.deleteaccountid());
  }

  if (body.has_transferaccountid())
  {
    mTransferAccountId = AccountId::fromProtobuf(body.transferaccountid());
  }
}

//-----
proto::CryptoDeleteTransactionBody* AccountDeleteTransaction::build() const
{
  auto body = std::make_unique<proto::CryptoDeleteTransactionBody>();

  if (mDeleteAccountId.has_value())
  {
    body->set_allocated_deleteaccountid(mDeleteAccountId->toProtobuf().release());
  }

  if (mTransferAccountId.has_value())
  {
    body->set_allocated_transferaccountid(mTransferAccountId->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero