// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_BALANCE_QUERY_H_
#define HIERO_SDK_CPP_ACCOUNT_BALANCE_QUERY_H_

#include "AccountId.h"
#include "ContractId.h"
#include "Query.h"

#include <optional>

namespace Hiero
{
class AccountBalance;
}

namespace Hiero
{
/**
 * Get the balance of a Hiero crypto-currency account. This returns only the balance, so it is a smaller and faster
 * reply than an AccountInfoQuery.
 *
 * This query is free.
 */
class AccountBalanceQuery : public Query<AccountBalanceQuery, AccountBalance>
{
public:
  /**
   * Set the ID of the account of which to request the balance. This is mutually exclusive with setContractId() and will
   * clear the contract ID if one is already set.
   *
   * @param accountId The ID of the desired account of which to request the balance.
   * @return A reference to this AccountBalanceQuery object with the newly-set account ID.
   */
  AccountBalanceQuery& setAccountId(const AccountId& accountId);

  /**
   * Set the ID of the contract of which to request the balance. This is mutually exclusive with setAccountId() and will
   * clear the account ID if one is already set.
   *
   * @param contractId The ID of the desired contract of which to request the balance.
   * @return A reference to this AccountBalanceQuery object with the newly-set contract ID.
   */
  AccountBalanceQuery& setContractId(const ContractId& contractId);

  /**
   * Get the ID of the account of which this query is currently configured to get the balance.
   *
   * @return The ID of the account for which this query is meant. Returns uninitialized if a value has not yet been set,
   *         or if a contract ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<AccountId> getAccountId() { return mAccountId; }

  /**
   * Get the ID of the contract of which this query is currently configured to get the balance.
   *
   * @return The ID of the contract for which this query is meant. Returns uninitialized if a value has not yet been
   *         set, or if an account ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<ContractId> getContractId() { return mContractId; }

private:
  /**
   * Derived from Executable. Construct an AccountBalance object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct an AccountBalance object.
   * @return An AccountBalance object filled with the Response protobuf object's data
   */
  [[nodiscard]] AccountBalance mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this AccountBalanceQuery's data to a Node.
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
   * Derived from Query. Verify that all the checksums in this AccountBalanceQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountBalanceQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this AccountBalanceQuery's data, with the input QueryHeader
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
   * Derived from Query. Does this AccountBalanceQuery require payment?
   *
   * @return \c FALSE, AccountBalanceQuery is free.
   */
  [[nodiscard]] inline bool isPaymentRequired() const override { return false; }

  /**
   * The ID of the account of which this query should get the balance.
   */
  std::optional<AccountId> mAccountId;

  /**
   * The ID of the contract of which this query should get the balance.
   */
  std::optional<ContractId> mContractId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_BALANCE_QUERY_H_
