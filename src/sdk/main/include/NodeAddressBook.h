// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NODE_ADDRESS_BOOK_H_
#define HIERO_SDK_CPP_NODE_ADDRESS_BOOK_H_

#include "NodeAddress.h"

#include <cstddef>
#include <vector>

namespace proto
{
class NodeAddressBook;
}

namespace Hiero
{
/**
 * An address book containing all known nodes on the network.
 */
class NodeAddressBook
{
public:
  /**
   * Construct a NodeAddressBook object from a NodeAddressBook protobuf object.
   *
   * @param proto The NodeAddressBook protobuf object from which to construct a NodeAddressBook object.
   * @return The constructed NodeAddressBook object.
   */
  [[nodiscard]] static NodeAddressBook fromProtobuf(const proto::NodeAddressBook& proto);

  /**
   * Construct a NodeAddressBook object from a byte array.
   *
   * @param bytes The byte array from which to construct a NodeAddressBook object.
   * @return The constructed NodeAddressBook object.
   */
  [[nodiscard]] static NodeAddressBook fromBytes(const std::vector<char>& bytes);

  /**
   * Construct a NodeAddressBook protobuf object from this NodeAddressBook object.
   *
   * @return A pointer to the created NodeAddressBook protobuf object filled with this NodeAddressBook object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::NodeAddressBook> toProtobuf() const;

  /**
   * Get the byte array representation of this NodeAddressBook.
   *
   * @return The byte array representation of this NodeAddressBook.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Set the list of NodeAddresses in this NodeAddressBook.
   *
   * @param addresses The list of NodeAddresses to set.
   * @return A reference to this NodeAddressBook with the newly-set NodeAddress list.
   */
  NodeAddressBook& setNodeAddresses(const std::vector<NodeAddress>& addresses);

  /**
   * Get the list of NodeAddresses.
   *
   * @return The list of NodeAddresses.
   */
  [[nodiscard]] inline std::vector<NodeAddress> getNodeAddresses() const { return mNodeAddresses; }

private:
  /**
   * The list of NodeAddresses.
   */
  std::vector<NodeAddress> mNodeAddresses;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_NODE_ADDRESS_BOOK_H_
