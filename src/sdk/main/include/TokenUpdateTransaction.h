// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_UPDATE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_UPDATE_TRANSACTION_H_

#include "AccountId.h"
#include "Key.h"
#include "TokenId.h"
#include "TokenKeyValidation.h"
#include "Transaction.h"

#include <chrono>
#include <memory>
#include <optional>
#include <string>

namespace proto
{
class TokenUpdateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that updates the properties of an existing token. The admin key must sign this transaction to update
 * any of the token properties. The admin key can update existing keys, but cannot add new keys if they were not set
 * during the creation of the token. If no value is given for a field, that field is left unchanged. For an immutable
 * token (that is, a token created without an admin key), only the expiry may be updated. Setting any other field, in
 * that case, will cause the transaction status to resolve to TOKEN_IS_IMMUTABlE.
 *
 * Transaction Signing Requirements
 *  - Admin key is required to sign to update any token properties.
 *  - Updating the admin key requires the new admin key to sign.
 *  - If a new treasury account is set, the new treasury key is required to sign.
 *  - The account that is paying for the transaction fee.
 */
class TokenUpdateTransaction : public Transaction<TokenUpdateTransaction>
{
public:
  TokenUpdateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenUpdate transaction.
   */
  explicit TokenUpdateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenUpdateTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the token to update.
   *
   * @param tokenId The ID of the token to update.
   * @return A reference to this TokenUpdateTransaction with the newly-set token ID.
   */
  TokenUpdateTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Set a new name for the token.
   *
   * @param name The new name for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set token name.
   */
  TokenUpdateTransaction& setTokenName(std::string_view name);

  /**
   * Set a new symbol for the token.
   *
   * @param symbol The new symbol for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set token symbol.
   */
  TokenUpdateTransaction& setTokenSymbol(std::string_view symbol);

  /**
   * Set the ID of a new treasury account, which will receive all of the tokens from the old treasury account.
   *
   * @param accountId The ID of the new treasury account for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set treasury account ID.
   */
  TokenUpdateTransaction& setTreasuryAccountId(const AccountId& accountId);

  /**
   * Set a new admin key for the token.
   *
   * @param key The new admin key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set admin key.
   */
  TokenUpdateTransaction& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new KYC key for the token.
   *
   * @param key The new KYC key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set KYC key.
   */
  TokenUpdateTransaction& setKycKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new freeze key for the token.
   *
   * @param key The new freeze key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set freeze key.
   */
  TokenUpdateTransaction& setFreezeKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new wipe key for the token.
   *
   * @param key The new wipe key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set wipe key.
   */
  TokenUpdateTransaction& setWipeKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new supply key for the token.
   *
   * @param key The new supply key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set supply key.
   */
  TokenUpdateTransaction& setSupplyKey(const std::shared_ptr<Key>& key);

  /**
   * Set the ID of a new auto-renew account for the token.
   *
   * @param accountId The ID of the new auto-renew account for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set auto-renew account ID.
   */
  TokenUpdateTransaction& setAutoRenewAccountId(const AccountId& accountId);

  /**
   * Set a new auto-renew period for the token.
   *
   * @param period The new auto-renew period for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set auto-renew period.
   */
  TokenUpdateTransaction& setAutoRenewPeriod(const std::chrono::system_clock::duration& period);

  /**
   * Set a new expiration time for the token.
   *
   * @param expiration The new expiration time for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set expiration time.
   */
  TokenUpdateTransaction& setExpirationTime(const std::chrono::system_clock::time_point& expiration);

  /**
   * Set a new memo for the token.
   *
   * @param memo The new memo for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set memo.
   */
  TokenUpdateTransaction& setTokenMemo(std::string_view memo);

  /**
   * Set a new fee schedule key for the token.
   *
   * @param key The new fee schedule key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set fee schedule key.
   */
  TokenUpdateTransaction& setFeeScheduleKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new pause key for the token.
   *
   * @param key The new pause key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set pause key.
   */
  TokenUpdateTransaction& setPauseKey(const std::shared_ptr<Key>& key);

