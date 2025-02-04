// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_PROXY_STAKER_H_
#define HIERO_SDK_CPP_PROXY_STAKER_H_

#include "AccountId.h"
#include "Hbar.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class ProxyStaker;
}

namespace Hiero
{
/**
 * Information about a single account that is proxy staking.
 */
class ProxyStaker
{
public:
  ProxyStaker() = default;

  /**
   * Construct with an account ID and an amount.
   *
   * @param accountID The account ID that is staking.
   * @param amount    The amount the account is staking.
   */
  ProxyStaker(AccountId accountId, int64_t amount);

  /**
   * Construct a ProxyStaker object from a ProxyStaker protobuf object.
   *
   * @param proto The ProxyStaker protobuf object from which to construct a ProxyStaker object.
   * @return The constructed ProxyStaker object.
   */
  [[nodiscard]] static ProxyStaker fromProtobuf(const proto::ProxyStaker& proto);

  /**
   * Construct a ProxyStaker object from a byte array.
   *
   * @param bytes The byte array from which to construct a ProxyStaker object.
   * @return The constructed ProxyStaker object.
   */
  [[nodiscard]] static ProxyStaker fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a ProxyStaker protobuf object from this ProxyStaker object.
   *
   * @return A pointer to the created ProxyStaker protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::ProxyStaker> toProtobuf() const;

  /**
   * Construct a representative byte array from this ProxyStaker object.
   *
   * @return The byte array representing this ProxyStaker object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this ProxyStaker object.
   *
   * @return The string representation of this ProxyStaker object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this ProxyStaker to an output stream.
   *
   * @param os     The output stream.
   * @param staker The ProxyStaker to print.
   * @return The output stream with this ProxyStaker written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const ProxyStaker& staker);

  /**
   * The ID of the account that is proxy staking.
   */
  AccountId mAccountId;

  /**
   * The number of Hbars that are currently proxy staked.
   */
  Hbar mAmount;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_PROXY_STAKER_H_
