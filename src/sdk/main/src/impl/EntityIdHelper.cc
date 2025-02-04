// SPDX-License-Identifier: Apache-2.0
#include "impl/EntityIdHelper.h"
#include "AccountId.h"
#include "Client.h"
#include "ContractId.h"
#include "DelegateContractId.h"
#include "FileId.h"
#include "LedgerId.h"
#include "ScheduleId.h"
#include "TokenId.h"
#include "TopicId.h"
#include "exceptions/BadEntityIdException.h"
#include "exceptions/OpenSSLException.h"
#include "exceptions/UninitializedException.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <cstddef>
#include <stdexcept>
#include <vector>

namespace Hiero::internal::EntityIdHelper
{
//-----
template<typename EntityType>
EntityType fromSolidityAddress(const std::vector<std::byte>& address)
{
  if (address.size() != SOLIDITY_ADDRESS_LEN)
  {
    throw std::invalid_argument("Input address is not a valid Solidity address. Solidity addresses should be " +
                                std::to_string(SOLIDITY_ADDRESS_LEN) + " bytes long");
  }

  return EntityType(static_cast<uint64_t>(*internal::Utilities::toTypePtr<uint32_t>(address.data())),
                    *internal::Utilities::toTypePtr<uint64_t>(address.data() + 4),
                    *internal::Utilities::toTypePtr<uint64_t>(address.data() + 12));
}

//-----
uint64_t getShardNum(std::string_view id)
{
  const size_t firstDot = id.find_first_of('.');

  if (firstDot == std::string::npos)
  {
    throw std::invalid_argument("Input entity ID string is malformed");
  }

  return getNum(id.substr(0, firstDot));
}

//-----
uint64_t getRealmNum(std::string_view id)
{
  const size_t firstDot = id.find_first_of('.');
  const size_t lastDot = id.find_last_of('.');

  if (firstDot == lastDot)
  {
    throw std::invalid_argument("Input entity ID string is malformed");
  }

  return getNum(id.substr(firstDot + 1, lastDot - firstDot - 1));
}

//-----
uint64_t getEntityNum(std::string_view id)
{
  return getNum(getEntityNumStr(id));
}

//-----
std::string_view getEntityNumStr(std::string_view id)
{
  const size_t lastDot = id.find_last_of('.');
  const size_t dash = id.find_last_of('-');

  if (lastDot == std::string::npos)
  {
    throw std::invalid_argument("Input entity ID string is malformed");
  }

  // If there's no checksum (i.e. there's a dash), then just return the rest of the string from the last dot.
  return id.substr(lastDot + 1, (dash == std::string::npos) ? id.size() - lastDot - 1 : dash - lastDot - 1);
}

//-----
std::string_view getChecksum(std::string_view id)
{
  const size_t dash = id.find_last_of('-');
  return (dash == std::string::npos) ? std::string_view() : id.substr(dash + 1, id.size() - dash - 1);
}

//-----
std::string checksum(std::string_view address, const LedgerId& ledgerId)
{
  const unsigned int weight = 31U;
  const unsigned int p3 = 26 * 26 * 26;           // The amount of permutations in a three-digit, base 26 string.
  const unsigned int p5 = 26 * 26 * 26 * 26 * 26; // The amount of permutations in a five-digit, base 26 string.

  // Step 1: Construct an array of the digits in the address, using 10 to represent '.'
  std::vector<unsigned int> digitArray;
  digitArray.reserve(address.size());
  std::for_each(
    address.cbegin(), address.cend(), [&digitArray](char c) { digitArray.push_back(c == '.' ? 10U : c - '0'); });

  // Step 2: Construct a byte array containing the ledger ID, followed by six zero bytes.
  const std::vector<std::byte> ledgerIdArray = Utilities::concatenateVectors({
    ledgerId.toBytes(),
    {std::byte(0x0), std::byte(0x0), std::byte(0x0), std::byte(0x0), std::byte(0x0), std::byte(0x0)}
  });

  // Step 3: Get the weighted sum of all the digits, modded by p3. Also get the sums of the digits in the array of
  //         digits, separating by even and odd indices. Finishing by modding 11.
  unsigned int evenIndexSum = 0U;
  unsigned int oddIndexSum = 0U;
  unsigned int digitArrayWeightedSum = 0U;
  for (unsigned int i = 0U; i < digitArray.size(); ++i)
  {
    digitArrayWeightedSum = (digitArrayWeightedSum * weight + digitArray.at(i)) % p3;
    if (i % 2U == 0U)
    {
      evenIndexSum += digitArray.at(i);
    }
    else
    {
      oddIndexSum += digitArray.at(i);
    }
  }

  evenIndexSum %= 11U;
  oddIndexSum %= 11U;

  // Step 4: Get the weighted sum of all bytes in the ledger ID bytes, modded by p5.
  unsigned int ledgerIdWeightedSum = 0U;
  std::for_each(ledgerIdArray.cbegin(),
                ledgerIdArray.cend(),
                [&ledgerIdWeightedSum, &weight](std::byte byte)
                { ledgerIdWeightedSum = ledgerIdWeightedSum * weight + std::to_integer<unsigned int>(byte); });
  ledgerIdWeightedSum %= p5;

  // Step 5: Calculate the checksum, as a number.
  unsigned int checksum = ((((((address.size() % 5U) * 11U + evenIndexSum) * 11U + oddIndexSum) * p3 +
                             digitArrayWeightedSum + ledgerIdWeightedSum) %
                            p5) *
                           1'000'003U) %
                          p5;

  // Step 6: Write the checksum in base 26, using a-z as the digits. There should only be five characters.
  std::string checksumStr;
  for (int i = 0; i < 5; ++i)
  {
    checksumStr.push_back(static_cast<char>('a' + checksum % 26U));
    checksum /= 26U;
  }

  // Step 7: Reverse the checksum string since it was constructed from the least significant digit.
  std::reverse(checksumStr.begin(), checksumStr.end());

  return checksumStr;
}

//-----
bool isLongZeroAddress(const std::vector<std::byte>& address)
{
  // The address is a possible long-zero address if the first 12 bytes are zeros.
  for (int i = 0; i < 12; ++i)
  {
    if (address.at(i) != std::byte(0x0))
    {
      return false;
    }
  }

  return true;
}

//-----
std::vector<std::byte> decodeSolidityAddress(std::string_view address)
{
  // Make sure the size of the address is correct.
  const std::string_view prefix = "0x";
  const bool hasPrefix = address.substr(0, prefix.size()) == prefix;
  if ((hasPrefix && address.size() != SOLIDITY_ADDRESS_LEN_HEX + prefix.size()) ||
      (!hasPrefix && address.size() != SOLIDITY_ADDRESS_LEN_HEX))
  {
    throw std::invalid_argument("Input Solidity address is not the correct size, it must be " +
                                std::to_string(SOLIDITY_ADDRESS_LEN_HEX) + " hex characters");
  }

  // Remove the prefix if it exists.
  if (hasPrefix)
  {
    address.remove_prefix(prefix.size());
  }

  try
  {
    return internal::HexConverter::hexToBytes(address);
  }
  catch (const OpenSSLException&)
  {
    throw std::invalid_argument(std::string("Input Solidity address ") + address.data() + " is not valid hex");
  }
}

//-----
std::string toString(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum)
{
  std::string str = std::to_string(shard) + '.' + std::to_string(realm) + '.' + std::to_string(num);
  if (!checksum.empty())
  {
    str += '-';
    str += checksum;
  }

  return str;
}

//-----
std::string toSolidityAddress(uint64_t shard, uint64_t realm, uint64_t num)
{
  // If the shard number is a value greater than 32 bits can represent, then creating a Long-Zero format is impossible.
  if (shard > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max()))
  {
    throw std::invalid_argument("Shard number is too big. Its value must be able to fit in 32 bits.");
  }

