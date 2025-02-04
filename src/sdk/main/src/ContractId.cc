// SPDX-License-Identifier: Apache-2.0
#include "ContractId.h"
#include "Client.h"
#include "LedgerId.h"
#include "exceptions/IllegalStateException.h"
#include "impl/EntityIdHelper.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <limits>

namespace Hiero
{
//-----
ContractId::ContractId(uint64_t num)
  : mContractNum(num)
{
}

//-----
ContractId::ContractId(const EvmAddress& address)
  : mEvmAddress(address)
{
}

//-----
ContractId::ContractId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mContractNum(num)
  , mChecksum(checksum)
{
}

//-----
ContractId::ContractId(uint64_t shard, uint64_t realm, const EvmAddress& address)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mEvmAddress(address)
{
}

//-----
bool ContractId::operator==(const ContractId& other) const
{
  return (mShardNum == other.mShardNum) && (mRealmNum == other.mRealmNum) &&
         ((mContractNum && other.mContractNum && *mContractNum == *other.mContractNum) ||
          (mEvmAddress && other.mEvmAddress && mEvmAddress->toBytes() == other.mEvmAddress->toBytes()) ||
          (!mContractNum && !other.mContractNum && !mEvmAddress && !other.mEvmAddress));
}

//-----
ContractId ContractId::fromString(std::string_view id)
{
  // Get the shard and realm numbers.
  const uint64_t shard = internal::EntityIdHelper::getShardNum(id);
  const uint64_t realm = internal::EntityIdHelper::getRealmNum(id);

  // Determine what the entity ID number is. First try to see if it's just a contract number. Get the entity number
  // string before the try-block to verify the input ID isn't malformed.
  const std::string_view entityNum = internal::EntityIdHelper::getEntityNumStr(id);
  const std::string_view checksum = internal::EntityIdHelper::getChecksum(id);
  try
  {
    return ContractId(shard, realm, internal::EntityIdHelper::getNum(entityNum), checksum);
  }
  catch (const std::invalid_argument&)
  {
    // If the entity number isn't a contract number, it's an EvmAddress. An EvmAddress cannot have checksums, so verify
    // that first.
    if (!checksum.empty())
    {
      throw std::invalid_argument("Contract IDs with EVM addresses can't have checksums");
    }

    // Try the entity number as an EVM address.
    try
    {
      return ContractId(shard, realm, EvmAddress::fromString(entityNum));
    }
    catch (const std::invalid_argument&)
    {
      // If not an EVM address, the entity ID cannot be realized.
      throw std::invalid_argument(std::string("Contract number/EVM address cannot be realized from ") +
                                  entityNum.data());
    }
  }
}

//-----
ContractId ContractId::fromEvmAddress(std::string_view evmAddress, uint64_t shard, uint64_t realm)
{
  return fromEvmAddress(EvmAddress::fromString(evmAddress), shard, realm);
}

//-----
ContractId ContractId::fromEvmAddress(const EvmAddress& evmAddress, uint64_t shard, uint64_t realm)
{
  return ContractId(shard, realm, evmAddress);
}

//-----
ContractId ContractId::fromSolidityAddress(std::string_view address)
{
  const std::vector<std::byte> bytes = internal::EntityIdHelper::decodeSolidityAddress(address);
  if (internal::EntityIdHelper::isLongZeroAddress(bytes))
  {
    return internal::EntityIdHelper::fromSolidityAddress<ContractId>(bytes);
  }
  else
  {
    return fromEvmAddress(address);
  }
}

//-----
ContractId ContractId::fromProtobuf(const proto::ContractID& proto)
{
  ContractId contractId;
  contractId.mShardNum = static_cast<uint64_t>(proto.shardnum());
  contractId.mRealmNum = static_cast<uint64_t>(proto.realmnum());

  if (proto.contract_case() == proto::ContractID::ContractCase::kContractNum)
  {
    contractId.mContractNum = static_cast<uint64_t>(proto.contractnum());
  }
  else if (proto.contract_case() == proto::ContractID::ContractCase::kEvmAddress)
  {
    contractId.mEvmAddress = EvmAddress::fromBytes(internal::Utilities::stringToByteVector(proto.evm_address()));
  }

  return contractId;
}

//-----
ContractId ContractId::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ContractID proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<Key> ContractId::clone() const
{
  return std::make_unique<ContractId>(*this);
}

//-----
std::unique_ptr<proto::Key> ContractId::toProtobufKey() const
{
  auto proto = std::make_unique<proto::Key>();
  proto->set_allocated_contractid(toProtobuf().release());
  return proto;
}

//-----
std::vector<std::byte> ContractId::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
void ContractId::validateChecksum(const Client& client) const
{
  if (mContractNum.has_value() && !mChecksum.empty())
  {
    internal::EntityIdHelper::validate(mShardNum, mRealmNum, mContractNum.value(), client, mChecksum);
  }
}

//-----
std::unique_ptr<proto::ContractID> ContractId::toProtobuf() const
{
  auto proto = std::make_unique<proto::ContractID>();
  proto->set_shardnum(static_cast<int64_t>(mShardNum));
  proto->set_realmnum(static_cast<int64_t>(mRealmNum));

  if (mContractNum.has_value())
  {
    proto->set_contractnum(static_cast<int64_t>(mContractNum.value()));
  }
  else if (mEvmAddress.has_value())
  {
    proto->set_evm_address(internal::Utilities::byteVectorToString(mEvmAddress->toBytes()));
  }

  return proto;
}

//-----
std::string ContractId::toSolidityAddress() const
{
  if (mEvmAddress.has_value())
  {
    return internal::HexConverter::bytesToHex(mEvmAddress->toBytes());
  }
  else if (mContractNum.has_value())
  {
    return internal::EntityIdHelper::toSolidityAddress(mShardNum, mRealmNum, mContractNum.value());
  }
  else
  {
    throw IllegalStateException(
      "ContractId must contain a contract number of EVM address to generate a Solidity address");
  }
}

//-----
std::string ContractId::toString() const
{
  std::string str = std::to_string(mShardNum) + '.' + std::to_string(mRealmNum) + '.';

  if (mContractNum.has_value())
  {
    return str + std::to_string(mContractNum.value());
  }
  else if (mEvmAddress.has_value())
  {
    return str + mEvmAddress->toString();
  }
  else
  {
    // Uninitialized case
    return str + '0';
  }
}

//-----
std::string ContractId::toStringWithChecksum(const Client& client) const
{
  // Checksums are only valid for contracts using a contract number.
  if (!mContractNum.has_value())
  {
    throw IllegalStateException("Checksums can only be generated for ContractIds that contain a contract number");
  }

  if (mChecksum.empty())
  {
    mChecksum = internal::EntityIdHelper::checksum(
      internal::EntityIdHelper::toString(mShardNum, mRealmNum, mContractNum.value()), client.getLedgerId());
  }

  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mContractNum.value(), mChecksum);
}

} // namespace Hiero
