// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "EthereumTransaction.h"
#include "FileId.h"
#include "Hbar.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <cstddef>
#include <ethereum_transaction.pb.h>
#include <gtest/gtest.h>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class EthereumTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::vector<std::byte>& getTestEthereumData() const { return mTestEthereumData; }
  [[nodiscard]] inline const FileId& getTestCallDataFileId() const { return mTestCallDataFileId; }
  [[nodiscard]] inline const Hbar& getTestMaxGasAllowance() const { return mTestMaxGasAllowance; }

private:
  const std::vector<std::byte> mTestEthereumData = { std::byte(0x01), std::byte(0x02), std::byte(0x03) };
  const FileId mTestCallDataFileId = FileId(4ULL);
  const Hbar mTestMaxGasAllowance = Hbar(5LL);
};

//-----
TEST_F(EthereumTransactionUnitTests, ConstructEthereumTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::EthereumTransactionBody>();
  body->set_ethereum_data(internal::Utilities::byteVectorToString(getTestEthereumData()));
  body->set_allocated_call_data(getTestCallDataFileId().toProtobuf().release());
  body->set_max_gas_allowance(getTestMaxGasAllowance().toTinybars());

  proto::TransactionBody txBody;
  txBody.set_allocated_ethereumtransaction(body.release());

  // When
  const EthereumTransaction ethereumTransaction(txBody);

  // Then
  EXPECT_EQ(ethereumTransaction.getEthereumData(), getTestEthereumData());
  ASSERT_TRUE(ethereumTransaction.getCallDataFileId().has_value());
  EXPECT_EQ(ethereumTransaction.getCallDataFileId(), getTestCallDataFileId());
  EXPECT_EQ(ethereumTransaction.getMaxGasAllowance(), getTestMaxGasAllowance());
}

//-----
TEST_F(EthereumTransactionUnitTests, GetSetEthereumData)
{
  // Given
  EthereumTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setEthereumData(getTestEthereumData()));

  // Then
  EXPECT_EQ(transaction.getEthereumData(), getTestEthereumData());
}

//-----
TEST_F(EthereumTransactionUnitTests, GetSetEthereumDataFrozen)
{
  // Given
  EthereumTransaction transaction = EthereumTransaction()
                                      .setNodeAccountIds({ AccountId(1ULL) })
                                      .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setEthereumData(getTestEthereumData()), IllegalStateException);
}

//-----
TEST_F(EthereumTransactionUnitTests, GetSetCallDataFileId)
{
  // Given
  EthereumTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setCallDataFileId(getTestCallDataFileId()));

  // Then
  EXPECT_EQ(transaction.getCallDataFileId(), getTestCallDataFileId());
}

//-----
TEST_F(EthereumTransactionUnitTests, GetSetCallDataFileIdFrozen)
{
  // Given
  EthereumTransaction transaction = EthereumTransaction()
                                      .setNodeAccountIds({ AccountId(1ULL) })
                                      .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setCallDataFileId(getTestCallDataFileId()), IllegalStateException);
}

//-----
TEST_F(EthereumTransactionUnitTests, GetSetMaxGasAllowance)
{
  // Given
  EthereumTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setMaxGasAllowance(getTestMaxGasAllowance()));

  // Then
  EXPECT_EQ(transaction.getMaxGasAllowance(), getTestMaxGasAllowance());
}

//-----
TEST_F(EthereumTransactionUnitTests, GetSetMaxGasAllowanceFrozen)
{
  // Given
  EthereumTransaction transaction = EthereumTransaction()
                                      .setNodeAccountIds({ AccountId(1ULL) })
                                      .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setMaxGasAllowance(getTestMaxGasAllowance()), IllegalStateException);
}
