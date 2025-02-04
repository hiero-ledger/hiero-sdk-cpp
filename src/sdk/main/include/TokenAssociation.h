// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_ASSOCIATION_H_
#define HIERO_SDK_CPP_TOKEN_ASSOCIATION_H_

#include "AccountId.h"
#include "TokenId.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class TokenAssociation;
}

namespace Hiero
{
/**
 * A helper class that represents an account's association with a particular token.
 */
class TokenAssociation
{
public:
  /**
   * Construct a TokenAssociation object from a TokenAssociation protobuf object.
   *
   * @param proto The TokenAssociation protobuf object from which to construct a TokenAssociation object.
   * @return The constructed TokenAssociation object.
   */
  [[nodiscard]] static TokenAssociation fromProtobuf(const proto::TokenAssociation& proto);

  /**
   * Construct a TokenAssociation object from a byte array.
   *
   * @param bytes The byte array from which to construct a TokenAssociation object.
   * @return The constructed TokenAssociation object.
   */
  [[nodiscard]] static TokenAssociation fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a TokenAssociation protobuf object from this TokenAssociation object.
   *
   * @return A pointer to the created TokenAssociation protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::TokenAssociation> toProtobuf() const;

  /**
   * Construct a representative byte array from this TokenAssociation object.
   *
   * @return The byte array representing this TokenAssociation object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this TokenAssociation object.
   *
   * @return The string representation of this TokenAssociation object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this TokenAssociation to an output stream.
   *
   * @param os     The output stream.
   * @param record The TokenAssociation to print.
   * @return The output stream with this TokenAssociation written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const TokenAssociation& record);

  /**
   * The ID of the account associated with the token.
   */
  AccountId mAccountId;

  /**
   * The ID of the token associated with the account.
   */
  TokenId mTokenId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_ASSOCIATION_H_
