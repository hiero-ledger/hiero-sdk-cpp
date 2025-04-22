// SPDX-License-Identifier: Apache-2.0
#include "AccountInfoQuery.h"
#include "AccountInfo.h"
#include "TokenId.h"
#include "TokenRelationship.h"
#include "impl/Node.h"

#include <nlohmann/json.hpp>

#include <services/crypto_get_info.pb.h>
#include <services/query.pb.h>
#include <services/query_header.pb.h>
#include <services/response.pb.h>

using json = nlohmann::json;

namespace Hiero
{
//-----
AccountInfoQuery& AccountInfoQuery::setAccountId(const AccountId& accountId)
{
  mAccountId = accountId;
  return *this;
}

//-----
AccountInfo AccountInfoQuery::mapResponse(const proto::Response& response) const
{
  AccountInfo accountInfo = AccountInfo::fromProtobuf(response.cryptogetinfo().accountinfo());
  return accountInfo;
}

//-----
grpc::Status AccountInfoQuery::submitRequest(const proto::Query& request,
                                             const std::shared_ptr<internal::Node>& node,
                                             const std::chrono::system_clock::time_point& deadline,
                                             proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kCryptoGetInfo, request, deadline, response);
}

//-----
void AccountInfoQuery::validateChecksums(const Client& client) const
{
  mAccountId.validateChecksum(client);
}

//-----
proto::Query AccountInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto accountInfoQuery = std::make_unique<proto::CryptoGetInfoQuery>();
  accountInfoQuery->set_allocated_header(header);
  accountInfoQuery->set_allocated_accountid(mAccountId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_cryptogetinfo(accountInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader AccountInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.cryptogetinfo().header());
  return response.cryptogetinfo().header();
}

} // namespace Hiero
