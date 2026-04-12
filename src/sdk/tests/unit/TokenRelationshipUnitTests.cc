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
  
  TokenRelationship createWithStatuses(int kyc, int freeze) {
    return TokenRelationship(
      getTestTokenId(), 
      getTestSymbol(), 
      getTestBalance(), 
      getTestDecimals(), 
      kyc, 
      freeze, 
      getTestAutomaticAssociation()
    );
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
  EXPECT_EQ(tokenRelationship.mTokenId, getTestTokenId());
  EXPECT_EQ(tokenRelationship.mSymbol, getTestSymbol());
  EXPECT_EQ(tokenRelationship.mBalance, getTestBalance());
  EXPECT_EQ(tokenRelationship.mDecimals, getTestDecimals());
  EXPECT_EQ(tokenRelationship.mKycStatus, getTestKycStatus());
  EXPECT_EQ(tokenRelationship.mFreezeStatus, getTestFreezeStatus());
  EXPECT_EQ(tokenRelationship.mAutomaticAssociation, getTestAutomaticAssociation());
}

//-----
TEST_F(TokenRelationshipUnitTests, FromProtobuf)
{
  
}

//-----
TEST_F(TokenRelationshipUnitTests, ToProtobuf)
{
  // Given
  TokenRelationship rel(getTestTokenId(), getTestSymbol(), getTestBalance(), 
    getTestDecimals(), getTestKycStatus(), getTestFreezeStatus(), getTestAutomaticAssociation()
  );

  // When
  std::unique_ptr<proto::TokenRelationship> proto = rel.toProtobuf();

  // Then
  
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
  TokenRelationship rel = createWithStatuses(1,1);

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
  EXPECT_EQ(relFrozen.mFreezeStatus, proto::TokenFreezeStatus::Frozen);
  EXPECT_EQ(relUnfrozen.mFreezeStatus, proto::TokenFreezeStatus::Unfrozen);
  EXPECT_EQ(relNotApplicable.mFreezeStatus, proto::TokenFreezeStatus::FreezeNotApplicable);
  EXPECT_THROW(
    TokenRelationship relInvalidFreeze = createWithStatuses(getTestKycStatus(), 3);
    , std::invalid_argument
  );
  EXPECT_THROW(
    TokenRelationship relInvalidFreeze1 = createWithStatuses(getTestKycStatus(), -1);
    , std::invalid_argument
  );
}

//-----
TEST_F(TokenRelationshipUnitTests, SetGetKycStatus)
{
  // Given / When
  TokenRelationship relGranted = createWithStatuses(1, getTestFreezeStatus());
  TokenRelationship relRevoked = createWithStatuses(2, getTestFreezeStatus());
  TokenRelationship relNotApplicable = createWithStatuses(0, getTestFreezeStatus());
  
  // Then
  EXPECT_EQ(relGranted.mKycStatus, proto::TokenKycStatus::Granted);
  EXPECT_EQ(relRevoked.mKycStatus, proto::TokenKycStatus::Revoked);
  EXPECT_EQ(relNotApplicable.mKycStatus, proto::TokenKycStatus::KycNotApplicable);
  EXPECT_THROW(
    TokenRelationship relInvalidKyc = createWithStatuses(3, getTestFreezeStatus());
    , std::invalid_argument
  );
  EXPECT_THROW(
    TokenRelationship relInvalidKyc1 = createWithStatuses(-1, getTestFreezeStatus());
    , std::invalid_argument
  );
}
