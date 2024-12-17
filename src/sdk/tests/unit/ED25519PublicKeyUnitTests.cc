// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PublicKey.h"
#include "ED25519PrivateKey.h"
#include "ED25519PublicKey.h"
#include "exceptions/BadKeyException.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using namespace Hiero;
using namespace Hiero::internal::Utilities;

class ED25519PublicKeyUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] static std::string toLowercase(std::string_view str)
  {
    std::string lowercaseStr;
    std::transform(str.cbegin(), str.cend(), std::back_inserter(lowercaseStr), [](char c) { return tolower(c); });
    return lowercaseStr;
  }

  [[nodiscard]] inline const std::string& getTestPublicKeyHex() const { return mPublicKeyHexString; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestPublicKeyBytes() const { return mPublicKeyBytes; }
  [[nodiscard]] inline const std::unordered_map<std::string_view, std::string_view> getExpectedPublicKeyPairs() const
  {
    return expectedPublicKeyPairs;
  };

private:
  const std::string mPublicKeyHexString = "F83DEF42411E046461D5AEEAE9311C56F6612557F349F3412DBD95C9FE8B0265";
  const std::vector<std::byte> mPublicKeyBytes = { std::byte(0xF8), std::byte(0x3D), std::byte(0xEF), std::byte(0x42),
                                                   std::byte(0x41), std::byte(0x1E), std::byte(0x04), std::byte(0x64),
                                                   std::byte(0x61), std::byte(0xD5), std::byte(0xAE), std::byte(0xEA),
                                                   std::byte(0xE9), std::byte(0x31), std::byte(0x1C), std::byte(0x56),
                                                   std::byte(0xF6), std::byte(0x61), std::byte(0x25), std::byte(0x57),
                                                   std::byte(0xF3), std::byte(0x49), std::byte(0xF3), std::byte(0x41),
                                                   std::byte(0x2D), std::byte(0xBD), std::byte(0x95), std::byte(0xC9),
                                                   std::byte(0xFE), std::byte(0x8B), std::byte(0x02), std::byte(0x65) };
  const std::string_view openSSLCompatibleDERPublicKey =
    "302A300506032B65700321008CCD31B53D1835B467AAC795DAB19B274DD3B37E3DAF12FCEC6BC02BAC87B53D";
  const std::unordered_map<std::string_view, std::string_view> expectedPublicKeyPairs{
    {openSSLCompatibleDERPublicKey, "8CCD31B53D1835B467AAC795DAB19B274DD3B37E3DAF12FCEC6BC02BAC87B53D"},
  };
};

//-----
TEST_F(ED25519PublicKeyUnitTests, FromString)
{
  const std::string derEncodedPublicKeyHexString = ED25519PublicKey::DER_ENCODED_PREFIX_HEX + getTestPublicKeyHex();

  const std::shared_ptr<ED25519PublicKey> publicKeyFromString = ED25519PublicKey::fromString(getTestPublicKeyHex());

  ASSERT_NE(publicKeyFromString, nullptr);
  EXPECT_EQ(publicKeyFromString->toStringDer(), derEncodedPublicKeyHexString);
  EXPECT_EQ(publicKeyFromString->toStringRaw(), getTestPublicKeyHex());
  EXPECT_EQ(publicKeyFromString->toBytesDer(),
            concatenateVectors({ ED25519PublicKey::DER_ENCODED_PREFIX_BYTES, getTestPublicKeyBytes() }));
  EXPECT_EQ(publicKeyFromString->toBytesRaw(), getTestPublicKeyBytes());

  const std::shared_ptr<ED25519PublicKey> publicKeyFromStringDer =
    ED25519PublicKey::fromString(derEncodedPublicKeyHexString);

  ASSERT_NE(publicKeyFromStringDer, nullptr);
  EXPECT_EQ(publicKeyFromStringDer->toStringDer(), publicKeyFromString->toStringDer());
  EXPECT_EQ(publicKeyFromStringDer->toStringRaw(), publicKeyFromString->toStringRaw());
  EXPECT_EQ(publicKeyFromStringDer->toBytesDer(), publicKeyFromString->toBytesDer());
  EXPECT_EQ(publicKeyFromStringDer->toBytesRaw(), publicKeyFromString->toBytesRaw());

  // Throw if input garbage
  EXPECT_THROW(const std::shared_ptr<ED25519PublicKey> key = ED25519PublicKey::fromString("fdsakfdsalf"),
               BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ED25519PublicKey> key =
                 ED25519PublicKey::fromString(ED25519PublicKey::DER_ENCODED_PREFIX_HEX + "fjdskaf;"),
               BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ED25519PublicKey> key =
                 ED25519PublicKey::fromString(ED25519PublicKey::DER_ENCODED_PREFIX_HEX +
                                              "F83DEF42411E046461D5AEEAE9S11C56F661 557F349F3412DBD95C9FE8B026X"),
               BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ED25519PublicKey> key = ED25519PublicKey::fromString(
                 std::string(ED25519PublicKey::DER_ENCODED_PREFIX_HEX.size(), 'A') + getTestPublicKeyHex()),
               BadKeyException);
  EXPECT_NO_THROW(const std::shared_ptr<ED25519PublicKey> key =
                    ED25519PublicKey::fromString(toLowercase(getTestPublicKeyHex())));
}

