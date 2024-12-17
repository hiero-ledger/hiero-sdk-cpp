// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "ED25519PublicKey.h"
#include "exceptions/BadKeyException.h"
#include "exceptions/UninitializedException.h"
#include "impl/Utilities.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace Hiero;
using namespace Hiero::internal::Utilities;

class ED25519PrivateKeyUnitTests : public ::testing::Test
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
  const std::string mPrivateKeyHexString = "68FBA516472B387C9F33C3E667616D806E5B9CEFF23A766E5D9A3818C77871F1";
  const std::vector<std::byte> mPrivateKeyBytes = {
    std::byte(0x68), std::byte(0xFB), std::byte(0xA5), std::byte(0x16), std::byte(0x47), std::byte(0x2B),
    std::byte(0x38), std::byte(0x7C), std::byte(0x9F), std::byte(0x33), std::byte(0xC3), std::byte(0xE6),
    std::byte(0x67), std::byte(0x61), std::byte(0x6D), std::byte(0x80), std::byte(0x6E), std::byte(0x5B),
    std::byte(0x9C), std::byte(0xEF), std::byte(0xF2), std::byte(0x3A), std::byte(0x76), std::byte(0x6E),
    std::byte(0x5D), std::byte(0x9A), std::byte(0x38), std::byte(0x18), std::byte(0xC7), std::byte(0x78),
    std::byte(0x71), std::byte(0xF1)
  };
  const std::string_view openSSLCompatiblePrivateKey =
    "302e020100300506032b657004220420feb858a4a69600a5eef2d9c76f7fb84fc0b6627f29e0ab17e160f640c267d404";
  const std::string_view unencryptedPemPrivateKey =
    "-----BEGIN PRIVATE KEY-----MC4CAQAwBQYDK2VwBCIEIOgbjaHgEqF7PY0t2dUf2VU0u1MRoKii/fywDlze4lvl-----END PRIVATE "
    "KEY-----";

  const std::unordered_map<std::string_view, std::pair<std::string_view, std::string_view>> expectedPrivateKeyPairs{
    {openSSLCompatiblePrivateKey,
     { "FEB858A4A69600A5EEF2D9C76F7FB84FC0B6627F29E0AB17E160F640C267D404",
        "8CCD31B53D1835B467AAC795DAB19B274DD3B37E3DAF12FCEC6BC02BAC87B53D" }},
    { unencryptedPemPrivateKey,
     { "E81B8DA1E012A17B3D8D2DD9D51FD95534BB5311A0A8A2FDFCB00E5CDEE25BE5",
        "F7B9AA4A8E4EEE94E4277DFE757D8D7CDE027E7CD5349B7D8E6EE21C9B9395BE" }},
  };
};

