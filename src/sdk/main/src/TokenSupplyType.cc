// SPDX-License-Identifier: Apache-2.0
#include "TokenSupplyType.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<proto::TokenSupplyType, TokenSupplyType> gProtobufTokenSupplyTypeToTokenSupplyType = {
  {proto::TokenSupplyType::INFINITE, TokenSupplyType::INFINITE},
  { proto::TokenSupplyType::FINITE,  TokenSupplyType::FINITE  }
};

//-----
const std::unordered_map<TokenSupplyType, proto::TokenSupplyType> gTokenSupplyTypeToProtobufTokenSupplyType = {
  {TokenSupplyType::INFINITE, proto::TokenSupplyType::INFINITE},
  { TokenSupplyType::FINITE,  proto::TokenSupplyType::FINITE  }
};

//-----
const std::unordered_map<TokenSupplyType, const char*> gTokenSupplyTypeToString = {
  {TokenSupplyType::INFINITE, "INFINITE"},
  { TokenSupplyType::FINITE,  "FINITE"  }
};

} // namespace Hiero
