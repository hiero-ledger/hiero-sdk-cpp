// SPDX-License-Identifier: Apache-2.0
#include "TransactionReceiptQuery.h"
#include "Client.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "impl/Node.h"

#include <services/query.pb.h>
#include <services/query_header.pb.h>
#include <services/response.pb.h>
#include <services/transaction_get_receipt.pb.h>

#include <algorithm>
#include <vector>

namespace Hiero
{
//-----
TransactionReceiptQuery& TransactionReceiptQuery::setTransactionId(const TransactionId& transactionId)
{
  mTransactionId = transactionId;
  return *this;
}

//-----
TransactionReceiptQuery& TransactionReceiptQuery::setSubmittingNodeId(const AccountId& nodeId)
{
  mSubmittingNodeId = nodeId;
  return *this;
}

//-----
void TransactionReceiptQuery::onExecute(const Client& client)
{
  if (getNodeAccountIds().empty() && mSubmittingNodeId.has_value())
  {
    if (client.getAllowReceiptNodeFailover())
    {
      // node list -> sorted order
      std::vector<AccountId> otherNodes;
      for (const auto& [url, accountId] : client.getNetwork())
      {
        if (!(accountId == *mSubmittingNodeId))
        {
          otherNodes.push_back(accountId);
        }
      }
      std::sort(otherNodes.begin(),
                otherNodes.end(),
                [](const AccountId& a, const AccountId& b)
                {
                  if (a.mShardNum != b.mShardNum)
                    return a.mShardNum < b.mShardNum;
                  if (a.mRealmNum != b.mRealmNum)
                    return a.mRealmNum < b.mRealmNum;
                  return a.mAccountNum.value_or(0ULL) < b.mAccountNum.value_or(0ULL);
                });

      std::vector<AccountId> nodeIds = { *mSubmittingNodeId };
      nodeIds.insert(nodeIds.end(), otherNodes.begin(), otherNodes.end());
      setNodeAccountIds(nodeIds);
    }
    else
    {
      // Default: single node pinning
      setNodeAccountIds({ *mSubmittingNodeId });
    }
  }

  Query<TransactionReceiptQuery, TransactionReceipt>::onExecute(client);
}

//-----
TransactionReceiptQuery& TransactionReceiptQuery::setIncludeChildren(bool children)
{
  mIncludeChildren = children;
  return *this;
}

//-----
TransactionReceiptQuery& TransactionReceiptQuery::setIncludeDuplicates(bool duplicates)
{
  mIncludeDuplicates = duplicates;
  return *this;
}

//-----
TransactionReceipt TransactionReceiptQuery::mapResponse(const proto::Response& response) const
{
  return TransactionReceipt::fromProtobuf(response.transactiongetreceipt(), mTransactionId.value());
}

//-----
grpc::Status TransactionReceiptQuery::submitRequest(const proto::Query& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kTransactionGetReceipt, request, deadline, response);
}

//-----
void TransactionReceiptQuery::validateChecksums(const Client& client) const
{
  if (mTransactionId.has_value())
  {
    mTransactionId->mAccountId.validateChecksum(client);
  }
}

//-----
typename Executable<TransactionReceiptQuery, proto::Query, proto::Response, TransactionReceipt>::ExecutionStatus
TransactionReceiptQuery::determineStatus(Status status, const Client& client, const proto::Response& response)
{
  if (const Executable<TransactionReceiptQuery, proto::Query, proto::Response, TransactionReceipt>::ExecutionStatus
        baseStatus =
          Executable<TransactionReceiptQuery, proto::Query, proto::Response, TransactionReceipt>::determineStatus(
            status, client, response);
      baseStatus == ExecutionStatus::SERVER_ERROR)
  {
    return baseStatus;
  }

  switch (status)
  {
    case Status::BUSY:
    case Status::UNKNOWN:
    case Status::RECEIPT_NOT_FOUND:
    case Status::RECORD_NOT_FOUND:
      return ExecutionStatus::RETRY;

    case Status::OK:
    {
      switch (gProtobufResponseCodeToStatus.at(response.transactiongetreceipt().receipt().status()))
      {
        case Status::BUSY:
        case Status::UNKNOWN:
        case Status::OK:
        case Status::RECEIPT_NOT_FOUND:
        case Status::RECORD_NOT_FOUND:
          return ExecutionStatus::RETRY;
        default:
          return ExecutionStatus::SUCCESS;
      }
    }

    default:
      return ExecutionStatus::REQUEST_ERROR;
  }
}

//-----
proto::Query TransactionReceiptQuery::buildRequest(proto::QueryHeader* header) const
{
  auto transactionGetReceiptQuery = std::make_unique<proto::TransactionGetReceiptQuery>();
  transactionGetReceiptQuery->set_allocated_header(header);

  if (mTransactionId.has_value())
  {
    transactionGetReceiptQuery->set_allocated_transactionid(mTransactionId->toProtobuf().release());
  }

  transactionGetReceiptQuery->set_includeduplicates(mIncludeDuplicates);
  transactionGetReceiptQuery->set_include_child_receipts(mIncludeChildren);

  proto::Query query;
  query.set_allocated_transactiongetreceipt(transactionGetReceiptQuery.release());
  return query;
}

//-----
proto::ResponseHeader TransactionReceiptQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.transactiongetreceipt().header());
  return response.transactiongetreceipt().header();
}

} // namespace Hiero
