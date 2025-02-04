// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FILE_ID_H_
#define HIERO_SDK_CPP_FILE_ID_H_

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class FileID;
}

namespace Hiero
{
class Client;
}

namespace Hiero
{
/**
 * The ID for a file on Hiero.
 */
class FileId
{
public:
  FileId() = default;

  /**
   * The public NodeAddressBook for the current network.
   */
  [[maybe_unused]] static const FileId ADDRESS_BOOK;

  /**
   * The fee schedule for the current network.
   */
  [[maybe_unused]] static const FileId FEE_SCHEDULE;

  /**
   * The current exchange rate of HBAR to USD for the current network.
   */
  [[maybe_unused]] static const FileId EXCHANGE_RATES;

  /**
   * Construct with a file number.
   *
   * @param num The file number.
   */
  explicit FileId(uint64_t num);

  /**
   * Construct with a shard, realm, a file number, and optionally a checksum.
   *
   * @param shard    The shard number.
   * @param realm    The realm number.
   * @param num      The file number.
   * @param checksum The checksum.
   */
  explicit FileId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum = "");

  /**
   * Compare this FileId to another FileId and determine if they represent the same file.
   *
   * @param other The other FileId with which to compare this FileId.
   * @return \c TRUE if this FileId is the same as the input FileId, otherwise \c FALSE.
   */
  bool operator==(const FileId& other) const;

  /**
   * Construct a FileId object from a string of the form "<shard>.<realm>.<num>".
   *
   * @param id The file ID string from which to construct.
   * @return The constructed FileId object.
   * @throws std::invalid_argument If the input string is malformed.
   */
  [[nodiscard]] static FileId fromString(std::string_view id);

  /**
   * Construct a FileId from a Solidity address.
   *
   * @param address The Solidity address from which to create a FileId, as a string.
   * @return The constructed FileId object.
   * @throws std::invalid_argument If a Solidity address cannot be realized from the input string.
   */
  [[nodiscard]] static FileId fromSolidityAddress(std::string_view address);

  /**
   * Construct a FileId from a FileId protobuf object.
   *
   * @param proto The FileId protobuf object from which to create a FileId object.
   * @return The constructed FileId object.
   */
  [[nodiscard]] static FileId fromProtobuf(const proto::FileID& proto);

  /**
   * Construct a FileId object from a representative byte array.
   *
   * @param bytes The byte array from which to construct a FileId object.
   * @return The constructed FileId object.
   */
  [[nodiscard]] static FileId fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Verify the checksum of this FileId using the input Client's network.
   *
   * @param client The Client with which to validate this FileId's checksum.
   * @throws BadEntityException If the checksum of this FileId is invalid.
   */
  void validateChecksum(const Client& client) const;

  /**
   * Construct a FileID protobuf object from this FileId object.
   *
   * @return A pointer to the created FileId protobuf object filled with this FileId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::FileID> toProtobuf() const;

  /**
   * Get the Solidity address representation of this FileId (Long-Zero address form).
   *
   * @return The Solidity address representation of this FileId.
   */
  [[nodiscard]] std::string toSolidityAddress() const;

  /**
   * Get the string representation of this FileId object.
   *
   * @return The string representation of this FileId.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Get the string representation of this FileId object with the checksum.
   *
   * @param client The Client with which to generate the checksum.
   * @return The string representation of this FileId object with the checksum.
   */
  [[nodiscard]] std::string toStringWithChecksum([[maybe_unused]] const Client& client) const;

  /**
   * Get a byte array representation of this FileId object.
   *
   * @return A byte array representation of this FileId object.
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
   * The file number.
   */
  uint64_t mFileNum = 0ULL;

private:
  /**
   * The checksum of this FileIds.
   */
  mutable std::string mChecksum;
};

} // namespace Hiero

namespace std
{
template<>
struct hash<Hiero::FileId>
{
  /**
   * Operator override to enable use of FileId as map key
   */
  size_t operator()(const Hiero::FileId& id) const { return hash<string>()(id.toString()); }
};

} // namespace std

#endif // HIERO_SDK_CPP_FILE_ID_H_
