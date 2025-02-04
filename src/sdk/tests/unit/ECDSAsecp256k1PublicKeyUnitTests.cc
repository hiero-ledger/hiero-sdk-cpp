// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PrivateKey.h"
#include "ECDSAsecp256k1PublicKey.h"
#include "EvmAddress.h"
#include "exceptions/BadKeyException.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace Hiero;
using namespace Hiero::internal::Utilities;

class ECDSAsecp256k1PublicKeyUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] static std::string toLowercase(std::string_view str)
  {
    std::string lowercaseStr;
    std::transform(str.cbegin(), str.cend(), std::back_inserter(lowercaseStr), [](char c) { return tolower(c); });
    return lowercaseStr;
  }

  [[nodiscard]] inline const std::string& getTestUncompressedPublicKeyHex() const { return mUncompressedPublicKeyHex; }
  [[nodiscard]] inline const std::string& getTestCompressedPublicKeyHex() const { return mCompressedPublicKeyHex; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestUncompressedPublicKeyBytes() const
  {
    return mUncompressedPublicKeyBytes;
  }
  [[nodiscard]] inline const std::vector<std::byte>& getTestCompressedPublicKeyBytes() const
  {
    return mCompressedPublicKeyBytes;
  }
  [[nodiscard]] inline const std::unordered_map<std::string_view, std::string_view> getExpectedPublicKeyPairs() const
  {
    return expectedPublicKeyPairs;
  };

private:
  const std::string mUncompressedPublicKeyHex = "045B36E22D710E79646F1A86D633EB38343BFE9DF39185EC730B1E7DFA79EE92CFD8C9"
                                                "80B4FB4DC5493A0EE40A85543FFC49E3CDC65E0B8B8C8A8AB64A00D9B5BE";
  const std::string mCompressedPublicKeyHex = "025B36E22D710E79646F1A86D633EB38343BFE9DF39185EC730B1E7DFA79EE92CF";
  const std::vector<std::byte> mUncompressedPublicKeyBytes = {
    std::byte(0x04), std::byte(0x5B), std::byte(0x36), std::byte(0xE2), std::byte(0x2D), std::byte(0x71),
    std::byte(0x0E), std::byte(0x79), std::byte(0x64), std::byte(0x6F), std::byte(0x1A), std::byte(0x86),
    std::byte(0xD6), std::byte(0x33), std::byte(0xEB), std::byte(0x38), std::byte(0x34), std::byte(0x3B),
    std::byte(0xFE), std::byte(0x9D), std::byte(0xF3), std::byte(0x91), std::byte(0x85), std::byte(0xEC),
    std::byte(0x73), std::byte(0x0B), std::byte(0x1E), std::byte(0x7D), std::byte(0xFA), std::byte(0x79),
    std::byte(0xEE), std::byte(0x92), std::byte(0xCF), std::byte(0xD8), std::byte(0xC9), std::byte(0x80),
    std::byte(0xB4), std::byte(0xFB), std::byte(0x4D), std::byte(0xC5), std::byte(0x49), std::byte(0x3A),
    std::byte(0x0E), std::byte(0xE4), std::byte(0x0A), std::byte(0x85), std::byte(0x54), std::byte(0x3F),
    std::byte(0xFC), std::byte(0x49), std::byte(0xE3), std::byte(0xCD), std::byte(0xC6), std::byte(0x5E),
    std::byte(0x0B), std::byte(0x8B), std::byte(0x8C), std::byte(0x8A), std::byte(0x8A), std::byte(0xB6),
    std::byte(0x4A), std::byte(0x00), std::byte(0xD9), std::byte(0xB5), std::byte(0xBE)
  };
  const std::vector<std::byte> mCompressedPublicKeyBytes = {
    std::byte(0x02), std::byte(0x5B), std::byte(0x36), std::byte(0xE2), std::byte(0x2D), std::byte(0x71),
    std::byte(0x0E), std::byte(0x79), std::byte(0x64), std::byte(0x6F), std::byte(0x1A), std::byte(0x86),
    std::byte(0xD6), std::byte(0x33), std::byte(0xEB), std::byte(0x38), std::byte(0x34), std::byte(0x3B),
    std::byte(0xFE), std::byte(0x9D), std::byte(0xF3), std::byte(0x91), std::byte(0x85), std::byte(0xEC),
    std::byte(0x73), std::byte(0x0B), std::byte(0x1E), std::byte(0x7D), std::byte(0xFA), std::byte(0x79),
    std::byte(0xEE), std::byte(0x92), std::byte(0xCF)
  };
  const std::string_view legacyDERPublicKey =
    "302d300706052b8104000a032200028173079d2e996ef6b2d064fc82d5fc7094367211e28422bec50a2f75c365f5fd";
  const std::string_view openSSLCompatibleCompressedDERPublicKey =
    "3036301006072a8648ce3d020106052b8104000a032200036843f5cb338bbb4cdb21b0da4ea739d910951d6e8a5f703d313efe31afe788f4";
  const std::string_view openSSLCompatibleUncompressedDERPublicKey =
    "3056301006072a8648ce3d020106052b8104000a03420004aaac1c3ac1bea0245b8e00ce1e2018f9eab61b6331fbef7266f2287750a6597795"
    "f855ddcad2377e22259d1fcb4e0f1d35e8f2056300c15070bcbfce3759cc9d";

  const std::unordered_map<std::string_view, std::string_view> expectedPublicKeyPairs{
    {legacyDERPublicKey,                         "028173079D2E996EF6B2D064FC82D5FC7094367211E28422BEC50A2F75C365F5FD"},
    { openSSLCompatibleCompressedDERPublicKey,   "036843F5CB338BBB4CDB21B0DA4EA739D910951D6E8A5F703D313EFE31AFE788F4"},
    { openSSLCompatibleUncompressedDERPublicKey, "03AAAC1C3AC1BEA0245B8E00CE1E2018F9EAB61B6331FBEF7266F2287750A65977"},
  };
};

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, FromString)
{
  const std::string derEncodedUncompressedPublicKeyHexString =
    ECDSAsecp256k1PublicKey::DER_ENCODED_UNCOMPRESSED_PREFIX_HEX + getTestUncompressedPublicKeyHex();
  const std::string derEncodedCompressedPublicKeyHexString =
    ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_HEX + getTestCompressedPublicKeyHex();

  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromUncompressed =
    ECDSAsecp256k1PublicKey::fromString(getTestUncompressedPublicKeyHex());
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromCompressed =
    ECDSAsecp256k1PublicKey::fromString(getTestCompressedPublicKeyHex());

  ASSERT_NE(publicKeyFromUncompressed, nullptr);
  ASSERT_NE(publicKeyFromCompressed, nullptr);
  EXPECT_EQ(publicKeyFromUncompressed->toStringRaw(), publicKeyFromCompressed->toStringRaw());
  EXPECT_EQ(publicKeyFromUncompressed->toStringDer(), publicKeyFromCompressed->toStringDer());
  EXPECT_EQ(publicKeyFromUncompressed->toBytesRaw(), publicKeyFromCompressed->toBytesRaw());
  EXPECT_EQ(publicKeyFromUncompressed->toBytesDer(), publicKeyFromCompressed->toBytesDer());

  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromUncompressedDer =
    ECDSAsecp256k1PublicKey::fromString(derEncodedUncompressedPublicKeyHexString);
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromCompressedDer =
    ECDSAsecp256k1PublicKey::fromString(derEncodedCompressedPublicKeyHexString);

  ASSERT_NE(publicKeyFromUncompressedDer, nullptr);
  ASSERT_NE(publicKeyFromCompressedDer, nullptr);
  EXPECT_EQ(publicKeyFromUncompressedDer->toStringRaw(), publicKeyFromCompressedDer->toStringRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toStringDer(), publicKeyFromCompressedDer->toStringDer());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesRaw(), publicKeyFromCompressedDer->toBytesRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesDer(), publicKeyFromCompressedDer->toBytesDer());

  EXPECT_EQ(publicKeyFromUncompressedDer->toStringRaw(), publicKeyFromCompressed->toStringRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toStringDer(), publicKeyFromCompressed->toStringDer());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesRaw(), publicKeyFromCompressed->toBytesRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesDer(), publicKeyFromCompressed->toBytesDer());

  // Throw if input garbage
  EXPECT_THROW(const std::shared_ptr<ECDSAsecp256k1PublicKey> key = ECDSAsecp256k1PublicKey::fromString("fdsakfdsalf"),
               BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ECDSAsecp256k1PublicKey> key = ECDSAsecp256k1PublicKey::fromString(
                 ECDSAsecp256k1PublicKey::DER_ENCODED_UNCOMPRESSED_PREFIX_HEX + "fjdskaf;"),
               BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ECDSAsecp256k1PublicKey> key = ECDSAsecp256k1PublicKey::fromString(
                 ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_HEX +
                 "XXXB36E22D710E79646F1A86D633PB38343BFE9DF39185EC7G0B1E7DFA79EE9ABC"),
               BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ECDSAsecp256k1PublicKey> key = ECDSAsecp256k1PublicKey::fromString(
                 std::string(ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_HEX.size(), 'A') +
                 getTestCompressedPublicKeyHex()),
               BadKeyException);
  EXPECT_NO_THROW(const std::shared_ptr<ECDSAsecp256k1PublicKey> key =
                    ECDSAsecp256k1PublicKey::fromString(toLowercase(getTestCompressedPublicKeyHex())));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, FromBytes)
{
  const std::vector<std::byte> derEncodedUncompressedPublicKeyBytes = concatenateVectors(
    { ECDSAsecp256k1PublicKey::DER_ENCODED_UNCOMPRESSED_PREFIX_BYTES, getTestUncompressedPublicKeyBytes() });
  const std::vector<std::byte> derEncodedCompressedPublicKeyBytes = concatenateVectors(
    { ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_BYTES, getTestCompressedPublicKeyBytes() });

  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromUncompressed =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes());
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromCompressed =
    ECDSAsecp256k1PublicKey::fromBytes(getTestCompressedPublicKeyBytes());

  ASSERT_NE(publicKeyFromUncompressed, nullptr);
  ASSERT_NE(publicKeyFromCompressed, nullptr);
  EXPECT_EQ(publicKeyFromUncompressed->toStringRaw(), publicKeyFromCompressed->toStringRaw());
  EXPECT_EQ(publicKeyFromUncompressed->toStringDer(), publicKeyFromCompressed->toStringDer());
  EXPECT_EQ(publicKeyFromUncompressed->toBytesRaw(), publicKeyFromCompressed->toBytesRaw());
  EXPECT_EQ(publicKeyFromUncompressed->toBytesDer(), publicKeyFromCompressed->toBytesDer());

  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromUncompressedDer =
    ECDSAsecp256k1PublicKey::fromBytes(derEncodedUncompressedPublicKeyBytes);
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKeyFromCompressedDer =
    ECDSAsecp256k1PublicKey::fromBytes(derEncodedCompressedPublicKeyBytes);

  ASSERT_NE(publicKeyFromUncompressedDer, nullptr);
  ASSERT_NE(publicKeyFromCompressedDer, nullptr);
  EXPECT_EQ(publicKeyFromUncompressedDer->toStringRaw(), publicKeyFromCompressedDer->toStringRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toStringDer(), publicKeyFromCompressedDer->toStringDer());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesRaw(), publicKeyFromCompressedDer->toBytesRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesDer(), publicKeyFromCompressedDer->toBytesDer());

  EXPECT_EQ(publicKeyFromUncompressedDer->toStringRaw(), publicKeyFromCompressed->toStringRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toStringDer(), publicKeyFromCompressed->toStringDer());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesRaw(), publicKeyFromCompressed->toBytesRaw());
  EXPECT_EQ(publicKeyFromUncompressedDer->toBytesDer(), publicKeyFromCompressed->toBytesDer());

  // Throw if input garbage
  EXPECT_THROW(const std::shared_ptr<ECDSAsecp256k1PublicKey> key =
                 ECDSAsecp256k1PublicKey::fromBytes({ std::byte(0x65),
                                                      std::byte(0x4D),
                                                      std::byte(0x58),
                                                      std::byte(0x13),
                                                      std::byte(0x47),
                                                      std::byte(0x21),
                                                      std::byte(0x04),
                                                      std::byte(0x76) }),
               BadKeyException);
  EXPECT_THROW(
    const std::shared_ptr<ECDSAsecp256k1PublicKey> key = ECDSAsecp256k1PublicKey::fromBytes(concatenateVectors({
      ECDSAsecp256k1PublicKey::DER_ENCODED_UNCOMPRESSED_PREFIX_BYTES,
      {std::byte(0x76), std::byte(0x47), std::byte(0x85), std::byte(0x47), std::byte(0x15), std::byte(0xd4)}
  })),
    BadKeyException);
  EXPECT_THROW(const std::shared_ptr<ECDSAsecp256k1PublicKey> key =
                 ECDSAsecp256k1PublicKey::fromBytes(ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_BYTES),
               BadKeyException);
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, CompressBytes)
{
  // Given / When
  const std::vector<std::byte> compressedBytes =
    ECDSAsecp256k1PublicKey::compressBytes(getTestUncompressedPublicKeyBytes());

  // Then
  EXPECT_EQ(compressedBytes, getTestCompressedPublicKeyBytes());

  // Throw if not correct size
  EXPECT_THROW(const std::vector<std::byte> bytes = ECDSAsecp256k1PublicKey::compressBytes(
                 { std::byte(0x4), std::byte(0x3), std::byte(0x2), std::byte(0x1) }),
               std::invalid_argument);
  // Throw if not starting with 0x4
  EXPECT_THROW(const std::vector<std::byte> bytes = ECDSAsecp256k1PublicKey::compressBytes(
                 std::vector<std::byte>(ECDSAsecp256k1PublicKey::UNCOMPRESSED_KEY_SIZE, std::byte(0x3))),
               std::invalid_argument);
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, UncompressBytes)
{
  // Given / When
  const std::vector<std::byte> uncompressedBytes =
    ECDSAsecp256k1PublicKey::uncompressBytes(getTestCompressedPublicKeyBytes());

  // Then
  EXPECT_EQ(uncompressedBytes, getTestUncompressedPublicKeyBytes());

  // Throw if not correct size
  EXPECT_THROW(const std::vector<std::byte> bytes =
                 ECDSAsecp256k1PublicKey::uncompressBytes({ std::byte(0x3), std::byte(0x2), std::byte(0x1) }),
               std::invalid_argument);
  // Throw if not starting with 0x2 or 0x3
  EXPECT_THROW(const std::vector<std::byte> bytes = ECDSAsecp256k1PublicKey::uncompressBytes(
                 std::vector<std::byte>(ECDSAsecp256k1PublicKey::COMPRESSED_KEY_SIZE, std::byte(0x1))),
               std::invalid_argument);
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, Clone)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes());

  // When
  const std::unique_ptr<Key> clonedPublicKey = publicKey->clone();

  // Then
  EXPECT_EQ(clonedPublicKey->toBytes(), clonedPublicKey->toBytes());
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, VerifyValidSignature)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();
  const std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };

  // When
  const std::vector<std::byte> signature = privateKey->sign(bytesToSign);

  // Then
  EXPECT_TRUE(publicKey->verifySignature(signature, bytesToSign));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, VerifyValidSignatureOfEmptyMessage)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  // When
  const std::vector<std::byte> signature = privateKey->sign({});

  // Then
  EXPECT_TRUE(publicKey->verifySignature(signature, {}));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, VerifySignatureAgainstModifiedBytes)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();
  std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };

  // When
  const std::vector<std::byte> signature = privateKey->sign(bytesToSign);
  bytesToSign.push_back(std::byte(0x4));

  // Then
  EXPECT_FALSE(publicKey->verifySignature(signature, bytesToSign));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, VerifyArbitrarySignature)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes());
  const std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };
  const std::vector<std::byte> arbitrarySignature = { std::byte(0x1), std::byte(0x2), std::byte(0x3), std::byte(0x4) };

  // When / Then
  EXPECT_FALSE(publicKey->verifySignature(arbitrarySignature, bytesToSign));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, VerifyEmptySignature)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes());
  const std::vector<std::byte> bytesToSign = { std::byte(0x1), std::byte(0x2), std::byte(0x3) };

  // When / Then
  EXPECT_FALSE(publicKey->verifySignature({}, bytesToSign));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, VerifyEmptyMessage)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  // When
  const std::vector<std::byte> signature = privateKey->sign({ std::byte(0x1), std::byte(0x2), std::byte(0x3) });

  // Then
  EXPECT_FALSE(publicKey->verifySignature(signature, {}));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, ToString)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes());

  // When
  const std::string stringDer = publicKey->toStringDer();
  const std::string stringRaw = publicKey->toStringRaw();

  // Then
  EXPECT_EQ(stringDer, ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_HEX + getTestCompressedPublicKeyHex());
  EXPECT_EQ(stringRaw, getTestCompressedPublicKeyHex());
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, ToBytes)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes());

  // When
  const std::vector<std::byte> bytesDer = publicKey->toBytesDer();
  const std::vector<std::byte> bytesRaw = publicKey->toBytesRaw();

  // Then
  EXPECT_EQ(bytesDer,
            concatenateVectors(
              { ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_BYTES, getTestCompressedPublicKeyBytes() }));
  EXPECT_EQ(bytesRaw, getTestCompressedPublicKeyBytes());
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, PublicKeyToProtobuf)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PublicKey> publicKey =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes());

  // When
  const std::unique_ptr<proto::Key> protobufKey = publicKey->toProtobufKey();

  // Then
  ASSERT_NE(protobufKey, nullptr);
  EXPECT_TRUE(protobufKey->has_ecdsa_secp256k1());

  const std::vector<std::byte> protobufEcdsaSecp256K1PublicKeyBytes =
    internal::Utilities::stringToByteVector(protobufKey->ecdsa_secp256k1());
  EXPECT_EQ(protobufEcdsaSecp256K1PublicKeyBytes, getTestCompressedPublicKeyBytes());
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, PublicKeyFromProtobuf)
{
  // Given
  const std::unique_ptr<proto::Key> protobufKey =
    ECDSAsecp256k1PublicKey::fromBytes(getTestUncompressedPublicKeyBytes())->toProtobufKey();

  // When
  const std::shared_ptr<Key> publicKey = Key::fromProtobuf(*protobufKey);

  // Then
  ASSERT_NE(publicKey, nullptr);
  EXPECT_EQ(publicKey->toBytes(),
            concatenateVectors(
              { ECDSAsecp256k1PublicKey::DER_ENCODED_COMPRESSED_PREFIX_BYTES, getTestCompressedPublicKeyBytes() }));
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, ToEvmAddress)
{
  // Given
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromString("DEBAE3CA62AB3157110DBA79C8DE26540DC320EE9BE73A77D70BA175643A3500");
  const auto publicKey = std::dynamic_pointer_cast<ECDSAsecp256k1PublicKey>(privateKey->getPublicKey());

  // When
  const EvmAddress evmAddress = publicKey->toEvmAddress();

  // Then
  EXPECT_EQ(evmAddress.toString(), "D8EB8DB03C699FAA3F47ADCDCD2AE91773B10F8B");
}

//-----
TEST_F(ECDSAsecp256k1PublicKeyUnitTests, ECDSACompatibility)
{
  // Given
  auto expectedKeys = getExpectedPublicKeyPairs();

  // When // Then
  for (auto pair : expectedKeys)
  {
    auto actualKey = pair.first;
    auto expectedKey = pair.second;

    auto actualResultKeyPair = ECDSAsecp256k1PublicKey::fromString(actualKey);
    ASSERT_EQ(actualResultKeyPair->toStringRaw(), expectedKey);
  }
}
