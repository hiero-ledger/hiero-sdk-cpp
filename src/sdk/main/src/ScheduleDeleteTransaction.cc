// SPDX-License-Identifier: Apache-2.0
#include "ScheduleDeleteTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <schedule_delete.pb.h>
#include <stdexcept>
#include <transaction.pb.h>

namespace Hiero
{
//-----
ScheduleDeleteTransaction::ScheduleDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<ScheduleDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
ScheduleDeleteTransaction::ScheduleDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<ScheduleDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
ScheduleDeleteTransaction& ScheduleDeleteTransaction::setScheduleId(const ScheduleId& ScheduleId)
{
  requireNotFrozen();
  mScheduleId = ScheduleId;
  return *this;
}

//-----
grpc::Status ScheduleDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                      const std::shared_ptr<internal::Node>& node,
                                                      const std::chrono::system_clock::time_point& deadline,
                                                      proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kScheduleDelete, request, deadline, response);
}

//-----
void ScheduleDeleteTransaction::validateChecksums(const Client& client) const
{
  if (mScheduleId.has_value())
  {
    mScheduleId->validateChecksum(client);
  }
}

//-----
void ScheduleDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_scheduledelete(build());
}

//-----
void ScheduleDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_scheduledelete())
  {
    throw std::invalid_argument("Transaction body doesn't contain ScheduleDelete data");
  }

  const proto::ScheduleDeleteTransactionBody& body = transactionBody.scheduledelete();

  if (body.has_scheduleid())
  {
    mScheduleId = ScheduleId::fromProtobuf(body.scheduleid());
  }
}

//-----
proto::ScheduleDeleteTransactionBody* ScheduleDeleteTransaction::build() const
{
  auto body = std::make_unique<proto::ScheduleDeleteTransactionBody>();

  if (mScheduleId.has_value())
  {
    body->set_allocated_scheduleid(mScheduleId->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero