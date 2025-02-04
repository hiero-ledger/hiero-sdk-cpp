// SPDX-License-Identifier: Apache-2.0
#include <basic_types.pb.h>

#include "AccountId.h"
#include "Client.h"
#include "LedgerId.h"
#include "PublicKey.h"
#include "exceptions/BadKeyException.h"
#include "exceptions/IllegalStateException.h"
#include "exceptions/UninitializedException.h"

#include "impl/EntityIdHelper.h"
#include "impl/HttpClient.h"
#include "impl/MirrorNetwork.h"
#include "impl/Utilities.h"

#include <limits>
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;

namespace Hiero
{
//-----
AccountId::AccountId(uint64_t num)
  : mAccountNum(num)
{
}

//-----
AccountId::AccountId(const std::shared_ptr<PublicKey>& alias)
  : mPublicKeyAlias(alias)
{
}

//-----
AccountId::AccountId(const EvmAddress& address)
  : mEvmAddressAlias(address)
{
}

//-----
AccountId::AccountId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mAccountNum(num)
  , mChecksum(checksum)
{
}

//-----
AccountId::AccountId(uint64_t shard, uint64_t realm, const std::shared_ptr<PublicKey>& alias)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mPublicKeyAlias(alias)
{
}

//-----
AccountId::AccountId(uint64_t shard, uint64_t realm, const EvmAddress& address)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mEvmAddressAlias(address)
{
}

//-----
bool AccountId::operator==(const AccountId& other) const
{
  return (mShardNum == other.mShardNum) && (mRealmNum == other.mRealmNum) &&
         ((mAccountNum && other.mAccountNum && mAccountNum == other.mAccountNum) ||
          (mPublicKeyAlias && other.mPublicKeyAlias &&
           mPublicKeyAlias->toStringDer() == other.mPublicKeyAlias->toStringDer()) ||
          (mEvmAddressAlias && other.mEvmAddressAlias &&
           mEvmAddressAlias->toString() == other.mEvmAddressAlias->toString()) ||
          (!mAccountNum && !other.mAccountNum && !mPublicKeyAlias && !other.mPublicKeyAlias && !mEvmAddressAlias &&
           !other.mEvmAddressAlias));
}

//-----
AccountId AccountId::fromString(std::string_view id)
{
  // First try the input id as an EvmAddress. If not an EvmAddress, process as normal.
  try
  {
    return fromEvmAddress(id);
  }
  catch (const std::invalid_argument&)
  {
    // Get the shard and realm numbers.
    const uint64_t shard = internal::EntityIdHelper::getShardNum(id);
    const uint64_t realm = internal::EntityIdHelper::getRealmNum(id);

    // Determine what the entity ID number is. First try to see if it's just an account number. Get the entity number
    // string before the try-block to verify the input ID isn't malformed.
    const std::string_view entityNum = internal::EntityIdHelper::getEntityNumStr(id);
    const std::string_view checksum = internal::EntityIdHelper::getChecksum(id);
    try
    {
      return AccountId(shard, realm, internal::EntityIdHelper::getNum(entityNum), checksum);
    }
    catch (const std::invalid_argument&)
    {
      // If the entity number isn't an account number, it's an alias. Aliases cannot have checksums, so verify that
      // first.
      if (!checksum.empty())
      {
        throw std::invalid_argument("Account ID aliases can't have checksums");
      }

      // First try the alias as a PublicKey.
      try
      {
        return AccountId(shard, realm, PublicKey::fromStringDer(entityNum));
      }
      catch (const BadKeyException&)
      {
        // If not a PublicKey, it must be an EVM address.
        try
        {
          return AccountId(shard, realm, EvmAddress::fromString(entityNum));
        }
        catch (const std::invalid_argument&)
        {
          // If not an EVM address, the entity ID cannot be realized.
          throw std::invalid_argument(std::string("Account number/alias cannot be realized from ") + entityNum.data());
        }
      }
    }
  }
}

//-----
AccountId AccountId::fromEvmAddress(std::string_view evmAddress, uint64_t shard, uint64_t realm)
{
  return fromEvmAddress(EvmAddress::fromString(evmAddress), shard, realm);
}

//-----
AccountId AccountId::fromEvmAddress(const EvmAddress& evmAddress, uint64_t shard, uint64_t realm)
{
  return AccountId(shard, realm, evmAddress);
}

//-----
AccountId AccountId::fromSolidityAddress(std::string_view address)
{
  const std::vector<std::byte> bytes = internal::EntityIdHelper::decodeSolidityAddress(address);
  if (internal::EntityIdHelper::isLongZeroAddress(bytes))
  {
    return internal::EntityIdHelper::fromSolidityAddress<AccountId>(bytes);
  }
  else
  {
    return fromEvmAddress(address);
  }
}

//-----
AccountId AccountId::fromProtobuf(const proto::AccountID& proto)
{
  AccountId accountId;
  accountId.mShardNum = static_cast<uint64_t>(proto.shardnum());
  accountId.mRealmNum = static_cast<uint64_t>(proto.realmnum());

  switch (proto.account_case())
  {
    case proto::AccountID::kAccountNum:
    {
      accountId.mAccountNum = static_cast<uint64_t>(proto.accountnum());
      break;
    }
    case proto::AccountID::kAlias:
    {
      if (proto.alias().size() == EvmAddress::NUM_BYTES)
      {
        accountId.mEvmAddressAlias = EvmAddress::fromBytes(internal::Utilities::stringToByteVector(proto.alias()));
      }
      else
      {
        accountId.mPublicKeyAlias = PublicKey::fromAliasBytes(internal::Utilities::stringToByteVector(proto.alias()));
      }
      break;
    }
    default:
      break;
  }

  return accountId;
}

//-----
AccountId AccountId::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::AccountID proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
void AccountId::validateChecksum(const Client& client) const
{
  if (mAccountNum.has_value() && !mChecksum.empty())
  {
    internal::EntityIdHelper::validate(mShardNum, mRealmNum, mAccountNum.value(), client, mChecksum);
  }
}

//-----
std::unique_ptr<proto::AccountID> AccountId::toProtobuf() const
{
  auto proto = std::make_unique<proto::AccountID>();
  proto->set_shardnum(static_cast<int64_t>(mShardNum));
  proto->set_realmnum(static_cast<int64_t>(mRealmNum));

  if (mAccountNum)
  {
    proto->set_accountnum(static_cast<int64_t>(*mAccountNum));
  }
  else if (mPublicKeyAlias)
  {
    proto->set_alias(mPublicKeyAlias->toProtobufKey()->SerializeAsString());
  }
  else if (mEvmAddressAlias)
  {
    proto->set_alias(internal::Utilities::byteVectorToString(mEvmAddressAlias->toBytes()));
  }

  return proto;
}

//-----
std::string AccountId::toSolidityAddress() const
{
  if (mEvmAddressAlias.has_value())
  {
    return mEvmAddressAlias.value().toString();
  }
  else if (mAccountNum.has_value())
  {
    return internal::EntityIdHelper::toSolidityAddress(mShardNum, mRealmNum, mAccountNum.value());
  }
  else
  {
    throw IllegalStateException("AccountId must contain an account number to generate a Solidity address");
  }
}

//-----
AccountId& AccountId::populateAccountEvmAddress(const Client& client)
{
  if (!mAccountNum.has_value())
  {
    throw new IllegalStateException("member `mAccountNum` should not be empty");
  }

  std::vector<std::string> mirrorNetworks = client.getClientMirrorNetwork()->getNetwork();
  if (mirrorNetworks.empty())
  {
    throw new UninitializedException("mirrorNetworks vector not populated!");
  }

  // build url for Mirror Node
  std::string url = "https://" + mirrorNetworks.front() + "/api/v1/accounts/0.0." + std::to_string(mAccountNum.value());

  // fetch account data for this account from Mirror Node
  std::string response = internal::HttpClient::invokeREST(url, "GET");
  json responseData = json::parse(response);

  if (responseData["account"].empty() || responseData["evm_address"].empty())
  {
    throw new IllegalStateException("No such account in MirrorNetwork: " + responseData.dump());
  }

  std::string evmAddress = responseData["evm_address"].dump();
  // json dump returns strings in dquotes, so we need to trim first and last characters
  evmAddress = evmAddress.substr(1, evmAddress.length() - 2);

  mEvmAddressAlias = EvmAddress::fromString(evmAddress);

  return *this;
}

//-----
std::string AccountId::toString() const
{
  std::string str = std::to_string(mShardNum) + '.' + std::to_string(mRealmNum) + '.';

  if (mPublicKeyAlias)
  {
    return str + mPublicKeyAlias->toStringDer();
  }
  else if (mEvmAddressAlias.has_value())
  {
    return str + mEvmAddressAlias->toString();
  }
  else if (mAccountNum.has_value())
  {
    return str + std::to_string(*mAccountNum);
  }
  else
  {
    // Uninitialized case
    return str + '0';
  }
}

//-----
std::string AccountId::toStringWithChecksum(const Client& client) const
{
  // Checksums are only valid for accounts not using an alias.
  if (!mAccountNum.has_value())
  {
    throw IllegalStateException("Checksums can only be generated for AccountIds that have an account number");
  }

  if (mChecksum.empty())
  {
    mChecksum = internal::EntityIdHelper::checksum(
      internal::EntityIdHelper::toString(mShardNum, mRealmNum, mAccountNum.value()), client.getLedgerId());
  }

  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mAccountNum.value(), mChecksum);
}

//-----
std::vector<std::byte> AccountId::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

} // namespace Hiero
