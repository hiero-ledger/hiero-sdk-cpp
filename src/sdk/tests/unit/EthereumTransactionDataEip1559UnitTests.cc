// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransactionDataEip1559.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>

using namespace Hiero;

class EthereumTransactionDataEip1559UnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::string& getTestTransaction() const { return mTestTransaction; }

private:
  const std::string mTestTransaction = "02f87082012a022f2f83018000947e3a9eaf9bcc39e2ffa38eb30bf7a93feacbc181880de0b6b3a"
                                       "7640000831234568001a0df48f2efd10421811de2bfb125ab75b2d3c44139c4642837fb1fccce91"
                                       "1fd479a01aaf7ae92bee896651dfc9d99ae422a296bf5d9f1ca49b2d96d82b79eb112d66";
};

//-----
TEST_F(EthereumTransactionDataEip1559UnitTests, FromBytes)
{
  // Given
  EthereumTransactionDataEip1559 ethereumTransactionDataEip1559;

  // When
  EXPECT_NO_THROW(ethereumTransactionDataEip1559 = EthereumTransactionDataEip1559::fromBytes(
                    internal::HexConverter::hexToBytes(getTestTransaction())));

  // Then
  EXPECT_EQ(ethereumTransactionDataEip1559.mChainId, internal::HexConverter::hexToBytes("012a"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mNonce, internal::HexConverter::hexToBytes("02"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mMaxPriorityGas, internal::HexConverter::hexToBytes("2f"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mMaxGas, internal::HexConverter::hexToBytes("2f"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mGasLimit, internal::HexConverter::hexToBytes("018000"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mTo,
            internal::HexConverter::hexToBytes("7e3a9eaf9bcc39e2ffa38eb30bf7a93feacbc181"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mValue, internal::HexConverter::hexToBytes("0de0b6b3a7640000"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mCallData, internal::HexConverter::hexToBytes("123456"));
  EXPECT_TRUE(ethereumTransactionDataEip1559.mAccessList.empty());
  EXPECT_EQ(ethereumTransactionDataEip1559.mRecoveryId, internal::HexConverter::hexToBytes("01"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mR,
            internal::HexConverter::hexToBytes("df48f2efd10421811de2bfb125ab75b2d3c44139c4642837fb1fccce911fd479"));
  EXPECT_EQ(ethereumTransactionDataEip1559.mS,
            internal::HexConverter::hexToBytes("1aaf7ae92bee896651dfc9d99ae422a296bf5d9f1ca49b2d96d82b79eb112d66"));
}

//-----
TEST_F(EthereumTransactionDataEip1559UnitTests, ToBytes)
{
  // Given
  const std::vector<std::byte> transactionBytes = internal::HexConverter::hexToBytes(getTestTransaction());

  // When
  const std::vector<std::byte> dataBytes = EthereumTransactionDataEip1559::fromBytes(transactionBytes).toBytes();

  // Then
  EXPECT_EQ(dataBytes, transactionBytes);
}

//-----
TEST_F(EthereumTransactionDataEip1559UnitTests, ToString)
{
  // Given / When
  const EthereumTransactionDataEip1559 EthereumTransactionDataEip1559 =
    EthereumTransactionDataEip1559::fromBytes(internal::HexConverter::hexToBytes(getTestTransaction()));

  // Then
  EXPECT_EQ(EthereumTransactionDataEip1559.toString(),
            "mChainId: 012A\n"
            "mNonce: 02\n"
            "mMaxPriorityGas: 2F\n"
            "mMaxGas: 2F\n"
            "mGasLimit: 018000\n"
            "mTo: 7E3A9EAF9BCC39E2FFA38EB30BF7A93FEACBC181\n"
            "mValue: 0DE0B6B3A7640000\n"
            "mCallData: 123456\n"
            "mAccessList: \n"
            "mRecoveryId: 01\n"
            "mR: DF48F2EFD10421811DE2BFB125AB75B2D3C44139C4642837FB1FCCCE911FD479\n"
            "mS: 1AAF7AE92BEE896651DFC9D99AE422A296BF5D9F1CA49B2D96D82B79EB112D66");
}
