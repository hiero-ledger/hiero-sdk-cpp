// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_STAKING_INFO_H_
#define HIERO_SDK_CPP_STAKING_INFO_H_

#include "AccountId.h"
#include "Hbar.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace proto
{
class StakingInfo;
}

namespace Hiero
{
/**
 * The staking metadata for an account or contract.
 */
class StakingInfo
{
public:
  /**
   * Construct a StakingInfo object from a StakingInfo protobuf object.
   *
   * @param proto The StakingInfo protobuf object from which to construct a StakingInfo object.
   * @return The constructed StakingInfo object.
   */
  [[nodiscard]] static StakingInfo fromProtobuf(const proto::StakingInfo& proto);

  /**
   * Construct a StakingInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct a StakingInfo object.
   * @return The constructed StakingInfo object.
   */
  [[nodiscard]] static StakingInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a StakingInfo protobuf object from this StakingInfo object.
   *
   * @return A pointer to the created StakingInfo protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::StakingInfo> toProtobuf() const;

  /**
   * Construct a representative byte array from this StakingInfo object.
   *
   * @return The byte array representing this StakingInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this StakingInfo object.
   *
   * @return The string representation of this StakingInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this StakingInfo to an output stream.
   *
   * @param os   The output stream.
   * @param info The StakingInfo to print.
   * @return The output stream with this StakingInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const StakingInfo& info);

  /**
   * Is this account/contract declining to receive staking rewards?
   */
  bool mDeclineRewards = false;

  /**
   * The staking period during which either the staking settings for this account/contract changed (such as starting
   * staking or changing mStakedNodeId) or the most recent reward was earned, whichever is later. If this
   * account/contract is not currently staked to a node, then this field is not set.
   */
  std::optional<std::chrono::system_clock::time_point> mStakePeriodStart;

  /**
   * The amount of Hbar that this account/contract will receive in the next reward situation.
   */
  Hbar mPendingReward = Hbar(0LL);

  /**
   * The total balance of all accounts staked to this account/contract.
   */
  Hbar mStakedToMe = Hbar(0LL);

  /**
   * The ID of the account to which the account/contract is staking. Mutually exclusive with mStakedNodeId.
   */
  std::optional<AccountId> mStakedAccountId;

  /**
   * The ID of the node to which the account/contract is staking. Mutually exclusive with mStakedAccountId.
   */
  std::optional<uint64_t> mStakedNodeId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_STAKING_INFO_H_
