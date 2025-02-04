// SPDX-License-Identifier: Apache-2.0
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "SystemUndeleteTransaction.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class SystemUndeleteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const FileId& getTestFileId() const { return mTestFileId; }
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }

private:
  const FileId mTestFileId = FileId(1ULL, 2ULL, 3ULL);
  const ContractId mTestContractId = ContractId(4ULL, 5ULL, 6ULL);
};

//-----
TEST_F(SystemUndeleteTransactionUnitTests, ConstructSystemUndeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto bodyWithFileId = std::make_unique<proto::SystemUndeleteTransactionBody>();
  auto bodyWithContractId = std::make_unique<proto::SystemUndeleteTransactionBody>();

  bodyWithFileId->set_allocated_fileid(getTestFileId().toProtobuf().release());
  bodyWithContractId->set_allocated_contractid(getTestContractId().toProtobuf().release());

  proto::TransactionBody txBodyWithFileId;
  proto::TransactionBody txBodyWithContractId;

  txBodyWithFileId.set_allocated_systemundelete(bodyWithFileId.release());
  txBodyWithContractId.set_allocated_systemundelete(bodyWithContractId.release());

  // When
  const SystemUndeleteTransaction systemDeleteTransactionWithFileId(txBodyWithFileId);
  const SystemUndeleteTransaction systemDeleteTransactionWithContractId(txBodyWithContractId);

  // Then
  ASSERT_TRUE(systemDeleteTransactionWithFileId.getFileId().has_value());
  EXPECT_EQ(systemDeleteTransactionWithFileId.getFileId(), getTestFileId());
  EXPECT_FALSE(systemDeleteTransactionWithFileId.getContractId().has_value());

  EXPECT_FALSE(systemDeleteTransactionWithContractId.getFileId().has_value());
  ASSERT_TRUE(systemDeleteTransactionWithContractId.getContractId().has_value());
  EXPECT_EQ(systemDeleteTransactionWithContractId.getContractId(), getTestContractId());
}

//-----
TEST_F(SystemUndeleteTransactionUnitTests, GetSetFileId)
{
  // Given
  SystemUndeleteTransaction transaction;

  // When
  ASSERT_NO_THROW(transaction.setFileId(getTestFileId()));

  // Then
  ASSERT_TRUE(transaction.getFileId().has_value());
  EXPECT_EQ(transaction.getFileId(), getTestFileId());
}

//-----
TEST_F(SystemUndeleteTransactionUnitTests, GetSetFileIdFrozen)
{
  // Given
  SystemUndeleteTransaction transaction = SystemUndeleteTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFileId(getTestFileId()), IllegalStateException);
}

//-----
TEST_F(SystemUndeleteTransactionUnitTests, GetSetContractId)
{
  // Given
  SystemUndeleteTransaction transaction;

  // When
  ASSERT_NO_THROW(transaction.setContractId(getTestContractId()));

  // Then
  ASSERT_TRUE(transaction.getContractId().has_value());
  EXPECT_EQ(transaction.getContractId(), getTestContractId());
}

//-----
TEST_F(SystemUndeleteTransactionUnitTests, GetSetContractIdFrozen)
{
  // Given
  SystemUndeleteTransaction transaction = SystemUndeleteTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContractId(getTestContractId()), IllegalStateException);
}

//-----
TEST_F(SystemUndeleteTransactionUnitTests, ResetFileId)
{
  // Given
  SystemUndeleteTransaction transaction;
  ASSERT_NO_THROW(transaction.setFileId(getTestFileId()));

  // When
  EXPECT_NO_THROW(transaction.setContractId(getTestContractId()));

  // Then
  EXPECT_FALSE(transaction.getFileId().has_value());
  EXPECT_TRUE(transaction.getContractId().has_value());
}

//-----
TEST_F(SystemUndeleteTransactionUnitTests, ResetContractId)
{
  // Given
  SystemUndeleteTransaction transaction;
  ASSERT_NO_THROW(transaction.setContractId(getTestContractId()));

  // When
  EXPECT_NO_THROW(transaction.setFileId(getTestFileId()));

  // Then
  EXPECT_TRUE(transaction.getFileId().has_value());
  EXPECT_FALSE(transaction.getContractId().has_value());
}