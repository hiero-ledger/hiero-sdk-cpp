// SPDX-License-Identifier: Apache-2.0
#include "FileUpdateTransaction.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <file_update.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
FileUpdateTransaction::FileUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<FileUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
FileUpdateTransaction::FileUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<FileUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
FileUpdateTransaction& FileUpdateTransaction::setFileId(const FileId& fileId)
{
  requireNotFrozen();
  mFileId = fileId;
  return *this;
}

//-----
FileUpdateTransaction& FileUpdateTransaction::setExpirationTime(
  const std::chrono::system_clock::time_point& expirationTime)
{
  requireNotFrozen();
  mExpirationTime = expirationTime;
  return *this;
}

//-----
FileUpdateTransaction& FileUpdateTransaction::setKeys(const std::vector<std::shared_ptr<Key>>& keys)
{
  requireNotFrozen();
  mKeys = KeyList::of(keys);
  return *this;
}

//-----
FileUpdateTransaction& FileUpdateTransaction::setKeys(const KeyList& keys)
{
  requireNotFrozen();
  mKeys = keys;
  return *this;
}

//-----
FileUpdateTransaction& FileUpdateTransaction::setContents(const std::vector<std::byte>& contents)
{
  requireNotFrozen();
  mContents = contents;
  return *this;
}

//-----
FileUpdateTransaction& FileUpdateTransaction::setContents(std::string_view contents)
{
  requireNotFrozen();
  mContents = internal::Utilities::stringToByteVector(contents);
  return *this;
}

//-----
FileUpdateTransaction& FileUpdateTransaction::setFileMemo(std::string_view memo)
{
  requireNotFrozen();
  mFileMemo = memo;
  return *this;
}

//-----
grpc::Status FileUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kFileUpdate, request, deadline, response);
}

//-----
void FileUpdateTransaction::validateChecksums(const Client& client) const
{
  mFileId.validateChecksum(client);
}

//-----
void FileUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_fileupdate(build());
}

//-----
void FileUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_fileupdate())
  {
    throw std::invalid_argument("Transaction body doesn't contain FileUpdate data");
  }

  const proto::FileUpdateTransactionBody& body = transactionBody.fileupdate();

  if (body.has_fileid())
  {
    mFileId = FileId::fromProtobuf(body.fileid());
  }

  if (body.has_expirationtime())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expirationtime());
  }

  if (body.has_keys())
  {
    mKeys = KeyList::fromProtobuf(body.keys());
  }

  mContents = internal::Utilities::stringToByteVector(body.contents());

  if (body.has_memo())
  {
    mFileMemo = body.memo().value();
  }
}

//-----
proto::FileUpdateTransactionBody* FileUpdateTransaction::build() const
{
  auto body = std::make_unique<proto::FileUpdateTransactionBody>();
  body->set_allocated_fileid(mFileId.toProtobuf().release());

  if (mExpirationTime.has_value())
  {
    body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(mExpirationTime.value()));
  }

  if (mKeys.has_value())
  {
    body->set_allocated_keys(mKeys->toProtobuf().release());
  }

  if (mContents.has_value())
  {
    body->set_contents(internal::Utilities::byteVectorToString(mContents.value()));
  }

  if (mFileMemo.has_value())
  {
    auto strValue = std::make_unique<google::protobuf::StringValue>();
    strValue->set_value(mFileMemo.value());
    body->set_allocated_memo(strValue.release());
  }

  return body.release();
}

} // namespace Hiero
