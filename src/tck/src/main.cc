// SPDX-License-Identifier: Apache-2.0
#include "TckServer.h"

#include <impl/EntityIdHelper.h>

using namespace Hiero::TCK;

int main(int argc, char** argv)
{
  // The TckServer constructor registers all methods and starts listening.
  TckServer tckServer((argc > 1) ? static_cast<int>(Hiero::internal::EntityIdHelper::getNum(argv[1])) : // NOLINT
                        TckServer::DEFAULT_HTTP_PORT);
  return 0;
}
