// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_BYTE_CODE_QUERY_H_
#define HIERO_SDK_CPP_CONTRACT_BYTE_CODE_QUERY_H_

#include "ContractId.h"
#include "Query.h"

#include <cstddef>
#include <vector>

namespace Hiero
{
/**
 * A query that returns the bytecode for a smart contract instance. Anyone can request the byte code of a smart contract
 * instance on the network. Queries do not change the state of the smart contract or require network consensus. The
 * information is returned from a single node processing the query.
 */
using ContractByteCode = std::vector<std::byte>;
class ContractByteCodeQuery : public Query<ContractByteCodeQuery, ContractByteCode>
{
public:
  /**
   * Set the ID of the contract of which to request the byte code.
   *
   * @param contractId The ID of the contract of which to request the byte code.
   * @return A reference to this ContractByteCodeQuery object with the newly-set contract ID.
   */
  ContractByteCodeQuery& setContractId(const ContractId& contractId);

  /**
   * Get the ID of the contract of which this query is currently configured to get the byte code.
   *
   * @return The ID of the contract for which this query is meant.
   */
  [[nodiscard]] inline ContractId getContractId() const { return mContractId; }

private:
  /**
   * Derived from Executable. Construct a ContractByteCode object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a ContractByteCode object.
   * @return A ContractByteCode object filled with the Response protobuf object's data
   */
  [[nodiscard]] ContractByteCode mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this ContractByteCodeQuery's data to a Node.
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
   * Derived from Query. Verify that all the checksums in this ContractByteCodeQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ContractByteCodeQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this ContractByteCodeQuery's data, with the input
   * QueryHeader protobuf object.
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
   * The ID of the contract of which this query should get the byte code.
   */
  ContractId mContractId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_BYTE_CODE_QUERY_H_
