// SPDX-License-Identifier: Apache-2.0
#include "PublicKey.h"
#include "AccountId.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PublicKey.h"
#include "exceptions/BadKeyException.h"
#include "impl/HexConverter.h"
#include "impl/PublicKeyImpl.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
PublicKey::~PublicKey() = default;

//-----
std::unique_ptr<PublicKey> PublicKey::fromStringDer(std::string_view key)
{
  try
  {
    return fromBytesDer(internal::HexConverter::hexToBytes(key));
  }
  catch (const std::exception&)
  {
    throw BadKeyException("Key type cannot be determined from input DER-encoded hex string");
  }
}

//-----
std::unique_ptr<PublicKey> PublicKey::fromBytes(const std::vector<std::byte>& bytes)
{
  if (bytes.size() == ED25519PublicKey::KEY_SIZE)
  {
    return ED25519PublicKey::fromBytes(bytes);
  }
  else if (bytes.size() == ECDSAsecp256k1PublicKey::COMPRESSED_KEY_SIZE ||
           bytes.size() == ECDSAsecp256k1PublicKey::UNCOMPRESSED_KEY_SIZE)
  {
    return ECDSAsecp256k1PublicKey::fromBytes(bytes);
  }
  else
  {
    return PublicKey::fromBytesDer(bytes);
  }
}

//-----
std::unique_ptr<PublicKey> PublicKey::fromBytesDer(const std::vector<std::byte>& bytes)
{
  if (internal::Utilities::isPrefixOf(bytes, ED25519PublicKey::DER_ENCODED_PREFIX_BYTES))
  {
    return ED25519PublicKey::fromBytes(bytes);
  }

  else if (internal::Utilities::isPrefixOf(bytes, ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_BYTES) ||
           internal::Utilities::isPrefixOf(bytes, ECDSAsecp256k1PublicKey::DER_ENCODED_UNCOMPRESSED_PREFIX_BYTES))
  {
    return ECDSAsecp256k1PublicKey::fromBytes(bytes);
  }

  throw BadKeyException("Key type cannot be determined from input DER-encoded byte array");
}

//-----
std::unique_ptr<PublicKey> PublicKey::fromAliasBytes(const std::vector<std::byte>& bytes)
{
  proto::Key protoKey;
  protoKey.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));

  if (std::unique_ptr<Key> key = Key::fromProtobuf(protoKey); auto* publicKey = dynamic_cast<PublicKey*>(key.get()))
  {
    key.release(); // NOLINT
    return std::unique_ptr<PublicKey>(publicKey);
  }
  else
  {
    return nullptr;
  }
}

//-----
AccountId PublicKey::toAccountId(uint64_t shard, uint64_t realm) const
{
  return AccountId(shard, realm, getShared());
}

//-----
std::ostream& operator<<(std::ostream& os, const PublicKey& key)
{
  os << key.toStringDer();
  return os;
}

//-----
PublicKey::PublicKey(const PublicKey& other)
  : mImpl(std::make_unique<PublicKeyImpl>(*other.mImpl))
{
}

//-----
PublicKey& PublicKey::operator=(const PublicKey& other)
{
  if (this != &other)
  {
    mImpl = std::make_unique<PublicKeyImpl>(*other.mImpl);
  }

  return *this;
}

//-----
PublicKey::PublicKey(PublicKey&& other) noexcept
  : mImpl(std::move(other.mImpl))
{
  // Leave the moved-from PublicKey in a valid state.
  other.mImpl = std::make_unique<PublicKeyImpl>();
}

//-----
PublicKey& PublicKey::operator=(PublicKey&& other) noexcept
{
  if (this != &other)
  {
    mImpl = std::move(other.mImpl);

    // Leave the moved-from PublicKey in a valid state.
    other.mImpl = std::make_unique<PublicKeyImpl>();
  }

  return *this;
}

//-----
PublicKey::PublicKey(internal::OpenSSLUtils::EVP_PKEY&& key)
  : mImpl(std::make_unique<PublicKeyImpl>())
{
  mImpl->mKey = std::move(key);
}

//-----
internal::OpenSSLUtils::EVP_PKEY PublicKey::getInternalKey() const
{
  return mImpl->mKey;
}

} // namespace Hiero
