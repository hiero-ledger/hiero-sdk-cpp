// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "CustomFixedFee.h"
#include "CustomFractionalFee.h"
#include "CustomRoyaltyFee.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "PublicKey.h"
#include "TokenCreateTransaction.h"
#include "TokenSupplyType.h"
#include "TokenType.h"
#include "exceptions/IllegalStateException.h"
#include "impl/DurationConverter.h"
#include "impl/TimestampConverter.h"

#include <chrono>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class TokenCreateTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const std::string& getTestTokenName() const { return mTestTokenName; }
  [[nodiscard]] inline const std::string& getTestTokenSymbol() const { return mTestTokenSymbol; }
  [[nodiscard]] inline uint32_t getTestDecimals() const { return mTestDecimals; }
  [[nodiscard]] inline const uint64_t& getTestInitialSupply() const { return mTestInitialSupply; }
  [[nodiscard]] inline const AccountId& getTestTreasuryAccountId() const { return mTestTreasuryAccountId; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestAdminKey() const { return mTestAdminKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestKycKey() const { return mTestKycKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestFreezeKey() const { return mTestFreezeKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestWipeKey() const { return mTestWipeKey; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestSupplyKey() const { return mTestSupplyKey; }
  [[nodiscard]] inline bool getTestFreezeDefault() const { return mTestFreezeDefault; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }
  [[nodiscard]] inline const AccountId& getTestAutoRenewAccountId() const { return mTestAutoRenewAccountId; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestAutoRenewPeriod() const
  {
    return mTestAutoRenewPeriod;
  }
  [[nodiscard]] inline const std::string& getTestTokenMemo() const { return mTestTokenMemo; }
  [[nodiscard]] inline TokenType getTestTokenType() const { return mTestTokenType; }
  [[nodiscard]] inline TokenSupplyType getTestTokenSupplyType() const { return mTestTokenSupplyType; }
  [[nodiscard]] inline const uint64_t& getTestMaxSupply() const { return mTestMaxSupply; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestFeeScheduleKey() const { return mTestFeeScheduleKey; }
  [[nodiscard]] inline const std::vector<std::shared_ptr<CustomFee>>& getTestCustomFees() const
  {
    return mTestCustomFees;
  }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestPauseKey() const { return mTestPauseKey; }

private:
  const std::string mTestTokenName = "test name";
  const std::string mTestTokenSymbol = "test symbol";
  const uint32_t mTestDecimals = 1U;
  const uint64_t mTestInitialSupply = 2ULL;
  const AccountId mTestTreasuryAccountId = AccountId(3ULL, 4ULL, 5ULL);
  const std::shared_ptr<PublicKey> mTestAdminKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestKycKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestFreezeKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestWipeKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mTestSupplyKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const bool mTestFreezeDefault = true;
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
  const AccountId mTestAutoRenewAccountId = AccountId(6ULL, 7ULL, 8ULL);
  const std::chrono::system_clock::duration mTestAutoRenewPeriod = std::chrono::hours(9);
  const std::string mTestTokenMemo = "test memo";
  const TokenType mTestTokenType = TokenType::NON_FUNGIBLE_UNIQUE;
  const TokenSupplyType mTestTokenSupplyType = TokenSupplyType::FINITE;
  const uint64_t mTestMaxSupply = 10ULL;
  const std::shared_ptr<PublicKey> mTestFeeScheduleKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const std::vector<std::shared_ptr<CustomFee>> mTestCustomFees = { std::make_shared<CustomFixedFee>(),
                                                                    std::make_shared<CustomFractionalFee>(),
                                                                    std::make_shared<CustomRoyaltyFee>() };
  const std::shared_ptr<PublicKey> mTestPauseKey = ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
};

//-----
TEST_F(TokenCreateTransactionUnitTests, ConstructTokenCreateTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::TokenCreateTransactionBody>();
  body->set_name(getTestTokenName());
  body->set_symbol(getTestTokenSymbol());
  body->set_decimals(getTestDecimals());
  body->set_initialsupply(getTestInitialSupply());
  body->set_allocated_treasury(getTestTreasuryAccountId().toProtobuf().release());
  body->set_allocated_adminkey(getTestAdminKey()->toProtobufKey().release());
  body->set_allocated_kyckey(getTestKycKey()->toProtobufKey().release());
  body->set_allocated_freezekey(getTestFreezeKey()->toProtobufKey().release());
  body->set_allocated_wipekey(getTestWipeKey()->toProtobufKey().release());
  body->set_allocated_supplykey(getTestSupplyKey()->toProtobufKey().release());
  body->set_freezedefault(getTestFreezeDefault());
  body->set_allocated_expiry(internal::TimestampConverter::toProtobuf(getTestExpirationTime()));
  body->set_allocated_autorenewaccount(getTestAutoRenewAccountId().toProtobuf().release());
  body->set_allocated_autorenewperiod(internal::DurationConverter::toProtobuf(getTestAutoRenewPeriod()));
  body->set_memo(getTestTokenMemo());
  body->set_tokentype(gTokenTypeToProtobufTokenType.at(getTestTokenType()));
  body->set_supplytype(gTokenSupplyTypeToProtobufTokenSupplyType.at(getTestTokenSupplyType()));
  body->set_maxsupply(static_cast<int64_t>(getTestMaxSupply()));
  body->set_allocated_fee_schedule_key(getTestFeeScheduleKey()->toProtobufKey().release());

  for (const auto& fee : getTestCustomFees())
  {
    *body->add_custom_fees() = *fee->toProtobuf();
  }

  body->set_allocated_pause_key(getTestPauseKey()->toProtobufKey().release());

  proto::TransactionBody txBody;
  txBody.set_allocated_tokencreation(body.release());

  // When
  const TokenCreateTransaction tokenCreateTransaction(txBody);

  // Then
  EXPECT_EQ(tokenCreateTransaction.getTokenName(), getTestTokenName());
  EXPECT_EQ(tokenCreateTransaction.getTokenSymbol(), getTestTokenSymbol());
  EXPECT_EQ(tokenCreateTransaction.getDecimals(), getTestDecimals());
  EXPECT_EQ(tokenCreateTransaction.getInitialSupply(), getTestInitialSupply());
  EXPECT_EQ(tokenCreateTransaction.getTreasuryAccountId(), getTestTreasuryAccountId());
  EXPECT_EQ(tokenCreateTransaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
  EXPECT_EQ(tokenCreateTransaction.getKycKey()->toBytes(), getTestKycKey()->toBytes());
  EXPECT_EQ(tokenCreateTransaction.getFreezeKey()->toBytes(), getTestFreezeKey()->toBytes());
  EXPECT_EQ(tokenCreateTransaction.getWipeKey()->toBytes(), getTestWipeKey()->toBytes());
  EXPECT_EQ(tokenCreateTransaction.getSupplyKey()->toBytes(), getTestSupplyKey()->toBytes());
  EXPECT_EQ(tokenCreateTransaction.getFreezeDefault(), getTestFreezeDefault());
  EXPECT_EQ(tokenCreateTransaction.getExpirationTime(), getTestExpirationTime());
  EXPECT_EQ(tokenCreateTransaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
  EXPECT_EQ(tokenCreateTransaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
  EXPECT_EQ(tokenCreateTransaction.getTokenMemo(), getTestTokenMemo());
  EXPECT_EQ(tokenCreateTransaction.getTokenType(), getTestTokenType());
  EXPECT_EQ(tokenCreateTransaction.getSupplyType(), getTestTokenSupplyType());
  EXPECT_EQ(tokenCreateTransaction.getMaxSupply(), getTestMaxSupply());
  EXPECT_EQ(tokenCreateTransaction.getFeeScheduleKey()->toBytes(), getTestFeeScheduleKey()->toBytes());
  EXPECT_EQ(tokenCreateTransaction.getCustomFees().size(), 3);
  EXPECT_EQ(tokenCreateTransaction.getPauseKey()->toBytes(), getTestPauseKey()->toBytes());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, ConstructTokenCreateTransactionFromWrongTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::CryptoDeleteTransactionBody>();
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodelete(body.release());

  // When / Then
  EXPECT_THROW(const TokenCreateTransaction tokenCreateTransaction(txBody), std::invalid_argument);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetName)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenName(getTestTokenName()));

  // Then
  EXPECT_EQ(transaction.getTokenName(), getTestTokenName());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetNameFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenName(getTestTokenName()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetSymbol)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenSymbol(getTestTokenSymbol()));

  // Then
  EXPECT_EQ(transaction.getTokenSymbol(), getTestTokenSymbol());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetSymbolFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenSymbol(getTestTokenSymbol()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetDecimals)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setDecimals(getTestDecimals()));

  // Then
  EXPECT_EQ(transaction.getDecimals(), getTestDecimals());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetDecimalsFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setDecimals(getTestDecimals()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetInitialSupply)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setInitialSupply(getTestInitialSupply()));

  // Then
  EXPECT_EQ(transaction.getInitialSupply(), getTestInitialSupply());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetInitialSupplyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setInitialSupply(getTestInitialSupply()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetTreasuryAccountId)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTreasuryAccountId(getTestTreasuryAccountId()));

  // Then
  EXPECT_EQ(transaction.getTreasuryAccountId(), getTestTreasuryAccountId());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetTreasuryAccountIdFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTreasuryAccountId(getTestTreasuryAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetAdminKey)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAdminKey(getTestAdminKey()));

  // Then
  EXPECT_EQ(transaction.getAdminKey()->toBytes(), getTestAdminKey()->toBytes());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetAdminKeyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAdminKey(getTestAdminKey()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetKycKey)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setKycKey(getTestKycKey()));

  // Then
  EXPECT_EQ(transaction.getKycKey()->toBytes(), getTestKycKey()->toBytes());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetKycKeyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setKycKey(getTestKycKey()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetFreezeKey)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFreezeKey(getTestFreezeKey()));

  // Then
  EXPECT_EQ(transaction.getFreezeKey()->toBytes(), getTestFreezeKey()->toBytes());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetFreezeKeyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFreezeKey(getTestFreezeKey()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetWipeKey)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setWipeKey(getTestWipeKey()));

  // Then
  EXPECT_EQ(transaction.getWipeKey()->toBytes(), getTestWipeKey()->toBytes());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetWipeKeyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setWipeKey(getTestWipeKey()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetSupplyKey)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setSupplyKey(getTestSupplyKey()));

  // Then
  EXPECT_EQ(transaction.getSupplyKey()->toBytes(), getTestSupplyKey()->toBytes());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetSupplyKeyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setSupplyKey(getTestSupplyKey()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetFreezeDefault)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFreezeDefault(getTestFreezeDefault()));

  // Then
  EXPECT_EQ(transaction.getFreezeDefault(), getTestFreezeDefault());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetFreezeDefaultFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFreezeDefault(getTestFreezeDefault()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetExpirationTime)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setExpirationTime(getTestExpirationTime()));

  // Then
  EXPECT_EQ(transaction.getExpirationTime(), getTestExpirationTime());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetExpirationTimeFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setExpirationTime(getTestExpirationTime()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetAutoRenewAccountId)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewAccountId(), getTestAutoRenewAccountId());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetAutoRenewAccountIdFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewAccountId(getTestAutoRenewAccountId()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetAutoRenewPeriod)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()));

  // Then
  EXPECT_EQ(transaction.getAutoRenewPeriod(), getTestAutoRenewPeriod());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetAutoRenewPeriodFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setAutoRenewPeriod(getTestAutoRenewPeriod()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetMemo)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenMemo(getTestTokenMemo()));

  // Then
  EXPECT_EQ(transaction.getTokenMemo(), getTestTokenMemo());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetMemoFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenMemo(getTestTokenMemo()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetTokenType)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setTokenType(getTestTokenType()));

  // Then
  EXPECT_EQ(transaction.getTokenType(), getTestTokenType());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetTokenTypeFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setTokenType(getTestTokenType()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetSupplyType)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setSupplyType(getTestTokenSupplyType()));

  // Then
  EXPECT_EQ(transaction.getSupplyType(), getTestTokenSupplyType());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetSupplyTypeFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setSupplyType(getTestTokenSupplyType()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetMaxSupply)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setMaxSupply(getTestMaxSupply()));

  // Then
  EXPECT_EQ(transaction.getMaxSupply(), getTestMaxSupply());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetMaxSupplyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setMaxSupply(getTestMaxSupply()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetFeeScheduleKey)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFeeScheduleKey(getTestFeeScheduleKey()));

  // Then
  EXPECT_EQ(transaction.getFeeScheduleKey()->toBytes(), getTestFeeScheduleKey()->toBytes());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetFeeScheduleKeyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFeeScheduleKey(getTestFeeScheduleKey()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetCustomFees)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setCustomFees(getTestCustomFees()));

  // Then
  EXPECT_EQ(transaction.getCustomFees().size(), getTestCustomFees().size());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetCustomFeesFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setCustomFees(getTestCustomFees()), IllegalStateException);
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetPauseKey)
{
  // Given
  TokenCreateTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setPauseKey(getTestPauseKey()));

  // Then
  EXPECT_EQ(transaction.getPauseKey(), getTestPauseKey());
}

//-----
TEST_F(TokenCreateTransactionUnitTests, GetSetPauseKeyFrozen)
{
  // Given
  TokenCreateTransaction transaction = TokenCreateTransaction()
                                         .setNodeAccountIds({ AccountId(1ULL) })
                                         .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setPauseKey(getTestPauseKey()), IllegalStateException);
}
