// SPDX-License-Identifier: Apache-2.0
#include "ContractCallQuery.h"
#include "ContractFunctionResult.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
ContractCallQuery& ContractCallQuery::setFunction(std::string_view name, const ContractFunctionParameters& parameters)
{
  setFunctionParameters(parameters.toBytes(name));
  return *this;
}

//-----
ContractCallQuery& ContractCallQuery::setContractId(const ContractId& contractId)
{
  mContractId = contractId;
  return *this;
}

//-----
ContractCallQuery& ContractCallQuery::setGas(const uint64_t& gas)
{
  mGas = gas;
  return *this;
}

//-----
ContractCallQuery& ContractCallQuery::setFunctionParameters(const std::vector<std::byte>& functionParameters)
{
  mFunctionParameters = functionParameters;
  return *this;
}

//-----
ContractCallQuery& ContractCallQuery::setSenderAccountId(const AccountId& accountId)
{
  mSenderAccountId = accountId;
  return *this;
}

//-----
ContractFunctionResult ContractCallQuery::mapResponse(const proto::Response& response) const
{
  return ContractFunctionResult::fromProtobuf(response.contractcalllocal().functionresult());
}

//-----
grpc::Status ContractCallQuery::submitRequest(const proto::Query& request,
                                              const std::shared_ptr<internal::Node>& node,
                                              const std::chrono::system_clock::time_point& deadline,
                                              proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kContractCallLocal, request, deadline, response);
}

//-----
void ContractCallQuery::validateChecksums(const Client& client) const
{
  mContractId.validateChecksum(client);
}

//-----
proto::Query ContractCallQuery::buildRequest(proto::QueryHeader* header) const
{
  auto contractCallQuery = std::make_unique<proto::ContractCallLocalQuery>();
  contractCallQuery->set_allocated_header(header);
  contractCallQuery->set_allocated_contractid(mContractId.toProtobuf().release());
  contractCallQuery->set_gas(static_cast<int64_t>(mGas));
  contractCallQuery->set_functionparameters(internal::Utilities::byteVectorToString(mFunctionParameters));

  if (mSenderAccountId.has_value())
  {
    contractCallQuery->set_allocated_sender_id(mSenderAccountId->toProtobuf().release());
  }

  proto::Query query;
  query.set_allocated_contractcalllocal(contractCallQuery.release());
  return query;
}

//-----
proto::ResponseHeader ContractCallQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.contractcalllocal().header());
  return response.contractcalllocal().header();
}

} // namespace Hiero
