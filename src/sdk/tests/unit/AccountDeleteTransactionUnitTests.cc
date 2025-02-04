// SPDX-License-Identifier: Apache-2.0
#include "AccountDeleteTransaction.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "exceptions/IllegalStateException.h"

#include <gtest/gtest.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class AccountDeleteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestDeleteAccountId() const { return mDeleteAccountId; }
  [[nodiscard]] inline const AccountId& getTestTransferAccountId() const { return mTransferAccountId; }

private:
  const AccountId mDeleteAccountId = AccountId(1ULL);
  const AccountId mTransferAccountId = AccountId(2ULL);
};

//-----
TEST_F(AccountDeleteTransactionUnitTests, ConstructAccountDeleteTransaction)
{
  // Given / When
  AccountDeleteTransaction transaction;

  // Then
  EXPECT_FALSE(transaction.getDeleteAccountId().has_value());
  EXPECT_FALSE(transaction.getTransferAccountId().has_value());
}

//-----
TEST_F(AccountDeleteTransactionUnitTests, ConstructAccountCreateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::CryptoDeleteTransactionBody>();
  body->set_allocated_deleteaccountid(getTestDeleteAccountId().toProtobuf().release());
  body->set_allocated_transferaccountid(getTestTransferAccountId().toProtobuf().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodelete(body.release());

  // When
  const AccountDeleteTransaction accountDeleteTransaction(txBody);

  // Then
  EXPECT_EQ(accountDeleteTransaction.getDeleteAccountId(), getTestDeleteAccountId());
  EXPECT_EQ(accountDeleteTransaction.getTransferAccountId(), getTestTransferAccountId());
}

//-----
TEST_F(AccountDeleteTransactionUnitTests, SetDeleteAccountId)
{
  // Given
  AccountDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setDeleteAccountId(getTestDeleteAccountId()));

  // Then
  EXPECT_EQ(transaction.getDeleteAccountId(), getTestDeleteAccountId());
}

//-----
TEST_F(AccountDeleteTransactionUnitTests, SetDeleteAccountIdFrozen)
{
  // Given
  AccountDeleteTransaction transaction = AccountDeleteTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setDeleteAccountId(getTestDeleteAccountId()), IllegalStateException);
}

//-----
TEST_F(AccountDeleteTransactionUnitTests, SetTransferAccountId)
{
  // Given
  AccountDeleteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTransferAccountId(getTestTransferAccountId()));

  // Then
  EXPECT_EQ(transaction.getTransferAccountId(), getTestTransferAccountId());
}

//-----
TEST_F(AccountDeleteTransactionUnitTests, SetTransferAccountIdFrozen)
{
  // Given
  AccountDeleteTransaction transaction = AccountDeleteTransaction()
                                           .setNodeAccountIds({ AccountId(1ULL) })
                                           .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTransferAccountId(getTestTransferAccountId()), IllegalStateException);
}
