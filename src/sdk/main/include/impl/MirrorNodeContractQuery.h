// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_MIRROR_NODE_CONTRACT_QUERY_H_
#define HIERO_SDK_CPP_MIRROR_NODE_CONTRACT_QUERY_H_

#include "AccountId.h"
#include "Client.h"
#include "ContractFunctionParameters.h"
#include "ContractId.h"
#include "impl/HexConverter.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Hiero
{
/**
 * MirrorNodeContractQuery returns a result from EVM execution such as cost-free execution of read-only smart
 * contract queries, gas estimation, and transient simulation of read-write operations.
 */
class MirrorNodeContractQuery
{
public:
  /**
   * Gets the contract ID of the contract for the Mirror Node call.
   *
   * @return The contract ID as an optional string.
   */
  std::optional<ContractId> getContractId() const { return mContractId; }

  /**
   * Sets the contract ID of the contract for the Mirror Node call.
   *
   * @param id The contract ID.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setContractId(const ContractId& id);

  /**
   * Gets the EVM address of the contract for the Mirror Node call.
   *
   * @return The contract EVM address as an optional string.
   */
  std::optional<std::string> getContractEvmAddress() const { return mContractEvmAddress; }

  /**
   * Sets the EVM address of the contract for the Mirror Node call.
   *
   * @param address The contract EVM address.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setContractEvmAddress(const std::string& address);

  /**
   * Gets the sender account ID for the Mirror Node call.
   *
   * @return The sender account ID as an optional string.
   */
  std::optional<AccountId> getSender() const { return mSender; }

  /**
   * Sets the sender account ID for the Mirror Node call.
   *
   * @param id The sender account ID.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setSender(const AccountId& id);

  /**
   * Gets the sender's EVM address for the Mirror Node call.
   *
   * @return The sender's EVM address as an optional string.
   */
  std::optional<std::string> getSenderEvmAddress() const { return mSenderEvmAddress; }

  /**
   * Sets the sender's EVM address for the Mirror Node call.
   *
   * @param address The sender's EVM address.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setSenderEvmAddress(const std::string& address);

  /**
   * Gets the call data for the Mirror Node call.
   *
   * @return The call data as a vector of bytes.
   */
  std::vector<std::byte> getCallData() const { return mCallData; }

  /**
   * Sets the call data for the Mirror Node call.
   *
   * @param data The call data as a vector of bytes.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setFunction(const std::string& functionName,
                                       std::optional<ContractFunctionParameters>& parameters);

  /**
   * Gets the value sent to the contract in the Mirror Node call.
   *
   * @return The value as a 64-bit integer.
   */
  int64_t getValue() const { return mValue; }

  /**
   * Sets the value sent to the contract in the Mirror Node call.
   *
   * @param val The value as a 64-bit integer.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setValue(int64_t val);

  /**
   * Gets the gas limit for the Mirror Node call.
   *
   * @return The gas limit as a 64-bit integer.
   */
  int64_t getGasLimit() const { return mGasLimit; }

  /**
   * Sets the gas limit for the Mirror Node call.
   *
   * @param limit The gas limit as a 64-bit integer.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setGasLimit(int64_t limit);

  /**
   * Gets the gas price for the Mirror Node call.
   *
   * @return The gas price as a 64-bit integer.
   */
  int64_t getGasPrice() const { return mGasPrice; }

  /**
   * Sets the gas price for the Mirror Node call.
   *
   * @param price The gas price as a 64-bit integer.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setGasPrice(int64_t price);

  /**
   * Gets the block number used for the Mirror Node call.
   *
   * @return The block number as a 64-bit unsigned integer.
   */
  uint64_t getBlockNumber() const { return mBlockNumber; }

  /**
   * Sets the block number used for the Mirror Node call.
   *
   * @param number The block number as a 64-bit unsigned integer.
   * @return Reference to the updated object.
   */
  MirrorNodeContractQuery& setBlockNumber(uint64_t number);

  /**
   * Gets the value of estimate.
   *
   * @return A boolean indicating whether the contract call should be estimated.
   */
  bool getEstimate() const { return mEstimate; }

  /**
   * Sets the value if the call should be estimated.
   *
   * @param estimate A boolean indicating whether the contract call should be estimated.
   */
  MirrorNodeContractQuery& setEstimate(bool estimate);

  /**
   * Serializes the query object to a JSON representation.
   *
   * @return A JSON object representing the state of the object.
   */
  json toJson() const;

  /**
   * Executes the Mirror Node query.
   *
   * @param client The Client object used for network access.
   * @return The result of the execution in string format.
   */
  [[nodiscard]] virtual std::string execute(const Client& client) = 0;

protected:
  /**
   * Populates the EVM addresses using the Mirror Node.
   *
   * @param client The Client object used for network access.
   * @return A reference to the modified AccountId object.
   * @throws IllegalStateException if mAccountNum is empty or if the account does not exist in the Mirror Network.
   */
  void populateContractEvmAddress(const Client& client);

  /**
   * The contract ID  of the contract for the Mirror Node call.
   */
  std::optional<ContractId> mContractId;

  /**
   * The EVM address of the contract for the Mirror Node call.
   */
  std::optional<std::string> mContractEvmAddress;

  /**
   * The sender account ID for the Mirror Node call.
   */
  std::optional<AccountId> mSender;

  /**
   * The sender's EVM address for the Mirror Node call.
   */
  std::optional<std::string> mSenderEvmAddress;

  /**
   * The call data for the Mirror Node call.
   */
  std::vector<std::byte> mCallData;

  /**
   * The value sent to the contract for the Mirror Node call.
   */
  int64_t mValue = 0;

  /**
   * The gas limit sent to the contract for the Mirror Node call.
   */
  int64_t mGasLimit = 0;

  /**
   * The gas price sent to the contract for the Mirror Node call.
   */
  int64_t mGasPrice = 0;

  /**
   * The block number sent to the contract for the Mirror Node call.
   */
  uint64_t mBlockNumber = 0;

  /**
   * Should contract call be estimated
   */
  bool mEstimate = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_MIRROR_NODE_CONTRACT_QUERY_H_