  /**
   * Set the desired metadata for the token.
   *
   * @param metadata The desired metadata for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set metadata.
   */
  TokenUpdateTransaction& setMetadata(const std::vector<std::byte>& metadata);

  /**
   * Set a new metadata key for the token.
   *
   * @param key The new metadata key for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set metadata key.
   */
  TokenUpdateTransaction& setMetadataKey(const std::shared_ptr<Key>& key);

  /**
   * Set the token verification mode for the token.
   *
   * @param mode The new verification mode for the token.
   * @return A reference to this TokenUpdateTransaction with the newly-set verification mode.
   */
  TokenUpdateTransaction& setTokenVerificationMode(TokenKeyValidation mode);

  /**
   * Get the ID of the token to update.
   *
   * @return The ID of the token to update.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

  /**
   * Get the new name for the token.
   *
   * @return The new name for the token. Returns uninitialized if no new name has been set.
   */
  [[nodiscard]] inline std::optional<std::string> getTokenName() const { return mTokenName; }

  /**
   * Get the new symbol for the token.
   *
   * @return The new symbol for the token. Returns uninitialized if no new symbol has been set.
   */
  [[nodiscard]] inline std::optional<std::string> getTokenSymbol() const { return mTokenSymbol; }

  /**
   * Get the ID of the new treasury account for the supply of tokens.
   *
   * @return The ID of the new treasury account for the supply of tokens. Returns uninitialized if no new treasury
   *         account has been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getTreasuryAccountId() const { return mTreasuryAccountId; }

  /**
   * Get the new admin key for the token.
   *
   * @return The new admin key for the token. Returns nullptr if no new admin key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getAdminKey() const { return mAdminKey; }

  /**
   * Get the new KYC key for the token.
   *
   * @return The new KYC key for the token. Returns nullptr if no new KYC key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getKycKey() const { return mKycKey; }

  /**
   * Get the new freeze key for the token.
   *
   * @return The new freeze key for the token. Returns nullptr if no new freeze key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getFreezeKey() const { return mFreezeKey; }

  /**
   * Get the new wipe key for the token.
   *
   * @return The new wipe key for the token. Returns nullptr if no new wipe key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getWipeKey() const { return mWipeKey; }

  /**
   * Get the new supply key for the token.
   *
   * @return The new supply key for the token. Returns nullptr if no new supply key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getSupplyKey() const { return mSupplyKey; }

  /**
   * Get the ID of the new auto-renew account for the token.
   *
   * @return The ID of the new auto-renew account for the token. Returns uninitialized if no new auto-renew account
   *         ID has been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getAutoRenewAccountId() const { return mAutoRenewAccountId; }

  /**
   * Get the new auto-renew period for the token.
   *
   * @return The new auto-renew period for the token. Returns uninitialized if no new auto-renew period has been set.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::duration> getAutoRenewPeriod() const
  {
    return mAutoRenewPeriod;
  }

  /**
   * Get the new expiration time for the token.
   *
   * @return The new expiration time for the token. Returns uninitialized if no new expiration time has been set.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::time_point> getExpirationTime() const
  {
    return mExpirationTime;
  }

  /**
   * Get the new memo for the token.
   *
   * @return The new memo for the token. Returns uninitialized if no new memo has been set.
   */
  [[nodiscard]] inline std::optional<std::string> getTokenMemo() const { return mTokenMemo; }

  /**
   * Get the new fee schedule key for the token.
   *
   * @return The new fee schedule key for the token. Returns nullptr if no new fee schedule key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getFeeScheduleKey() const { return mFeeScheduleKey; }

  /**
   * Get the new pause key for the token.
   *
   * @return The new pause key for the token. Returns nullptr if no new pause key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getPauseKey() const { return mPauseKey; }

  /**
   * Get the new metadata key for the token.
   *
   * @return The new metadata key for the token. Returns nullptr if no new metadata key has been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getMetadataKey() const { return mMetadataKey; }

  /**
   * Get the token verification mode for the token.
   *
   * @return The token verification mode for the token.
   */
  [[nodiscard]] inline TokenKeyValidation getTokenVerificationMode() const { return mKeyVerificationMode; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenUpdateTransaction's data to
   * a Node.
   *
   * @param request  The Transaction protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Transaction& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::TransactionResponse* response) const override;

