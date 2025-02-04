// SPDX-License-Identifier: Apache-2.0
#include "SemanticVersion.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>
#include <string>

using namespace Hiero;

class SemanticVersionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const int& getTestMajor() const { return mTestMajor; }
  [[nodiscard]] inline const int& getTestMinor() const { return mTestMinor; }
  [[nodiscard]] inline const int& getTestPatch() const { return mTestPatch; }
  [[nodiscard]] inline const std::string& getTestPrerelease() const { return mTestPrerelease; }
  [[nodiscard]] inline const std::string& getTestBuildMetadata() const { return mTestBuildMetadata; }

private:
  const int mTestMajor = 1;
  const int mTestMinor = 2;
  const int mTestPatch = 3;
  const std::string mTestPrerelease = "test pre-release";
  const std::string mTestBuildMetadata = "test build metadata";
};

//-----
TEST_F(SemanticVersionUnitTests, ConstructWithValues)
{
  // Given / When
  const SemanticVersion semanticVersion(
    getTestMajor(), getTestMinor(), getTestPatch(), getTestPrerelease(), getTestBuildMetadata());

  // Then
  EXPECT_EQ(semanticVersion.mMajor, getTestMajor());
  EXPECT_EQ(semanticVersion.mMinor, getTestMinor());
  EXPECT_EQ(semanticVersion.mPatch, getTestPatch());
  EXPECT_EQ(semanticVersion.mPre, getTestPrerelease());
  EXPECT_EQ(semanticVersion.mBuild, getTestBuildMetadata());
}

//-----
TEST_F(SemanticVersionUnitTests, FromProtobuf)
{
  // Given
  proto::SemanticVersion protoSemanticVersion;
  protoSemanticVersion.set_major(getTestMajor());
  protoSemanticVersion.set_minor(getTestMinor());
  protoSemanticVersion.set_patch(getTestPatch());
  protoSemanticVersion.set_pre(getTestPrerelease());
  protoSemanticVersion.set_build(getTestBuildMetadata());

  // When
  const SemanticVersion semanticVersion = SemanticVersion::fromProtobuf(protoSemanticVersion);

  // Then
  EXPECT_EQ(semanticVersion.mMajor, getTestMajor());
  EXPECT_EQ(semanticVersion.mMinor, getTestMinor());
  EXPECT_EQ(semanticVersion.mPatch, getTestPatch());
  EXPECT_EQ(semanticVersion.mPre, getTestPrerelease());
  EXPECT_EQ(semanticVersion.mBuild, getTestBuildMetadata());
}

//-----
TEST_F(SemanticVersionUnitTests, FromBytes)
{
  // Given
  proto::SemanticVersion protoSemanticVersion;
  protoSemanticVersion.set_major(getTestMajor());
  protoSemanticVersion.set_minor(getTestMinor());
  protoSemanticVersion.set_patch(getTestPatch());
  protoSemanticVersion.set_pre(getTestPrerelease());
  protoSemanticVersion.set_build(getTestBuildMetadata());

  // When
  const SemanticVersion semanticVersion =
    SemanticVersion::fromBytes(internal::Utilities::stringToByteVector(protoSemanticVersion.SerializeAsString()));

  // Then
  EXPECT_EQ(semanticVersion.mMajor, getTestMajor());
  EXPECT_EQ(semanticVersion.mMinor, getTestMinor());
  EXPECT_EQ(semanticVersion.mPatch, getTestPatch());
  EXPECT_EQ(semanticVersion.mPre, getTestPrerelease());
  EXPECT_EQ(semanticVersion.mBuild, getTestBuildMetadata());
}

//-----
TEST_F(SemanticVersionUnitTests, ToProtobuf)
{
  // Given
  const SemanticVersion semanticVersion(
    getTestMajor(), getTestMinor(), getTestPatch(), getTestPrerelease(), getTestBuildMetadata());

  // When
  const std::unique_ptr<proto::SemanticVersion> protoSemanticVersion = semanticVersion.toProtobuf();

  // Then
  EXPECT_EQ(protoSemanticVersion->major(), getTestMajor());
  EXPECT_EQ(protoSemanticVersion->minor(), getTestMinor());
  EXPECT_EQ(protoSemanticVersion->patch(), getTestPatch());
  EXPECT_EQ(protoSemanticVersion->pre(), getTestPrerelease());
  EXPECT_EQ(protoSemanticVersion->build(), getTestBuildMetadata());
}

//-----
TEST_F(SemanticVersionUnitTests, ToBytes)
{
  // Given
  const SemanticVersion semanticVersion(
    getTestMajor(), getTestMinor(), getTestPatch(), getTestPrerelease(), getTestBuildMetadata());

  // When
  const std::vector<std::byte> bytes = semanticVersion.toBytes();

  // Then
  EXPECT_EQ(bytes, internal::Utilities::stringToByteVector(semanticVersion.toProtobuf()->SerializeAsString()));
}
