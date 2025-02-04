// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_DATA_H_
#define HIERO_SDK_CPP_FEE_DATA_H_

#include "FeeComponents.h"
#include "FeeDataType.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace proto
{
class FeeData;
}

namespace Hiero
{
/**
 * The total fee charged for a transaction. It is composed of three components - a node fee that compensates the
 * specific node that submitted the transaction, a network fee that compensates the network for assigning the
 * transaction a consensus timestamp, and a service fee that compensates the network for the ongoing maintenance of the
 * consequences of the transaction.
 */
class FeeData
{
public:
  /**
   * Construct an FeeData object from a FeeData protobuf object.
   *
   * @param proto The FeeData protobuf object from which to construct an FeeData object.
   * @return The constructed FeeData object.
   */
  [[nodiscard]] static FeeData fromProtobuf(const proto::FeeData& proto);

  /**
   * Construct an FeeData object from a byte array.
   *
   * @param bytes The byte array from which to construct an FeeData object.
   * @return The constructed FeeData object.
   */
  [[nodiscard]] static FeeData fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a FeeData protobuf object from this FeeData object.
   *
   * @return A pointer to the created FeeData protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::FeeData> toProtobuf() const;

  /**
   * Construct a representative byte array from this FeeData object.
   *
   * @return The byte array representing this FeeData object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this FeeData object.
   *
   * @return The string representation of this FeeData object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Set the node fee data.
   *
   * @param fee The node fee data to set.
   * @return A reference to this FeeData with the newly-set node fee data.
   */
  inline FeeData& setNodeData(const FeeComponents& fee)
  {
    mNodeData = fee;
    return *this;
  }

  /**
   * Set the network fee data.
   *
   * @param fee The network fee data to set.
   * @return A reference to this FeeData with the newly-set network fee data.
   */
  inline FeeData& setNetworkData(const FeeComponents& fee)
  {
    mNetworkData = fee;
    return *this;
  }

  /**
   * Set the service fee data.
   *
   * @param fee The service fee data to set.
   * @return A reference to this FeeData with the newly-set service fee data.
   */
  inline FeeData& setServiceData(const FeeComponents& fee)
  {
    mServiceData = fee;
    return *this;
  }

  /**
   * Set the fee data type.
   *
   * @param type The fee data type to set.
   * @return A reference to this FeeData with the newly-set fee data type.
   */
  inline FeeData& setType(FeeDataType type)
  {
    mType = type;
    return *this;
  }

  /**
   * Get the node fee data.
   *
   * @return The node fee data.
   */
  [[nodiscard]] inline FeeComponents getNodeData() const { return mNodeData; }

  /**
   * Get the network fee data.
   *
   * @return The network fee data.
   */
  [[nodiscard]] inline FeeComponents getNetworkData() const { return mNetworkData; }

  /**
   * Get the service fee data.
   *
   * @return The service fee data.
   */
  [[nodiscard]] inline FeeComponents getServiceData() const { return mServiceData; }

  /**
   * Get the fee data type.
   *
   * @return The fee data type.
   */
  [[nodiscard]] inline FeeDataType getType() const { return mType; }

private:
  /**
   * The components for the fee being paid to the submitting node.
   */
  FeeComponents mNodeData;

  /**
   * The components for the fee being paid to the network for processing a transaction.
   */
  FeeComponents mNetworkData;

  /**
   * The components for the fee being paid to the network for providing the service associated with the transaction.
   */
  FeeComponents mServiceData;

  /**
   * The data type distinguishing between different types of FeeData, correlating to the same Hiero functionality.
   */
  FeeDataType mType = FeeDataType::DEFAULT;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_DATA_H_
