// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "impl/Utilities.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>
#include <unordered_map>

using json = nlohmann::json;
using namespace std;

namespace Hiero
{
//-----
void BaseIntegrationTest::SetUp()
{
  mClient = Client::fromConfigFile((filesystem::current_path() / "local_node.json").string());
  mClient.setNetworkUpdatePeriod(std::chrono::hours(24));

  mFileContent = internal::Utilities::stringToByteVector(
    json::parse(std::ifstream(std::filesystem::current_path() / "hello_world.json", std::ios::in))["object"]
      .get<std::string>());
}

} // namespace Hiero
