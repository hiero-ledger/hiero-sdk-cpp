// SPDX-License-Identifier: Apache-2.0
#include "SystemDeleteTransaction.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <system_delete.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
SystemDeleteTransaction::SystemDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<SystemDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
SystemDeleteTransaction::SystemDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<SystemDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
SystemDeleteTransaction& SystemDeleteTransaction::setFileId(const FileId& fileId)
{
  requireNotFrozen();
  mFileId = fileId;
  mContractId.reset();
  return *this;
}

//-----
SystemDeleteTransaction& SystemDeleteTransaction::setContractId(const ContractId& contractId)
{
  requireNotFrozen();
  mContractId = contractId;
  mFileId.reset();
  return *this;
}

//-----
SystemDeleteTransaction& SystemDeleteTransaction::setExpirationTime(
  const std::chrono::system_clock::time_point& timestamp)
{
  requireNotFrozen();
  mExpirationTime = timestamp;
  return *this;
}

//-----
grpc::Status SystemDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kSystemDelete, request, deadline, response);
}

//-----
void SystemDeleteTransaction::validateChecksums(const Client& client) const
{
  if (mFileId.has_value())
  {
    mFileId->validateChecksum(client);
  }

  if (mContractId.has_value())
  {
    mContractId->validateChecksum(client);
  }
}

//-----
void SystemDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_systemdelete(build());
}

//-----
void SystemDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_systemdelete())
  {
    throw std::invalid_argument("Transaction body doesn't contain SystemDelete data");
  }

  const proto::SystemDeleteTransactionBody& body = transactionBody.systemdelete();

  if (body.has_fileid())
  {
    mFileId = FileId::fromProtobuf(body.fileid());
  }
  else if (body.has_contractid())
  {
    mContractId = ContractId::fromProtobuf(body.contractid());
  }

  if (body.has_expirationtime())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expirationtime());
  }
}

//-----
proto::SystemDeleteTransactionBody* SystemDeleteTransaction::build() const
{
  auto body = std::make_unique<proto::SystemDeleteTransactionBody>();

  if (mFileId.has_value())
  {
    body->set_allocated_fileid(mFileId->toProtobuf().release());
  }
  else if (mContractId.has_value())
  {
    body->set_allocated_contractid(mContractId->toProtobuf().release());
  }

  body->set_allocated_expirationtime(internal::TimestampConverter::toSecondsProtobuf(mExpirationTime));
  return body.release();
}

} // namespace Hiero
