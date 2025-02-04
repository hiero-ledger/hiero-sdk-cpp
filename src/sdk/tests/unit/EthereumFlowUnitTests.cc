// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "EthereumFlow.h"
#include "FileId.h"
#include "Hbar.h"
#include "impl/HexConverter.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class EthereumFlowUnitTests : public ::testing::Test
{
protected:
  void SetUp() override
  {
    mClient.setOperator(AccountId(), ECDSAsecp256k1PrivateKey::generatePrivateKey());
    mTestEthereumData = internal::HexConverter::hexToBytes(
      "02f87082012a022f2f83018000947e3a9eaf9bcc39e2ffa38eb30bf7a93feacbc181880de0b6b3a"
      "7640000831234568001a0df48f2efd10421811de2bfb125ab75b2d3c44139c4642837fb1fccce91"
      "1fd479a01aaf7ae92bee896651dfc9d99ae422a296bf5d9f1ca49b2d96d82b79eb112d66");
  }

  [[nodiscard]] inline const std::vector<std::byte>& getTestEthereumData() const { return mTestEthereumData; }
  [[nodiscard]] inline const Hbar& getTestMaxGasAllowance() const { return mTestMaxGasAllowance; }

private:
  Client mClient;
  std::vector<std::byte> mTestEthereumData;
  const Hbar mTestMaxGasAllowance = Hbar(1LL);
};

//-----
TEST_F(EthereumFlowUnitTests, GetSetEthereumData)
{
  // Given
  EthereumFlow flow;

  // When
  EXPECT_NO_THROW(flow.setEthereumData(getTestEthereumData()));

  // Then
  EXPECT_EQ(flow.getEthereumData()->toBytes(), getTestEthereumData());
}

//-----
TEST_F(EthereumFlowUnitTests, GetSetMaxGasAllowance)
{
  // Given
  EthereumFlow flow;

  // When
  flow.setMaxGasAllowance(getTestMaxGasAllowance());

  // Then
  EXPECT_EQ(flow.getMaxGasAllowance(), getTestMaxGasAllowance());
}
