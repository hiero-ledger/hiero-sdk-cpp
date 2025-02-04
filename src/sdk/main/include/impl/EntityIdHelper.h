// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_ENTITY_ID_HELPER_H_
#define HIERO_SDK_CPP_IMPL_ENTITY_ID_HELPER_H_

#include <charconv>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace Hiero
{
class Client;
class LedgerId;
}

namespace Hiero::internal::EntityIdHelper
{
/**
 * The length of a Solidity address in bytes.
 */
constexpr unsigned int SOLIDITY_ADDRESS_LEN = 20;

/**
 * The length of a hex-encoded Solidity address.
 */
constexpr unsigned int SOLIDITY_ADDRESS_LEN_HEX = SOLIDITY_ADDRESS_LEN * 2;

/**
 * Create an EntityType from a Solidity address byte array.
 *
 * @tparam EntityType The type of entity to create.
 * @param address The Solidity address byte array from which to construct the EntityType.
 * @return The constructed EntityType.
 * @throws std::invalid_argument If the input byte array is not a valid Solidity address.
 */
template<typename EntityType>
[[nodiscard]] EntityType fromSolidityAddress(const std::vector<std::byte>& address);

/**
 * Convert a string to a number type. E.g. "123" will return 123.
 *
 * @param str The string to convert.
 * @return The number type contained in the string.
 */
template<typename T = uint64_t>
[[nodiscard]] T getNum(std::string_view str)
{
  T num;
  if (auto result = std::from_chars(str.data(), str.data() + str.size(), num);
      result.ec != std::errc() || result.ptr != str.data() + str.size())
  {
    throw std::invalid_argument("Input entity ID string is malformed");
  }

  return num;
}

/**
 * Get the shard from an entity ID.
 *
 * @param id The ID of the entity from which to grab the shard number.
 * @return The shard number of the input entity ID.
 * @throws std::invalid_argument If the shard number is unable to be realized from the input entity ID.
 */
[[nodiscard]] uint64_t getShardNum(std::string_view id);

/**
 * Get the realm from an entity ID.
 *
 * @param id The ID of the entity from which to grab the realm number.
 * @return The realm number of the input entity ID.
 * @throws std::invalid_argument If the realm number is unable to be realized from the input entity ID.
 */
[[nodiscard]] uint64_t getRealmNum(std::string_view id);

/**
 * Get the number from an entity ID.
 *
 * @param id The ID of the entity from which to grab the entity number.
 * @return The entity number of the input entity ID.
 * @throws std::invalid_argument If the entity number is unable to be realized from the input entity ID.
 */
[[nodiscard]] uint64_t getEntityNum(std::string_view id);

/**
 * Get the number from an entity ID as a string.
 *
 * @param id The ID of the entity from which to grab the entity number.
 * @return The entity number of the input entity ID as a string.
 * @throws std::invalid_argument If the entity number is unable to be realized from the input entity ID.
 */
[[nodiscard]] std::string_view getEntityNumStr(std::string_view id);

/**
 * Get the checksum from an entity ID.
 *
 * @param id The ID of the entity from which to grab the checksum.
 * @return The checksum of the input entity ID. Returns an empty string if there is no checksum.
 */
[[nodiscard]] std::string_view getChecksum(std::string_view id);

/**
 * Generate the checksum of a Hiero address for a particular ledger.
 *
 * @param address  The Hiero address from which to generate a checksum.
 * @param ledgerId The ID for which the checksum should be generated.
 */
[[nodiscard]] std::string checksum(std::string_view address, const LedgerId& ledgerId);

/**
 * Determine if the input byte array could represent a long-zero address.
 *
 * @param address The byte array to determine if it could be a long-zero address.
 * @return \c TRUE if the input byte array could represent a long-zero address, otherwise \c FALSE.
 */
[[nodiscard]] bool isLongZeroAddress(const std::vector<std::byte>& address);

/**
 * Decode a Solidity address contained in a string to a byte array.
 *
 * @param address The Solidity address to decode.
 * @return The bytes of the Solidity address.
 * @throws std::invalid_argument If a Solidity address cannot be realized from the input string.
 */
[[nodiscard]] std::vector<std::byte> decodeSolidityAddress(std::string_view address);

/**
 * Get the string representation of an entity, optionally with a checksum.
 *
 * @param shard    The shard of the entity.
 * @param realm    The realm of the entity.
 * @param num      The number of the entity.
 * @param checksum The checksum of the entity.
 * @return The string representation of an entity.
 */
[[nodiscard]] std::string toString(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum = "");

/**
 * Get the Solidity address of an entity.
 *
 * @param shard The shard of the entity.
 * @param realm The realm of the entity.
 * @param num   The number of the entity.
 * @return The Solidity address of the entity, in a string.
 * @throws std::invalid_argument If the shard number is too large (value must be able to fit in 4 bytes).
 */
[[nodiscard]] std::string toSolidityAddress(uint64_t shard, uint64_t realm, uint64_t num);

/**
 * Validate the checksum of an entity.
 *
 * @param shard            The shard of the entity.
 * @param realm            The realm of the entity.
 * @param num              The number of the entity.
 * @param client           The Client to use to validate the checksum.
 * @param expectedChecksum The checksum to validate.
 * @throws BadEntityException If the checksum of the entity doesn't match up with the expected checksum.
 */
void validate(uint64_t shard, uint64_t realm, uint64_t num, const Client& client, std::string_view expectedChecksum);

} // namespace Hiero::internal::EntityIdHelper

#endif // HIERO_SDK_CPP_IMPL_ENTITY_ID_HELPER_H_
