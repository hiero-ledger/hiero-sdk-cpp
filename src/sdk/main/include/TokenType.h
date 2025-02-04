// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_TYPE_H_
#define HIERO_SDK_CPP_TOKEN_TYPE_H_

#include <string_view>
#include <unordered_map>

namespace proto
{
enum TokenType : int;
}

namespace Hiero
{
/**
 * Possible token types (IWA Compatibility).
 *
 * Apart from fungible and non-fungible, tokens can have either a common or unique representation. This distinction
 * might seem subtle, but it is important when considering how tokens can be traced and if they can have isolated and
 * unique properties.
 */
enum class TokenType
{
  /**
   * Interchangeable value with one another, where any quantity of them has the same value as another equal quantity if
   * they are in the same class. Share a single set of properties, not distinct from one another. Simply represented as
   * a balance or quantity to a given Hiero account.
   */
  FUNGIBLE_COMMON,
  /**
   * Unique, not interchangeable with other tokens of the same type as they typically have different values.
   * Individually traced and can carry unique properties (e.g. serial number).
   */
  NON_FUNGIBLE_UNIQUE
};

/**
 * Map of protobuf TokenTypes to the corresponding TokenType.
 */
extern const std::unordered_map<proto::TokenType, TokenType> gProtobufTokenTypeToTokenType;

/**
 * Map of TokenTypes to the corresponding protobuf TokenType.
 */
extern const std::unordered_map<TokenType, proto::TokenType> gTokenTypeToProtobufTokenType;

/**
 * Map of TokenType to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<TokenType, const char*> gTokenTypeToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_TYPE_H_
