// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountId.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "ED25519PublicKey.h"
#include "KeyList.h"
#include "PrivateKey.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "WrappedTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>

using namespace Hiero;

class TokenUpdateTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] inline const std::vector<std::byte>& getTestMetadata() const { return mMetadata; }

  enum class UpdateKeyType
  {
    WIPE_KEY, // = 0
    KYC_KEY,
    SUPPLY_KEY,
    FREEZE_KEY,
    FEE_SCHEDULE_KEY,
    PAUSE_KEY,
    METADATA_KEY,
    ADMIN_KEY,
    LOWER_PRIVILEGE,
    ALL,
    NONE,
  };

  //-----
  TransactionResponse createTokenWithKeysHelper(UpdateKeyType createKeyType,
                                                const std::shared_ptr<PrivateKey>& initialKey,
                                                const std::shared_ptr<PrivateKey>& signerKey)
  {
    TokenCreateTransaction tx;
    tx = TokenCreateTransaction().setTokenName("ffff").setTokenSymbol("F").setTreasuryAccountId(
      getTestClient().getOperatorAccountId().value());

    switch (createKeyType)
    {
      case UpdateKeyType::WIPE_KEY:
        tx.setWipeKey(initialKey);
        break;
      case UpdateKeyType::KYC_KEY:
        tx.setKycKey(initialKey);
        break;
      case UpdateKeyType::SUPPLY_KEY:
        tx.setSupplyKey(initialKey);
        break;
      case UpdateKeyType::FREEZE_KEY:
        tx.setFreezeKey(initialKey);
        break;
      case UpdateKeyType::FEE_SCHEDULE_KEY:
        tx.setFeeScheduleKey(initialKey);
        break;
      case UpdateKeyType::PAUSE_KEY:
        tx.setPauseKey(initialKey);
        break;
      case UpdateKeyType::METADATA_KEY:
        tx.setMetadataKey(initialKey);
        break;
      case UpdateKeyType::ADMIN_KEY:
        tx.setAdminKey(initialKey);
        break;
      case UpdateKeyType::LOWER_PRIVILEGE:
        tx.setWipeKey(initialKey)
          .setKycKey(initialKey)
          .setSupplyKey(initialKey)
          .setFreezeKey(initialKey)
          .setFeeScheduleKey(initialKey)
          .setPauseKey(initialKey)
          .setMetadataKey(initialKey);
        break;
      case UpdateKeyType::ALL:
        tx.setWipeKey(initialKey)
          .setKycKey(initialKey)
          .setSupplyKey(initialKey)
          .setFreezeKey(initialKey)
          .setFeeScheduleKey(initialKey)
          .setPauseKey(initialKey)
          .setMetadataKey(initialKey)
          .setAdminKey(signerKey);
        break;
      case UpdateKeyType::NONE:
        break;
    }

    TransactionResponse txResponse;
    txResponse = tx.freezeWith(&getTestClient()).sign(signerKey).execute(getTestClient());

    return txResponse;
  }

  //-----
  TransactionResponse updateTokenKeysHelper(TokenId tokenId,
                                            UpdateKeyType updateKeyType,
                                            const std::shared_ptr<PrivateKey>& newKey,
                                            const std::shared_ptr<PrivateKey>& signerKey,
                                            TokenKeyValidation tokenKeyValidation)
  {
    TokenUpdateTransaction tx =
      TokenUpdateTransaction().setTokenId(tokenId).setTokenVerificationMode(tokenKeyValidation);

    switch (updateKeyType)
    {
      case UpdateKeyType::WIPE_KEY:
        tx.setWipeKey(newKey);
        break;
      case UpdateKeyType::KYC_KEY:
        tx.setKycKey(newKey);
        break;
      case UpdateKeyType::SUPPLY_KEY:
        tx.setSupplyKey(newKey);
        break;
      case UpdateKeyType::FREEZE_KEY:
        tx.setFreezeKey(newKey);
        break;
      case UpdateKeyType::FEE_SCHEDULE_KEY:
        tx.setFeeScheduleKey(newKey);
        break;
      case UpdateKeyType::PAUSE_KEY:
        tx.setPauseKey(newKey);
        break;
      case UpdateKeyType::METADATA_KEY:
        tx.setMetadataKey(newKey);
        break;
      case UpdateKeyType::ADMIN_KEY:
        tx.setAdminKey(newKey);
        break;
      case UpdateKeyType::LOWER_PRIVILEGE:
        tx.setWipeKey(newKey)
          .setKycKey(newKey)
          .setSupplyKey(newKey)
          .setFreezeKey(newKey)
          .setFeeScheduleKey(newKey)
          .setPauseKey(newKey)
          .setMetadataKey(newKey);
        break;
      case UpdateKeyType::ALL:
        tx.setWipeKey(newKey)
          .setKycKey(newKey)
          .setSupplyKey(newKey)
          .setFreezeKey(newKey)
          .setFeeScheduleKey(newKey)
          .setPauseKey(newKey)
          .setMetadataKey(newKey)
          .setAdminKey(newKey);
        break;
      case UpdateKeyType::NONE:
        break;
    }
    TransactionResponse txResponse;

    if (updateKeyType == UpdateKeyType::ALL || updateKeyType == UpdateKeyType::ADMIN_KEY ||
        tokenKeyValidation == TokenKeyValidation::FULL_VALIDATION)
    {
      txResponse = tx.freezeWith(&getTestClient()).sign(newKey).sign(signerKey).execute(getTestClient());
    }
    else
    {
      txResponse = tx.freezeWith(&getTestClient()).sign(signerKey).execute(getTestClient());
    }

    return txResponse;
  }

