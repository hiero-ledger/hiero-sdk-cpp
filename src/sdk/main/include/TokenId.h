// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_ID_H_
#define HIERO_SDK_CPP_TOKEN_ID_H_

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class TokenID;
}

namespace Hiero
{
class Client;
class NftId;
}

namespace Hiero
{
/**
 * The ID for a token on Hiero.
 */
class TokenId
{
public:
  TokenId() = default;

  /**
   * Construct with a token number.
   *
   * @param num The token number.
   */
  explicit TokenId(uint64_t num);

  /**
   * Construct with a shard, realm, a token number, and optionally a checksum.
   *
   * @param shard    The shard number.
   * @param realm    The realm number.
   * @param num      The token number.
   * @param checksum The checksum.
   */
  explicit TokenId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum = "");

  /**
   * Compare this TokenId to another TokenId and determine if they represent the same token.
   *
   * @param other The other TokenId with which to compare this TokenId.
   * @return \c TRUE if this TokenId is the same as the input TokenId, otherwise \c FALSE.
   */
  bool operator==(const TokenId& other) const;

  /**
   * Construct a TokenId object from a string of the form "<shard>.<realm>.<num>".
   *
   * @param id The token ID string from which to construct.
   * @return The constructed TokenId object.
   * @throws std::invalid_argument If the input string is malformed.
   */
  [[nodiscard]] static TokenId fromString(std::string_view id);

  /**
   * Construct a TokenId from a Solidity address.
   *
   * @param address The Solidity address from which to create a TokenId, as a string.
   * @return The constructed TokenId object.
   * @throws std::invalid_argument If a Solidity address cannot be realized from the input string.
   */
  [[nodiscard]] static TokenId fromSolidityAddress(std::string_view address);

  /**
   * Construct a TokenId from a TokenId protobuf object.
   *
   * @param proto The TokenId protobuf object from which to create a TokenId object.
   * @return The constructed TokenId object.
   */
  [[nodiscard]] static TokenId fromProtobuf(const proto::TokenID& proto);

  /**
   * Construct a TokenId object from a representative byte array.
   *
   * @param bytes The byte array from which to construct a TokenId object.
   * @return The constructed TokenId object.
   */
  [[nodiscard]] static TokenId fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Verify the checksum of this TokenId using the input Client's network.
   *
   * @param client The Client with which to validate this TokenId's checksum.
   * @throws BadEntityException If the checksum of this TokenId is invalid.
   */
  void validateChecksum(const Client& client) const;

  /**
   * Construct an NftId from this TokenId and a serial number.
   *
   * @param serial The serial number of the NftId.
   * @param The constructed NftId.
   */
  [[nodiscard]] NftId nft(uint64_t serial) const;

  /**
   * Construct a TokenID protobuf object from this TokenId object.
   *
   * @return A pointer to the created TokenId protobuf object filled with this TokenId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::TokenID> toProtobuf() const;

  /**
   * Get the string representation of this TokenId object.
   *
   * @return The string representation of this TokenId.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Get the string representation of this TokenId object with the checksum.
   *
   * @param client The Client with which to generate the checksum.
   * @return The string representation of this TokenId object with the checksum.
   */
  [[nodiscard]] std::string toStringWithChecksum([[maybe_unused]] const Client& client) const;

  /**
   * Get a byte array representation of this TokenId object.
   *
   * @return A byte array representation of this TokenId object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Get the checksum of this ContractId.
   *
   * @return The checksum of this ContractId.
   */
  [[nodiscard]] inline std::string getChecksum() const { return mChecksum; }

  /**
   * The shard number.
   */
  uint64_t mShardNum = 0ULL;

  /**
   * The realm number.
   */
  uint64_t mRealmNum = 0ULL;

  /**
   * The token number.
   */
  uint64_t mTokenNum = 0ULL;

private:
  /**
   * The checksum of this TokenIds.
   */
  mutable std::string mChecksum;
};

} // namespace Hiero

namespace std
{
template<>
struct hash<Hiero::TokenId>
{
  /**
   * Operator override to enable use of TokenId as map key
   */
  size_t operator()(const Hiero::TokenId& id) const { return hash<string>()(id.toString()); }
};

} // namespace std

#endif // HIERO_SDK_CPP_TOKEN_ID_H_
