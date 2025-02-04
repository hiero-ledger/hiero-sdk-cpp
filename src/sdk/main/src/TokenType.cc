// SPDX-License-Identifier: Apache-2.0
#include "TokenType.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<proto::TokenType, TokenType> gProtobufTokenTypeToTokenType = {
  {proto::TokenType::FUNGIBLE_COMMON,      TokenType::FUNGIBLE_COMMON    },
  { proto::TokenType::NON_FUNGIBLE_UNIQUE, TokenType::NON_FUNGIBLE_UNIQUE}
};

//-----
const std::unordered_map<TokenType, proto::TokenType> gTokenTypeToProtobufTokenType = {
  {TokenType::FUNGIBLE_COMMON,      proto::TokenType::FUNGIBLE_COMMON    },
  { TokenType::NON_FUNGIBLE_UNIQUE, proto::TokenType::NON_FUNGIBLE_UNIQUE}
};

//-----
const std::unordered_map<TokenType, const char*> gTokenTypeToString = {
  {TokenType::FUNGIBLE_COMMON,      "FUNGIBLE_COMMON"    },
  { TokenType::NON_FUNGIBLE_UNIQUE, "NON_FUNGIBLE_UNIQUE"}
};

} // namespace Hiero
