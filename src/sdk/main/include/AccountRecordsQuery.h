// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_RECORDS_QUERY_H_
#define HIERO_SDK_CPP_ACCOUNT_RECORDS_QUERY_H_

#include "AccountId.h"
#include "Query.h"

#include <vector>

namespace Hiero
{
class AccountRecords;
class TransactionRecord;
}

namespace Hiero
{
/**
 * Get all the records for an account for any transfers into it and out of it, that were above the threshold, during the
 * last 25 hours.
 */
class AccountRecordsQuery : public Query<AccountRecordsQuery, AccountRecords>
{
public:
  /**
   * Set the ID of the account of which to request the records.
   *
   * @param accountId The ID of the desired account of which to request the records.
   * @return A reference to this AccountRecordsQuery object with the newly-set account ID.
   */
  AccountRecordsQuery& setAccountId(const AccountId& accountId);

  /**
   * Get the ID of the account of which this query is currently configured to get the records.
   *
   * @return The ID of the account for which this query is meant.
   */
  [[nodiscard]] inline AccountId getAccountId() const { return mAccountId; }

private:
  /**
   * Derived from Executable. Construct an AccountRecords object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct an AccountRecords object.
   * @return An AccountRecords object filled with the Response protobuf object's data
   */
  [[nodiscard]] AccountRecords mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this AccountRecordsQuery's data to a Node.
   *
   * @param request  The Query protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Query& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::Response* response) const override;

  /**
   * Derived from Query. Verify that all the checksums in this AccountRecordsQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountRecordsQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this AccountRecordsQuery's data, with the input QueryHeader
   * protobuf object.
   *
   * @param header A pointer to the QueryHeader protobuf object to add to the Query protobuf object.
   * @return The constructed Query protobuf object.
   */
  [[nodiscard]] proto::Query buildRequest(proto::QueryHeader* header) const override;

  /**
   * Derived from Query. Get the ResponseHeader protobuf object from the input Response protobuf object.
   *
   * @param response The Response protobuf object from which to get the ResponseHeader protobuf object.
   * @return The ResponseHeader protobuf object of the input Response protobuf object for this derived Query.
   */
  [[nodiscard]] proto::ResponseHeader mapResponseHeader(const proto::Response& response) const override;

  /**
   * The ID of the account of which this query should get the records.
   */
  AccountId mAccountId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_RECORDS_QUERY_H_
