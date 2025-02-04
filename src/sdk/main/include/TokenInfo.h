// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_INFO_H_
#define HIERO_SDK_CPP_TOKEN_INFO_H_

#include "AccountId.h"
#include "CustomFee.h"
#include "Key.h"
#include "LedgerId.h"
#include "TokenId.h"
#include "TokenSupplyType.h"
#include "TokenType.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class TokenInfo;
}

namespace Hiero
{
/**
 * Response from a Hiero network when the client sends an TokenInfoQuery.
 */
class TokenInfo
{
public:
  /**
   * Construct a TokenInfo object from a TokenInfo protobuf object.
   *
   * @param proto The TokenInfo protobuf object from which to construct a TokenInfo object.
   * @return The constructed TokenInfo object.
   */
  [[nodiscard]] static TokenInfo fromProtobuf(const proto::TokenInfo& proto);

  /**
   * Construct a TokenInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct a TokenInfo object.
   * @return The constructed TokenInfo object.
   */
  [[nodiscard]] static TokenInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a TokenInfo protobuf object from this TokenInfo object.
   *
   * @return A pointer to the created TokenInfo protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::TokenInfo> toProtobuf() const;

  /**
   * Construct a representative byte array from this TokenInfo object.
   *
   * @return The byte array representing this TokenInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this TokenInfo object.
   *
   * @return The string representation of this TokenInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this TokenInfo to an output stream.
   *
   * @param os   The output stream.
   * @param info The TokenInfo to print.
   * @return The output stream with this TokenInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const TokenInfo& info);

  /**
   * The ID of the token.
   */
  TokenId mTokenId;

  /**
   * The name of the token. It is a string of ASCII only characters.
   */
  std::string mTokenName;

  /**
   * The symbol of the token. It is a UTF-8 capitalized alphabetical string.
   */
  std::string mTokenSymbol;

  /**
   * The number of decimal places by which a token is divisible. Always 0 for NON_FUNGIBLE_UNIQUE tokens.
   */
  uint32_t mDecimals = 0U;

  /**
   * For FUNGIBLE_COMMON tokens - the total supply of tokens that are currently in circulation. For NON_FUNGIBLE_UNIQUE
   * tokens - the number of NFTs created of this token instance.
   */
  uint64_t mTotalSupply = 0ULL;

  /**
   * The ID of the treasury account for the token.
   */
  AccountId mTreasuryAccountId;

  /**
   * The key which can perform update/delete operations on the token. If nullptr, the token can be perceived as
   * immutable (not being able to be updated/deleted).
   */
  std::shared_ptr<Key> mAdminKey = nullptr;

  /**
   * The key which can grant or revoke KYC of an account for the token's transactions. If nullptr, KYC is not required,
   * and KYC grant or revoke operations are not possible.
   */
  std::shared_ptr<Key> mKycKey = nullptr;

  /**
   * The key which can  freeze or unfreeze an account for token transactions. If nullptr, freezing is not possible.
   */
  std::shared_ptr<Key> mFreezeKey = nullptr;

  /**
   * The key which can wipe the token balance of an account. If nullptr, wipe is not possible.
   */
  std::shared_ptr<Key> mWipeKey = nullptr;

  /**
   * The key which can change the supply of a token. The key is used to sign token mint and burn operations.
   */
  std::shared_ptr<Key> mSupplyKey = nullptr;

  /**
   * The default freeze status (FreezeNotApplicable, Frozen, or Unfrozen) of Hiero accounts relative to this token.
   * Uninitialized if mFreezeKey is empty, \c TRUE if mFreezeKey is set and mDefaultFreeze is set to true, or \c FALSE
   * if mFreezeKey is set and mDefaultFreeze is set to false.
   */
  std::optional<bool> mDefaultFreezeStatus;

  /**
   * The default KYC status (KycNotApplicable or Revoked) of Hiero accounts relative to this token. KycNotApplicable if
   * mKycKey is not set, otherwise \c FALSE (no way to have the default KYC status be Granted (or \c TRUE)).
   */
  std::optional<bool> mDefaultKycStatus;

  /**
   * Specifies whether the token was deleted or not.
   */
  bool mIsDeleted = false;

  /**
   * The ID of the account which will be automatically charged to renew the token's expiration, at the interval
   * specified in mAutoRenewPeriod.
   */
  AccountId mAutoRenewAccountId;

  /**
   * The interval at which the auto-renew account will be charged to extend the token's expiry.
   */
  std::chrono::system_clock::duration mAutoRenewPeriod;

  /**
   * The epoch second at which the token should expire.
   */
  std::chrono::system_clock::time_point mExpirationTime;

  /**
   * The memo associated with the token (UTF-8 encoding max 100 bytes).
   */
  std::string mTokenMemo;

  /**
   * The token type.
   */
  TokenType mTokenType = TokenType::FUNGIBLE_COMMON;

  /**
   * The token supply type.
   */
  TokenSupplyType mSupplyType = TokenSupplyType::INFINITE;

  /**
   * For FUNGIBLE_COMMON tokens - the maximum number of fungible tokens that can be in circulation. For
   * NON_FUNGIBLE_UNIQUE tokens - the maximum number of NFTs (serial numbers) that can be in circulation.
   */
  uint64_t mMaxSupply = 0ULL;

  /**
   * The key which can change the token's custom fee schedule. If nullptr, the fee schedule is immutable.
   */
  std::shared_ptr<Key> mFeeScheduleKey = nullptr;

  /**
   * The custom fees to be assessed during a TransferTransaction that transfers units of the token.
   */
  std::vector<std::shared_ptr<CustomFee>> mCustomFees;

  /**
   * The key which can pause and unpause the new token. If nullptr, the token cannot be paused.
   */
  std::shared_ptr<Key> mPauseKey = nullptr;

  /**
   * Specifies whether the token is paused or not. Uninitialized if mPauseKey is not set, \c TRUE if mPauseKey is set
   * and the token is paused, or \c FALSE if mPauseKey is set and the token is not paused.
   */
  std::optional<bool> mPauseStatus;

  /**
   * The ID of the ledger from which this response was returned.
   */
  LedgerId mLedgerId;

  /**
   * Represents the metadata of the token definition.
   */
  std::vector<std::byte> mMetadata;

  /**
   * The key which can change the metadata of a token (token definition and individual NFTs).
   */
  std::shared_ptr<Key> mMetadataKey = nullptr;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_INFO_H_