  /**
   * Derived from Transaction. Verify that all the checksums in this TokenUpdateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenUpdateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenUpdateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenUpdateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenUpdateTransactionBody protobuf object from this TokenUpdateTransaction object.
   *
   * @return A pointer to a TokenUpdateTransactionBody protobuf object filled with this TokenUpdateTransaction object's
   *         data.
   */
  [[nodiscard]] proto::TokenUpdateTransactionBody* build() const;

  /**
   * The ID of the token to update.
   */
  TokenId mTokenId;

  /**
   * The new publicly visible name of the token. The token name is specified as a string of UTF-8 characters in Unicode.
   * UTF-8 encoding of this Unicode cannot contain the 0 byte (NUL). The token name is not unique. Maximum of 100
   * characters.
   */
  std::optional<std::string> mTokenName;

  /**
   * The new publicly visible token symbol. The token symbol is specified as a string of UTF-8 characters in Unicode.
   * UTF-8 encoding of this Unicode cannot contain the 0 byte (NUL). The token symbol is not unique. Maximum of 100
   * characters.
   */
  std::optional<std::string> mTokenSymbol;

  /**
   * The new account which will act as a treasury for the token. If the provided treasury account does not exist or is
   * deleted, the response will be INVALID_TREASURY_ACCOUNT_FOR_TOKEN. If successful, the token balance held in the
   * previous treasury account is transferred to the new one.
   */
  std::optional<AccountId> mTreasuryAccountId;

  /**
   * The new admin key for the token.
   */
  std::shared_ptr<Key> mAdminKey = nullptr;

  /**
   * The new KYC key for the token.
   */
  std::shared_ptr<Key> mKycKey = nullptr;

  /**
   * The new freeze key for the token.
   */
  std::shared_ptr<Key> mFreezeKey = nullptr;

  /**
   * The new wipe key for the token.
   */
  std::shared_ptr<Key> mWipeKey = nullptr;

  /**
   * The new supply key for the token.
   */
  std::shared_ptr<Key> mSupplyKey = nullptr;

  /**
   * The ID of the new account which will be automatically charged to renew the token's expiration, at the interval
   * specified in by token's auto renew period.
   */
  std::optional<AccountId> mAutoRenewAccountId;

  /**
   * The new interval at which the auto-renew account will be charged to extend the token's expiry.
   */
  std::optional<std::chrono::system_clock::duration> mAutoRenewPeriod;

  /**
   * The new expiration time of the token. Expiration time can be updated even if the admin key is not set. If the
   * provided expiration time is earlier than the current token expiration time, the transaction will resolved to
   * INVALID_EXPIRATION_TIME.
   */
  std::optional<std::chrono::system_clock::time_point> mExpirationTime;

  /**
   * The new memo associated with the token (UTF-8 encoding max 100 bytes).
   */
  std::optional<std::string> mTokenMemo;

  /**
   * The new fee schedule key for the token.
   */
  std::shared_ptr<Key> mFeeScheduleKey = nullptr;

  /**
   * The new pause key for the token.
   */
  std::shared_ptr<Key> mPauseKey = nullptr;

  /**
   * Metadata of the updated token.
   */
  std::vector<std::byte> mMetadata;

  /**
   * The new metadata key of the token. The metadata key has the ability to
   * change the metadata of a token (token definition, partition definition,
   * and individual NFTs). If the Token does not currently have a Metadata key,
   * the transaction will resolve to TOKEN_HAS_NO_METADATA_KEY.
   */
  std::shared_ptr<Key> mMetadataKey = nullptr;

  /**
   * Determines whether the system should check the validity of the passed keys for update.
   * Defaults to FULL_VALIDATION
   */
  TokenKeyValidation mKeyVerificationMode = TokenKeyValidation::FULL_VALIDATION;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_UPDATE_TRANSACTION_H_
