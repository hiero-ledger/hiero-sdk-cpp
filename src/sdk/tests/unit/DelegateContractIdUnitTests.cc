// SPDX-License-Identifier: Apache-2.0
#include "DelegateContractId.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class DelegateContractIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const uint64_t& getTestShardNum() const { return mShardNum; }
  [[nodiscard]] inline const uint64_t& getTestRealmNum() const { return mRealmNum; }
  [[nodiscard]] inline const uint64_t& getTestContractNum() const { return mContractNum; }

private:
  const uint64_t mShardNum = 1ULL;
  const uint64_t mRealmNum = 2ULL;
  const uint64_t mContractNum = 3ULL;
};

//-----
TEST_F(DelegateContractIdUnitTests, ConstructWithContractNum)
{
  // Given / When
  const DelegateContractId delegateContractId(getTestContractNum());

  // Then
  EXPECT_EQ(delegateContractId.mShardNum, 0ULL);
  EXPECT_EQ(delegateContractId.mRealmNum, 0ULL);
  ASSERT_TRUE(delegateContractId.mContractNum.has_value());
  EXPECT_EQ(delegateContractId.mContractNum, getTestContractNum());
  EXPECT_FALSE(delegateContractId.mEvmAddress.has_value());
}

//-----
TEST_F(DelegateContractIdUnitTests, ConstructWithShardRealmContractNum)
{
  // Given / When
  const DelegateContractId delegateContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum());

  // Then
  EXPECT_EQ(delegateContractId.mShardNum, getTestShardNum());
  EXPECT_EQ(delegateContractId.mRealmNum, getTestRealmNum());
  ASSERT_TRUE(delegateContractId.mContractNum.has_value());
  EXPECT_EQ(delegateContractId.mContractNum, getTestContractNum());
  EXPECT_FALSE(delegateContractId.mEvmAddress.has_value());
}

//-----
TEST_F(DelegateContractIdUnitTests, CompareDelegateContractIds)
{
  // Given / When / Then
  EXPECT_TRUE(DelegateContractId() == DelegateContractId());
  EXPECT_TRUE(DelegateContractId(getTestContractNum()) == DelegateContractId(getTestContractNum()));
  EXPECT_TRUE(DelegateContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()) ==
              DelegateContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()));
  EXPECT_FALSE(DelegateContractId(getTestContractNum()) == DelegateContractId(getTestContractNum() - 1ULL));
  EXPECT_FALSE(DelegateContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()) ==
               DelegateContractId(getTestShardNum() - 1ULL, getTestRealmNum(), getTestContractNum()));
  EXPECT_FALSE(DelegateContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum()) ==
               DelegateContractId(getTestShardNum(), getTestRealmNum() - 1ULL, getTestContractNum()));
}

//-----
TEST_F(DelegateContractIdUnitTests, FromCorrectlyFormedString)
{
  // Given / When
  DelegateContractId delegateContractId;
  EXPECT_NO_THROW(delegateContractId = DelegateContractId::fromString(std::to_string(getTestShardNum()) + '.' +
                                                                      std::to_string(getTestRealmNum()) + '.' +
                                                                      std::to_string(getTestContractNum())));

  // Then
  EXPECT_EQ(delegateContractId.mShardNum, getTestShardNum());
  EXPECT_EQ(delegateContractId.mRealmNum, getTestRealmNum());
  ASSERT_TRUE(delegateContractId.mContractNum.has_value());
  EXPECT_EQ(delegateContractId.mContractNum, getTestContractNum());
}

//-----
TEST_F(DelegateContractIdUnitTests, FromMalformedString)
{
  // Given
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testContractNumStr = std::to_string(getTestContractNum());
  DelegateContractId delegateContractId;

  // When / Then
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + testRealmNumStr + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString('.' + testShardNumStr + testRealmNumStr + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + '.' + testRealmNumStr + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + testRealmNumStr + '.' + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + testRealmNumStr + testContractNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(".." + testShardNumStr + testRealmNumStr + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString('.' + testShardNumStr + testRealmNumStr + '.' + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString('.' + testShardNumStr + testRealmNumStr + testContractNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + ".." + testRealmNumStr + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + '.' + testRealmNumStr + testContractNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + testRealmNumStr + ".." + testContractNumStr),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId =
                 DelegateContractId::fromString(testShardNumStr + testRealmNumStr + '.' + testContractNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(delegateContractId = DelegateContractId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + '.' +
                                                                   testContractNumStr + '.'),
               std::invalid_argument);

  EXPECT_THROW(delegateContractId = DelegateContractId::fromString("abc"), std::invalid_argument);
  EXPECT_THROW(delegateContractId = DelegateContractId::fromString("o.o.e"), std::invalid_argument);
  EXPECT_THROW(delegateContractId = DelegateContractId::fromString("0.0.1!"), std::invalid_argument);
}

//-----
TEST_F(DelegateContractIdUnitTests, FromProtobuf)
{
  // Given
  proto::ContractID protoContractId;
  protoContractId.set_shardnum(static_cast<int64_t>(getTestShardNum()));
  protoContractId.set_realmnum(static_cast<int64_t>(getTestRealmNum()));
  protoContractId.set_contractnum(static_cast<int64_t>(getTestContractNum()));

  // When
  const DelegateContractId delegateContractId = DelegateContractId::fromProtobuf(protoContractId);

  // Then
  EXPECT_EQ(delegateContractId.mShardNum, getTestShardNum());
  EXPECT_EQ(delegateContractId.mRealmNum, getTestRealmNum());
  ASSERT_TRUE(delegateContractId.mContractNum.has_value());
  EXPECT_EQ(delegateContractId.mContractNum, getTestContractNum());
  EXPECT_FALSE(delegateContractId.mEvmAddress.has_value());
}

//-----
TEST_F(DelegateContractIdUnitTests, Clone)
{
  // Given
  const DelegateContractId delegateContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum());

  // When
  const std::unique_ptr<Key> clonedDelegateContractId = delegateContractId.clone();

  // Then
  EXPECT_EQ(clonedDelegateContractId->toBytes(), delegateContractId.toBytes());
}

//-----
TEST_F(DelegateContractIdUnitTests, ToProtobufKey)
{
  // Given
  const DelegateContractId delegateContractId(getTestShardNum(), getTestRealmNum(), getTestContractNum());

  // When
  const std::unique_ptr<proto::Key> protoDelegateContractId = delegateContractId.toProtobufKey();

  // Then
  ASSERT_TRUE(protoDelegateContractId->has_delegatable_contract_id());

  EXPECT_EQ(protoDelegateContractId->delegatable_contract_id().shardnum(), static_cast<int64_t>(getTestShardNum()));
  EXPECT_EQ(protoDelegateContractId->delegatable_contract_id().realmnum(), static_cast<int64_t>(getTestRealmNum()));
  ASSERT_EQ(protoDelegateContractId->delegatable_contract_id().contract_case(),
            proto::ContractID::ContractCase::kContractNum);
  EXPECT_EQ(protoDelegateContractId->delegatable_contract_id().contractnum(),
            static_cast<int64_t>(getTestContractNum()));
}
