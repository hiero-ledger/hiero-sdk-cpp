// SPDX-License-Identifier: Apache-2.0
#include "TokenRelationship.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>
#include <services/crypto_get_info.pb.h>

using namespace Hiero;

class TokenRelationshipUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTokenId; }
  [[nodiscard]] inline const std::string& getTestSymbol() const { return mSymbol; }
  [[nodiscard]] inline const uint64_t getTestBalance() const { return mBalance; }
  [[nodiscard]] inline const uint32_t getTestDecimals() const { return mDecimals; }
  [[nodiscard]] inline int getTestKycStatus() const { return mKycStatus; }
  [[nodiscard]] inline int getTestFreezeStatus() const { return mFreezeStatus; }
  [[nodiscard]] inline bool getTestAutomaticAssociation() const { return mAutomaticAssociation; }

  TokenRelationship createWithStatuses(int kyc, int freeze)
  {
    return TokenRelationship(getTestTokenId(),
                             getTestSymbol(),
                             getTestBalance(),
                             getTestDecimals(),
                             kyc,
                             freeze,
                             getTestAutomaticAssociation());
  }

private:
  const TokenId mTokenId = TokenId(10ULL);
  const std::string mSymbol = "";
  const uint64_t mBalance = 3000LL;
  const uint32_t mDecimals = 0;
  const int mKycStatus = 0;
  const int mFreezeStatus = 0;
  const bool mAutomaticAssociation = false;
};

//-----
TEST_F(TokenRelationshipUnitTests, DefaultConstruction)
{
  // Given / When
  const TokenRelationship tokenRelationship;

  // Then
  EXPECT_EQ(tokenRelationship.mTokenId, TokenId());
  EXPECT_EQ(tokenRelationship.mSymbol, "");
  EXPECT_EQ(tokenRelationship.mBalance, 0LL);
  EXPECT_EQ(tokenRelationship.mDecimals, 0);
  EXPECT_FALSE(tokenRelationship.mKycStatus.has_value());
  EXPECT_FALSE(tokenRelationship.mFreezeStatus.has_value());
  EXPECT_EQ(tokenRelationship.mAutomaticAssociation, false);
}

//-----
TEST_F(TokenRelationshipUnitTests, ConstructWithParameters)
{
  // Given / When
  const TokenRelationship tokenRelationship(getTestTokenId(),
                                            getTestSymbol(),
                                            getTestBalance(),
                                            getTestDecimals(),
                                            getTestKycStatus(),
                                            getTestFreezeStatus(),
                                            getTestAutomaticAssociation());

  // Then
  EXPECT_EQ(tokenRelationship.mTokenId, getTestTokenId());
  EXPECT_EQ(tokenRelationship.mSymbol, getTestSymbol());
  EXPECT_EQ(tokenRelationship.mBalance, getTestBalance());
  EXPECT_EQ(tokenRelationship.mDecimals, getTestDecimals());
  EXPECT_FALSE(tokenRelationship.mKycStatus.has_value());
  EXPECT_FALSE(tokenRelationship.mFreezeStatus.has_value());
  EXPECT_EQ(tokenRelationship.mAutomaticAssociation, getTestAutomaticAssociation());
}

//-----
TEST_F(TokenRelationshipUnitTests, FromProtobuf)
{
  // Given
  proto::TokenRelationship protoRel;
  protoRel.set_allocated_tokenid(getTestTokenId().toProtobuf().release());
  protoRel.set_symbol(getTestSymbol());
  protoRel.set_balance(getTestBalance());
  protoRel.set_decimals(getTestDecimals());
  protoRel.set_kycstatus(proto::TokenKycStatus::Granted);
  protoRel.set_freezestatus(proto::TokenFreezeStatus::Frozen);
  protoRel.set_automatic_association(true);

  // When
  const TokenRelationship rel = TokenRelationship::fromProtobuf(protoRel);

  // Then
  EXPECT_EQ(rel.mTokenId, getTestTokenId());
  EXPECT_EQ(rel.mSymbol, getTestSymbol());
  EXPECT_EQ(rel.mBalance, getTestBalance());
  EXPECT_EQ(rel.mDecimals, getTestDecimals());
  ASSERT_TRUE(rel.mKycStatus.has_value());
  EXPECT_TRUE(rel.mKycStatus.value());
  ASSERT_TRUE(rel.mFreezeStatus.has_value());
  EXPECT_TRUE(rel.mFreezeStatus.value());
  EXPECT_TRUE(rel.mAutomaticAssociation);

  // Round-trip: automatic_association must survive toProtobuf()
  const auto roundTripped = rel.toProtobuf();
  EXPECT_TRUE(roundTripped->automatic_association());
}

