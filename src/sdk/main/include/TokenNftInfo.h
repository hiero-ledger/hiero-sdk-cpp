// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_NFT_INFO_H_
#define HIERO_SDK_CPP_TOKEN_NFT_INFO_H_

#include "AccountId.h"
#include "LedgerId.h"
#include "NftId.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class TokenNftInfo;
}

namespace Hiero
{
/**
 * Response from a Hiero network when the client sends an TokenNftInfoQuery.
 */
class TokenNftInfo
{
public:
  /**
   * Construct a TokenNftInfo object from a TokenNftInfo protobuf object.
   *
   * @param proto The TokenNftInfo protobuf object from which to construct a TokenNftInfo object.
   * @return The constructed TokenNftInfo object.
   */
  [[nodiscard]] static TokenNftInfo fromProtobuf(const proto::TokenNftInfo& proto);

  /**
   * Construct a TokenNftInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct a TokenNftInfo object.
   * @return The constructed TokenNftInfo object.
   */
  [[nodiscard]] static TokenNftInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a TokenNftInfo protobuf object from this TokenNftInfo object.
   *
   * @return A pointer to the created TokenNftInfo protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::TokenNftInfo> toProtobuf() const;

  /**
   * Construct a representative byte array from this TokenNftInfo object.
   *
   * @return The byte array representing this TokenNftInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this TokenNftInfo object.
   *
   * @return The string representation of this TokenNftInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this TokenNftInfo to an output stream.
   *
   * @param os   The output stream.
   * @param info The TokenNftInfo to print.
   * @return The output stream with this TokenNftInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const TokenNftInfo& info);

  /**
   * The ID of the NFT.
   */
  NftId mNftId;

  /**
   * The current owner of the NFT.
   */
  AccountId mAccountId;

  /**
   * The effective consensus timestamp at which the NFT was minted.
   */
  std::chrono::system_clock::time_point mCreationTime;

  /**
   * The unique metadata of the NFT.
   */
  std::vector<std::byte> mMetadata;

  /**
   * The ID of the ledger from which this response was returned.
   */
  LedgerId mLedgerId;

  /**
   * The corresponding spender account if an allowance is granted for the NFT.
   */
  std::optional<AccountId> mSpenderId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_NFT_INFO_H_
