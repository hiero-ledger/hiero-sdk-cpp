// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ADDRESS_BOOK_QUERY_H_
#define HIERO_SDK_CPP_ADDRESS_BOOK_QUERY_H_

#include "Defaults.h"
#include "FileId.h"

#include <chrono>

namespace Hiero
{
class Client;
class NodeAddressBook;
}

namespace com::hedera::mirror::api::proto
{
class AddressBookQuery;
}

namespace Hiero
{
/**
 * A query that returns the current address book being used by the network's consensus nodes.
 */
class AddressBookQuery
{
public:
  /**
   * Submit this AddressBookQuery to a Hiero network.
   *
   * @param client The Client to use to submit this AddressBookQuery.
   * @return The NodeAddressBook object sent from the Hiero network that contains the result of this AddressBookQuery.
   * @throws MaxAttemptsExceededException If this AddressBookQuery attempts to execute past the number of allowable
   *                                      attempts.
   * @throws PrecheckStatusException      If this AddressBookQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  NodeAddressBook execute(const Client& client);

  /**
   * Submit this AddressBookQuery to a Hiero network with a specific timeout.
   *
   * @param client  The Client to use to submit this AddressBookQuery.
   * @param timeout The desired timeout for the execution of this AddressBookQuery.
   * @return The NodeAddressBook object sent from the Hiero network that contains the result of this AddressBookQuery.
   * @throws MaxAttemptsExceededException If this AddressBookQuery attempts to execute past the number of allowable
   *                                      attempts.
   * @throws PrecheckStatusException      If this AddressBookQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  NodeAddressBook execute(const Client& client, const std::chrono::system_clock::duration& timeout);

  /**
   * Set the ID of the file of which to request the address book.
   *
   * @param fileId The ID of the file of which to request the address book.
   * @return A reference to this AddressBookQuery object with the newly-set file ID.
   */
  AddressBookQuery& setFileId(const FileId& fileId);

  /**
   * Set the number of node addresses to retrieve. Set to 0 to retrieve all node addresses.
   *
   * @param limit The number of node addresses to retrieve.
   * @return A reference to this AddressBookQuery object with the newly-set node address limit.
   */
  AddressBookQuery& setLimit(unsigned int limit);

  /**
   * Set the maximum number of attempts to try and execute this AddressBookQuery.
   *
   * @param attempts The maximum number of attempts to try and execute this AddressBookQuery.
   * @return A reference to this AddressBookQuery object with the newly-set maximum attempts.
   */
  AddressBookQuery& setMaxAttempts(unsigned int attempts);

  /**
   * Set the maximum amount of time to wait before attempting to resubmit this AddressBookQuery.
   *
   * @param backoff The maximum amount of time to wait before attempting to resubmit this AddressBookQuery.
   * @return A reference to this Client with the newly-set maximum backoff time.
   * @throws std::invalid_argument If the desired maximum backoff duration is shorter than DEFAULT_MIN_BACKOFF.
   */
  AddressBookQuery& setMaxBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Get the ID of the file of which this query is currently configured to get the address book.
   *
   * @return The ID of the file for which this query is meant.
   */
  [[nodiscard]] inline FileId getFileId() const { return mFileId; }

  /**
   * Get the number of node addresses to retrieve.
   *
   * @return The number of node addresses to retrieve.
   */
  [[nodiscard]] inline unsigned int getLimit() const { return mLimit; }

  /**
   * Get the maximum number of attempts to try and execute this AddressBookQuery.
   *
   * @return The maximum number of attempts to try and execute this AddressBookQuery.
   */
  [[nodiscard]] inline unsigned int getMaxAttempts() const { return mMaxAttempts; }

  /**
   * Get the maximum amount of time to wait before attempting to resubmit this AddressBookQuery.
   *
   * @return The the maximum amount of time to wait before attempting to resubmit this AddressBookQuery.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getMaxBackoff() const { return mMaxBackoff; }

private:
  /**
   * Build an AddressBookQuery protobuf object from this AddressBookQuery.
   *
   * @return The AddressBookQuery protobuf object built from this AddressBookQuery.
   */
  com::hedera::mirror::api::proto::AddressBookQuery build() const;

  /**
   * The ID of the file of which this query should get the address book.
   */
  FileId mFileId;

  /**
   * The number of node addresses to retrieve. 0 retrieves all node addresses.
   */
  unsigned int mLimit = 0U;

  /**
   * The maximum number of attempts to try and execute this AddressBookQuery.
   */
  unsigned int mMaxAttempts = DEFAULT_MAX_ATTEMPTS;

  /**
   * The the maximum amount of time to wait before attempting to resubmit this AddressBookQuery.
   */
  std::chrono::system_clock::duration mMaxBackoff = DEFAULT_MAX_BACKOFF;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ADDRESS_BOOK_QUERY_H_
