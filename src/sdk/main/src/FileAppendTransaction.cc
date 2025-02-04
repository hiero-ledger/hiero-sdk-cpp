// SPDX-License-Identifier: Apache-2.0
#include "FileAppendTransaction.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <file_append.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
FileAppendTransaction::FileAppendTransaction()
  : ChunkedTransaction<FileAppendTransaction>()
{
  setDefaultMaxTransactionFee(Hbar(5LL));
  setChunkSize(2048U);
  setShouldGetReceipt(true);
}

//-----
FileAppendTransaction::FileAppendTransaction(const proto::TransactionBody& transactionBody)
  : ChunkedTransaction<FileAppendTransaction>(transactionBody)
{
  setShouldGetReceipt(true);
  initFromSourceTransactionBody();
}

//-----
FileAppendTransaction::FileAppendTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : ChunkedTransaction<FileAppendTransaction>(transactions)
{
  setShouldGetReceipt(true);
  initFromSourceTransactionBody();
}

//-----
FileAppendTransaction& FileAppendTransaction::setFileId(const FileId& fileId)
{
  requireNotFrozen();
  mFileId = fileId;
  return *this;
}

//-----
FileAppendTransaction& FileAppendTransaction::setContents(const std::vector<std::byte>& contents)
{
  requireNotFrozen();
  setData(contents);
  return *this;
}

//-----
FileAppendTransaction& FileAppendTransaction::setContents(std::string_view contents)
{
  requireNotFrozen();
  setData(internal::Utilities::stringToByteVector(contents));
  return *this;
}

//-----
grpc::Status FileAppendTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kFileAppend, request, deadline, response);
}

//-----
void FileAppendTransaction::validateChecksums(const Client& client) const
{
  mFileId.validateChecksum(client);
}

//-----
void FileAppendTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_fileappend(build());
}

//-----
void FileAppendTransaction::addToChunk(uint32_t chunk, uint32_t, proto::TransactionBody& body) const
{
  body.set_allocated_fileappend(build(static_cast<int>(chunk)));
}

//-----
void FileAppendTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_fileappend())
  {
    throw std::invalid_argument("Transaction body doesn't contain FileAppend data");
  }

  const proto::FileAppendTransactionBody& body = transactionBody.fileappend();

  if (body.has_fileid())
  {
    mFileId = FileId::fromProtobuf(body.fileid());
  }

  setData(internal::Utilities::stringToByteVector(body.contents()));
}

//-----
proto::FileAppendTransactionBody* FileAppendTransaction::build(int chunk) const
{
  auto body = std::make_unique<proto::FileAppendTransactionBody>();
  body->set_allocated_fileid(mFileId.toProtobuf().release());
  body->set_contents(internal::Utilities::byteVectorToString(
    (chunk >= 0) ? getDataForChunk(static_cast<unsigned int>(chunk)) : getData()));
  return body.release();
}

} // namespace Hiero
