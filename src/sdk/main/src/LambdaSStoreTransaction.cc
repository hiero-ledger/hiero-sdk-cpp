// SPDX-License-Identifier: Apache-2.0
#include "LambdaSStoreTransaction.h"
#include "impl/Node.h"

#include <services/lambda_sstore.pb.h>
#include <services/transaction.pb.h>

namespace Hiero
{
//-----
LambdaSStoreTransaction::LambdaSStoreTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<LambdaSStoreTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
LambdaSStoreTransaction::LambdaSStoreTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<LambdaSStoreTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
LambdaSStoreTransaction& LambdaSStoreTransaction::setHookId(const HookId& hookId)
{
  requireNotFrozen();
  mHookId = hookId;
  return *this;
}

//-----
LambdaSStoreTransaction& LambdaSStoreTransaction::addStorageUpdate(const LambdaStorageUpdate& storageUpdate)
{
  requireNotFrozen();
  mStorageUpdates.push_back(storageUpdate);
  return *this;
}

//-----
LambdaSStoreTransaction& LambdaSStoreTransaction::setStorageUpdates(const std::vector<LambdaStorageUpdate>& storageUpdates)
{
  requireNotFrozen();
  mStorageUpdates = storageUpdates;
  return *this;
}

//-----
LambdaSStoreTransaction& LambdaSStoreTransaction::clearStorageUpdates()
{
  requireNotFrozen();
  mStorageUpdates.clear();
  return *this;
}

//-----
grpc::Status LambdaSStoreTransaction::submitRequest(const proto::Transaction& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kLambdaSstore, request, deadline, response);
}

//-----
void LambdaSStoreTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_lambda_sstore(build());
}

//-----
void LambdaSStoreTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_lambda_sstore())
  {
    throw std::invalid_argument("Transaction body doesn't contain LambdaSStore data");
  }

  const com::hedera::hapi::node::hooks::LambdaSStoreTransactionBody& body = transactionBody.lambda_sstore();

  if (body.has_hook_id())
  {
    mHookId = HookId::fromProtobuf(body.hook_id());
  }

  for (int i = 0; i < body.storage_updates_size(); ++i)
  {
    mStorageUpdates.push_back(LambdaStorageUpdate::fromProtobuf(body.storage_updates(i)));
  }
}

//-----
com::hedera::hapi::node::hooks::LambdaSStoreTransactionBody* LambdaSStoreTransaction::build() const
{
  auto body = std::make_unique<com::hedera::hapi::node::hooks::LambdaSStoreTransactionBody>();
  body->set_allocated_hook_id(mHookId.toProtobuf().release());

  for (const LambdaStorageUpdate& update : mStorageUpdates)
  {
    *body->add_storage_updates() = *update.toProtobuf();
  }

  return body.release();
}

} // namespace Hiero
