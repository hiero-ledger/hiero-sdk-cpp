// SPDX-License-Identifier: Apache-2.0
#include "FileDeleteTransaction.h"
#include "impl/Node.h"

#include <file_delete.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
FileDeleteTransaction::FileDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<FileDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
FileDeleteTransaction::FileDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<FileDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
FileDeleteTransaction& FileDeleteTransaction::setFileId(const FileId& fileId)
{
  requireNotFrozen();
  mFileId = fileId;
  return *this;
}

//-----
grpc::Status FileDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kFileDelete, request, deadline, response);
}

//-----
void FileDeleteTransaction::validateChecksums(const Client& client) const
{
  mFileId.validateChecksum(client);
}

//-----
void FileDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_filedelete(build());
}

//-----
void FileDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_filedelete())
  {
    throw std::invalid_argument("Transaction body doesn't contain FileDelete data");
  }

  const proto::FileDeleteTransactionBody& body = transactionBody.filedelete();

  if (body.has_fileid())
  {
    mFileId = FileId::fromProtobuf(body.fileid());
  }
}

//-----
proto::FileDeleteTransactionBody* FileDeleteTransaction::build() const
{
  auto body = std::make_unique<proto::FileDeleteTransactionBody>();
  body->set_allocated_fileid(mFileId.toProtobuf().release());
  return body.release();
}

} // namespace Hiero
