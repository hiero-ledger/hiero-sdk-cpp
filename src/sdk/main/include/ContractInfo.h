// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_INFO_H_
#define HIERO_SDK_CPP_CONTRACT_INFO_H_

#include "AccountId.h"
#include "ContractId.h"
#include "Hbar.h"
#include "Key.h"
#include "LedgerId.h"
#include "StakingInfo.h"
#include "TokenId.h"
#include "TokenRelationship.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace proto
{
class ContractGetInfoResponse_ContractInfo;
}

namespace Hiero
{
/**
 * Contains information about a smart contract instance. This includes the account that it uses, the file containing its
 * initcode (if a file was used to initialize the contract), and the time when it will expire.
 */
class ContractInfo
{
public:
  /**
   * Construct a ContractInfo object from a ContractGetInfoResponse_ContractInfo protobuf object.
   *
   * @param proto The ContractGetInfoResponse_ContractInfo protobuf object from which to construct a ContractInfo
   *              object.
   * @return The constructed ContractInfo object.
   */
  [[nodiscard]] static ContractInfo fromProtobuf(const proto::ContractGetInfoResponse_ContractInfo& proto);

  /**
   * Construct a ContractInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct a ContractInfo object.
   * @return The constructed ContractInfo object.
   */
  [[nodiscard]] static ContractInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a ContractGetInfoResponse_ContractInfo protobuf object from this ContractInfo object.
   *
   * @return A pointer to the created ContractGetInfoResponse_ContractInfo protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::ContractGetInfoResponse_ContractInfo> toProtobuf() const;

  /**
   * Construct a representative byte array from this ContractInfo object.
   *
   * @return The byte array representing this ContractInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this ContractInfo object.
   *
   * @return The string representation of this ContractInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this ContractInfo to an output stream.
   *
   * @param os   The output stream.
   * @param info The ContractInfo to print.
   * @return The output stream with this ContractInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const ContractInfo& info);

  /**
   * The ID of the contract.
   */
  ContractId mContractId;

  /**
   * The ID of the account that is owned by the contract.
   */
  AccountId mAccountId;

  /**
   * The ID of both the contract and the account owned by the contract, in the format used by Solidity.
   */
  std::string mContractAccountId;

  /**
   * Pointer to the admin key that can be used to modify the state of the contract.
   */
  std::shared_ptr<Key> mAdminKey = nullptr;

  /**
   * The expiration time of the queried contract and its account.
   */
  std::chrono::system_clock::time_point mExpirationTime;

  /**
   * The duration of time the queried contract uses to automatically extend its expiration period.
   */
  std::chrono::system_clock::duration mAutoRenewPeriod;

  /**
   * Get the number of bytes of storage the queried contract is using (which affects the cost to extend the expiration
   * time).
   */
  uint64_t mStorage = 0ULL;

  /**
   * The contract memo.
   */
  std::string mMemo;

  /**
   * The current contract balance.
   */
  Hbar mBalance = Hbar(0LL);

  /**
   * Has this contract been deleted?
   */
  bool mIsDeleted = false;

  /**
   * The ledger ID from which the response was returned.
   */
  LedgerId mLedgerId;

  /**
   * The ID of the account to charge for auto-renewal. If not set, or set to an account with zero Hbar balance, the
   * contract's own Hbar balance will be used to cover auto-renewal fees.
   */
  std::optional<AccountId> mAutoRenewAccountId;

  /**
   * The maximum number of token associations for this contract.
   */
  int32_t mMaxAutomaticTokenAssociations = 0U;

  /**
   * The staking metadata for this contract.
   */
  StakingInfo mStakingInfo;

  /**
   * The token relationships mappings for the queried account.
   */
  std::unordered_map<TokenId, TokenRelationship> mTokenRelationships;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_INFO_H_
