// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNodeContractEstimateGasQuery.h"
#include "impl/MirrorNetwork.h"
#include "impl/MirrorNodeGateway.h"

#include "exceptions/IllegalStateException.h"

namespace Hiero
{

std::string MirrorNodeContractEstimateGasQuery::execute(const Client& client)
{
  setEstimate(true);

  if (!getContractEvmAddress().has_value())
  {
    populateContractEvmAddress(client);
  }

  json contractCallResult =
    internal::MirrorNodeGateway::MirrorNodeQuery(client.getClientMirrorNetwork()->getNetwork()[0],
                                                 { "call" },
                                                 internal::MirrorNodeGateway::CONTRACT_INFO_QUERY,
                                                 toJson().dump(),
                                                 "POST");

  if (contractCallResult["result"].empty())
  {
    throw IllegalStateException("No result was found for the gas estimation.");
  }
  std::string estimatedGas = contractCallResult["result"].dump();
  // json dump returns strings in dquotes, so we need to trim first and last characters
  estimatedGas = estimatedGas.substr(1, estimatedGas.length() - 2);
  // trim 0x
  estimatedGas = estimatedGas.substr(2);

  return estimatedGas;
}

} // namespace Hiero