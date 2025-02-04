// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_INFO_QUERY_H_
#define HIERO_SDK_CPP_ACCOUNT_INFO_QUERY_H_

#include "AccountId.h"
#include "Query.h"

namespace Hiero
{
class AccountInfo;
class TransactionRecord;
}

namespace Hiero
{
/**
 * A query that returns the current state of the account. This query does not include the list of records associated
 * with the account. Anyone on the network can request account info for a given account. Queries do not change the state
 * of the account or require network consensus. The information is returned from a single node processing the query.
 */
class AccountInfoQuery : public Query<AccountInfoQuery, AccountInfo>
{
public:
  /**
   * Set the ID of the account of which to request the info.
   *
   * @param accountId The ID of the desired account of which to request the info.
   * @return A reference to this AccountInfoQuery object with the newly-set account ID.
   */
  AccountInfoQuery& setAccountId(const AccountId& accountId);

  /**
   * Get the ID of the account of which this query is currently configured to get the info.
   *
   * @return The ID of the account for which this query is meant.
   */
  [[nodiscard]] inline AccountId getAccountId() const { return mAccountId; }

private:
  /**
   * Derived from Executable. Construct an AccountInfo object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct an AccountInfo object.
   * @return An AccountInfo object filled with the Response protobuf object's data
   */
  [[nodiscard]] AccountInfo mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this AccountInfoQuery's data to a Node.
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
   * Derived from Query. Verify that all the checksums in this AccountInfoQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountInfoQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this AccountInfoQuery's data, with the input QueryHeader
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
   * The ID of the account of which this query should get the info.
   */
  AccountId mAccountId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_INFO_QUERY_H_
