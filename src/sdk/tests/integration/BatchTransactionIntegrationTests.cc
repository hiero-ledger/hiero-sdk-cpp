// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "BaseIntegrationTest.h"
#include "BatchTransaction.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "WrappedTransaction.h"

#include <gtest/gtest.h>

using namespace Hiero;

class BatchTransactionIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(BatchTransactionIntegrationTests, CanExecuteBatchTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> accountKey;
  ASSERT_NO_THROW(accountKey = ED25519PrivateKey::generatePrivateKey());

  // When
  WrappedTransaction innerTransaction(AccountCreateTransaction()
                                        .setKeyWithoutAlias(accountKey)
                                        .setInitialBalance(Hbar(1))
                                        .batchify(getTestClient(), operatorKey));

  // Then
  TransactionResponse txResponse;
  ASSERT_NO_THROW(txResponse = BatchTransaction().addInnerTransaction(innerTransaction).execute(getTestClient()););
}