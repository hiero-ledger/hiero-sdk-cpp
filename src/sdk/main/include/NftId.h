// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NFT_ID_H_
#define HIERO_SDK_CPP_NFT_ID_H_

#include "TokenId.h"

#include <string>
#include <string_view>

namespace proto
{
class NftID;
}

namespace Hiero
{
/**
 * The ID of a non-fungible token (NFT).
 */
class NftId
{
public:
  NftId() = default;

  /**
   * Construct with a TokenId and a serial number.
   *
   * @param id The token ID to set.
   * @param serialNumber The serial number to set.
   * @throws std::invalid_argument If the serial number is too big (max value is std::numeric_limits<int64_t>::max()).
   */
  explicit NftId(TokenId id, const uint64_t& serialNumber);

  /**
   * Compare this NftId to another NftId and determine if they represent the same token.
   *
   * @param other The other NftId with which to compare this NftId.
   * @return \c TRUE if this NftId is the same as the input NftId, otherwise \c FALSE.
   */
  bool operator==(const NftId& other) const;

  /**
   * Construct an NftId object from a string of the form "<shard>.<realm>.<num>/<serialNum>".
   *
   * @param id The NFT ID string from which to construct.
   * @throws std::invalid_argument If the input string is malformed.
   */
  [[nodiscard]] static NftId fromString(std::string_view id);

  /**
   * Construct an NftId object from a NftID protobuf object.
   *
   * @param proto The NftID protobuf object from which to construct a NftId object.
   * @return The constructed NftId object.
   */
  [[nodiscard]] static NftId fromProtobuf(const proto::NftID& proto);

  /**
   * Construct an NftID protobuf object from this NftId object.
   *
   * @return A pointer to the created NftID protobuf object filled with this NftId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::NftID> toProtobuf() const;

  /**
   * Get the string representation of this NftId object with the form "<shard>.<realm>.<num>/<serialNum>".
   *
   * @return The string representation of this NftId.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * The ID of the non-fungible token of which this NFT is an instance.
   */
  TokenId mTokenId;

  /**
   * The serial number of this instance.
   */
  uint64_t mSerialNum = 0ULL;
};

} // namespace Hiero

namespace std
{
template<>
struct hash<Hiero::NftId>
{
  /**
   * Operator override to enable use of NftId as map key.
   */
  size_t operator()(const Hiero::NftId& id) const { return hash<string>()(id.toString()); }
};

} // namespace std

#endif // HIERO_SDK_CPP_NFT_ID_H_
