// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "CustomFixedFee.h"
#include "CustomFractionalFee.h"
#include "CustomRoyaltyFee.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "PublicKey.h"
#include "TokenUpdateTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"

#include <chrono>
#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class TokenUpdateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }
  [[nodiscard]] inline const std::string& getTestTokenName() const { return mTestTokenName; }
  [[nodiscard]] inline const std::string& getTestTokenSymbol() const { return mTestTokenSymbol; }
  [[nodiscard]] inline const AccountId& getTestTreasuryAccountId() const { return mTestTreasuryAccountId; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestKycKey() const { return mTestKycKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestFreezeKey() const { return mTestFreezeKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestWipeKey() const { return mTestWipeKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestSupplyKey() const { return mTestSupplyKey; }
  [[nodiscard]] inline const AccountId& getTestAutoRenewAccountId() const { return mTestAutoRenewAccountId; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mTestAutoRenewPeriod;
  }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const std::string& getTestTokenMemo() const { return mTestTokenMemo; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestFeeScheduleKey() const { return mTestFeeScheduleKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPauseKey() const { return mTestPauseKey; }

private:
  const TokenId mTestTokenId = TokenId(1ULL, 2ULL, 3ULL);
  const std::string mTestTokenName = "test name";
  const std::string mTestTokenSymbol = "test symbol";
  const AccountId mTestTreasuryAccountId = AccountId(4ULL, 5ULL, 6ULL);
  const std::shared_ptr<PublicKey> mTestAdminKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestKycKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestFreezeKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestWipeKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestSupplyKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const AccountId mTestAutoRenewAccountId = AccountId(7ULL, 8ULL, 9ULL);
  const std::chrono::system_clock::duration mTestAutoRenewPeriod = std::chrono::hours(10);
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const std::string mTestTokenMemo = "test memo";
  const std::shared_ptr<PublicKey> mTestFeeScheduleKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestPauseKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
};

//-----
TEST_F(TokenUpdateTransactionUnitTests, ConstructTokenUpdateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenUpdateTransactionBody>();
  body->set_allocated_token(getTestTokenId().toProtobuf().release());
  body->set_name(getTestTokenName());
  body->set_symbol(getTestTokenSymbol());
  body->set_allocated_treasury(getTestTreasuryAccountId().toProtobuf().release());
  body->set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  body->set_allocated_kyckey(getTestKycKey()->toProtobufKey().release());
  body->set_allocated_freezekey(getTestFreezeKey()->toProtobufKey().release());
  body->set_allocated_wipekey(getTestWipeKey()->toProtobufKey().release());
  body->set_allocated_supplykey(getTestSupplyKey()->toProtobufKey().release());
  body->set_allocated_autorenewaccount(getTestAutoRenewAccountId().toProtobuf().release());
  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  body->set_allocated_expiry(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->mutable_memo()->set_value(getTestTokenMemo());
  body->set_allocated_fee_schedule_key(getTestFeeScheduleKey()->toProtobufKey().release());
  body->set_allocated_pause_key(getTestPauseKey()->toProtobufKey().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_tokenupdate(body.release());

  // When
  const TokenUpdateTransaction tokenUpdateTransaction(txBody);

  // Then
  EXPECT_EQ(tokenUpdateTransaction.getTokenId(), getTestTokenId());
  EXPECT_EQ(tokenUpdateTransaction.getTokenName(), getTestTokenName());
  EXPECT_EQ(tokenUpdateTransaction.getTokenSymbol(), getTestTokenSymbol());
  EXPECT_EQ(tokenUpdateTransaction.getTreasuryAccountId(), getTestTreasuryAccountId());
  EXPECT_EQ(tokenUpdateTransaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(tokenUpdateTransaction.getKycKey()->toBytes(), getTestKycKey()->toBytes());
  EXPECT_EQ(tokenUpdateTransaction.getFreezeKey()->toBytes(), getTestFreezeKey()->toBytes());
  EXPECT_EQ(tokenUpdateTransaction.getWipeKey()->toBytes(), getTestWipeKey()->toBytes());
  EXPECT_EQ(tokenUpdateTransaction.getSupplyKey()->toBytes(), getTestSupplyKey()->toBytes());
  EXPECT_EQ(tokenUpdateTransaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
  EXPECT_EQ(tokenUpdateTransaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
  EXPECT_EQ(tokenUpdateTransaction.getExpirationTime(), getTestExpirationTime());
  EXPECT_EQ(tokenUpdateTransaction.getTokenMemo(), getTestTokenMemo());
  EXPECT_EQ(tokenUpdateTransaction.getFeeScheduleKey()->toBytes(), getTestFeeScheduleKey()->toBytes());
  EXPECT_EQ(tokenUpdateTransaction.getPauseKey()->toBytes(), getTestPauseKey()->toBytes());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, ConstructTokenUpdateTransactionFromWrongTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::CryptoDeleteTransactionBody>();
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodelete(body.release());

  // When / Then
  EXPECT_THROW(const TokenUpdateTransaction tokenUpdateTransaction(txBody), std::invalid_argument);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetTokenId)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenId(getTestTokenId()));

  // Then
  EXPECT_EQ(transaction.getTokenId(), getTestTokenId());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetTokenIdFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenId(getTestTokenId()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetName)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenName(getTestTokenName()));

  // Then
  EXPECT_EQ(transaction.getTokenName(), getTestTokenName());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetNameFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenName(getTestTokenName()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetSymbol)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenSymbol(getTestTokenSymbol()));

  // Then
  EXPECT_EQ(transaction.getTokenSymbol(), getTestTokenSymbol());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetSymbolFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenSymbol(getTestTokenSymbol()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetTreasuryAccountId)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTreasuryAccountId(getTestTreasuryAccountId()));

  // Then
  EXPECT_EQ(transaction.getTreasuryAccountId(), getTestTreasuryAccountId());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetTreasuryAccountIdFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTreasuryAccountId(getTestTreasuryAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetAdminKey)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAdminKey(getTestAdminKey()));

  // Then
  EXPECT_EQ(transaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetAdminKeyFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAdminKey(getTestAdminKey()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetKycKey)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setKycKey(getTestKycKey()));

  // Then
  EXPECT_EQ(transaction.getKycKey()->toBytes(), getTestKycKey()->toBytes());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetKycKeyFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setKycKey(getTestKycKey()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetFreezeKey)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFreezeKey(getTestFreezeKey()));

  // Then
  EXPECT_EQ(transaction.getFreezeKey()->toBytes(), getTestFreezeKey()->toBytes());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetFreezeKeyFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFreezeKey(getTestFreezeKey()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetWipeKey)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setWipeKey(getTestWipeKey()));

  // Then
  EXPECT_EQ(transaction.getWipeKey()->toBytes(), getTestWipeKey()->toBytes());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetWipeKeyFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setWipeKey(getTestWipeKey()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetSupplyKey)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setSupplyKey(getTestSupplyKey()));

  // Then
  EXPECT_EQ(transaction.getSupplyKey()->toBytes(), getTestSupplyKey()->toBytes());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetSupplyKeyFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setSupplyKey(getTestSupplyKey()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetAutoRenewAccountId)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetAutoRenewAccountIdFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetAutoRenewPeriod)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetAutoRenewPeriodFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetExpirationTime)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setExpirationTime(getTestExpirationTime()));

  // Then
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetExpirationTimeFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetMemo)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenMemo(getTestTokenMemo()));

  // Then
  EXPECT_EQ(transaction.getTokenMemo(), getTestTokenMemo());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetMemoFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenMemo(getTestTokenMemo()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetFeeScheduleKey)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFeeScheduleKey(getTestFeeScheduleKey()));

  // Then
  EXPECT_EQ(transaction.getFeeScheduleKey()->toBytes(), getTestFeeScheduleKey()->toBytes());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetFeeScheduleKeyFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFeeScheduleKey(getTestFeeScheduleKey()), IllegalStateException);
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetPauseKey)
{
  // Given
  TokenUpdateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setPauseKey(getTestPauseKey()));

  // Then
  EXPECT_EQ(transaction.getPauseKey(), getTestPauseKey());
}

//-----
TEST_F(TokenUpdateTransactionUnitTests, GetSetPauseKeyFrozen)
{
  // Given
  TokenUpdateTransaction transaction = TokenUpdateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setPauseKey(getTestPauseKey()), IllegalStateException);
}
