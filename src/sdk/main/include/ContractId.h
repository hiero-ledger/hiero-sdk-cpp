// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_ID_H_
#define HIERO_SDK_CPP_CONTRACT_ID_H_

#include "EvmAddress.h"
#include "Key.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class ContractID;
}

namespace Hiero
{
class Client;
}

namespace Hiero
{
/**
 * The ID for a smart contract instance.
 */
class ContractId : public Key
{
public:
  ContractId() = default;

  /**
   * Construct with a contract number.
   *
   * @param num The contract number.
   */
  explicit ContractId(uint64_t num);

  /**
   * Construct with an EVM address.
   *
   * @param address The EVM address.
   */
  explicit ContractId(const EvmAddress& address);

  /**
   * Construct with a shard, realm, a contract number, and optionally a checksum.
   *
   * @param shard    The shard number.
   * @param realm    The realm number.
   * @param num      The contract number.
   * @param checksum The checksum.
   */
  explicit ContractId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum = "");

  /**
   * Construct with shard and realm numbers, and an EVM address.
   *
   * @param shard   The shard number.
   * @param realm   The realm number.
   * @param address The EVM address.
   */
  explicit ContractId(uint64_t shard, uint64_t realm, const EvmAddress& address);

  /**
   * Compare this ContractId to another ContractId and determine if they represent the same contract.
   *
   * @param other The other ContractId with which to compare this ContractId.
   * @return \c TRUE if this ContractId is the same as the input ContractId, otherwise \c FALSE.
   */
  bool operator==(const ContractId& other) const;

  /**
   * Construct a ContractId object from a string of the form "<shard>.<realm>.<num>". <num> can be the contract number
   * or a stringified EVM address.
   *
   * @param id The contract ID string from which to construct.
   * @return The constructed ContractId object.
   * @throws std::invalid_argument If the input string is malformed or the type of <num> cannot be determined.
   */
  [[nodiscard]] static ContractId fromString(std::string_view id);

  /**
   * Construct a ContractId object from a string that represents an EvmAddress and, optionally, a shard and realm
   * number.
   *
   * @param evmAddress The string that represents an EvmAddress.
   * @param shard      The shard number.
   * @param realm      The realm number.
   * @return The constructed ContractId object.
   * @throws std::invalid_argument If the input string is malformed.
   * @throws OpenSSLException If OpenSSL is unable to convert the string to a byte array.
   */
  [[nodiscard]] static ContractId fromEvmAddress(std::string_view evmAddress,
                                                 uint64_t shard = 0ULL,
                                                 uint64_t realm = 0ULL);

  /**
   * Construct a ContractId object from an EvmAddress object and, optionally, a shard and realm number.
   *
   * @param evmAddress The EvmAddress from which to construct a ContractId.
   * @param shard      The shard number.
   * @param realm      The realm number.
   * @return The constructed ContractId object.
   */
  [[nodiscard]] static ContractId fromEvmAddress(const EvmAddress& evmAddress,
                                                 uint64_t shard = 0ULL,
                                                 uint64_t realm = 0ULL);

  /**
   * Construct a ContractId from a Solidity address.
   *
   * @param address The Solidity address from which to create a ContractId, as a string.
   * @return The constructed ContractId object.
   * @throws std::invalid_argument If a Solidity address cannot be realized from the input string.
   */
  [[nodiscard]] static ContractId fromSolidityAddress(std::string_view address);

  /**
   * Construct an ContractId object from an ContractID protobuf object.
   *
   * @param proto The ContractID protobuf object from which to create an ContractId object.
   * @return The constructed ContractId object.
   */
  [[nodiscard]] static ContractId fromProtobuf(const proto::ContractID& id);

  /**
   * Construct a ContractId object from a representative byte array.
   *
   * @param bytes The byte array from which to construct a ContractId object.
   * @return The constructed ContractId object.
   */
  [[nodiscard]] static ContractId fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Derived from Key. Create a clone of this ContractId object.
   *
   * @return A pointer to the created clone of this ContractId.
   */
  [[nodiscard]] std::unique_ptr<Key> clone() const override;

  /**
   * Derived from Key. Construct a Key protobuf object from this ContractId object.
   *
   * @return A pointer to the created Key protobuf object filled with this ContractId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::Key> toProtobufKey() const override;

  /**
   * Derived from Key. Get a byte array representation of this ContractId object.
   *
   * @return A byte array representation of this ContractId object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const override;

  /**
   * Verify the checksum of this ContractId using the input Client's network. Does nothing if this ContractId does not
   * use a contract number (i.e. it contains an EvmAddress).
   *
   * @param client The Client with which to validate this ContractId's checksum.
   * @throws BadEntityException If the checksum of this ContractId is invalid.
   */
  void validateChecksum(const Client& client) const;

  /**
   * Construct a ContractID protobuf object from this ContractId object.
   *
   * @return A pointer to the created ContractID protobuf object filled with this ContractId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::ContractID> toProtobuf() const;

  /**
   * Get the Solidity address representation of this ContractId (Long-Zero address form).
   *
   * @return The Solidity address representation of this ContractId.
   */
  [[nodiscard]] std::string toSolidityAddress() const;

  /**
   * Get the string representation of this ContractId object.
   *
   * @return The string representation of this ContractId.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Get the string representation of this ContractId object with the checksum.
   *
   * @param client The Client with which to generate the checksum.
   * @return The string representation of this ContractId object with the checksum.
   * @throws IllegalStateException If this ContractId contains an alias.
   */
  [[nodiscard]] std::string toStringWithChecksum([[maybe_unused]] const Client& client) const;

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
   * The contract number.
   */
  std::optional<uint64_t> mContractNum;

  /**
   * The 20-byte EVM address of the contract.
   *
   * Every contract has an EVM address determined by its <tt>shard.realm.num</tt> id. This address is as follows:
   *  - The first 4 bytes are the big-endian representation of the shard.
   *  - The next 8 bytes are the big-endian representation of the realm.
   *  - The final 8 bytes are the big-endian representation of the number.
   *
   * Contracts created via CREATE2 have an additional, primary address that is derived from the EIP-1014 specification,
   * and does not have a simple relation to a shard.realm.num id.
   *
   * (Please do note that CREATE2 contracts can also be referenced by the three-part EVM address described above.)
   */
  std::optional<EvmAddress> mEvmAddress;

private:
  /**
   * The checksum of this ContractId.
   */
  mutable std::string mChecksum;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_ID_H_
