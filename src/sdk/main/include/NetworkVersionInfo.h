// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NETWORK_VERSION_INFO_H_
#define HIERO_SDK_CPP_NETWORK_VERSION_INFO_H_

#include "SemanticVersion.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class NetworkGetVersionInfoResponse;
}

namespace Hiero
{
/**
 * Contains information about the network's version.
 */
class NetworkVersionInfo
{
public:
  NetworkVersionInfo() = default;

  /**
   * Construct from a protobuf schema SemanticVersion and a Hiero services SemanticVersion.
   *
   * @param hapi   The SemanticVersion of the protobuf schema.
   * @param hiero The SemanticVersion of the Hiero services.
   */
  NetworkVersionInfo(const SemanticVersion& hapi, const SemanticVersion& hiero);

  /**
   * Construct a NetworkVersionInfo object from a NetworkGetVersionInfoResponse protobuf object.
   *
   * @param proto The NetworkGetVersionInfoResponse protobuf object from which to construct a NetworkVersionInfo object.
   * @return The constructed NetworkVersionInfo object.
   */
  [[nodiscard]] static NetworkVersionInfo fromProtobuf(const proto::NetworkGetVersionInfoResponse& proto);

  /**
   * Construct a NetworkVersionInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct a NetworkVersionInfo object.
   * @return The constructed NetworkVersionInfo object.
   */
  [[nodiscard]] static NetworkVersionInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a NetworkGetVersionInfoResponse protobuf object from this NetworkVersionInfo object.
   *
   * @return A pointer to the created NetworkGetVersionInfoResponse protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::NetworkGetVersionInfoResponse> toProtobuf() const;

  /**
   * Construct a representative byte array from this NetworkVersionInfo object.
   *
   * @return The byte array representing this NetworkVersionInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this NetworkVersionInfo object.
   *
   * @return The string representation of this NetworkVersionInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this NetworkVersionInfo to an output stream.
   *
   * @param os   The output stream.
   * @param info The NetworkVersionInfo to print.
   * @return The output stream with this NetworkVersionInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const NetworkVersionInfo& info);

  /**
   * The version of the protobuf schema in use by the network.
   */
  SemanticVersion mProtobufVersion;

  /**
   * The version of the Hiero services in use by the network.
   */
  SemanticVersion mServicesVersion;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_NETWORK_VERSION_INFO_H_
