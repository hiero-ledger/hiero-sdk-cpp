// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_LOG_INFO_H_
#define HIERO_SDK_CPP_CONTRACT_LOG_INFO_H_

#include "ContractId.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class ContractLoginfo;
}

namespace Hiero
{
/**
 * The log information for an event returned by a smart contract function call. One function call may return several
 * such events.
 */
class ContractLogInfo
{
public:
  /**
   * Construct a ContractLogInfo object from a ContractLoginfo protobuf object.
   *
   * @param proto The ContractLoginfo protobuf object from which to construct a ContractLogInfo object.
   * @return The constructed ContractLogInfo object.
   */
  [[nodiscard]] static ContractLogInfo fromProtobuf(const proto::ContractLoginfo& proto);

  /**
   * Construct a ContractLogInfo object from a byte array.
   *
   * @param bytes The byte array from which to construct a TransactionRecord object.
   * @return The constructed ContractLogInfo object.
   */
  [[nodiscard]] static ContractLogInfo fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a ContractLoginfo protobuf object from this ContractLogInfo object.
   *
   * @return A pointer to the created ContractLoginfo protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::ContractLoginfo> toProtobuf() const;

  /**
   * Construct a representative byte array from this ContractLogInfo object.
   *
   * @return The byte array representing this ContractLogInfo object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this ContractLogInfo object.
   *
   * @return The string representation of this ContractLogInfo object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this ContractLogInfo to an output stream.
   *
   * @param os      The output stream.
   * @param logInfo The ContractLogInfo to print.
   * @return The output stream with this ContractLogInfo written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const ContractLogInfo& logInfo);

  /**
   * The ID of the contract that emitted this log event.
   */
  ContractId mContractId;

  /**
   * The bloom filter for this particular log event.
   */
  std::vector<std::byte> mBloom;

  /**
   * The topics for this particular log event.
   */
  std::vector<std::vector<std::byte>> mTopics;

  /**
   * The log event data.
   */
  std::vector<std::byte> mData;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_LOG_INFO_H_
