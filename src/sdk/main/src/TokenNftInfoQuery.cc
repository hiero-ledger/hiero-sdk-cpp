// SPDX-License-Identifier: Apache-2.0
#include "TokenNftInfoQuery.h"
#include "Client.h"
#include "TokenNftInfo.h"
#include "TransactionRecord.h"
#include "TransferTransaction.h"
#include "impl/Node.h"

#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>
#include <token_get_nft_info.pb.h>

namespace Hiero
{
//-----
TokenNftInfoQuery& TokenNftInfoQuery::setNftId(const NftId& nft)
{
  mNftId = nft;
  return *this;
}

//-----
TokenNftInfo TokenNftInfoQuery::mapResponse(const proto::Response& response) const
{
  return TokenNftInfo::fromProtobuf(response.tokengetnftinfo().nft());
}

//-----
grpc::Status TokenNftInfoQuery::submitRequest(const proto::Query& request,
                                              const std::shared_ptr<internal::Node>& node,
                                              const std::chrono::system_clock::time_point& deadline,
                                              proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kTokenGetNftInfo, request, deadline, response);
}

//-----
void TokenNftInfoQuery::validateChecksums(const Client& client) const
{
  mNftId.mTokenId.validateChecksum(client);
}

//-----
proto::Query TokenNftInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto tokenGetNftInfoQuery = std::make_unique<proto::TokenGetNftInfoQuery>();
  tokenGetNftInfoQuery->set_allocated_header(header);
  tokenGetNftInfoQuery->set_allocated_nftid(mNftId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_tokengetnftinfo(tokenGetNftInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader TokenNftInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.tokengetnftinfo().header());
  return response.tokengetnftinfo().header();
}

} // namespace Hiero
