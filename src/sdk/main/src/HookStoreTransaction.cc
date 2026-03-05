// SPDX-License-Identifier: Apache-2.0
#include "HookStoreTransaction.h"
#include "impl/Node.h"

#include <services/hook_store.pb.h>
#include <services/transaction.pb.h>

namespace Hiero
{
//-----
HookStoreTransaction::HookStoreTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<HookStoreTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
HookStoreTransaction::HookStoreTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<HookStoreTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
HookStoreTransaction& HookStoreTransaction::setHookId(const HookId& hookId)
{
  requireNotFrozen();
  mHookId = hookId;
  return *this;
}

//-----
HookStoreTransaction& HookStoreTransaction::addStorageUpdate(const EvmHookStorageUpdate& storageUpdate)
{
  requireNotFrozen();
  mStorageUpdates.push_back(storageUpdate);
  return *this;
}

//-----
HookStoreTransaction& HookStoreTransaction::setStorageUpdates(
  const std::vector<EvmHookStorageUpdate>& storageUpdates)
{
  requireNotFrozen();
  mStorageUpdates = storageUpdates;
  return *this;
}

//-----
HookStoreTransaction& HookStoreTransaction::clearStorageUpdates()
{
  requireNotFrozen();
  mStorageUpdates.clear();
  return *this;
}

//-----
grpc::Status HookStoreTransaction::submitRequest(const proto::Transaction& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kHookStore, request, deadline, response);
}

//-----
void HookStoreTransaction::validateChecksums(const Client& client) const
{
  mHookId.getEntityId().validateChecksums(client);
}

//-----
void HookStoreTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_hook_store(build());
}

//-----
void HookStoreTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_hook_store())
  {
    throw std::invalid_argument("Transaction body doesn't contain HookStore data");
  }

  const com::hedera::hapi::node::hooks::HookStoreTransactionBody& body = transactionBody.hook_store();

  if (body.has_hook_id())
  {
    mHookId = HookId::fromProtobuf(body.hook_id());
  }

  for (int i = 0; i < body.storage_updates_size(); ++i)
  {
    mStorageUpdates.push_back(EvmHookStorageUpdate::fromProtobuf(body.storage_updates(i)));
  }
}

//-----
com::hedera::hapi::node::hooks::HookStoreTransactionBody* HookStoreTransaction::build() const
{
  auto body = std::make_unique<com::hedera::hapi::node::hooks::HookStoreTransactionBody>();
  body->set_allocated_hook_id(mHookId.toProtobuf().release());

  for (const EvmHookStorageUpdate& update : mStorageUpdates)
  {
    *body->add_storage_updates() = *update.toProtobuf();
  }

  return body.release();
}

} // namespace Hiero
