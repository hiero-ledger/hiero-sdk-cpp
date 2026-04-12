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
  EXPECT_EQ(tokenRelationship.mKycStatus, 0);
  EXPECT_EQ(tokenRelationship.mFreezeStatus, 0);
  EXPECT_EQ(tokenRelationship.mAutomaticAssociation, false);
}

//-----
TEST_F(TokenRelationshipUnitTests, ConstructWithParameters)
{
  // Given / When
  const TokenRelationship tokenRelationship(
    getTestTokenId(), getTestSymbol(), getTestBalance(), 
    getTestDecimals(), getTestKycStatus(), getTestFreezeStatus(), 
    getTestAutomaticAssociation()
  );

  // Then
  EXPECT_EQ(TokenRelationship.mTokenId, getTestTokenId());
  EXPECT_EQ(TokenRelationship.mSymbol, getTestSymbol());
  EXPECT_EQ(TokenRelationship.mBalance, getTestBalance());
  EXPECT_EQ(TokenRelationship.mDecimals, getTestDecimals());
  EXPECT_EQ(TokenRelationship.mKycStatus, getTestKycStatus());
  EXPECT_EQ(TokenRelationship.mFreezeStatus, getTestFreezeStatus());
  EXPECT_EQ(TokenRelationship.mAutomaticAssociation, getTestAutomaticAssociation());
}

TEST_F(TokenRelationshipUnitTests, FromProtobuf)
{
  
}

TEST_F(TokenRelationshipUnitTests, ToProtobuf)
{

}

TEST_F(TokenRelationshipUnitTests, ToString)
{

}

//-----
TEST_F(TokenRelationshipUnitTests, SetGetFreezeStatus)
{
  // Given
  TokenRelationship relFrozen;
  TokenRelationship relUnfrozen;
  TokenRelationship relNotApplicable;
  TokenRelationship rel;
  
  // When
  relFrozen.setFreezeStatus(1);
  relUnfrozen.setFreezeStatus(2);
  relNotApplicable.setFreezeStatus(0);
  
  // Then
  EXPECT_EQ(relFrozen.getFreezeStatus(), proto::TokenFreezeStatus::Frozen);
  EXPECT_EQ(relUnfrozen.getFreezeStatus(), proto::TokenFreezeStatus::Unfrozen);
  EXPECT_EQ(relNotApplicable.getFreezeStatus(), proto::TokenFreezeStatus::FreezeNotApplicable);
  EXPECT_THROW(rel.setFreezeStatus(3), std::invalid_argument);
  EXPECT_THROW(rel.setFreezeStatus(-1), std::invalid_argument);
}

//-----
TEST_F(TokenRelationshipUnitTests, SetGetKycStatus)
{
  // Given
  TokenRelationship relGranted;
  TokenRelationship relRevoked;
  TokenRelationship relNotApplicable;
  TokenRelationship rel;
  
  // When
  relGranted.setKycStatus(1);
  relRevoked.setKycStatus(2);
  relNotApplicable.setKycStatus(0);
  
  // Then
  EXPECT_EQ(relGranted.getKycStatus(), proto::TokenKycStatus::Granted);
  EXPECT_EQ(relRevoked.getKycStatus(), proto::TokenKycStatus::Revoked);
  EXPECT_EQ(relNotApplicable.getKycStatus(), proto::TokenKycStatus::KycNotApplicable);
  EXPECT_THROW(rel.setKycStatus(3), std::invalid_argument);
  EXPECT_THROW(rel.setKycStatus(-1), std::invalid_argument);
}
