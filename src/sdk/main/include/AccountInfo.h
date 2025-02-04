// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_INFO_H_
#define HIERO_SDK_CPP_ACCOUNT_INFO_H_

#include "AccountId.h"
#include "EvmAddress.h"
#include "Hbar.h"
#include "Key.h"
#include "LedgerId.h"
#include "StakingInfo.h"
#include "TokenId.h"
#include "TokenRelationship.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace proto
{
class CryptoGetInfoResponse_AccountInfo;
}

namespace Hiero
{
/**
 * Response from a Hiero network when the client sends an AccountInfoQuery.
 */
class AccountInfo
{
public:
  /**
   * Construct an AccountInfo object from a CryptoGetInfoResponse_AccountInfo protobuf object.
   *
   * @param proto The CryptoGetInfoResponse_AccountInfo protobuf object from which to construct an AccountInfo object.
   * @return The constructed AccountInfo object.
   */
  [[nodiscard]] static AccountInfo fromProtobuf(const proto::CryptoGetInfoResponse_AccountInfo& proto);

  /**
   * Construct an AccountInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct an AccountInfo object.
   * @return The constructed AccountInfo object.
   */
  [[nodiscard]] static AccountInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a CryptoGetInfoResponse_AccountInfo protobuf object from this AccountInfo object.
   *
   * @return A pointer to the created CryptoGetInfoResponse_AccountInfo protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::CryptoGetInfoResponse_AccountInfo> toProtobuf() const;

  /**
   * Construct a representative byte array from this AccountInfo object.
   *
   * @return The byte array representing this AccountInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this AccountInfo object.
   *
   * @return The string representation of this AccountInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this AccountInfo to an output stream.
   *
   * @param os   The output stream.
   * @param info The AccountInfo to print.
   * @return The output stream with this AccountInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const AccountInfo& info);

  /**
   * The ID of the queried account.
   */
  AccountId mAccountId;

  /**
   * The contract account ID comprising of both the contract instance and the cryptocurrency account owned by the
   * contract instance, in the format used by Solidity.
   */
  std::string mContractAccountId;

  /**
   * Is the queried account deleted or not? If \c TRUE, then the account has been deleted, it will disappear when it
   * expires, and all transactions for it will fail except the transaction to extend its expiration date.
   */
  bool mIsDeleted = false;

  /**
   * The total amount of Hbar proxy staked to the queried account.
   */
  Hbar mProxyReceived = Hbar(0LL);

  /**
   * The key for the account, which must sign in order to transfer out, or to modify the account in any way other than
   * extending its expiration date.
   */
  std::shared_ptr<Key> mKey = nullptr;

  /**
   * The current balance of the queried account.
   */
  Hbar mBalance = Hbar(0LL);

  /**
   * If \c TRUE, the queried account's key must sign any transaction being deposited into it (in addition to all
   * withdrawals).
   */
  bool mReceiverSignatureRequired = false;

  /**
   * The time at which the queried account will expire.
   */
  std::chrono::system_clock::time_point mExpirationTime = std::chrono::system_clock::now();

  /**
   * The duration of time the queried account uses to automatically extend its expiration period. It it doesn't have
   * enough balance, it extends as long as possible. If it is empty when it expires, then it is deleted.
   */
  std::chrono::system_clock::duration mAutoRenewPeriod;

  /**
   * The queried account's memo.
   */
  std::string mMemo;

  /**
   * The number of NFTs owned by the queried account.
   */
  uint64_t mOwnedNfts = 0ULL;

  /**
   * The maximum number of tokens with which the queried account can be associated.
   */
  int32_t mMaxAutomaticTokenAssociations = 0;

  /**
   * The PublicKey alias of the queried account.
   */
  std::shared_ptr<PublicKey> mPublicKeyAlias = nullptr;

  /**
   * The EVM address alias of the queried account.
   */
  std::optional<EvmAddress> mEvmAddressAlias;

  /**
   * The ID of the ledger from which this AccountInfo was received.
   */
  LedgerId mLedgerId;

  /**
   * The staking metadata for the queried account.
   */
  StakingInfo mStakingInfo;

  /**
   * The token relationships mappings for the queried account.
   */
  std::unordered_map<TokenId, TokenRelationship> mTokenRelationships;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_INFO_H_
