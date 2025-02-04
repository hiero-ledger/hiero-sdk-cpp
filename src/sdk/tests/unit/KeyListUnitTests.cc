// SPDX-License-Identifier: Apache-2.0
#include "ED25519PrivateKey.h"
#include "KeyList.h"
#include "PublicKey.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>
#include <memory>

using namespace Hiero;

class KeyListUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPublicKey1() const { return mTestPublicKey1; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPublicKey2() const { return mTestPublicKey2; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPublicKey3() const { return mTestPublicKey3; }
  [[nodiscard]] inline uint32_t getTestThreshold() const { return mTestThreshold; }

private:
  const std::shared_ptr<PublicKey> mTestPublicKey1 =
    ED25519PrivateKey::fromString(
      "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e10")
      ->getPublicKey();
  const std::shared_ptr<PublicKey> mTestPublicKey2 =
    ED25519PrivateKey::fromString(
      "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e11")
      ->getPublicKey();
  const std::shared_ptr<PublicKey> mTestPublicKey3 =
    ED25519PrivateKey::fromString(
      "302e020100300506032b657004220420db484b828e64b2d8f12ce3c0a0e93a0b8cce7af1bb8f39c97732394482538e12")
      ->getPublicKey();
  const uint32_t mTestThreshold = 2U;
};

//-----
TEST_F(KeyListUnitTests, FromProtobufKeyList)
{
  // Given
  proto::KeyList protoKeyList;
  protoKeyList.add_keys()->set_ed25519(internal::Utilities::byteVectorToString(getTestPublicKey1()->toBytesDer()));
  protoKeyList.add_keys()->set_ed25519(internal::Utilities::byteVectorToString(getTestPublicKey2()->toBytesDer()));
  protoKeyList.add_keys()->set_ed25519(internal::Utilities::byteVectorToString(getTestPublicKey3()->toBytesDer()));

  // When
  KeyList keyList;
  EXPECT_NO_THROW(keyList = KeyList::fromProtobuf(protoKeyList));

  // Then
  EXPECT_TRUE(keyList.contains(getTestPublicKey1().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey2().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey3().get()));
}

//-----
TEST_F(KeyListUnitTests, FromProtobufThresholdKey)
{
  // Given
  proto::ThresholdKey protoThresholdKey;
  protoThresholdKey.set_threshold(getTestThreshold());
  protoThresholdKey.mutable_keys()->add_keys()->set_ed25519(
    internal::Utilities::byteVectorToString(getTestPublicKey1()->toBytesDer()));
  protoThresholdKey.mutable_keys()->add_keys()->set_ed25519(
    internal::Utilities::byteVectorToString(getTestPublicKey2()->toBytesDer()));
  protoThresholdKey.mutable_keys()->add_keys()->set_ed25519(
    internal::Utilities::byteVectorToString(getTestPublicKey3()->toBytesDer()));

  // When
  KeyList keyList;
  EXPECT_NO_THROW(keyList = KeyList::fromProtobuf(protoThresholdKey));

  // Then
  EXPECT_EQ(keyList.getThreshold(), getTestThreshold());
  EXPECT_TRUE(keyList.contains(getTestPublicKey1().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey2().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey3().get()));
}

//-----
TEST_F(KeyListUnitTests, Of)
{
  // Given / When
  const KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });

  // Then
  EXPECT_TRUE(keyList.contains(getTestPublicKey1().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey2().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey3().get()));
}

//-----
TEST_F(KeyListUnitTests, ToProtobufKey)
{
  // Given
  const KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });

  // When
  std::unique_ptr<proto::Key> protoKey;
  EXPECT_NO_THROW(protoKey = keyList.toProtobufKey());

  // Then
  ASSERT_TRUE(protoKey->has_keylist());
  ASSERT_EQ(protoKey->keylist().keys_size(), 3);
  EXPECT_EQ(protoKey->keylist().keys(0).ed25519(),
            internal::Utilities::byteVectorToString(getTestPublicKey1()->toBytesRaw()));
  EXPECT_EQ(protoKey->keylist().keys(1).ed25519(),
            internal::Utilities::byteVectorToString(getTestPublicKey2()->toBytesRaw()));
  EXPECT_EQ(protoKey->keylist().keys(2).ed25519(),
            internal::Utilities::byteVectorToString(getTestPublicKey3()->toBytesRaw()));
}

//-----
TEST_F(KeyListUnitTests, ToProtobuf)
{
  // Given
  const KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });

  // When
  std::unique_ptr<proto::KeyList> protoKeyList;
  EXPECT_NO_THROW(protoKeyList = keyList.toProtobuf());

  // Then
  ASSERT_EQ(protoKeyList->keys_size(), 3);
  EXPECT_EQ(protoKeyList->keys(0).ed25519(),
            internal::Utilities::byteVectorToString(getTestPublicKey1()->toBytesRaw()));
  EXPECT_EQ(protoKeyList->keys(1).ed25519(),
            internal::Utilities::byteVectorToString(getTestPublicKey2()->toBytesRaw()));
  EXPECT_EQ(protoKeyList->keys(2).ed25519(),
            internal::Utilities::byteVectorToString(getTestPublicKey3()->toBytesRaw()));
}

//-----
TEST_F(KeyListUnitTests, Size)
{
  // Given / When
  const KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });
  const KeyList emptyKeyList;

  // Then
  EXPECT_EQ(keyList.size(), 3);
  EXPECT_EQ(emptyKeyList.size(), 0);
}

//-----
TEST_F(KeyListUnitTests, Empty)
{
  // Given / When
  const KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });
  const KeyList emptyKeyList;

  // Then
  EXPECT_FALSE(keyList.empty());
  EXPECT_TRUE(emptyKeyList.empty());
}

//-----
TEST_F(KeyListUnitTests, Contains)
{
  // Given / When
  const KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });
  const KeyList emptyKeyList;

  // Then
  EXPECT_TRUE(keyList.contains(getTestPublicKey1().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey2().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey3().get()));

  EXPECT_FALSE(emptyKeyList.contains(getTestPublicKey1().get()));
  EXPECT_FALSE(emptyKeyList.contains(getTestPublicKey2().get()));
  EXPECT_FALSE(emptyKeyList.contains(getTestPublicKey3().get()));
}

//-----
TEST_F(KeyListUnitTests, PushBack)
{
  // Given
  KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2() });

  // When
  keyList.push_back(getTestPublicKey3());

  // Then
  EXPECT_EQ(keyList.size(), 3);
  EXPECT_TRUE(keyList.contains(getTestPublicKey3().get()));
}

//-----
TEST_F(KeyListUnitTests, Remove)
{
  // Given
  KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });

  // When
  keyList.remove(getTestPublicKey1().get());

  // Then
  EXPECT_EQ(keyList.size(), 2);
  EXPECT_FALSE(keyList.contains(getTestPublicKey1().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey2().get()));
  EXPECT_TRUE(keyList.contains(getTestPublicKey3().get()));
}

//-----
TEST_F(KeyListUnitTests, Clear)
{
  // Given
  KeyList keyList = KeyList::of({ getTestPublicKey1(), getTestPublicKey2(), getTestPublicKey3() });

  // When
  keyList.clear();

  // Then
  EXPECT_TRUE(keyList.empty());
}