  return internal::HexConverter::bytesToHex(internal::Utilities::concatenateVectors(
    { internal::Utilities::removePrefix(internal::Utilities::getBytes(shard), sizeof(uint64_t) - 4),
      internal::Utilities::getBytes(realm),
      internal::Utilities::getBytes(num) }));
}

//-----
void validate(uint64_t shard, uint64_t realm, uint64_t num, const Client& client, std::string_view expectedChecksum)
{
  // Make sure the Client's network is initialized.
  if (!client.getClientNetwork())
  {
    throw UninitializedException("Client has no network with which to validate checksum");
  }

  // Calculate the checksum of the input entity.
  const std::string calculatedChecksum =
    checksum(std::to_string(shard) + '.' + std::to_string(realm) + '.' + std::to_string(num), client.getLedgerId());

  if (calculatedChecksum != expectedChecksum)
  {
    throw BadEntityIdException(shard, realm, num, expectedChecksum, calculatedChecksum);
  }
}

/**
 * Explicit template instantiations.
 */
// clang-format off
template AccountId          fromSolidityAddress<AccountId>          (const std::vector<std::byte>&);
template ContractId         fromSolidityAddress<ContractId>         (const std::vector<std::byte>&);
template DelegateContractId fromSolidityAddress<DelegateContractId> (const std::vector<std::byte>&);
template FileId             fromSolidityAddress<FileId>             (const std::vector<std::byte>&);
template ScheduleId         fromSolidityAddress<ScheduleId>         (const std::vector<std::byte>&);
template TokenId            fromSolidityAddress<TokenId>            (const std::vector<std::byte>&);
template TopicId            fromSolidityAddress<TopicId>            (const std::vector<std::byte>&);
// clang-format on

} // namespace Hiero::internal::EntityIdHelper
