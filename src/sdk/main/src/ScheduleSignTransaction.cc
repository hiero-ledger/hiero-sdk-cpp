// SPDX-License-Identifier: Apache-2.0
#include "ScheduleSignTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <schedule_sign.pb.h>
#include <stdexcept>
#include <transaction.pb.h>

namespace Hiero
{
//-----
ScheduleSignTransaction::ScheduleSignTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<ScheduleSignTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
ScheduleSignTransaction::ScheduleSignTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<ScheduleSignTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
ScheduleSignTransaction& ScheduleSignTransaction::setScheduleId(const ScheduleId& ScheduleId)
{
  requireNotFrozen();
  mScheduleId = ScheduleId;
  return *this;
}

//-----
ScheduleSignTransaction& ScheduleSignTransaction::clearScheduleId()
{
  requireNotFrozen();
  mScheduleId.reset();
  return *this;
}

//-----
grpc::Status ScheduleSignTransaction::submitRequest(const proto::Transaction& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kScheduleSign, request, deadline, response);
}

//-----
void ScheduleSignTransaction::validateChecksums(const Client& client) const
{
  if (mScheduleId.has_value())
  {
    mScheduleId->validateChecksum(client);
  }
}

//-----
void ScheduleSignTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_schedulesign(build());
}

//-----
void ScheduleSignTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_schedulesign())
  {
    throw std::invalid_argument("Transaction body doesn't contain ScheduleSign data");
  }

  const proto::ScheduleSignTransactionBody& body = transactionBody.schedulesign();

  if (body.has_scheduleid())
  {
    mScheduleId = ScheduleId::fromProtobuf(body.scheduleid());
  }
}

//-----
proto::ScheduleSignTransactionBody* ScheduleSignTransaction::build() const
{
  auto body = std::make_unique<proto::ScheduleSignTransactionBody>();

  if (mScheduleId.has_value())
  {
    body->set_allocated_scheduleid(mScheduleId->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero