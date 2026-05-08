// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_QUERY_H_
#define HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_QUERY_H_

#include <cstdint>
#include <optional>

namespace Hiero
{
class Client;
class RegisteredNodeAddressBook;

/**
 * Queries the mirror node for all registered nodes and returns a RegisteredNodeAddressBook.
 * Calls GET /api/v1/network/registered-nodes and follows pagination links until all pages are consumed.
 */
class RegisteredNodeAddressBookQuery
{
public:
  /**
   * Submit this RegisteredNodeAddressBookQuery to a Hiero network.
   *
   * @param client The Client to use to submit this RegisteredNodeAddressBookQuery.
   * @return The RegisteredNodeAddressBook returned by the mirror node.
   */
  [[nodiscard]] RegisteredNodeAddressBook execute(const Client& client) const;

  /**
   * Set the maximum number of registered nodes returned per page.
   *
   * @param limit The page size (default: 25).
   * @return A reference to this RegisteredNodeAddressBookQuery with the newly-set limit.
   */
  RegisteredNodeAddressBookQuery& setLimit(uint32_t limit);

  /**
   * Get the configured page size limit.
   *
   * @return The optional limit, or empty if the default is used.
   */
  [[nodiscard]] const std::optional<uint32_t>& getLimit() const { return mLimit; }

private:
  /**
   * Maximum registered nodes per page. When unset the mirror node default applies.
   */
  std::optional<uint32_t> mLimit;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_NODE_ADDRESS_BOOK_QUERY_H_
