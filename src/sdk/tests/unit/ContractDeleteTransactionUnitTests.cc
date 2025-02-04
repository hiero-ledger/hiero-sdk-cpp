// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ContractDeleteTransaction.h"
#include "ContractId.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "exceptions/IllegalStateException.h"

#include <contract_delete.pb.h>
#include <gtest/gtest.h>
#include <memory>
#include <transaction_body.pb.h>

using namespace Hiero;

class ContractDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const AccountId& getTestTransferAccountId() const { return mTestTransferAccountId; }
  [[nodiscard]] inline const ContractId& getTestTransferContractId() const { return mTestTransferContractId; }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const AccountId mTestTransferAccountId = AccountId(2ULL);
  const ContractId mTestTransferContractId = ContractId(3ULL);
};

//-----
TEST_F(ContractDeleteTransactionUnitTests, ConstructContractDeleteTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto bodyWithTransferAccountId = std::make_unique<proto::ContractDeleteTransactionBody>();
  auto bodyWithTransferContractId = std::make_unique<proto::ContractDeleteTransactionBody>();

  bodyWithTransferAccountId->set_allocated_contractid(getTestContractId().toProtobuf().release());
  bodyWithTransferContractId->set_allocated_contractid(getTestContractId().toProtobuf().release());

  bodyWithTransferAccountId->set_allocated_transferaccountid(getTestTransferAccountId().toProtobuf().release());
  bodyWithTransferContractId->set_allocated_transfercontractid(getTestTransferContractId().toProtobuf().release());

  proto::TransactionBody txBodyWithTransferAccountId;
  proto::TransactionBody txBodyWithTransferContractId;
  txBodyWithTransferAccountId.set_allocated_contractdeleteinstance(bodyWithTransferAccountId.release());
  txBodyWithTransferContractId.set_allocated_contractdeleteinstance(bodyWithTransferContractId.release());

  // When
  const ContractDeleteTransaction contractDeleteTransactionWithTransferAccountId(txBodyWithTransferAccountId);
  const ContractDeleteTransaction contractDeleteTransactionWithTransferContractId(txBodyWithTransferContractId);

  // Then
  EXPECT_EQ(contractDeleteTransactionWithTransferAccountId.getContractId(), getTestContractId());
  ASSERT_TRUE(contractDeleteTransactionWithTransferAccountId.getTransferAccountId().has_value());
  EXPECT_EQ(contractDeleteTransactionWithTransferAccountId.getTransferAccountId(), getTestTransferAccountId());
  EXPECT_FALSE(contractDeleteTransactionWithTransferAccountId.getTransferContractId().has_value());

  EXPECT_EQ(contractDeleteTransactionWithTransferContractId.getContractId(), getTestContractId());
  EXPECT_FALSE(contractDeleteTransactionWithTransferContractId.getTransferAccountId().has_value());
  ASSERT_TRUE(contractDeleteTransactionWithTransferContractId.getTransferContractId().has_value());
  EXPECT_EQ(contractDeleteTransactionWithTransferContractId.getTransferContractId(), getTestTransferContractId());
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, GetSetContractId)
{
  // Given
  ContractDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setContractId(getTestContractId()));

  // Then
  EXPECT_EQ(transaction.getContractId(), getTestContractId());
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, GetSetContractIdFrozen)
{
  // Given
  ContractDeleteTransaction transaction = ContractDeleteTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContractId(getTestContractId()), IllegalStateException);
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, GetSetTransferAccountId)
{
  // Given
  ContractDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTransferAccountId(getTestTransferAccountId()));

  // Then
  EXPECT_EQ(transaction.getTransferAccountId(), getTestTransferAccountId());
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, GetSetTransferAccountIdFrozen)
{
  // Given
  ContractDeleteTransaction transaction = ContractDeleteTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTransferAccountId(getTestTransferAccountId()), IllegalStateException);
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, GetSetTransferContractId)
{
  // Given
  ContractDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTransferContractId(getTestTransferContractId()));

  // Then
  EXPECT_EQ(transaction.getTransferContractId(), getTestTransferContractId());
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, GetSetTransferContractIdFrozen)
{
  // Given
  ContractDeleteTransaction transaction = ContractDeleteTransaction()
                                            .setNodeAccountIds({ AccountId(1ULL) })
                                            .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTransferContractId(getTestTransferContractId()), IllegalStateException);
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, ResetTransferAccountIdWhenSettingTransferContractId)
{
  // Given
  ContractDeleteTransaction transaction;
  ASSERT_NO_THROW(transaction.setTransferAccountId(getTestTransferAccountId()));

  // When
  EXPECT_NO_THROW(transaction.setTransferContractId(getTestTransferContractId()));

  // Then
  EXPECT_FALSE(transaction.getTransferAccountId().has_value());
}

//-----
TEST_F(ContractDeleteTransactionUnitTests, ResetTransferContractIdWhenSettingTransferAccountId)
{
  // Given
  ContractDeleteTransaction transaction;
  ASSERT_NO_THROW(transaction.setTransferContractId(getTestTransferContractId()));

  // When
  EXPECT_NO_THROW(transaction.setTransferAccountId(getTestTransferAccountId()));

  // Then
  EXPECT_FALSE(transaction.getTransferContractId().has_value());
}
