// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNodeContractCallQuery.h"
#include "impl/MirrorNetwork.h"
#include "impl/MirrorNodeGateway.h"

#include "exceptions/IllegalStateException.h"

namespace Hiero
{

std::string MirrorNodeContractCallQuery::execute(const Client& client)
{
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
  std::string callResult = contractCallResult["result"].dump();
  // json dump returns strings in dquotes, so we need to trim first and last characters
  callResult = callResult.substr(1, callResult.length() - 2);
  // trim 0x
  callResult = callResult.substr(2);

  return callResult;
}

} // namespace Hiero