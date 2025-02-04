// SPDX-License-Identifier: Apache-2.0
#include "TokenInfoQuery.h"
#include "TokenInfo.h"
#include "impl/Node.h"

#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>
#include <token_get_info.pb.h>

namespace Hiero
{
//-----
TokenInfoQuery& TokenInfoQuery::setTokenId(const TokenId& tokenId)
{
  mTokenId = tokenId;
  return *this;
}

//-----
TokenInfo TokenInfoQuery::mapResponse(const proto::Response& response) const
{
  return TokenInfo::fromProtobuf(response.tokengetinfo().tokeninfo());
}

//-----
grpc::Status TokenInfoQuery::submitRequest(const proto::Query& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kTokenGetInfo, request, deadline, response);
}

//-----
void TokenInfoQuery::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
}

//-----
proto::Query TokenInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto tokenGetInfoQuery = std::make_unique<proto::TokenGetInfoQuery>();
  tokenGetInfoQuery->set_allocated_header(header);
  tokenGetInfoQuery->set_allocated_token(mTokenId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_tokengetinfo(tokenGetInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader TokenInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.tokengetinfo().header());
  return response.tokengetinfo().header();
}

} // namespace Hiero
