// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_QUERY_H_
#define HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_QUERY_H_

namespace Hiero
{
class Client;
class RegisteredNodeAddressBook;

/**
 * Queries the mirror node for registered nodes and returns a RegisteredNodeAddressBook.
 *
 * NOTE: The concrete implementation depends on the mirror node API, which is not yet available. The execute() method
 * is a stub and will be implemented when the mirror node API is available.
 */
class RegisteredNodeAddressBookQuery
{
public:
  /**
   * Submit this RegisteredNodeAddressBookQuery to a Hiero network.
   *
   * @param client The Client to use to submit this RegisteredNodeAddressBookQuery.
   * @return The RegisteredNodeAddressBook returned by the mirror node.
   *
   * NOTE: Not yet implemented. Returns an empty RegisteredNodeAddressBook.
   */
  RegisteredNodeAddressBook execute(const Client& client);
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_QUERY_H_
