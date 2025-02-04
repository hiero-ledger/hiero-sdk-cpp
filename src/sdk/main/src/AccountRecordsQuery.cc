// SPDX-License-Identifier: Apache-2.0
#include "AccountRecordsQuery.h"
#include "AccountRecords.h"
#include "impl/Node.h"

#include <crypto_get_account_records.pb.h>
#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
AccountRecordsQuery& AccountRecordsQuery::setAccountId(const AccountId& accountId)
{
  mAccountId = accountId;
  return *this;
}

//-----
AccountRecords AccountRecordsQuery::mapResponse(const proto::Response& response) const
{
  return AccountRecords::fromProtobuf(response.cryptogetaccountrecords());
}

//-----
grpc::Status AccountRecordsQuery::submitRequest(const proto::Query& request,
                                                const std::shared_ptr<internal::Node>& node,
                                                const std::chrono::system_clock::time_point& deadline,
                                                proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kCryptoGetAccountRecords, request, deadline, response);
}

//-----
void AccountRecordsQuery::validateChecksums(const Client& client) const
{
  mAccountId.validateChecksum(client);
}

//-----
proto::Query AccountRecordsQuery::buildRequest(proto::QueryHeader* header) const
{
  auto accountRecordsQuery = std::make_unique<proto::CryptoGetAccountRecordsQuery>();
  accountRecordsQuery->set_allocated_header(header);
  accountRecordsQuery->set_allocated_accountid(mAccountId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_cryptogetaccountrecords(accountRecordsQuery.release());
  return query;
}

//-----
proto::ResponseHeader AccountRecordsQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.cryptogetaccountrecords().header());
  return response.cryptogetaccountrecords().header();
}

} // namespace Hiero
