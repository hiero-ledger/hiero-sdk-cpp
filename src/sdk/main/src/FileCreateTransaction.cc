// SPDX-License-Identifier: Apache-2.0
#include "FileCreateTransaction.h"
#include "impl/Node.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <file_create.pb.h>
#include <grpcpp/client_context.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
FileCreateTransaction::FileCreateTransaction()
{
  setDefaultMaxTransactionFee(Hbar(5LL));
}

//-----
FileCreateTransaction::FileCreateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<FileCreateTransaction>(transactionBody)
{
  setDefaultMaxTransactionFee(Hbar(5LL));
  initFromSourceTransactionBody();
}

//-----
FileCreateTransaction::FileCreateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<FileCreateTransaction>(transactions)
{
  setDefaultMaxTransactionFee(Hbar(5LL));
  initFromSourceTransactionBody();
}

//-----
FileCreateTransaction& FileCreateTransaction::setExpirationTime(
  const std::chrono::system_clock::time_point& expirationTime)
{
  requireNotFrozen();
  mExpirationTime = expirationTime;
  return *this;
}

//-----
FileCreateTransaction& FileCreateTransaction::setKeys(const std::vector<std::shared_ptr<Key>>& keys)
{
  requireNotFrozen();
  mKeys = KeyList::of(keys);
  return *this;
}

//-----
FileCreateTransaction& FileCreateTransaction::setKeys(const KeyList& keys)
{
  requireNotFrozen();
  mKeys = keys;
  return *this;
}

//-----
FileCreateTransaction& FileCreateTransaction::setContents(const std::vector<std::byte>& contents)
{
  requireNotFrozen();
  mContents = contents;
  return *this;
}

//-----
FileCreateTransaction& FileCreateTransaction::setContents(std::string_view contents)
{
  requireNotFrozen();
  return setContents(internal::Utilities::stringToByteVector(contents));
}

//-----
FileCreateTransaction& FileCreateTransaction::setFileMemo(std::string_view memo)
{
  requireNotFrozen();
  mFileMemo = memo;
  return *this;
}

//-----
grpc::Status FileCreateTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kFileCreate, request, deadline, response);
}

//-----
void FileCreateTransaction::validateChecksums(const Client& client) const
{
  // No entity IDs to validate.
}

//-----
void FileCreateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_filecreate(build());
}

//-----
void FileCreateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_filecreate())
  {
    throw std::invalid_argument("Transaction body doesn't contain FileCreate data");
  }

  const proto::FileCreateTransactionBody& body = transactionBody.filecreate();

  if (body.has_expirationtime())
  {
    mExpirationTime = internal::TimestampConverter::fromProtobuf(body.expirationtime());
  }

  if (body.has_keys())
  {
    mKeys = KeyList::fromProtobuf(body.keys());
  }

  mContents = internal::Utilities::stringToByteVector(body.contents());
  mFileMemo = body.memo();
}

//-----
proto::FileCreateTransactionBody* FileCreateTransaction::build() const
{
  auto body = std::make_unique<proto::FileCreateTransactionBody>();
  body->set_allocated_expirationtime(internal::TimestampConverter::toProtobuf(mExpirationTime));
  body->set_allocated_keys(mKeys.toProtobuf().release());
  body->set_contents(internal::Utilities::byteVectorToString(mContents));
  body->set_memo(mFileMemo);
  return body.release();
}

} // namespace Hiero
