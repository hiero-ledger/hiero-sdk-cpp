// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_CALL_QUERY_H_
#define HIERO_SDK_CPP_CONTRACT_CALL_QUERY_H_

#include "AccountId.h"
#include "ContractFunctionParameters.h"
#include "ContractId.h"
#include "Query.h"

#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace Hiero
{
class ContractFunctionResult;
}

namespace Hiero
{
/**
 * A query that calls a function of the given smart contract instance, giving it function parameters as its inputs. This
 * is performed locally on the particular node that the client is communicating with. It cannot change the state of the
 * contract instance (and so, cannot spend anything from the instance's cryptocurrency account). It will not have a
 * consensus timestamp. It cannot generate a record or a receipt. The response will contain the output returned by the
 * function call. This is useful for calling getter functions, which purely read the state and don't change it. It is
 * faster and cheaper than a normal call because it is purely local to a single node.
 *
 * Unlike a contract execution transaction, the node will consume the entire amount of provided gas in determining the
 * fee for this query.
 */
class ContractCallQuery : public Query<ContractCallQuery, ContractFunctionResult>
{
public:
  /**
   * Set the function name to call. Optionally, function parameters can be passed as well.
   *
   * @param name       The name of the function to call.
   * @param parameters The parameters to pass to the function.
   * @return A reference to this ContractCallQuery object with the newly-set function name and optional function
   *         parameters.
   */
  ContractCallQuery& setFunction(std::string_view name,
                                 const ContractFunctionParameters& parameters = ContractFunctionParameters());

  /**
   * Set the ID of the contract from which to call a function.
   *
   * @param contractId The ID of the contract from which to call a function.
   * @return A reference to this ContractCallQuery object with the newly-set contract ID.
   */
  ContractCallQuery& setContractId(const ContractId& contractId);

  /**
   * Set the amount of gas to use for the call. All of the gas offered will be used and charged a corresponding fee.
   *
   * @param gas The amount of gas to use for the call.
   * @return A reference to this ContractCallQuery object with the newly-set gas.
   */
  ContractCallQuery& setGas(const uint64_t& gas);

  /**
   * Set the function parameters for the function call.
   *
   * @param functionParameters The function parameters for the function call.
   * @return A reference to this ContractCallQuery object with the newly-set function parameters.
   */
  ContractCallQuery& setFunctionParameters(const std::vector<std::byte>& functionParameters);

  /**
   * Set the ID of the account to be used as the "sender" for the function call.
   *
   * @param sender The ID of the account to be used as the "sender" for the function call.
   * @return A reference to this ContractCallQuery object with the newly-set sender account ID.
   */
  ContractCallQuery& setSenderAccountId(const AccountId& accountId);

  /**
   * Get the ID of the contract from which this query is currently configured to call a function.
   *
   * @return The ID of the contract from which this query is currently configured to call a function.
   */
  [[nodiscard]] inline ContractId getContractId() const { return mContractId; }

  /**
   * Get the amount of gas this query is currently configured to use for the function call.
   *
   * @return The amount of gas this query is currently configured to use for the function call.
   */
  [[nodiscard]] inline uint64_t getGas() const { return mGas; }

  /**
   * Get the function parameters this query is currently configured to use for the function call.
   *
   * @return The function parameters this query is currently configured to use for the function call.
   */
  [[nodiscard]] inline std::vector<std::byte> getFunctionParameters() const { return mFunctionParameters; }

  /**
   * Get the ID of the account this query is currently configured to use as the sender.
   *
   * @return The ID of the account this query is currently configured to use as the sender. Uninitialized if no sender
   *         account ID has been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getSenderAccountId() const { return mSenderAccountId; }

private:
  /**
   * Derived from Executable. Construct a ContractFunctionResult object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a ContractFunctionResult object.
   * @return A ContractFunctionResult object filled with the Response protobuf object's data
   */
  [[nodiscard]] ContractFunctionResult mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this ContractCallQuery's data to a Node.
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
   * Derived from Query. Verify that all the checksums in this ContractCallQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ContractCallQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this ContractCallQuery's data, with the input QueryHeader
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
   * The ID of the contract from which this query should call a function.
   */
  ContractId mContractId;

  /**
   * The amount of gas this query should use for the function call.
   */
  uint64_t mGas = 0ULL;

  /**
   * The function parameters this query should use for the function call.
   */
  std::vector<std::byte> mFunctionParameters;

  /**
   * The ID of the account this query should use as the sender.
   */
  std::optional<AccountId> mSenderAccountId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_CALL_QUERY_H_
