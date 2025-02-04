// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_KEY_VALIDATION_TYPE_H_
#define HIERO_SDK_CPP_TOKEN_KEY_VALIDATION_TYPE_H_

#include <string_view>
#include <unordered_map>

namespace proto
{
enum TokenKeyValidation : int;
}

namespace Hiero
{
/**
 * Types of validation strategies for token keys.
 */
enum class TokenKeyValidation
{
  /**
   * Currently the default behaviour. It will perform all token key validations.
   */
  FULL_VALIDATION,

  /**
   * Perform no validations at all for all passed token keys.
   */
  NO_VALIDATION
};

/**
 * Map of protobuf TokenKeyValidation to the corresponding TokenKeyValidation.
 */
extern const std::unordered_map<proto::TokenKeyValidation, TokenKeyValidation>
  gProtobufTokenKeyValidationToTokenKeyValidation;

/**
 * Map of TokenKeyValidation to the corresponding protobuf TokenKeyValidation.
 */
extern const std::unordered_map<TokenKeyValidation, proto::TokenKeyValidation>
  gTokenKeyValidationToProtobufTokenKeyValidation;

/**
 * Map of TokenKeyValidation to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<TokenKeyValidation, const char*> gTokenKeyValidationToString;

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_KEY_VALIDATION_TYPE_H_