//-----
TEST_F(ED25519PrivateKeyUnitTests, GeneratePrivateKey)
{
  // When
  const std::unique_ptr<ED25519PrivateKey> privateKeyGenerated = ED25519PrivateKey::generatePrivateKey();

  // Then
  ASSERT_NE(privateKeyGenerated, nullptr);
  EXPECT_EQ(privateKeyGenerated->toStringRaw().size(), ED25519PrivateKey::KEY_SIZE * 2);
  EXPECT_EQ(privateKeyGenerated->toStringDer().size(),
            ED25519PrivateKey::KEY_SIZE * 2 + ED25519PrivateKey::DER_ENCODED_PREFIX_HEX.size());
  EXPECT_EQ(privateKeyGenerated->toBytesRaw().size(), ED25519PrivateKey::KEY_SIZE);
  EXPECT_EQ(privateKeyGenerated->toBytesDer().size(),
            ED25519PrivateKey::KEY_SIZE + ED25519PrivateKey::DER_ENCODED_PREFIX_BYTES.size());
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, FromString)
{
  const std::string derEncodedPrivateKeyHexString =
    ED25519PrivateKey::DER_ENCODED_PREFIX_HEX + getTestPrivateKeyHexString();

  const std::unique_ptr<ED25519PrivateKey> privateKeyFromString =
    ED25519PrivateKey::fromString(getTestPrivateKeyHexString());

  ASSERT_NE(privateKeyFromString, nullptr);
  EXPECT_EQ(privateKeyFromString->toStringDer(), derEncodedPrivateKeyHexString);
  EXPECT_EQ(privateKeyFromString->toStringRaw(), getTestPrivateKeyHexString());
  EXPECT_EQ(privateKeyFromString->toBytesDer(),
            concatenateVectors({ ED25519PrivateKey::DER_ENCODED_PREFIX_BYTES, getTestPrivateKeyBytes() }));
  EXPECT_EQ(privateKeyFromString->toBytesRaw(), getTestPrivateKeyBytes());

  const std::unique_ptr<ED25519PrivateKey> privateKeyFromStringDer =
    ED25519PrivateKey::fromString(derEncodedPrivateKeyHexString);

  ASSERT_NE(privateKeyFromStringDer, nullptr);
  EXPECT_EQ(privateKeyFromStringDer->toStringDer(), privateKeyFromString->toStringDer());
  EXPECT_EQ(privateKeyFromStringDer->toStringRaw(), privateKeyFromString->toStringRaw());
  EXPECT_EQ(privateKeyFromStringDer->toBytesDer(), privateKeyFromString->toBytesDer());
  EXPECT_EQ(privateKeyFromStringDer->toBytesRaw(), privateKeyFromString->toBytesRaw());

  // Throw if input garbage
  EXPECT_THROW(const std::unique_ptr<ED25519PrivateKey> key = ED25519PrivateKey::fromString("fdsakfdsalf"),
               BadKeyException);
  EXPECT_THROW(const std::unique_ptr<ED25519PrivateKey> key =
                 ED25519PrivateKey::fromString(ED25519PrivateKey::DER_ENCODED_PREFIX_HEX + "fjdskaf;"),
               BadKeyException);
  EXPECT_THROW(const std::unique_ptr<ED25519PrivateKey> key =
                 ED25519PrivateKey::fromString(ED25519PrivateKey::DER_ENCODED_PREFIX_HEX +
                                               "F83DEF42411E046461D5AEEAE9S11C56F661 557F349F3412DBD95C9FE8B026X"),
               BadKeyException);
  EXPECT_THROW(const std::unique_ptr<ED25519PrivateKey> key = ED25519PrivateKey::fromString(
                 std::string(ED25519PrivateKey::DER_ENCODED_PREFIX_HEX.size(), 'A') + getTestPrivateKeyHexString()),
               BadKeyException);
  EXPECT_NO_THROW(const std::unique_ptr<ED25519PrivateKey> key =
                    ED25519PrivateKey::fromString(toLowercase(getTestPrivateKeyHexString())));
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, FromBytes)
{
  const std::vector<std::byte> derEncodedPrivateKeyBytes =
    concatenateVectors({ ED25519PrivateKey::DER_ENCODED_PREFIX_BYTES, getTestPrivateKeyBytes() });

  const std::unique_ptr<ED25519PrivateKey> privateKeyFromBytes = ED25519PrivateKey::fromBytes(getTestPrivateKeyBytes());

  ASSERT_NE(privateKeyFromBytes, nullptr);
  EXPECT_EQ(privateKeyFromBytes->toStringDer(),
            ED25519PrivateKey::DER_ENCODED_PREFIX_HEX + getTestPrivateKeyHexString());
  EXPECT_EQ(privateKeyFromBytes->toStringRaw(), getTestPrivateKeyHexString());
  EXPECT_EQ(privateKeyFromBytes->toBytesDer(), derEncodedPrivateKeyBytes);
  EXPECT_EQ(privateKeyFromBytes->toBytesRaw(), getTestPrivateKeyBytes());

  const std::unique_ptr<ED25519PrivateKey> privateKeyFromBytesDer =
    ED25519PrivateKey::fromBytes(derEncodedPrivateKeyBytes);

  ASSERT_NE(privateKeyFromBytesDer, nullptr);
  EXPECT_EQ(privateKeyFromBytesDer->toStringDer(), privateKeyFromBytes->toStringDer());
  EXPECT_EQ(privateKeyFromBytesDer->toStringRaw(), privateKeyFromBytes->toStringRaw());
  EXPECT_EQ(privateKeyFromBytesDer->toBytesDer(), privateKeyFromBytes->toBytesDer());
  EXPECT_EQ(privateKeyFromBytesDer->toBytesRaw(), privateKeyFromBytes->toBytesRaw());
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, Clone)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::unique_ptr<Key> clonedPrivateKey = privateKey->clone();

  // Then
  ASSERT_NE(clonedPrivateKey, nullptr);
  EXPECT_EQ(clonedPrivateKey->toBytes(), getTestPrivateKeyBytes());
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, Derive)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromString(getTestPrivateKeyHexString());

  // When / Then
  // The privateKey was not initialized with a chain code, so derivation shouldn't be supported.
  EXPECT_THROW(auto key = privateKey->derive(0), UninitializedException);

  // Derivation functionality is further tested in SLIP10 test vectors
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, Sign)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromString(getTestPrivateKeyHexString());
  const std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };

  // When / Then
  EXPECT_NO_THROW(const std::vector<std::byte> signature = privateKey->sign(bytesToSign));

  // Signature functionality is further tested in RFC8032 test vectors
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, SignEmptyBytes)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromString(getTestPrivateKeyHexString());

  // When / Then
  EXPECT_NO_THROW(const std::vector<std::byte> signature = privateKey->sign({}));

  // Signature functionality is further tested in RFC8032 test vectors
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, ToString)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::string stringDer = privateKey->toStringDer();
  const std::string stringRaw = privateKey->toStringRaw();

  // Then
  EXPECT_EQ(stringDer, ED25519PrivateKey::DER_ENCODED_PREFIX_HEX + getTestPrivateKeyHexString());
  EXPECT_EQ(stringRaw, getTestPrivateKeyHexString());
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, ToBytes)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::vector<std::byte> bytesDer = privateKey->toBytesDer();
  const std::vector<std::byte> bytesRaw = privateKey->toBytesRaw();

  // Then
  EXPECT_EQ(bytesDer, concatenateVectors({ ED25519PrivateKey::DER_ENCODED_PREFIX_BYTES, getTestPrivateKeyBytes() }));
  EXPECT_EQ(bytesRaw, getTestPrivateKeyBytes());
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, GetChainCode)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromString(getTestPrivateKeyHexString());

  // When
  const std::vector<std::byte> chainCode = privateKey->getChainCode();

  // Then
  EXPECT_TRUE(chainCode.empty());

  // Chain code functionality is further tested in SLIP10 test vectors
}

//-----
TEST_F(ED25519PrivateKeyUnitTests, ED25519Compatibility)
{
  // Given
  auto expectedKeys = getExpectedPrivateKeyPairs();

  // When // Then
  for (auto pair : expectedKeys)
  {
    auto actualKey = pair.first;

    auto expectedPrivateKey = pair.second.first;
    auto expectedPublicKey = pair.second.second;

    auto actualResultKeyPair = ED25519PrivateKey::fromString(actualKey);
    ASSERT_EQ(actualResultKeyPair->toStringRaw(), expectedPrivateKey);
    ASSERT_EQ(actualResultKeyPair->getPublicKey()->toStringRaw(), expectedPublicKey);
  }
}
