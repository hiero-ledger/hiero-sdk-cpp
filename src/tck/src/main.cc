// SPDX-License-Identifier: Apache-2.0
#include "TckServer.h"
#include "account/AccountService.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"
#include "token/TokenService.h"

#include <impl/EntityIdHelper.h>

using namespace Hiero::TCK;

int main(int argc, char** argv)
{
  // Initialize the TCK server with the port number.
  TckServer tckServer((argc > 1) ? static_cast<int>(Hiero::internal::EntityIdHelper::getNum(argv[1])) : // NOLINT
                        TckServer::DEFAULT_HTTP_PORT);

  // Add the SDK client functions.
  tckServer.add("setup", tckServer.getHandle(&SdkClient::setup));
  tckServer.add("reset", tckServer.getHandle(&SdkClient::reset));

  // Add the KeyService functions.
  tckServer.add("generateKey", tckServer.getHandle(&KeyService::generateKey));

  // Add the AccountService functions.
  tckServer.add("createAccount", tckServer.getHandle(&AccountService::createAccount));
  tckServer.add("deleteAccount", tckServer.getHandle(&AccountService::deleteAccount));
  tckServer.add("updateAccount", tckServer.getHandle(&AccountService::updateAccount));

  // Add the TokenService functions.
  tckServer.add("createToken", tckServer.getHandle(&TokenService::createToken));
  tckServer.add("deleteToken", tckServer.getHandle(&TokenService::deleteToken));
  tckServer.add("mintToken", tckServer.getHandle(&TokenService::mintToken));
  tckServer.add("updateToken", tckServer.getHandle(&TokenService::updateToken));

  // Start listening for requests.
  tckServer.startServer();

  return 0;
}
