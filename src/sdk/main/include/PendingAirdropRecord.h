// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_PENDING_AIRDROP_RECORD_H_
#define HIERO_SDK_CPP_PENDING_AIRDROP_RECORD_H_

#include "PendingAirdropId.h"

namespace proto
{
class PendingAirdropRecord;
class PendingAirdropId;
}

namespace Hiero
{
/**
 * A record of a new pending airdrop.
 */
class PendingAirdropRecord
{
public:
  PendingAirdropRecord() = default;

  /**
   * Construct with two AccountIDs.
   *
   * @param PendingAirdropId The PendingAirdropId.
   * @param amount The amount airdropped.
   */
  explicit PendingAirdropRecord(PendingAirdropId PendingAirdropId, uint64_t amount);

  /**
   * Construct an PendingAirdropRecord object from an PendingAirdropRecord protobuf object.
   *
   * @param proto The PendingAirdropRecord protobuf object from which to create an PendingAirdropRecord object.
   * @return The constructed PendingAirdropRecord object.
   */
  [[nodiscard]] static PendingAirdropRecord fromProtobuf(const proto::PendingAirdropRecord& proto);

  /**
   * Construct an PendingAirdropRecord protobuf object from this PendingAirdropRecord object.
   *
   * @return A pointer to the created PendingAirdropRecord protobuf object filled with this PendingAirdropRecord
   * object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::PendingAirdropRecord> toProtobuf() const;

  /**
   * Get the string representation of this PendingAirdropRecord object.
   *
   * @return The string representation of this PendingAirdropRecord.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * A unique, composite, identifier for a pending airdrop.
   * This field is REQUIRED.
   */
  PendingAirdropId mPendingAirdropId;

  /**
   * A single pending airdrop amount.
   * If the pending airdrop is for a fungible/common token this field is REQUIRED
   * and SHALL be the current amount of tokens offered.
   * If the pending airdrop is for a non-fungible/unique token, this field SHALL NOT
   * be set.
   */
  uint64_t mAmount;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_PENDING_AIRDROP_RECORD_H_