//-----
TEST_F(ED25519PublicKeyUnitTests, FromBytes)
{
  const std::vector<std::byte> derEncodedPublicKeyBytes =
    concatenateVectors({ ED25519PublicKey::DER_ENCODED_PREFIX_BYTES, getTestPublicKeyBytes() });

  const std::shared_ptr<ED25519PublicKey> publicKeyFromBytes = ED25519PublicKey::fromBytes(getTestPublicKeyBytes());

  ASSERT_NE(publicKeyFromBytes, nullptr);
  EXPECT_EQ(publicKeyFromBytes->toStringDer(), ED25519PublicKey::DER_ENCODED_PREFIX_HEX + getTestPublicKeyHex());
  EXPECT_EQ(publicKeyFromBytes->toStringRaw(), getTestPublicKeyHex());
  EXPECT_EQ(publicKeyFromBytes->toBytesDer(), derEncodedPublicKeyBytes);
  EXPECT_EQ(publicKeyFromBytes->toBytesRaw(), getTestPublicKeyBytes());

  const std::shared_ptr<ED25519PublicKey> publicKeyFromBytesDer = ED25519PublicKey::fromBytes(derEncodedPublicKeyBytes);

  ASSERT_NE(publicKeyFromBytesDer, nullptr);
  EXPECT_EQ(publicKeyFromBytesDer->toStringDer(), publicKeyFromBytes->toStringDer());
  EXPECT_EQ(publicKeyFromBytesDer->toStringRaw(), publicKeyFromBytes->toStringRaw());
  EXPECT_EQ(publicKeyFromBytesDer->toBytesDer(), publicKeyFromBytes->toBytesDer());
  EXPECT_EQ(publicKeyFromBytesDer->toBytesRaw(), publicKeyFromBytes->toBytesRaw());

  // Throw if input garbage
  EXPECT_THROW(const std::shared_ptr<ED25519PublicKey> key = ED25519PublicKey::fromBytes({ std::byte(0x65),
                                                                                           std::byte(0x4D),
                                                                                           std::byte(0x58),
                                                                                           std::byte(0x13),
                                                                                           std::byte(0x47),
                                                                                           std::byte(0x21),
                                                                                           std::byte(0x04),
                                                                                           std::byte(0x76) }),
               BadKeyException);
  EXPECT_THROW(
    const std::shared_ptr<ED25519PublicKey> key = ED25519PublicKey::fromBytes(concatenateVectors({
      ED25519PublicKey::DER_ENCODED_PREFIX_BYTES,
      {std::byte(0x76), std::byte(0x47), std::byte(0x85), std::byte(0x47), std::byte(0x15), std::byte(0xD4)}
  })),
    BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ED25519PublicKey> key =
                 ED25519PublicKey::fromBytes(ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_BYTES),
               BadKeyException);
}

//-----
TEST_F(ED25519PublicKeyUnitTests, Clone)
{
  // Given
  const std::shared_ptr<ED25519PublicKey> publicKey = ED25519PublicKey::fromBytes(getTestPublicKeyBytes());

  // When
  const std::unique_ptr<Key> clonedPublicKey = publicKey->clone();

  // Then
  EXPECT_EQ(clonedPublicKey->toBytes(), publicKey->toBytes());
}

