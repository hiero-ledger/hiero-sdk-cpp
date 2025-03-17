// SPDX-License-Identifier: Apache-2.0
#include "TransactionId.h"
#include "Client.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionRecordQuery.h"
#include "impl/EntityIdHelper.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
TransactionId TransactionId::withValidStart(const AccountId& accountId,
                                            const std::chrono::system_clock::time_point& start)
{
  return TransactionId(accountId, start);
}

//-----
TransactionId TransactionId::generate(const AccountId& accountId)
{
  return TransactionId(accountId, std::chrono::system_clock::now());
}

//-----
TransactionId TransactionId::fromProtobuf(const proto::TransactionID& proto)
{
  TransactionId id;

  if (proto.has_transactionvalidstart())
  {
    id.mValidTransactionTime = internal::TimestampConverter::fromProtobuf(proto.transactionvalidstart());
  }

  if (proto.has_accountid())
  {
    id.mAccountId = AccountId::fromProtobuf(proto.accountid());
  }

  id.mScheduled = proto.scheduled();
  id.mNonce = proto.nonce();

  return id;
}

//-----
TransactionId TransactionId::fromString(std::string_view id)
{
  TransactionId transactionId;

  // First, determine if there is a nonce. If there is, get it.
  if (const size_t slash = id.find_last_of('/'); slash != std::string::npos)
  {
    const std::string_view nonce = id.substr(slash + 1, id.size() - slash - 1);
    id.remove_suffix(nonce.size() + 1);
    transactionId.mNonce = static_cast<int>(internal::EntityIdHelper::getNum(nonce));
  }

  // Second, determine if it's scheduled.
  if (const size_t questionMark = id.find_last_of('?'); questionMark != std::string::npos)
  {
    const std::string_view scheduled = id.substr(questionMark + 1, id.size() - questionMark - 1);
    id.remove_suffix(scheduled.size() + 1);
    transactionId.mScheduled = scheduled == "scheduled";
  }

  // Parse the time of the TransactionId.
  try
  {
    // Get the nanoseconds string.
    const size_t decimal = id.find_last_of('.');
    const std::string_view nanosecondsStr = id.substr(decimal + 1, id.size() - decimal - 1);
    id.remove_suffix(nanosecondsStr.size() + 1);

    // Get the seconds string.
    const size_t atSign = id.find_last_of('@');
    const std::string_view secondsStr = id.substr(atSign + 1, id.size() - atSign - 1);
    id.remove_suffix(secondsStr.size() + 1);

    // Parse the strings into a std::chrono::system_clock::time_point.
    transactionId.mValidTransactionTime =
      std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(
        std::chrono::seconds(internal::EntityIdHelper::getNum(secondsStr)) +
        std::chrono::nanoseconds(internal::EntityIdHelper::getNum(nanosecondsStr))));

    // Finally, parse the ID of the payer account.
    transactionId.mAccountId = AccountId::fromString(id);
  }
  catch (const std::exception&)
  {
    throw std::invalid_argument("Input transaction ID string is malformed");
  }

  return transactionId;
}

//-----
TransactionId TransactionId::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TransactionID proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
TransactionReceipt TransactionId::getReceipt(const Client& client) const
{
  return getReceipt(client, client.getRequestTimeout());
}

//-----
TransactionReceipt TransactionId::getReceipt(const Client& client,
                                             const std::chrono::system_clock::duration& timeout) const
{
  return TransactionReceiptQuery().setTransactionId(*this).execute(client, timeout);
}

//-----
std::future<TransactionReceipt> TransactionId::getReceiptAsync(const Client& client) const
{
  return getReceiptAsync(client, client.getRequestTimeout());
}

//-----
std::future<TransactionReceipt> TransactionId::getReceiptAsync(const Client& client,
                                                               const std::chrono::system_clock::duration& timeout) const
{
  return std::async(std::launch::async,
                    [this, &client, &timeout]()
                    { return TransactionReceiptQuery().setTransactionId(*this).execute(client, timeout); });
}

//-----
void TransactionId::getReceiptAsync(
  const Client& client,
  const std::function<void(const TransactionReceipt&, const std::exception&)>& callback) const
{
  return getReceiptAsync(client, client.getRequestTimeout(), callback);
}

//-----
void TransactionId::getReceiptAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout,
  const std::function<void(const TransactionReceipt&, const std::exception&)>& callback) const
{
  std::future<TransactionReceipt> future = std::async(
    std::launch::async,
    [this, &client, &timeout]() { return TransactionReceiptQuery().setTransactionId(*this).execute(client, timeout); });

  try
  {
    callback(future.get(), std::exception());
  }
  catch (const std::exception& exception)
  {
    callback(TransactionReceipt(), exception);
  }
}

//-----
void TransactionId::getReceiptAsync(const Client& client,
                                    const std::function<void(const TransactionReceipt&)>& responseCallback,
                                    const std::function<void(const std::exception&)>& exceptionCallback) const
{
  return getReceiptAsync(client, client.getRequestTimeout(), responseCallback, exceptionCallback);
}

