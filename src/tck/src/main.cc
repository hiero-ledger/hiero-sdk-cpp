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
  tckServer.add("approveAllowance", tckServer.getHandle(&AccountService::approveAllowance));
  tckServer.add("createAccount", tckServer.getHandle(&AccountService::createAccount));
  tckServer.add("deleteAccount", tckServer.getHandle(&AccountService::deleteAccount));
  tckServer.add("deleteAllowance", tckServer.getHandle(&AccountService::deleteAllowance));
  tckServer.add("transferCrypto", tckServer.getHandle(&AccountService::transferCrypto));
  tckServer.add("updateAccount", tckServer.getHandle(&AccountService::updateAccount));

  // Add the TokenService functions.
  tckServer.add("airdropToken", tckServer.getHandle(&TokenService::airdropToken));
  tckServer.add("associateToken", tckServer.getHandle(&TokenService::associateToken));
  tckServer.add("burnToken", tckServer.getHandle(&TokenService::burnToken));
  tckServer.add("cancelAirdrop", tckServer.getHandle(&TokenService::cancelAirdrop));
  tckServer.add("claimAirdrop", tckServer.getHandle(&TokenService::claimAirdrop));
  tckServer.add("createToken", tckServer.getHandle(&TokenService::createToken));
  tckServer.add("deleteToken", tckServer.getHandle(&TokenService::deleteToken));
  tckServer.add("dissociateToken", tckServer.getHandle(&TokenService::dissociateToken));
  tckServer.add("freezeToken", tckServer.getHandle(&TokenService::freezeToken));
  tckServer.add("grantTokenKyc", tckServer.getHandle(&TokenService::grantTokenKyc));
  tckServer.add("mintToken", tckServer.getHandle(&TokenService::mintToken));
  tckServer.add("pauseToken", tckServer.getHandle(&TokenService::pauseToken));
  tckServer.add("revokeTokenKyc", tckServer.getHandle(&TokenService::revokeTokenKyc));
  tckServer.add("unpauseToken", tckServer.getHandle(&TokenService::unpauseToken));
  tckServer.add("unfreezeToken", tckServer.getHandle(&TokenService::unfreezeToken));
  tckServer.add("updateTokenFeeSchedule", tckServer.getHandle(&TokenService::updateTokenFeeSchedule));
  tckServer.add("updateToken", tckServer.getHandle(&TokenService::updateToken));
  tckServer.add("wipeToken", tckServer.getHandle(&TokenService::wipeToken));

  // Start listening for requests.
  tckServer.startServer();

  return 0;
}
