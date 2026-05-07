// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_H_
#define HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_H_

#include "RegisteredNode.h"

#include <string>
#include <vector>

namespace Hiero
{
/**
 * A collection of registered nodes returned by a RegisteredNodeAddressBookQuery. Follows the same pattern as
 * NodeAddressBook.
 */
class RegisteredNodeAddressBook
{
public:
  /**
   * Construct a string representation of this RegisteredNodeAddressBook object.
   *
   * @return The string representation of this RegisteredNodeAddressBook object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * The registered nodes in this address book.
   */
  std::vector<RegisteredNode> mRegisteredNodes;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_H_