//-----
void TransactionId::getReceiptAsync(const Client& client,
                                    const std::chrono::system_clock::duration& timeout,
                                    const std::function<void(const TransactionReceipt&)>& responseCallback,
                                    const std::function<void(const std::exception&)>& exceptionCallback) const
{
  std::future<TransactionReceipt> future = std::async(
    std::launch::async,
    [this, &client, &timeout]() { return TransactionReceiptQuery().setTransactionId(*this).execute(client, timeout); });

  try
  {
    responseCallback(future.get());
  }
  catch (const std::exception& exception)
  {
    exceptionCallback(exception);
  }
}

//-----
TransactionRecord TransactionId::getRecord(const Client& client) const
{
  return getRecord(client, client.getRequestTimeout());
}

//-----
TransactionRecord TransactionId::getRecord(const Client& client,
                                           const std::chrono::system_clock::duration& timeout) const
{
  // Make sure the Transaction has been fully processed by getting the receipt.
  getReceipt(client, timeout);

  // Now can safely get the record.
  return TransactionRecordQuery().setTransactionId(*this).execute(client, timeout);
}

//-----
std::future<TransactionRecord> TransactionId::getRecordAsync(const Client& client) const
{
  return getRecordAsync(client, client.getRequestTimeout());
}

//-----
std::future<TransactionRecord> TransactionId::getRecordAsync(const Client& client,
                                                             const std::chrono::system_clock::duration& timeout) const
{
  return std::async(std::launch::async,
                    [this, &client, &timeout]()
                    {
                      this->getReceipt(client, timeout);
                      return TransactionRecordQuery().setTransactionId(*this).execute(client, timeout);
                    });
}

//-----
void TransactionId::getRecordAsync(
  const Client& client,
  const std::function<void(const TransactionRecord&, const std::exception&)>& callback) const
{
  return getRecordAsync(client, client.getRequestTimeout(), callback);
}

//-----
void TransactionId::getRecordAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout,
  const std::function<void(const TransactionRecord&, const std::exception&)>& callback) const
{
  std::future<TransactionRecord> future =
    std::async(std::launch::async,
               [this, &client, &timeout]()
               {
                 this->getReceipt(client, timeout);
                 return TransactionRecordQuery().setTransactionId(*this).execute(client, timeout);
               });

  try
  {
    callback(future.get(), std::exception());
  }
  catch (const std::exception& exception)
  {
    callback(TransactionRecord(), exception);
  }
}

//-----
void TransactionId::getRecordAsync(const Client& client,
                                   const std::function<void(const TransactionRecord&)>& responseCallback,
                                   const std::function<void(const std::exception&)>& exceptionCallback) const
{
  return getRecordAsync(client, client.getRequestTimeout(), responseCallback, exceptionCallback);
}

//-----
void TransactionId::getRecordAsync(const Client& client,
                                   const std::chrono::system_clock::duration& timeout,
                                   const std::function<void(const TransactionRecord&)>& responseCallback,
                                   const std::function<void(const std::exception&)>& exceptionCallback) const
{
  std::future<TransactionRecord> future =
    std::async(std::launch::async,
               [this, &client, &timeout]()
               {
                 this->getReceipt(client, timeout);
                 return TransactionRecordQuery().setTransactionId(*this).execute(client, timeout);
               });

  try
  {
    responseCallback(future.get());
  }
  catch (const std::exception& exception)
  {
    exceptionCallback(exception);
  }
}

//-----
bool TransactionId::operator==(const TransactionId& other) const
{
  return (mValidTransactionTime == other.mValidTransactionTime) && (mAccountId == other.mAccountId) &&
         (mScheduled == other.mScheduled) && (mNonce == other.mNonce);
}

//-----
bool TransactionId::operator!=(const TransactionId& other) const
{
  return !operator==(other);
}

//-----
std::unique_ptr<proto::TransactionID> TransactionId::toProtobuf() const
{
  auto proto = std::make_unique<proto::TransactionID>();
  proto->set_allocated_transactionvalidstart(internal::TimestampConverter::toProtobuf(mValidTransactionTime));
  proto->set_allocated_accountid(mAccountId.toProtobuf().release());
  proto->set_scheduled(mScheduled);
  proto->set_nonce(mNonce);
  return proto;
}

//-----
std::string TransactionId::toString() const
{
  return mAccountId.toString() + getTimestampScheduleNonceString();
}

//-----
std::string TransactionId::toStringWithChecksum(const Client& client) const
{
  return mAccountId.toStringWithChecksum(client) + getTimestampScheduleNonceString();
}

//-----
std::vector<std::byte> TransactionId::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
TransactionId& TransactionId::setScheduled(bool scheduled)
{
  mScheduled = scheduled;
  return *this;
}

//-----
TransactionId& TransactionId::setNonce(int nonce)
{
  mNonce = nonce;
  return *this;
}

//-----
TransactionId::TransactionId(Hiero::AccountId accountId, const std::chrono::system_clock::time_point& validStartTime)
  : mAccountId(std::move(accountId))
  , mValidTransactionTime(validStartTime)
{
}

//-----
std::string TransactionId::getTimestampScheduleNonceString() const
{
  return '@' + internal::TimestampConverter::toString(mValidTransactionTime) + (mScheduled ? "?scheduled" : "") +
         (mNonce != 0 ? ('/' + std::to_string(mNonce)) : "");
}

} // namespace Hiero
