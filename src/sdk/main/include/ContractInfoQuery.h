// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_INFO_QUERY_H_
#define HIERO_SDK_CPP_CONTRACT_INFO_QUERY_H_

#include "ContractId.h"
#include "Query.h"

namespace Hiero
{
class ContractInfo;
}

namespace Hiero
{
/**
 * A query that returns the current state of a smart contract instance, including its balance. Queries do not change the
 * state of the smart contract or require network consensus. The information is returned from a single node processing
 * the query.
 *
 * Query Signing Requirements:
 *  - The client operator account's private key (fee payer) is required to sign this query.
 */
class ContractInfoQuery : public Query<ContractInfoQuery, ContractInfo>
{
public:
  /**
   * Set the ID of the contract of which to request the info.
   *
   * @param contractId The ID of the contract of which to request the info.
   * @return A reference to this ContractInfoQuery object with the newly-set account ID.
   */
  ContractInfoQuery& setContractId(const ContractId& contractId);

  /**
   * Get the ID of the contract of which this query is currently configured to get the info.
   *
   * @return The ID of the contract for which this query is meant.
   */
  [[nodiscard]] inline ContractId getContractId() { return mContractId; }

private:
  /**
   * Derived from Executable. Construct a ContractInfo object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a ContractInfo object.
   * @return A ContractInfo object filled with the Response protobuf object's data
   */
  [[nodiscard]] ContractInfo mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this ContractInfoQuery's data to a Node.
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
   * Derived from Query. Verify that all the checksums in this ContractInfoQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ContractInfoQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this ContractInfoQuery's data, with the input QueryHeader
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
   * The ID of the contract of which this query should get the info.
   */
  ContractId mContractId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_INFO_QUERY_H_