//-----
TEST_F(ED25519PublicKeyUnitTests, VerifySignatureAgainstModifiedBytes)
{
  // Given
  const std::unique_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::fromBytes(getTestPublicKeyBytes());
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();
  std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };

  // When
  const std::vector<std::byte> signature = privateKey->sign(bytesToSign);
  bytesToSign.push_back(std::byte(0x4));

  // Then
  EXPECT_FALSE(publicKey->verifySignature(signature, bytesToSign));

  // Signature verification is further tested in RFV8032 test vectors
}

//-----
TEST_F(ED25519PublicKeyUnitTests, VerifyArbitrarySignature)
{
  // Given
  const std::shared_ptr<ED25519PublicKey> publicKey = ED25519PublicKey::fromBytes(getTestPublicKeyBytes());
  const std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };
  const std::vector<std::byte> arbitrarySignature = { std::byte(0x1), std::byte(0x2), std::byte(0x3), std::byte(0x4) };

  // When / Then
  EXPECT_FALSE(publicKey->verifySignature(arbitrarySignature, bytesToSign));

  // Signature verification is further tested in RFV8032 test vectors
}

//-----
TEST_F(ED25519PublicKeyUnitTests, VerifyEmptySignature)
{
  // Given / When / Then
  EXPECT_FALSE(ED25519PublicKey::fromBytes(getTestPublicKeyBytes())
                 ->verifySignature({}, { std::byte(0x1), std::byte(0x2), std::byte(0x3) }));

  // Signature verification is further tested in RFV8032 test vectors
}

//-----
TEST_F(ED25519PublicKeyUnitTests, ToString)
{
  // Given
  const std::shared_ptr<ED25519PublicKey> publicKey = ED25519PublicKey::fromBytes(getTestPublicKeyBytes());

  // When
  const std::string stringDer = publicKey->toStringDer();
  const std::string stringRaw = publicKey->toStringRaw();

  // Then
  EXPECT_EQ(stringDer, ED25519PublicKey::DER_ENCODED_PREFIX_HEX + getTestPublicKeyHex());
  EXPECT_EQ(stringRaw, getTestPublicKeyHex());
}

//-----
TEST_F(ED25519PublicKeyUnitTests, ToBytes)
{
  // Given
  const std::shared_ptr<ED25519PublicKey> publicKey = ED25519PublicKey::fromString(getTestPublicKeyHex());

  // When
  const std::vector<std::byte> bytesDer = publicKey->toBytesDer();
  const std::vector<std::byte> bytesRaw = publicKey->toBytesRaw();

  // Then
  EXPECT_EQ(bytesDer, concatenateVectors({ ED25519PublicKey::DER_ENCODED_PREFIX_BYTES, getTestPublicKeyBytes() }));
  EXPECT_EQ(bytesRaw, getTestPublicKeyBytes());
}

//-----
TEST_F(ED25519PublicKeyUnitTests, PublicKeyToProtobuf)
{
  // Given
  const std::shared_ptr<ED25519PublicKey> publicKey = ED25519PublicKey::fromBytes(getTestPublicKeyBytes());

  // When
  const std::unique_ptr<proto::Key> protobufKey = publicKey->toProtobufKey();

  // Then
  ASSERT_NE(protobufKey, nullptr);
  EXPECT_TRUE(protobufKey->has_ed25519());

  const std::vector<std::byte> protobufEd25519PublicKeyBytes =
    internal::Utilities::stringToByteVector(protobufKey->ed25519());
  EXPECT_EQ(protobufEd25519PublicKeyBytes, getTestPublicKeyBytes());
}

//-----
TEST_F(ED25519PublicKeyUnitTests, PublicKeyFromProtobuf)
{
  // Given
  const std::unique_ptr<proto::Key> protobufKey = ED25519PublicKey::fromBytes(getTestPublicKeyBytes())->toProtobufKey();

  // When
  const std::shared_ptr<Key> publicKey = Key::fromProtobuf(*protobufKey);

  // Then
  ASSERT_NE(publicKey, nullptr);
  EXPECT_EQ(publicKey->toBytes(), getTestPublicKeyBytes());
}

//-----
TEST_F(ED25519PublicKeyUnitTests, ED25519Compatibility)
{
  // Given
  auto expectedKeys = getExpectedPublicKeyPairs();

  // When // Then
  for (auto pair : expectedKeys)
  {
    auto actualKey = pair.first;
    auto expectedKey = pair.second;

    auto actualResultKeyPair = ED25519PublicKey::fromString(actualKey);
    ASSERT_EQ(actualResultKeyPair->toStringRaw(), expectedKey);
  }
}
