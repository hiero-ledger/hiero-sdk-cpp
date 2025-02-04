// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_SUPPLY_TYPE_H_
#define HIERO_SDK_CPP_TOKEN_SUPPLY_TYPE_H_

#include <string_view>
#include <unordered_map>

namespace proto
{
enum TokenSupplyType : int;
}

namespace Hiero
{
/**
 * Possible token supply types (IWA Compatibility). Indicates how many tokens can have during its lifetime.
 */
enum class TokenSupplyType
{
  /**
   * Indicates that tokens have an upper bound of std::numeric_limit<uint64_t>::max().
   */
  INFINITE,
  /**
   * Indicates that tokens have an upper bound of maxSupply, provided on token creation.
   */
  FINITE
};

/**
 * Map of protobuf TokenSupplyTypes to the corresponding TokenSupplyType.
 */
extern const std::unordered_map<proto::TokenSupplyType, TokenSupplyType> gProtobufTokenSupplyTypeToTokenSupplyType;

/**
 * Map of TokenSupplyTypes to the corresponding protobuf TokenSupplyType.
 */
extern const std::unordered_map<TokenSupplyType, proto::TokenSupplyType> gTokenSupplyTypeToProtobufTokenSupplyType;

/**
 * Map of TokenSupplyType to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<TokenSupplyType, const char*> gTokenSupplyTypeToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_SUPPLY_TYPE_H_
