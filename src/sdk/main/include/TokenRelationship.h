// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_RELATIONSHIP_H_
#define HIERO_SDK_CPP_TOKEN_RELATIONSHIP_H_

#include "TokenId.h"

#include <iostream>
#include <optional>
#include <string>

#include <basic_types.pb.h>

namespace proto
{
class TokenRelationship;
}

namespace Hiero
{
/**
 * Represents the relationship between an account and a token.
 *
 * This class encapsulates information about the account's relationship with a specific token,
 * including the token ID, symbol, balance, KYC status, freeze status, and whether the relationship is created
 * implicitly.
 */
class TokenRelationship
{
public:
  TokenRelationship() = default;

  /**
   * Constructor for TokenRelationship.
   *
   * @param tokenId The unique token ID.
   * @param symbol The symbol of the token.
   * @param balance The balance of the account in the smallest denomination for fungible common tokens,
   *                or the number of NFTs held by the account for non-fungible unique tokens.
   * @param decimals The token decimals
   * @param kycStatus The KYC status of the account (optional).
   * @param freezeStatus The freeze status of the account (optional).
   * @param automaticAssociation Specifies if the relationship is created implicitly.
   */
  TokenRelationship(const TokenId& tokenId,
                    const std::string& symbol,
                    uint64_t balance,
                    uint32_t decimals,
                    int kycStatus,
                    int freezeStatus,
                    bool automaticAssociation);

  /**
   * Construct a TokenRelationship object from a TokenRelationship protobuf object.
   *
   * @param proto The TokenRelationship protobuf object from which to construct a TokenRelationship
   * object.
   * @return The constructed TokenRelationship object.
   */
  [[nodiscard]] static TokenRelationship fromProtobuf(const proto::TokenRelationship& proto);

  /**
   * Construct a TokenRelationship protobuf object from this TokenRelationship object.
   *
   * @return A pointer to the created TokenRelationship protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::TokenRelationship> toProtobuf() const;

  /**
   * Converts the TokenRelationship object to a string representation.
   *
   * @return A string representation of the TokenRelationship.
   */
  std::string toString() const;

  /**
   *  The unique token ID.
   */
  TokenId mTokenId;

  /**
   * The symbol of the token.
   */
  std::string mSymbol;

  /**
   * The balance of the account.
   */
  uint64_t mBalance;

  /**
   * The token decimals.
   */
  uint32_t mDecimals;

  /**
   * The KYC status of the account (optional).
   */
  std::optional<bool> mKycStatus;

  /**
   * The freeze status of the account (optional).
   */
  std::optional<bool> mFreezeStatus;

  /**
   * Specifies if the relationship is created implicitly.
   */
  bool mAutomaticAssociation;

private:
  /**
   * Sets the KYC status based on the provided int.
   *
   * @param kycStatus The int representation of KYC status (1 - GRANTED, 2 - REVOKED, or 0 - NOT_APPLICABLE).
   * @throws std::invalid_argument if the provided int is not a valid KYC status.
   */
  void setKycStatus(int kycStatusString);

  /**
   * Sets the freeze status based on the provided int.
   *
   * @param freezeStatus The string representation of freeze status (1 - FROZEN, 2 - UNFROZEN, or 0 - NOT_APPLICABLE).
   * @throws std::invalid_argument if the provided int is not a valid freeze status.
   */
  void setFreezeStatus(int freezeStatus);

  /**
   * Get the KYC status of the token.
   *
   * This method converts the internal KYC status to the corresponding protobuf enum value.
   *
   * @return The protobuf enum value representing the KYC status.
   */
  proto::TokenKycStatus getKycStatus() const;

  /**
   * Get the freeze status of the token.
   *
   * This method converts the internal freeze status to the corresponding protobuf enum value.
   *
   * @return The protobuf enum value representing the freeze status.
   */
  proto::TokenFreezeStatus getFreezeStatus() const;
};
} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_RELATIONSHIP_H_
