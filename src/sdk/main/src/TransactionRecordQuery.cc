// SPDX-License-Identifier: Apache-2.0
#include "TransactionRecordQuery.h"
#include "Status.h"
#include "TransactionRecord.h"
#include "impl/Node.h"

#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>
#include <transaction_get_record.pb.h>

#include <vector>

namespace Hiero
{
//-----
TransactionRecordQuery& TransactionRecordQuery::setTransactionId(const TransactionId& transactionId)
{
  mTransactionId = transactionId;
  return *this;
}

//-----
TransactionRecordQuery& TransactionRecordQuery::setIncludeChildren(bool children)
{
  mIncludeChildren = children;
  return *this;
}

//-----
TransactionRecordQuery& TransactionRecordQuery::setIncludeDuplicates(bool duplicates)
{
  mIncludeDuplicates = duplicates;
  return *this;
}

//-----
TransactionRecord TransactionRecordQuery::mapResponse(const proto::Response& response) const
{
  return TransactionRecord::fromProtobuf(response.transactiongetrecord().transactionrecord());
}

//-----
grpc::Status TransactionRecordQuery::submitRequest(const proto::Query& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kTransactionGetRecord, request, deadline, response);
}

//-----
void TransactionRecordQuery::validateChecksums(const Client& client) const
{
  if (mTransactionId.has_value())
  {
    mTransactionId->mAccountId.validateChecksum(client);
  }
}

//-----
typename Executable<TransactionRecordQuery, proto::Query, proto::Response, TransactionRecord>::ExecutionStatus
TransactionRecordQuery::determineStatus(Status status, const Client& client, const proto::Response& response)
{
  if (const Executable<TransactionRecordQuery, proto::Query, proto::Response, TransactionRecord>::ExecutionStatus
        baseStatus =
          Executable<TransactionRecordQuery, proto::Query, proto::Response, TransactionRecord>::determineStatus(
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
      if (isCostQuery())
      {
        return ExecutionStatus::SUCCESS;
      }

      switch (gProtobufResponseCodeToStatus.at(response.transactiongetrecord().transactionrecord().receipt().status()))
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
proto::Query TransactionRecordQuery::buildRequest(proto::QueryHeader* header) const
{
  auto transactionGetRecordQuery = std::make_unique<proto::TransactionGetRecordQuery>();
  transactionGetRecordQuery->set_allocated_header(header);

  if (mTransactionId.has_value())
  {
    transactionGetRecordQuery->set_allocated_transactionid(mTransactionId->toProtobuf().release());
  }

  proto::Query query;
  query.set_allocated_transactiongetrecord(transactionGetRecordQuery.release());
  return query;
}

//-----
proto::ResponseHeader TransactionRecordQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.transactiongetrecord().header());
  return response.transactiongetrecord().header();
}

} // namespace Hiero
