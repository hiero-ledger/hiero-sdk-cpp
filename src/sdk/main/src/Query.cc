// SPDX-License-Identifier: Apache-2.0
#include "Query.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "AccountRecords.h"
#include "AccountRecordsQuery.h"
#include "AccountStakersQuery.h"
#include "Client.h"
#include "ContractByteCodeQuery.h"
#include "ContractCallQuery.h"
#include "ContractFunctionResult.h"
#include "ContractInfo.h"
#include "ContractInfoQuery.h"
#include "FileContentsQuery.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "Hbar.h"
#include "NetworkVersionInfo.h"
#include "NetworkVersionInfoQuery.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "Status.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenNftInfo.h"
#include "TokenNftInfoQuery.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionRecordQuery.h"
#include "TransferTransaction.h"
#include "exceptions/MaxQueryPaymentExceededException.h"
#include "exceptions/UninitializedException.h"
#include "impl/Network.h"

#include <query.pb.h>
#include <query_header.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
template<typename SdkRequestType, typename SdkResponseType>
struct Query<SdkRequestType, SdkResponseType>::QueryImpl
{
  // The explicit amount to pay for this Query.
  std::optional<Hbar> mPayment;

  // The maximum amount to pay for this Query.
  std::optional<Hbar> mMaxPayment;

  // The transaction ID to use for the payment transaction for this Query.
  std::optional<TransactionId> mPaymentTransactionId;

  // Is this Query meant to get the cost?
  bool mGetCost = false;

  // The cost to execute this Query.
  Hbar mCost;

  // The Client that should be used to pay for the payment transaction of this Query.
  const Client* mClient = nullptr;
};

