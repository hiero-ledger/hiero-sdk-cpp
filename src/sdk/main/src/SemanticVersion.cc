// SPDX-License-Identifier: Apache-2.0
#include "SemanticVersion.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
SemanticVersion::SemanticVersion(int major, int minor, int patch, std::string_view pre, std::string_view build)
  : mMajor(major)
  , mMinor(minor)
  , mPatch(patch)
  , mPre(pre)
  , mBuild(build)
{
}

//-----
bool SemanticVersion::operator==(const SemanticVersion& other) const
{
  return (mMajor == other.mMajor) && (mMinor == other.mMinor) && (mPatch == other.mPatch) && (mPre == other.mPre) &&
         (mBuild == other.mBuild);
}

//-----
SemanticVersion SemanticVersion::fromProtobuf(const proto::SemanticVersion& proto)
{
  return { proto.major(), proto.minor(), proto.patch(), proto.pre(), proto.build() };
}

//-----
SemanticVersion SemanticVersion::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::SemanticVersion semanticVersion;
  semanticVersion.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(semanticVersion);
}

//-----
std::unique_ptr<proto::SemanticVersion> SemanticVersion::toProtobuf() const
{
  auto proto = std::make_unique<proto::SemanticVersion>();
  proto->set_major(mMajor);
  proto->set_minor(mMinor);
  proto->set_patch(mPatch);
  proto->set_pre(mPre);
  proto->set_build(mBuild);
  return proto;
}

//-----
std::vector<std::byte> SemanticVersion::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string SemanticVersion::toString() const
{
  std::string str = std::to_string(mMajor) + '.' + std::to_string(mMinor) + '.' + std::to_string(mPatch);

  if (!mPre.empty())
  {
    str += '-' + mPre;
  }

  if (!mBuild.empty())
  {
    str += '+' + mBuild;
  }

  return str;
}

} // namespace Hiero