private:
  const std::vector<std::byte> mMetadata = { std::byte(0xAA), std::byte(0xAB), std::byte(0xAC), std::byte(0xAD) };
};

// HIP-540 tests

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, TokenUpdateTransactionUpdateKeysToZeroKey)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(operatorKey =
                    ED25519PrivateKey::fromString("bbd0894de0b4ecfa862e963825c5448d2d17f807a16869526bff29185747acdb"));

  std::shared_ptr<PrivateKey> zeroKey = ED25519PrivateKey::getZeroKey();

  // When
  // Make token immutable
  TransactionResponse txResponse = createTokenWithKeysHelper(UpdateKeyType::ALL, operatorKey, operatorKey);

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  EXPECT_NO_THROW(txResponse = updateTokenKeysHelper(
                    tokenId, UpdateKeyType::ALL, zeroKey, operatorKey, TokenKeyValidation::NO_VALIDATION));

  EXPECT_THROW(txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient()), ReceiptStatusException);
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, UpdateLowerPrivilegeKeysWithAdminKeyFullValidation)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> validKey;
  ASSERT_NO_THROW(validKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::ALL, operatorKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  EXPECT_NO_THROW(
    txResponse = updateTokenKeysHelper(
      tokenId, UpdateKeyType::LOWER_PRIVILEGE, validKey, operatorKey, TokenKeyValidation::FULL_VALIDATION));

  txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, UpdateLowerPrivilegeKeysWithAdminKeyNoValidation)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));

  std::shared_ptr<PrivateKey> validKey;
  ASSERT_NO_THROW(validKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::ALL, operatorKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  EXPECT_NO_THROW(txResponse = updateTokenKeysHelper(
                    tokenId, UpdateKeyType::LOWER_PRIVILEGE, validKey, operatorKey, TokenKeyValidation::NO_VALIDATION));

  txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, UpdateLowerPrivilegeKeysWithInvalidAdminKeyFails)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(operatorKey =
                    ED25519PrivateKey::fromString("bbd0894de0b4ecfa862e963825c5448d2d17f807a16869526bff29185747acdb"));

  std::shared_ptr<PrivateKey> validKey;
  ASSERT_NO_THROW(validKey = ED25519PrivateKey::generatePrivateKey());

  std::shared_ptr<PrivateKey> someKey;
  ASSERT_NO_THROW(someKey = ED25519PrivateKey::generatePrivateKey());

  std::shared_ptr<PrivateKey> nonAdminKey;
  ASSERT_NO_THROW(nonAdminKey = ED25519PrivateKey::generatePrivateKey());

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::ALL, nonAdminKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  EXPECT_THROW(txReceipt =
                 updateTokenKeysHelper(
                   tokenId, UpdateKeyType::LOWER_PRIVILEGE, validKey, someKey, TokenKeyValidation::NO_VALIDATION)
                   .getReceipt(getTestClient()),
               ReceiptStatusException);
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, UpdateKeyWithoutAlreadySetAdminKey)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(operatorKey =
                    ED25519PrivateKey::fromString("bbd0894de0b4ecfa862e963825c5448d2d17f807a16869526bff29185747acdb"));

  std::shared_ptr<PrivateKey> someKey;
  ASSERT_NO_THROW(someKey = ED25519PrivateKey::generatePrivateKey());

  std::vector<UpdateKeyType> updateTypes = { UpdateKeyType::WIPE_KEY,         UpdateKeyType::KYC_KEY,
                                             UpdateKeyType::SUPPLY_KEY,       UpdateKeyType::FREEZE_KEY,
                                             UpdateKeyType::FEE_SCHEDULE_KEY, UpdateKeyType::PAUSE_KEY,
                                             UpdateKeyType::METADATA_KEY,     UpdateKeyType::ADMIN_KEY };

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::NONE, operatorKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  for (UpdateKeyType updateType : updateTypes)
  {
    EXPECT_THROW(txReceipt =
                   updateTokenKeysHelper(tokenId, updateType, someKey, operatorKey, TokenKeyValidation::NO_VALIDATION)
                     .setValidateStatus(true)
                     .getReceipt(getTestClient()),
                 ReceiptStatusException); // TOKEN_IS_IMMUTABLE
  }
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, LowerPrivilageKeysCanSelfUpdateToValidKeyNoValidation)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(operatorKey =
                    ED25519PrivateKey::fromString("bbd0894de0b4ecfa862e963825c5448d2d17f807a16869526bff29185747acdb"));

  std::shared_ptr<PrivateKey> someKey;
  ASSERT_NO_THROW(someKey = ED25519PrivateKey::generatePrivateKey());

  std::vector<UpdateKeyType> lowerPrivilageUpdateTypes = { UpdateKeyType::WIPE_KEY,         UpdateKeyType::KYC_KEY,
                                                           UpdateKeyType::SUPPLY_KEY,       UpdateKeyType::FREEZE_KEY,
                                                           UpdateKeyType::FEE_SCHEDULE_KEY, UpdateKeyType::PAUSE_KEY,
                                                           UpdateKeyType::METADATA_KEY };

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::LOWER_PRIVILEGE, operatorKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  for (UpdateKeyType updateType : lowerPrivilageUpdateTypes)
  {
    EXPECT_NO_THROW(
      txReceipt = updateTokenKeysHelper(tokenId, updateType, someKey, operatorKey, TokenKeyValidation::NO_VALIDATION)
                    .setValidateStatus(true)
                    .getReceipt(getTestClient()));
  }
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, LowerPrivilageKeysCanSelfUpdateToZeroKeyNoValidation)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(operatorKey =
                    ED25519PrivateKey::fromString("bbd0894de0b4ecfa862e963825c5448d2d17f807a16869526bff29185747acdb"));

  std::shared_ptr<PrivateKey> zeroKey = ED25519PrivateKey::getZeroKey();

  std::vector<UpdateKeyType> lowerPrivilageUpdateTypes = { UpdateKeyType::WIPE_KEY,         UpdateKeyType::KYC_KEY,
                                                           UpdateKeyType::SUPPLY_KEY,       UpdateKeyType::FREEZE_KEY,
                                                           UpdateKeyType::FEE_SCHEDULE_KEY, UpdateKeyType::PAUSE_KEY,
                                                           UpdateKeyType::METADATA_KEY };

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::LOWER_PRIVILEGE, operatorKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  for (UpdateKeyType updateType : lowerPrivilageUpdateTypes)
  {
    EXPECT_NO_THROW(
      txReceipt = updateTokenKeysHelper(tokenId, updateType, zeroKey, operatorKey, TokenKeyValidation::NO_VALIDATION)
                    .setValidateStatus(true)
                    .getReceipt(getTestClient()));
  }
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, LowerPrivilageKeysCanSelfUpdateToValidKeyFullValidation)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(operatorKey =
                    ED25519PrivateKey::fromString("bbd0894de0b4ecfa862e963825c5448d2d17f807a16869526bff29185747acdb"));

  std::shared_ptr<PrivateKey> someKey;
  ASSERT_NO_THROW(someKey = ED25519PrivateKey::generatePrivateKey());

  std::vector<UpdateKeyType> lowerPrivilageUpdateTypes = { UpdateKeyType::WIPE_KEY,         UpdateKeyType::KYC_KEY,
                                                           UpdateKeyType::SUPPLY_KEY,       UpdateKeyType::FREEZE_KEY,
                                                           UpdateKeyType::FEE_SCHEDULE_KEY, UpdateKeyType::PAUSE_KEY,
                                                           UpdateKeyType::METADATA_KEY };

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::LOWER_PRIVILEGE, operatorKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  for (UpdateKeyType updateType : lowerPrivilageUpdateTypes)
  {
    EXPECT_NO_THROW(
      txReceipt = updateTokenKeysHelper(tokenId, updateType, someKey, operatorKey, TokenKeyValidation::FULL_VALIDATION)
                    .setValidateStatus(true)
                    .getReceipt(getTestClient()));
  }
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, LowerPrivilageKeysCanSelfUpdateToZeroKeyFullValidationFails)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(operatorKey =
                    ED25519PrivateKey::fromString("bbd0894de0b4ecfa862e963825c5448d2d17f807a16869526bff29185747acdb"));

  std::shared_ptr<PrivateKey> zeroKey = ED25519PrivateKey::getZeroKey();

  std::vector<UpdateKeyType> lowerPrivilageUpdateTypes = { UpdateKeyType::WIPE_KEY,         UpdateKeyType::KYC_KEY,
                                                           UpdateKeyType::SUPPLY_KEY,       UpdateKeyType::FREEZE_KEY,
                                                           UpdateKeyType::FEE_SCHEDULE_KEY, UpdateKeyType::PAUSE_KEY,
                                                           UpdateKeyType::METADATA_KEY };

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = createTokenWithKeysHelper(UpdateKeyType::LOWER_PRIVILEGE, operatorKey, operatorKey));

  TransactionReceipt txReceipt = txResponse.setValidateStatus(true).getReceipt(getTestClient());
  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = txReceipt.mTokenId.value());

  // Then
  for (UpdateKeyType updateType : lowerPrivilageUpdateTypes)
  {
    EXPECT_THROW(txReceipt =
                   updateTokenKeysHelper(tokenId, updateType, zeroKey, operatorKey, TokenKeyValidation::FULL_VALIDATION)
                     .setValidateStatus(true)
                     .getReceipt(getTestClient()),
                 ReceiptStatusException); // INVALID_SIGNATURE;
  }
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, ExecuteTokenUpdateTransaction)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  const std::string updatedTokenName = "Token";
  const std::string updatedTokenSymbol = "T";

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setDecimals(3)
                              .setInitialSupply(100000)
                              .setTreasuryAccountId(AccountId(2ULL))
                              .setAdminKey(operatorKey)
                              .setFreezeKey(operatorKey)
                              .setWipeKey(operatorKey)
                              .setKycKey(operatorKey)
                              .setSupplyKey(operatorKey)
                              .setFeeScheduleKey(operatorKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When
  TransactionReceipt txReceipt;
  EXPECT_NO_THROW(txReceipt = TokenUpdateTransaction()
                                .setTokenId(tokenId)
                                .setTokenName(updatedTokenName)
                                .setTokenSymbol(updatedTokenSymbol)
                                .execute(getTestClient())
                                .getReceipt(getTestClient()));

  // Then
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  EXPECT_EQ(tokenInfo.mTokenName, updatedTokenName);
  EXPECT_EQ(tokenInfo.mTokenSymbol, updatedTokenSymbol);

  // Clean up
  ASSERT_NO_THROW(txReceipt =
                    TokenDeleteTransaction().setTokenId(tokenId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, CannotUpdateImmutableKey)
{
  // Given
  std::shared_ptr<PrivateKey> operatorKey;
  ASSERT_NO_THROW(
    operatorKey = ED25519PrivateKey::fromString(
      "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137"));
  const std::string updatedTokenName = "Token";
  const std::string updatedTokenSymbol = "T";

  TokenId tokenId;
  EXPECT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setTreasuryAccountId(AccountId(2ULL))
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // When / Then
  EXPECT_THROW(const TransactionReceipt txReceipt = TokenUpdateTransaction()
                                                      .setTokenId(tokenId)
                                                      .setTokenName(updatedTokenName)
                                                      .setTokenSymbol(updatedTokenSymbol)
                                                      .execute(getTestClient())
                                                      .getReceipt(getTestClient()),
               ReceiptStatusException); // TOKEN_IS_IMMUTABLE
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, CanUpdateFungibleTokenMetadata)
{
  // Given
  std::shared_ptr<PrivateKey> metadataKey;
  ASSERT_NO_THROW(metadataKey = ED25519PrivateKey::generatePrivateKey());
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };

  // create a fungible token with metadata and metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::FUNGIBLE_COMMON)
                              .setDecimals(3)
                              .setInitialSupply(1000000)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setMetadataKey(metadataKey)
                              .setFreezeDefault(false)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if token created successfully with metadata and metadata key
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, getTestMetadata());
  EXPECT_EQ(tokenInfo.mMetadataKey->toBytes(), metadataKey->getPublicKey()->toBytes());

  // When
  // update token's metadata
  ASSERT_NO_THROW(TokenUpdateTransaction()
                    .setTokenId(tokenId)
                    .setMetadata(updatedMetadataRecord)
                    .freezeWith(&getTestClient())
                    .sign(metadataKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, updatedMetadataRecord);
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, CanUpdateNonFungibleTokenMetadata)
{
  // Given
  std::shared_ptr<PrivateKey> metadataKey;
  ASSERT_NO_THROW(metadataKey = ED25519PrivateKey::generatePrivateKey());
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };

  // create a NFT with metadata and metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .setMetadataKey(metadataKey)
                              .setFreezeDefault(false)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if NFT created successfully with metadata and metadata key
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, getTestMetadata());
  EXPECT_EQ(tokenInfo.mMetadataKey->toBytes(), metadataKey->getPublicKey()->toBytes());

  // When
  // update token's metadata
  ASSERT_NO_THROW(TokenUpdateTransaction()
                    .setTokenId(tokenId)
                    .setMetadata(updatedMetadataRecord)
                    .freezeWith(&getTestClient())
                    .sign(metadataKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, updatedMetadataRecord);
}

