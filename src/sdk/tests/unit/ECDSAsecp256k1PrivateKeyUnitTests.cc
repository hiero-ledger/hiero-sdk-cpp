// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PrivateKey.h"
#include "exceptions/BadKeyException.h"
#include "exceptions/UninitializedException.h"
#include "impl/Utilities.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace Hiero;
using namespace Hiero::internal::Utilities;

class ECDSAsecp256k1PrivateKeyUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] static std::string toLowercase(std::string_view str)
  {
    std::string lowercaseStr;
    std::transform(str.cbegin(), str.cend(), std::back_inserter(lowercaseStr), [](char c) { return tolower(c); });
    return lowercaseStr;
  }

  [[nodiscard]] inline const std::string& getTestPrivateKeyHexString() const { return mPrivateKeyHexString; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestPrivateKeyBytes() const { return mPrivateKeyBytes; }
  [[nodiscard]] inline const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>>
  getExpectedPrivateKeyPairs() const
  {
    return expectedPrivateKeyPairs;
  };

private:
  const std::string mPrivateKeyHexString = "E8F32E723DECF4051AEFAC8E2C93C9C5B214313817CDB01A1494B917C8436B35";
  const std::vector<std::byte> mPrivateKeyBytes = {
    std::byte(0xE8), std::byte(0xF3), std::byte(0x2E), std::byte(0x72), std::byte(0x3D), std::byte(0xEC),
    std::byte(0xF4), std::byte(0x05), std::byte(0x1A), std::byte(0xEF), std::byte(0xAC), std::byte(0x8E),
    std::byte(0x2C), std::byte(0x93), std::byte(0xC9), std::byte(0xC5), std::byte(0xB2), std::byte(0x14),
    std::byte(0x31), std::byte(0x38), std::byte(0x17), std::byte(0xCD), std::byte(0xB0), std::byte(0x1A),
    std::byte(0x14), std::byte(0x94), std::byte(0xB9), std::byte(0x17), std::byte(0xC8), std::byte(0x43),
    std::byte(0x6B), std::byte(0x35)
  };
  const std::string_view unencryptedUncompressedPEMPrivateKey =
    "-----BEGIN EC PRIVATE "
    "KEY-----MHQCAQEEIG8I+jKi+"
    "iGVa7ttbfnlnML5AdvPugbgBWnseYjrle6qoAcGBSuBBAAKoUQDQgAEqf5BmMeBzkU1Ra9UAbZJo3tytVOlb7erTc36LRLP20mOLU7+mFY+"
    "3CfefAZgBtPXRAmDtRvYGODswAalW85GKA==-----END EC PRIVATE KEY-----";
  const std::string_view unencryptedCompressedPEMPrivateKey =
    "-----BEGIN EC PRIVATE "
    "KEY-----MFQCAQEEIOHyhclwHbha3f281Kvd884rhBzltxGJxCZyaQCagH9joAcGBSuBBAAKoSQDIgACREr6gFZa4K7hBP+bA25VdgQ+0ABFgM+"
    "g5RYw/W6T1Og=-----END EC PRIVATE KEY-----";
  const std::string_view legacyDERPrivateKey =
    "3030020100300706052b8104000a042204208c2cdc9575fe67493443967d74958fd7808a3787fd3337e99cfeebbc7566b586";
  const std::string_view openSSLCompatibleCompressedDERPrivateKey =
    "30540201010420ac318ea8ff8d991ab2f16172b4738e74dc35a56681199cfb1c0cb2e7cb560ffda00706052b8104000aa124032200036843f5"
    "cb338bbb4cdb21b0da4ea739d910951d6e8a5f703d313efe31afe788f4";
  const std::string_view openSSLCompatibleUncompressedDERPrivateKey =
    "307402010104208927647ad12b29646a1d051da8453462937bb2c813c6815cac6c0b720526ffc6a00706052b8104000aa14403420004aaac1c"
    "3ac1bea0245b8e00ce1e2018f9eab61b6331fbef7266f2287750a6597795f855ddcad2377e22259d1fcb4e0f1d35e8f2056300c15070bcbfce"
    "3759cc9d";
  const std::string_view openSSLCompatibleDERPrivateKeyWithoutPublicKey =
    "302e0201010420a6170a6aa6389a5bd3a3a8f9375f57bd91aa7f7d8b8b46ce0b702e000a21a5fea00706052b8104000a";

  const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>> expectedPrivateKeyPairs{
    {unencryptedUncompressedPEMPrivateKey,
     { "6F08FA32A2FA21956BBB6D6DF9E59CC2F901DBCFBA06E00569EC7988EB95EEAA",
        "02A9FE4198C781CE453545AF5401B649A37B72B553A56FB7AB4DCDFA2D12CFDB49" }},
    { unencryptedCompressedPEMPrivateKey,
     { "E1F285C9701DB85ADDFDBCD4ABDDF3CE2B841CE5B71189C4267269009A807F63",
        "02444AFA80565AE0AEE104FF9B036E5576043ED0004580CFA0E51630FD6E93D4E8" }},
    { legacyDERPrivateKey,
     { "8C2CDC9575FE67493443967D74958FD7808A3787FD3337E99CFEEBBC7566B586",
        "028173079D2E996EF6B2D064FC82D5FC7094367211E28422BEC50A2F75C365F5FD" }},
    { openSSLCompatibleCompressedDERPrivateKey,
     { "AC318EA8FF8D991AB2F16172B4738E74DC35A56681199CFB1C0CB2E7CB560FFD",
        "036843F5CB338BBB4CDB21B0DA4EA739D910951D6E8A5F703D313EFE31AFE788F4" }},
    { openSSLCompatibleUncompressedDERPrivateKey,
     { "8927647AD12B29646A1D051DA8453462937BB2C813C6815CAC6C0B720526FFC6",
        "03AAAC1C3AC1BEA0245B8E00CE1E2018F9EAB61B6331FBEF7266F2287750A65977" }},
    { openSSLCompatibleDERPrivateKeyWithoutPublicKey,
     { "A6170A6AA6389A5BD3A3A8F9375F57BD91AA7F7D8B8B46CE0B702E000A21A5FE",
        "03B69A75A5DDB1C0747E995D47555019E5D8A28003AB5202BD92F534361FB4EC8A" }}
  };
};

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, GeneratePrivateKey)
{
  // Given / When
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKeyGenerated = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  // Then
  ASSERT_NE(privateKeyGenerated, nullptr);
  EXPECT_EQ(privateKeyGenerated->toStringRaw().size(), ECDSAsecp256k1PrivateKey::KEY_SIZE * 2);
  EXPECT_EQ(privateKeyGenerated->toStringDer().size(),
            ECDSAsecp256k1PrivateKey::KEY_SIZE * 2 + ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_HEX.size());
  EXPECT_EQ(privateKeyGenerated->toBytesRaw().size(), ECDSAsecp256k1PrivateKey::KEY_SIZE);
  EXPECT_EQ(privateKeyGenerated->toBytesDer().size(),
            ECDSAsecp256k1PrivateKey::KEY_SIZE + ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_BYTES.size());
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, FromString)
{
  const std::string derEncodedPrivateKeyHexString =
    ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_HEX + getTestPrivateKeyHexString();

  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKeyFromString =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());

  ASSERT_NE(privateKeyFromString, nullptr);
  EXPECT_EQ(privateKeyFromString->toStringDer(), derEncodedPrivateKeyHexString);
  EXPECT_EQ(privateKeyFromString->toStringRaw(), getTestPrivateKeyHexString());
  EXPECT_EQ(privateKeyFromString->toBytesDer(),
            concatenateVectors({ ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_BYTES, getTestPrivateKeyBytes() }));
  EXPECT_EQ(privateKeyFromString->toBytesRaw(), getTestPrivateKeyBytes());

  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKeyFromStringDer =
    ECDSAsecp256k1PrivateKey::fromString(derEncodedPrivateKeyHexString);

  ASSERT_NE(privateKeyFromStringDer, nullptr);
  EXPECT_EQ(privateKeyFromStringDer->toStringDer(), privateKeyFromString->toStringDer());
  EXPECT_EQ(privateKeyFromStringDer->toStringRaw(), privateKeyFromString->toStringRaw());
  EXPECT_EQ(privateKeyFromStringDer->toBytesDer(), privateKeyFromString->toBytesDer());
  EXPECT_EQ(privateKeyFromStringDer->toBytesRaw(), privateKeyFromString->toBytesRaw());

  // Throw if input garbage
  EXPECT_THROW(const std::unique_ptr<ECDSAsecp256k1PrivateKey> key =
                 ECDSAsecp256k1PrivateKey::fromString("fdsakfdsalf"),
               BadKeyException);
  EXPECT_THROW(const std::unique_ptr<ECDSAsecp256k1PrivateKey> key =
                 ECDSAsecp256k1PrivateKey::fromString(ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_HEX + "fjdskaf;"),
               BadKeyException);
  EXPECT_THROW(const std::unique_ptr<ECDSAsecp256k1PrivateKey> key = ECDSAsecp256k1PrivateKey::fromString(
                 ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_HEX +
                 "F83DEF42411E046461D5AEEAE9S11C56F661 557F349F3412DBD95C9FE8B026X"),
               BadKeyException);
  EXPECT_THROW(
    const std::unique_ptr<ECDSAsecp256k1PrivateKey> key = ECDSAsecp256k1PrivateKey::fromString(
      std::string(ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_HEX.size(), 'A') + getTestPrivateKeyHexString()),
    BadKeyException);
  EXPECT_NO_THROW(const std::unique_ptr<ECDSAsecp256k1PrivateKey> key =
                    ECDSAsecp256k1PrivateKey::fromString(toLowercase(getTestPrivateKeyHexString())));
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, FromBytes)
{
  const std::vector<std::byte> derEncodedPrivateKeyBytes =
    concatenateVectors({ ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_BYTES, getTestPrivateKeyBytes() });

  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKeyFromBytes =
    ECDSAsecp256k1PrivateKey::fromBytes(getTestPrivateKeyBytes());

  ASSERT_NE(privateKeyFromBytes, nullptr);
  EXPECT_EQ(privateKeyFromBytes->toStringDer(),
            ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_HEX + getTestPrivateKeyHexString());
  EXPECT_EQ(privateKeyFromBytes->toStringRaw(), getTestPrivateKeyHexString());
  EXPECT_EQ(privateKeyFromBytes->toBytesDer(), derEncodedPrivateKeyBytes);
  EXPECT_EQ(privateKeyFromBytes->toBytesRaw(), getTestPrivateKeyBytes());

  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKeyFromBytesDer =
    ECDSAsecp256k1PrivateKey::fromBytes(derEncodedPrivateKeyBytes);

  ASSERT_NE(privateKeyFromBytesDer, nullptr);
  EXPECT_EQ(privateKeyFromBytesDer->toStringDer(), privateKeyFromBytes->toStringDer());
  EXPECT_EQ(privateKeyFromBytesDer->toStringRaw(), privateKeyFromBytes->toStringRaw());
  EXPECT_EQ(privateKeyFromBytesDer->toBytesDer(), privateKeyFromBytes->toBytesDer());
  EXPECT_EQ(privateKeyFromBytesDer->toBytesRaw(), privateKeyFromBytes->toBytesRaw());
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, Clone)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::unique_ptr<Key> clonedPrivateKey = privateKey->clone();

  // Then
  ASSERT_NE(clonedPrivateKey, nullptr);
  EXPECT_EQ(clonedPrivateKey->toBytes(),
            internal::Utilities::concatenateVectors(
              { ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_BYTES, getTestPrivateKeyBytes() }));
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, Derive)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());

  // When / Then
  // The privateKey was not initialized with a chain code, so derivation shouldn't be supported.
  EXPECT_THROW(auto key = privateKey->derive(0), UninitializedException);

  // Derivation functionality is further tested in SLIP10 test vectors
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, Sign)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());
  const std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };

  // When
  const std::vector<std::byte> signature = privateKey->sign(bytesToSign);

  // Then
  // ECDSA signatures incorporate random elements, so equality can't be tested. Just make sure its size makes sense.
  EXPECT_LE(signature.size(), ECDSAsecp256k1PrivateKey::MAX_SIGNATURE_SIZE);
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, SignEmptyBytes)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::vector<std::byte> signature = privateKey->sign({});

  // Then
  EXPECT_LE(signature.size(), ECDSAsecp256k1PrivateKey::MAX_SIGNATURE_SIZE);
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, ToString)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::string stringDer = privateKey->toStringDer();
  const std::string stringRaw = privateKey->toStringRaw();

  // Then
  EXPECT_EQ(stringDer, ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_HEX + getTestPrivateKeyHexString());
  EXPECT_EQ(stringRaw, getTestPrivateKeyHexString());
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, ToBytes)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::vector<std::byte> bytesDer = privateKey->toBytesDer();
  const std::vector<std::byte> bytesRaw = privateKey->toBytesRaw();

  // Then
  EXPECT_EQ(bytesDer,
            concatenateVectors({ ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_BYTES, getTestPrivateKeyBytes() }));
  EXPECT_EQ(bytesRaw, getTestPrivateKeyBytes());
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, GetChainCode)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::vector<std::byte> chainCode = privateKey->getChainCode();

  // Then
  EXPECT_TRUE(chainCode.empty());

  // Chain code functionality is further tested in SLIP10 test vectors
}

//-----
TEST_F(ECDSAsecp256k1PrivateKeyUnitTests, ECDSACompatibility)
{
  // Given
  auto expectedKeys = getExpectedPrivateKeyPairs();

  // When // Then
  for (auto pair : expectedKeys)
  {
    auto actualKey = pair.first;

    auto expectedPrivateKey = pair.second.first;
    auto expectedPublicKey = pair.second.second;

    auto actualResultKeyPair = ECDSAsecp256k1PrivateKey::fromString(actualKey);
    ASSERT_EQ(actualResultKeyPair->toStringRaw(), expectedPrivateKey);
    ASSERT_EQ(actualResultKeyPair->getPublicKey()->toStringRaw(), expectedPublicKey);
  }
}
