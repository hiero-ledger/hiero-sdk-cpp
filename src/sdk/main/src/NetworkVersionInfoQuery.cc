// SPDX-License-Identifier: Apache-2.0
#include "NetworkVersionInfoQuery.h"
#include "Client.h"
#include "NetworkVersionInfo.h"
#include "Status.h"
#include "TransferTransaction.h"
#include "impl/Node.h"

#include <network_get_version_info.pb.h>
#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
NetworkVersionInfo NetworkVersionInfoQuery::mapResponse(const proto::Response& response) const
{
  return NetworkVersionInfo::fromProtobuf(response.networkgetversioninfo());
}

//-----
grpc::Status NetworkVersionInfoQuery::submitRequest(const proto::Query& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kNetworkGetVersionInfo, request, deadline, response);
}

//-----
void NetworkVersionInfoQuery::validateChecksums(const Client& client) const
{
  // No entity IDs to validate.
}

//-----
proto::Query NetworkVersionInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto networkVersionInfoQuery = std::make_unique<proto::NetworkGetVersionInfoQuery>();
  networkVersionInfoQuery->set_allocated_header(header);

  proto::Query query;
  query.set_allocated_networkgetversioninfo(networkVersionInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader NetworkVersionInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.networkgetversioninfo().header());
  return response.networkgetversioninfo().header();
}

} // namespace Hiero
