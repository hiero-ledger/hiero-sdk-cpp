// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_DELEGATE_CONTRACT_ID_H_
#define HIERO_SDK_CPP_DELEGATE_CONTRACT_ID_H_

#include "ContractId.h"

#include <string_view>

namespace proto
{
class ContractID;
}

namespace Hiero
{
/**
 * A smart contract that, if the recipient of the active message frame, should be treated as having signed. (Note this
 * does not mean the code being executed in the frame code being executed in the frame delegatecall. So setting this key
 * is a more permissive version of setting the contractId key, which also requires the code in the active message frame
 * belong to the contract with the given id. The delegate contract ID can be set as Key.
 */
class DelegateContractId : public ContractId
{
public:
  DelegateContractId() = default;

  /**
   * Construct with a contract number.
   *
   * @param num The contract number.
   */
  explicit DelegateContractId(uint64_t num);

  /**
   * Construct with a shard, realm, a contract number, and optionally a checksum.
   *
   * @param shard    The shard number.
   * @param realm    The realm number.
   * @param num      The account number.
   * @param checksum The checksum.
   */
  explicit DelegateContractId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum = "");

  /**
   * Compare this DelegateContractId to another DelegateContractId and determine if they represent the same contract.
   *
   * @param other The other DelegateContractId with which to compare this DelegateContractId.
   * @return \c TRUE if this DelegateContractId is the same as the input DelegateContractId, otherwise \c FALSE.
   */
  [[nodiscard]] bool operator==(const DelegateContractId& other) const;

  /**
   * Construct a DelegateContractId object from a string of the form "<shard>.<realm>.<num>".
   *
   * @param id The contract ID string from which to construct.
   * @return The constructed DelegateContractId object.
   */
  [[nodiscard]] static DelegateContractId fromString(std::string_view id);

  /**
   * Construct a DelegateContractId from a Solidity address.
   *
   * @param address The Solidity address from which to create a DelegateContractId, as a string.
   * @return The constructed DelegateContractId object.
   * @throws std::invalid_argument If a Solidity address cannot be realized from the input string.
   */
  [[nodiscard]] static DelegateContractId fromSolidityAddress(std::string_view address);

  /**
   * Construct an DelegateContractId object from an DelegateContractId protobuf object.
   *
   * @param proto The DelegateContractId protobuf object from which to create an DelegateContractId object.
   * @return The constructed DelegateContractId object.
   */
  [[nodiscard]] static DelegateContractId fromProtobuf(const proto::ContractID& id);

  /**
   * Construct a DelegateContractId object from a representative byte array.
   *
   * @param bytes The byte array from which to construct a DelegateContractId object.
   * @return The constructed ContractId object.
   */
  [[nodiscard]] static DelegateContractId fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Derived from Key. Create a clone of this DelegateContractId object.
   *
   * @return A pointer to the created clone of this DelegateContractId.
   */
  [[nodiscard]] std::unique_ptr<Key> clone() const override;

  /**
   * Derived from Key. Construct a Key protobuf object from this DelegateContractId object.
   *
   * @return A pointer to the created Key protobuf object filled with this DelegateContractId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::Key> toProtobufKey() const override;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_DELEGATE_CONTRACT_ID_H_
