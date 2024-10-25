/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "TckServer.h"
#include "account/AccountService.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"

#include <impl/EntityIdHelper.h>

using namespace Hedera::TCK;

int main(int argc, char** argv)
{
  // Initialize the TCK server with the port number.
  TckServer tckServer((argc > 1) ? static_cast<int>(Hedera::internal::EntityIdHelper::getNum(argv[1])) : // NOLINT
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

  // Start listening for requests.
  tckServer.startServer();

  return 0;
}
