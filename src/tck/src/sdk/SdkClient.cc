// SPDX-License-Identifier: Apache-2.0
#include "sdk/SdkClient.h"
#include "sdk/params/ResetParams.h"
#include "sdk/params/SetupParams.h"

#include <AccountId.h>
#include <Client.h>
#include <PrivateKey.h>

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK::SdkClient
{
namespace
{
// The Hiero C++ SDK Client the SdkClient will use to communicate with the network.
Client mClient;
}

//-----
nlohmann::json reset(const ResetParams&)
{
  mClient.close();
  return {
    {"status", "SUCCESS"}
  };
}

//-----
nlohmann::json setup(const SetupParams& params)
{
  std::string clientType;

  if (params.nodeIp.has_value() && params.nodeAccountId.has_value() && params.mirrorNetworkIp.has_value())
  {
    mClient = Client::forNetwork({
      {params.nodeIp.value(), AccountId::fromString(params.nodeAccountId.value())}
    });
    mClient.setMirrorNetwork({ params.mirrorNetworkIp.value() });
    clientType = "custom";
  }
  else
  {
    mClient = Client::forTestnet();
    clientType = "testnet";
  }

  mClient.setOperator(AccountId::fromString(params.operatorAccountId),
                      PrivateKey::fromStringDer(params.operatorPrivateKey));
  mClient.setRequestTimeout(DEFAULT_TCK_REQUEST_TIMEOUT);

  return {
    {"message", "Successfully setup " + clientType + " client."},
    { "status", "SUCCESS"                                      }
  };
}

//-----
const Client& getClient()
{
  return mClient;
}

} // namespace Hiero::TCK::SdkClient
