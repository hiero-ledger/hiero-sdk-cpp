// SPDX-License-Identifier: Apache-2.0
#include "PrngTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <transaction.pb.h>
#include <util_prng.pb.h>

namespace Hiero
{
//-----
PrngTransaction::PrngTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<PrngTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
PrngTransaction::PrngTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<PrngTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
PrngTransaction& PrngTransaction::setRange(int range)
{
  requireNotFrozen();
  mRange = range;
  return *this;
}

//-----
grpc::Status PrngTransaction::submitRequest(const proto::Transaction& request,
                                            const std::shared_ptr<internal::Node>& node,
                                            const std::chrono::system_clock::time_point& deadline,
                                            proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kUtilPrng, request, deadline, response);
}

//-----
void PrngTransaction::validateChecksums(const Client& client) const
{
  // Nothing to be validated.
}

//-----
void PrngTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_util_prng(build());
}

//-----
void PrngTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_util_prng())
  {
    throw std::invalid_argument("Transaction body doesn't contain UtilPrng data");
  }

  mRange = transactionBody.util_prng().range();
}

//-----
proto::UtilPrngTransactionBody* PrngTransaction::build() const
{
  auto body = std::make_unique<proto::UtilPrngTransactionBody>();
  body->set_range(mRange);
  return body.release();
}

} // namespace Hiero