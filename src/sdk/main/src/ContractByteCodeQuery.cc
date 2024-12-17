// SPDX-License-Identifier: Apache-2.0
#include "ContractByteCodeQuery.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <contract_get_bytecode.pb.h>
#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
ContractByteCodeQuery& ContractByteCodeQuery::setContractId(const ContractId& contractId)
{
  mContractId = contractId;
  return *this;
}

//-----
ContractByteCode ContractByteCodeQuery::mapResponse(const proto::Response& response) const
{
  return internal::Utilities::stringToByteVector(response.contractgetbytecoderesponse().bytecode());
}

//-----
grpc::Status ContractByteCodeQuery::submitRequest(const proto::Query& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kContractGetBytecode, request, deadline, response);
}

//-----
void ContractByteCodeQuery::validateChecksums(const Client& client) const
{
  mContractId.validateChecksum(client);
}

//-----
proto::Query ContractByteCodeQuery::buildRequest(proto::QueryHeader* header) const
{
  auto contractByteCodeQuery = std::make_unique<proto::ContractGetBytecodeQuery>();
  contractByteCodeQuery->set_allocated_header(header);
  contractByteCodeQuery->set_allocated_contractid(mContractId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_contractgetbytecode(contractByteCodeQuery.release());
  return query;
}

//-----
proto::ResponseHeader ContractByteCodeQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.contractgetbytecoderesponse().header());
  return response.contractgetbytecoderesponse().header();
}

} // namespace Hiero