//-----
template<typename SdkRequestType, typename SdkResponseType>
Hbar Query<SdkRequestType, SdkResponseType>::getCost(const Client& client)
{
  return getCost(client, client.getRequestTimeout());
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
Hbar Query<SdkRequestType, SdkResponseType>::getCost(const Client& client,
                                                     const std::chrono::system_clock::duration& timeout)
{
  // Configure this Query to get the cost.
  mImpl->mGetCost = true;
  Executable<SdkRequestType, proto::Query, proto::Response, SdkResponseType>::execute(client, timeout);

  // Reset this Query to not get the cost.
  mImpl->mGetCost = false;

  return mImpl->mCost;
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
std::future<Hbar> Query<SdkRequestType, SdkResponseType>::getCostAsync(const Client& client)
{
  return getCostAsync(client, client.getRequestTimeout());
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
std::future<Hbar> Query<SdkRequestType, SdkResponseType>::getCostAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout)
{
  return std::async(std::launch::async, [this, &client, &timeout]() { return this->getCost(client, timeout); });
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
void Query<SdkRequestType, SdkResponseType>::getCostAsync(
  const Client& client,
  const std::function<void(const Hbar&, const std::exception&)>& callback)
{
  return getCostAsync(client, client.getRequestTimeout(), callback);
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
void Query<SdkRequestType, SdkResponseType>::getCostAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout,
  const std::function<void(const Hbar&, const std::exception&)>& callback)
{
  std::future<Hbar> future =
    std::async(std::launch::async, [this, &client, &timeout]() { return this->getCost(client, timeout); });

  try
  {
    callback(future.get(), std::exception());
  }
  catch (const std::exception& exception)
  {
    callback(Hbar(), exception);
  }
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
void Query<SdkRequestType, SdkResponseType>::getCostAsync(
  const Client& client,
  const std::function<void(const Hbar&)>& responseCallback,
  const std::function<void(const std::exception&)>& exceptionCallback)
{
  return getCostAsync(client, client.getRequestTimeout(), responseCallback, exceptionCallback);
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
void Query<SdkRequestType, SdkResponseType>::getCostAsync(
  const Client& client,
  const std::chrono::system_clock::duration& timeout,
  const std::function<void(const Hbar&)>& responseCallback,
  const std::function<void(const std::exception&)>& exceptionCallback)
{
  std::future<Hbar> future =
    std::async(std::launch::async, [this, &client, &timeout]() { return this->getCost(client, timeout); });

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
template<typename SdkRequestType, typename SdkResponseType>
SdkRequestType& Query<SdkRequestType, SdkResponseType>::setQueryPayment(const Hbar& amount)
{
  mImpl->mPayment = amount;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
SdkRequestType& Query<SdkRequestType, SdkResponseType>::setMaxQueryPayment(const Hbar& maxAmount)
{
  mImpl->mMaxPayment = maxAmount;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
SdkRequestType& Query<SdkRequestType, SdkResponseType>::setPaymentTransactionId(const TransactionId& transactionId)
{
  mImpl->mPaymentTransactionId = transactionId;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
std::optional<TransactionId> Query<SdkRequestType, SdkResponseType>::getPaymentTransactionId() const
{
  return mImpl->mPaymentTransactionId;
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
void Query<SdkRequestType, SdkResponseType>::saveCostFromHeader(const proto::ResponseHeader& header) const
{
  if (mImpl->mGetCost)
  {
    mImpl->mCost = Hbar(static_cast<int64_t>(header.cost()), HbarUnit::TINYBAR());
  }
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
bool Query<SdkRequestType, SdkResponseType>::isCostQuery() const
{
  return mImpl->mGetCost;
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
Query<SdkRequestType, SdkResponseType>::Query()
  : mImpl(std::make_unique<QueryImpl>())
{
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
Query<SdkRequestType, SdkResponseType>::~Query() = default;

//-----
template<typename SdkRequestType, typename SdkResponseType>
Query<SdkRequestType, SdkResponseType>::Query(const Query<SdkRequestType, SdkResponseType>& other)
  : mImpl(std::make_unique<QueryImpl>(*other.mImpl))
{
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
Query<SdkRequestType, SdkResponseType>& Query<SdkRequestType, SdkResponseType>::operator=(
  const Query<SdkRequestType, SdkResponseType>& other)
{
  if (this != &other)
  {
    mImpl = std::make_unique<QueryImpl>(*other.mImpl);
  }

  return *this;
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
Query<SdkRequestType, SdkResponseType>::Query(Query<SdkRequestType, SdkResponseType>&& other) noexcept
  : mImpl(std::move(other.mImpl))
{
  // Leave moved-from object in a valid state.
  other.mImpl = std::make_unique<QueryImpl>();
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
Query<SdkRequestType, SdkResponseType>& Query<SdkRequestType, SdkResponseType>::operator=(
  Query<SdkRequestType, SdkResponseType>&& other) noexcept
{
  if (this != &other)
  {
    mImpl = std::move(other.mImpl);

    // Leave moved-from object in a valid state.
    other.mImpl = std::make_unique<QueryImpl>();
  }

  return *this;
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
proto::Query Query<SdkRequestType, SdkResponseType>::makeRequest(unsigned int index) const
{
  auto header = std::make_unique<proto::QueryHeader>();

  if (isPaymentRequired() && !mImpl->mGetCost)
  {
    // Get the node account ID for this index.
    const AccountId accountId =
      Executable<SdkRequestType, proto::Query, proto::Response, SdkResponseType>::getNodeAccountIds().at(index);

    header->set_allocated_payment(
      std::make_unique<proto::Transaction>(
        TransferTransaction()
          .setTransactionId(mImpl->mPaymentTransactionId.has_value()
                              ? mImpl->mPaymentTransactionId.value()
                              : TransactionId::generate(mImpl->mClient->getOperatorAccountId().value()))
          .setNodeAccountIds({ accountId })
          .addHbarTransfer(mImpl->mClient->getOperatorAccountId().value(), mImpl->mCost.negated())
          .addHbarTransfer(accountId, mImpl->mCost)
          .freeze()
          .signWithOperator(*mImpl->mClient)
          // There's only one node account ID, therefore only one Transaction protobuf object will be created, and that
          // will be put in the 0th index.
          .makeRequest(0U))
        .release());
  }

  header->set_responsetype(mImpl->mGetCost ? proto::ResponseType::COST_ANSWER : proto::ResponseType::ANSWER_ONLY);
  return buildRequest(header.release());
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
Status Query<SdkRequestType, SdkResponseType>::mapResponseStatus(const proto::Response& response) const
{
  return gProtobufResponseCodeToStatus.at(mapResponseHeader(response).nodetransactionprecheckcode());
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
void Query<SdkRequestType, SdkResponseType>::onExecute(const Client& client)
{
  // Set the node IDs if none have been manually set.
  if (Executable<SdkRequestType, proto::Query, proto::Response, SdkResponseType>::getNodeAccountIds().empty())
  {
    // Make sure the client has a valid network.
    if (!client.getClientNetwork())
    {
      throw UninitializedException("Client has not been initialized with a valid network");
    }

    // Have the Client's network generate the node account IDs to which to send this Query.
    Executable<SdkRequestType, proto::Query, proto::Response, SdkResponseType>::setNodeAccountIds(
      client.getClientNetwork()->getNodeAccountIdsForExecute());
  }

  // Validate checksums if that option is enabled.
  if (client.isAutoValidateChecksumsEnabled())
  {
    validateChecksums(client);
  }

  // There's nothing else to do if this Query is free or is meant to get the cost.
  if (!isPaymentRequired() || mImpl->mGetCost)
  {
    return;
  }

  // Save the Client for use later to generate payment Transaction protobuf objects.
  mImpl->mClient = &client;

  // Get the cost and make sure it's willing to be paid.
  mImpl->mCost = getCost(client);
  if (mImpl->mCost.toTinybars() > ((mImpl->mMaxPayment.has_value()) ? mImpl->mMaxPayment->toTinybars()
                                                                    : ((client.getMaxQueryPayment().has_value())
                                                                         ? client.getMaxQueryPayment()->toTinybars()
                                                                         : DEFAULT_MAX_QUERY_PAYMENT.toTinybars())))
  {
    throw MaxQueryPaymentExceededException("Cost to execute Query (" + std::to_string(mImpl->mCost.toTinybars()) +
                                           HbarUnit::TINYBAR().getSymbol() + ") is larger than allowed amount.");
  }
}

//-----
template<typename SdkRequestType, typename SdkResponseType>
std::optional<TransactionId> Query<SdkRequestType, SdkResponseType>::getTransactionIdInternal() const
{
  return getPaymentTransactionId();
}

/**
 * Explicit template instantiations.
 */
template class Query<AccountBalanceQuery, AccountBalance>;
template class Query<AccountInfoQuery, AccountInfo>;
template class Query<AccountRecordsQuery, AccountRecords>;
template class Query<AccountStakersQuery, AccountStakers>;
template class Query<ContractByteCodeQuery, ContractByteCode>;
template class Query<ContractCallQuery, ContractFunctionResult>;
template class Query<ContractInfoQuery, ContractInfo>;
template class Query<FileContentsQuery, FileContents>;
template class Query<FileInfoQuery, FileInfo>;
template class Query<NetworkVersionInfoQuery, NetworkVersionInfo>;
template class Query<ScheduleInfoQuery, ScheduleInfo>;
template class Query<TokenInfoQuery, TokenInfo>;
template class Query<TokenNftInfoQuery, TokenNftInfo>;
template class Query<TopicInfoQuery, TopicInfo>;
template class Query<TransactionReceiptQuery, TransactionReceipt>;
template class Query<TransactionRecordQuery, TransactionRecord>;

} // namespace Hiero