//-----
TEST_F(TokenRelationshipUnitTests, ToProtobuf)
{
  // Given
  TokenRelationship rel(getTestTokenId(),
                        getTestSymbol(),
                        getTestBalance(),
                        getTestDecimals(),
                        getTestKycStatus(),
                        getTestFreezeStatus(),
                        getTestAutomaticAssociation());

  // When
  auto protoRel = rel.toProtobuf();

  // Then
  EXPECT_EQ(protoRel->tokenid().tokennum(), getTestTokenId().mTokenNum);
  EXPECT_EQ(protoRel->symbol(), getTestSymbol());
  EXPECT_EQ(protoRel->balance(), getTestBalance());
  EXPECT_EQ(protoRel->decimals(), getTestDecimals());
  EXPECT_EQ(protoRel->kycstatus(), getTestKycStatus());
  EXPECT_EQ(protoRel->freezestatus(), getTestFreezeStatus());
  EXPECT_EQ(protoRel->automatic_association(), getTestAutomaticAssociation());
}

//-----
TEST_F(TokenRelationshipUnitTests, ToStringDefaultValues)
{
  // Given
  TokenRelationship rel;

  // When
  std::string result = rel.toString();

  // Then
  EXPECT_NE(result.find("tokenId: " + rel.mTokenId.toString()), std::string::npos);
  EXPECT_NE(result.find("symbol: " + rel.mSymbol), std::string::npos);
  EXPECT_NE(result.find("balance: " + std::to_string(rel.mBalance)), std::string::npos);
  EXPECT_NE(result.find("decimals: " + std::to_string(rel.mDecimals)), std::string::npos);
  EXPECT_NE(result.find("kycStatus: null"), std::string::npos);
  EXPECT_NE(result.find("freezeStatus: null"), std::string::npos);
  EXPECT_NE(result.find("automaticAssociation: false"), std::string::npos);
}

//-----
TEST_F(TokenRelationshipUnitTests, ToStringWithStatuses)
{
  // Given
  TokenRelationship rel = createWithStatuses(1, 1);

  // When
  std::string result = rel.toString();

  // Then
  EXPECT_NE(result.find("kycStatus: 1"), std::string::npos);
  EXPECT_NE(result.find("freezeStatus: 1"), std::string::npos);
}

//-----
TEST_F(TokenRelationshipUnitTests, SetGetFreezeStatus)
{
  // Given / When
  TokenRelationship relFrozen = createWithStatuses(getTestKycStatus(), 1);
  TokenRelationship relUnfrozen = createWithStatuses(getTestKycStatus(), 2);
  TokenRelationship relNotApplicable = createWithStatuses(getTestKycStatus(), 0);

  // Then
  ASSERT_TRUE(relFrozen.mFreezeStatus.has_value());
  EXPECT_TRUE(relFrozen.mFreezeStatus.value());
  ASSERT_TRUE(relUnfrozen.mFreezeStatus.has_value());
  EXPECT_FALSE(relUnfrozen.mFreezeStatus.value());
  EXPECT_FALSE(relNotApplicable.mFreezeStatus.has_value());
  EXPECT_THROW(TokenRelationship relInvalidFreeze = createWithStatuses(getTestKycStatus(), 3);, std::invalid_argument);
  EXPECT_THROW(TokenRelationship relInvalidFreeze1 = createWithStatuses(getTestKycStatus(), -1);
               , std::invalid_argument);
}

//-----
TEST_F(TokenRelationshipUnitTests, SetGetKycStatus)
{
  // Given / When
  TokenRelationship relGranted = createWithStatuses(1, getTestFreezeStatus());
  TokenRelationship relRevoked = createWithStatuses(2, getTestFreezeStatus());
  TokenRelationship relNotApplicable = createWithStatuses(0, getTestFreezeStatus());

  // Then
  ASSERT_TRUE(relGranted.mKycStatus.has_value());
  EXPECT_TRUE(relGranted.mKycStatus.value());
  ASSERT_TRUE(relRevoked.mKycStatus.has_value());
  EXPECT_FALSE(relRevoked.mKycStatus.value());
  EXPECT_FALSE(relNotApplicable.mKycStatus.has_value());
  EXPECT_THROW(TokenRelationship relInvalidKyc = createWithStatuses(3, getTestFreezeStatus());, std::invalid_argument);
  EXPECT_THROW(TokenRelationship relInvalidKyc1 = createWithStatuses(-1, getTestFreezeStatus());
               , std::invalid_argument);
}
