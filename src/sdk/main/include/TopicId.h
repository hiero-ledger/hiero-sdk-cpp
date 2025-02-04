// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_ID_H_
#define HIERO_SDK_CPP_TOPIC_ID_H_

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class TopicID;
}

namespace Hiero
{
class Client;
}

namespace Hiero
{
/**
 * The ID for a topic on Hiero.
 */
class TopicId
{
public:
  TopicId() = default;

  /**
   * Construct with a topic number.
   *
   * @param num The topic number.
   */
  explicit TopicId(uint64_t num);

  /**
   * Construct with a shard, realm, a topic number, and optionally a checksum.
   *
   * @param shard    The shard number.
   * @param realm    The realm number.
   * @param num      The topic number.
   * @param checksum The checksum.
   */
  explicit TopicId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum = "");

  /**
   * Compare this TopicId to another TopicId and determine if they represent the same topic.
   *
   * @param other The other TopicId with which to compare this TopicId.
   * @return \c TRUE if this TopicId is the same as the input TopicId, otherwise \c FALSE.
   */
  bool operator==(const TopicId& other) const;

  /**
   * Construct a TopicId object from a string of the form "<shard>.<realm>.<num>".
   *
   * @param id The topic ID string from which to construct.
   * @return The constructed TopicId object.
   * @throws std::invalid_argument If the input string is malformed.
   */
  [[nodiscard]] static TopicId fromString(std::string_view id);

  /**
   * Construct a TopicId from a Solidity address.
   *
   * @param address The Solidity address from which to create a TopicId, as a string.
   * @return The constructed TopicId object.
   * @throws std::invalid_argument If a Solidity address cannot be realized from the input string.
   */
  [[nodiscard]] static TopicId fromSolidityAddress(std::string_view address);

  /**
   * Construct a TopicId from a TopicId protobuf object.
   *
   * @param proto The TopicId protobuf object from which to create a TopicId object.
   * @return The constructed TopicId object.
   */
  [[nodiscard]] static TopicId fromProtobuf(const proto::TopicID& proto);

  /**
   * Construct a TopicId object from a representative byte array.
   *
   * @param bytes The byte array from which to construct a TopicId object.
   * @return The constructed TopicId object.
   */
  [[nodiscard]] static TopicId fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Verify the checksum of this TopicId using the input Client's network.
   *
   * @param client The Client with which to validate this TopicId's checksum.
   * @throws BadEntityException If the checksum of this TopicId is invalid.
   */
  void validateChecksum(const Client& client) const;

  /**
   * Construct a TopicID protobuf object from this TopicId object.
   *
   * @return A pointer to the created TopicId protobuf object filled with this TopicId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::TopicID> toProtobuf() const;

  /**
   * Get the Solidity address representation of this TopicId (Long-Zero address form).
   *
   * @return The Solidity address representation of this TopicId.
   */
  [[nodiscard]] std::string toSolidityAddress() const;

  /**
   * Get the string representation of this TopicId object.
   *
   * @return The string representation of this TopicId.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Get the string representation of this TopicId object with the checksum.
   *
   * @param client The Client with which to generate the checksum.
   * @return The string representation of this TopicId object with the checksum.
   */
  [[nodiscard]] std::string toStringWithChecksum([[maybe_unused]] const Client& client) const;

  /**
   * Get a byte array representation of this TopicId object.
   *
   * @return A byte array representation of this TopicId object.
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
   * The topic number.
   */
  uint64_t mTopicNum = 0ULL;

private:
  /**
   * The checksum of this TopicIds.
   */
  mutable std::string mChecksum;
};

} // namespace Hiero

namespace std
{
template<>
struct hash<Hiero::TopicId>
{
  /**
   * Operator override to enable use of TopicId as map key
   */
  size_t operator()(const Hiero::TopicId& id) const { return hash<string>()(id.toString()); }
};

} // namespace std

#endif // HIERO_SDK_CPP_TOPIC_ID_H_
