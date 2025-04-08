// SPDX-License-Identifier: Apache-2.0
#include <services/contract_get_info.pb.h>

#include "ContractInfo.h"
#include "ContractInfoQuery.h"
#include "impl/Node.h"

#include <services/query.pb.h>
#include <services/query_header.pb.h>
#include <services/response.pb.h>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Hiero
{
//-----
ContractInfoQuery& ContractInfoQuery::setContractId(const ContractId& contractId)
{
  mContractId = contractId;
  return *this;
}

//-----
ContractInfo ContractInfoQuery::mapResponse(const proto::Response& response) const
{
  ContractInfo contractInfo = ContractInfo::fromProtobuf(response.contractgetinfo().contractinfo());
  return contractInfo;
}

//-----
grpc::Status ContractInfoQuery::submitRequest(const proto::Query& request,
                                              const std::shared_ptr<internal::Node>& node,
                                              const std::chrono::system_clock::time_point& deadline,
                                              proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kContractGetInfo, request, deadline, response);
}

//-----
void ContractInfoQuery::validateChecksums(const Client& client) const
{
  mContractId.validateChecksum(client);
}

//-----
proto::Query ContractInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto contractGetInfoQuery = std::make_unique<proto::ContractGetInfoQuery>();
  contractGetInfoQuery->set_allocated_header(header);
  contractGetInfoQuery->set_allocated_contractid(mContractId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_contractgetinfo(contractGetInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader ContractInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.contractgetinfo().header());
  return response.contractgetinfo().header();
}

} // namespace Hiero
