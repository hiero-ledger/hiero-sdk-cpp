// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SEMANTIC_VERSION_H_
#define HIERO_SDK_CPP_SEMANTIC_VERSION_H_

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class SemanticVersion;
}

namespace Hiero
{
/**
 * Hiero follows semantic versioning for both the HAPI protobufs and the Services software. This type allows the
 * getVersionInfo query in the NetworkService to return the deployed versions of both protobufs and software on the node
 * answering the query.
 */
class SemanticVersion
{
public:
  SemanticVersion() = default;

  /**
   * Construct from a major, minor, and patch number.
   *
   * @param major The major number.
   * @param minor The minor number.
   * @param patch The patch number.
   * @param pre   The pre-release version number.
   * @param build The build metadata.
   */
  SemanticVersion(int major, int minor, int patch, std::string_view pre = "", std::string_view build = "");

  /**
   * Compare this SemanticVersion to another SemanticVersion and determine if they represent the same semantic version.
   *
   * @param other The other SemanticVersion with which to compare this SemanticVersion.
   * @return \c TRUE if this SemanticVersion is the same as the input SemanticVersion, otherwise \c FALSE.
   */
  bool operator==(const SemanticVersion& other) const;

  /**
   * Construct a SemanticVersion object from a SemanticVersion protobuf object.
   *
   * @param proto The SemanticVersion protobuf object from which to construct a SemanticVersion object.
   * @return The constructed SemanticVersion object.
   */
  [[nodiscard]] static SemanticVersion fromProtobuf(const proto::SemanticVersion& proto);

  /**
   * Construct a SemanticVersion object from a byte array.
   *
   * @param bytes The byte array representing a SemanticVersion object.
   * @return The constructed SemanticVersion object.
   */
  [[nodiscard]] static SemanticVersion fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a SemanticVersion protobuf object from this SemanticVersion object.
   *
   * @return A pointer to the created SemanticVersion protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::SemanticVersion> toProtobuf() const;

  /**
   * Construct a representative byte array from this SemanticVersion object.
   *
   * @return The byte array representing this SemanticVersion object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this SemanticVersion object.
   *
   * @return The string representation of this SemanticVersion object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Major number. Increases with incompatible API changes.
   */
  int mMajor = 0;

  /**
   * Minor number. Increases with backwards-compatible new functionality.
   */
  int mMinor = 0;

  /**
   * Patch number. Increases with backwards-compatible bug fixes.
   */
  int mPatch = 0;

  /**
   * Pre-release version. This may be denoted by appending a hyphen and a series of dot-separated identifiers.
   */
  std::string mPre;

  /**
   * Build metadata. This may be denoted by appending a plus sign and a series of dot-separated identifiers immediately
   * following the patch or pre-release version.
   */
  std::string mBuild;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SEMANTIC_VERSION_H_