//-----
// This test verifies that metadata is not updated silently. This test is kept for regression
// because previously the metadata got updated silently
TEST_F(TokenUpdateTransactionIntegrationTests, CannotUpdateFungibleTokenMetadataKeyNotSet)
{
  // Given
  // create a NFT without metadata and metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::FUNGIBLE_COMMON)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .setFreezeDefault(false)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if FT created successfully with metadata and metadata key
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, getTestMetadata());
  EXPECT_EQ(tokenInfo.mMetadataKey, nullptr);

  // When
  // update token's metadata
  ASSERT_NO_THROW(TokenUpdateTransaction()
                    .setTokenId(tokenId)
                    .freezeWith(&getTestClient())
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, getTestMetadata());
}

//-----
// This test verifies that metadata is not updated silently. This test is kept for regression
// because previously the metadata got updated silently
TEST_F(TokenUpdateTransactionIntegrationTests, CannotUpdateNonFungibleTokenMetadataKeyNotSet)
{
  // Given
  // create a NFT without metadata and metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(getTestClient().getOperatorPublicKey())
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .setFreezeDefault(false)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if NFT created successfully with metadata and metadata key
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, getTestMetadata());
  EXPECT_EQ(tokenInfo.mMetadataKey, nullptr);

  // When
  // update token's metadata
  ASSERT_NO_THROW(TokenUpdateTransaction()
                    .setTokenId(tokenId)
                    .setTokenMemo("abc")
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Then
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));
  EXPECT_EQ(tokenInfo.mMetadata, getTestMetadata());
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests,
       CannotUpdateFungibleTokenMetadataWhenTransactionNotSignedWithMetadataOrAdminKey)
{
  // Given
  std::shared_ptr<PrivateKey> metadataKey;
  ASSERT_NO_THROW(metadataKey = ED25519PrivateKey::generatePrivateKey());
  std::shared_ptr<PrivateKey> adminKey;
  ASSERT_NO_THROW(adminKey = ED25519PrivateKey::generatePrivateKey());
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };

  // create a FT with metadata and metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::FUNGIBLE_COMMON)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(adminKey)
                              .setMetadataKey(metadataKey)
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .freezeWith(&getTestClient())
                              .sign(adminKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if FT created successfully with metadata and metadata key
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  // When / Then
  // update token's metadata
  EXPECT_THROW(TokenUpdateTransaction()
                 .setTokenId(tokenId)
                 .setMetadata(updatedMetadataRecord)
                 .freezeWith(&getTestClient())
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests,
       CannotUpdateNonFungibleTokenMetadataWhenTransactionNotSignedWithMetadataOrAdminKey)
{
  // Given
  std::shared_ptr<PrivateKey> metadataKey;
  ASSERT_NO_THROW(metadataKey = ED25519PrivateKey::generatePrivateKey());
  std::shared_ptr<PrivateKey> adminKey;
  ASSERT_NO_THROW(adminKey = ED25519PrivateKey::generatePrivateKey());
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };

  // create a NFT with metadata and metadata key
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("FF")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setAdminKey(adminKey)
                              .setMetadataKey(metadataKey)
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .freezeWith(&getTestClient())
                              .sign(adminKey)
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if NFT created successfully with metadata and metadata key
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  // When / Then
  // update token's metadata
  EXPECT_THROW(TokenUpdateTransaction()
                 .setTokenId(tokenId)
                 .setMetadata(updatedMetadataRecord)
                 .freezeWith(&getTestClient())
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // INVALID_SIGNATURE
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, CannotUpdateNonFungibleTokenMetadataWhenMetadataKeyNotSet)
{
  // Given
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };

  // create a NFT with metadata
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if NFT created successfully with metadata
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  // When / Then
  // update token's metadata
  EXPECT_THROW(TokenUpdateTransaction()
                 .setTokenId(tokenId)
                 .setMetadata(updatedMetadataRecord)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // TOKEN_IS_IMMUTABLE
}

//-----
TEST_F(TokenUpdateTransactionIntegrationTests, CannotUpdateFungibleTokenMetadataWhenMetadataKeyNotSet)
{
  // Given
  const std::vector<std::byte> updatedMetadataRecord = {
    std::byte(0xBA), std::byte(0xBB), std::byte(0xBC), std::byte(0xBD)
  };

  // create a NFT with metadata
  TokenId tokenId;
  ASSERT_NO_THROW(tokenId = TokenCreateTransaction()
                              .setTokenName("ffff")
                              .setTokenSymbol("F")
                              .setMetadata(getTestMetadata())
                              .setTokenType(TokenType::FUNGIBLE_COMMON)
                              .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                              .setSupplyKey(getTestClient().getOperatorPublicKey())
                              .execute(getTestClient())
                              .getReceipt(getTestClient())
                              .mTokenId.value());

  // check if NFT created successfully with metadata
  TokenInfo tokenInfo;
  ASSERT_NO_THROW(tokenInfo = TokenInfoQuery().setTokenId(tokenId).execute(getTestClient()));

  // When / Then
  // update token's metadata
  EXPECT_THROW(TokenUpdateTransaction()
                 .setTokenId(tokenId)
                 .setMetadata(updatedMetadataRecord)
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // TOKEN_IS_IMMUTABLE
}