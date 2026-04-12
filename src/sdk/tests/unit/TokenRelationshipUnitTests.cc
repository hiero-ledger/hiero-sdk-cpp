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
  [[nodiscard]] inline const uint64_t& getTestBalance() const { return mBalance; }
  [[nodiscard]] inline const uint32_t& getTestDecimals() const { return mDecimals; }
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
  const TokenRelationship TokenRelationship;

  // Then
  EXPECT_EQ(TokenRelationship.mTokenId, TokenId());
  EXPECT_EQ(TokenRelationship.mSymbol, "");
  EXPECT_EQ(TokenRelationship.mBalance, 0LL);
  EXPECT_EQ(TokenRelationship.mDecimals, 0);
  EXPECT_EQ(TokenRelationship.mKycStatus, 0);
  EXPECT_EQ(TokenRelationship.mFreezeStatus, 0);
  EXPECT_FALSE(TokenRelationship.mAutomaticAssociation, false);
}

//-----
TEST_F(TokenRelationshipUnitTests, ConstructWithParameters)
{
  // Given / When
  const TokenRelationship TokenRelationship(
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

TEST_F(TokenRelationshipUnitTests, SetKycStatus)
{

}

TEST_F(TokenRelationshipUnitTests, GetKycStatus)
{

}

TEST_F(TokenRelationshipUnitTests, SetFreezeStatus)
{

}

TEST_F(TokenRelationshipUnitTests, SetFreezeStatus)
{

}
