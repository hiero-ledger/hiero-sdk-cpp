// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_ID_H_
#define HIERO_SDK_CPP_ACCOUNT_ID_H_

#include "EvmAddress.h"
#include "PublicKey.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class AccountID;
}

namespace Hiero
{
class Client;
}

namespace Hiero
{
/**
 * The ID for a crypto-currency account on Hiero.
 */
class AccountId
{
public:
  AccountId() = default;

  /**
   * Construct with an account number.
   *
   * @param num The account number.
   */
  explicit AccountId(uint64_t num);

  /**
   * Construct with a PublicKey alias.
   *
   * @param alias The PublicKey alias.
   */
  explicit AccountId(const std::shared_ptr<PublicKey>& alias);

  /**
   * Construct with an EVM address alias.
   *
   * @param address The EVM address alias.
   */
  explicit AccountId(const EvmAddress& address);

  /**
   * Construct with a shard, realm, an account number, and optionally a checksum.
   *
   * @param shard    The shard number.
   * @param realm    The realm number.
   * @param num      The account number.
   * @param checksum The checksum.
   */
  explicit AccountId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum = "");

  /**
   * Construct with shard and realm numbers, and a PublicKey alias.
   *
   * @param shard The shard number.
   * @param realm The realm number.
   * @param alias The PublicKey alias.
   */
  explicit AccountId(uint64_t shard, uint64_t realm, const std::shared_ptr<PublicKey>& alias);

  /**
   * Construct with shard and realm numbers, and an EVM address alias.
   *
   * @param shard   The shard number.
   * @param realm   The realm number.
   * @param address The EVM address alias.
   */
  explicit AccountId(uint64_t shard, uint64_t realm, const EvmAddress& address);

  /**
   * Compare this AccountId to another AccountId and determine if they represent the same account.
   *
   * @param other The other AccountId with which to compare this AccountId.
   * @return \c TRUE if this AccountId is the same as the input AccountId, otherwise \c FALSE.
   */
  bool operator==(const AccountId& other) const;

  /**
   * Construct an AccountId object from a string of the form "<shard>.<realm>.<num>". <num> can be the account number,
   * the stringified PublicKey alias (DER-encoded), or the stringified EVM address.
   *
   * @param id The account ID string from which to construct.
   * @return The constructed AccountId object.
   * @throws std::invalid_argument If the input string is malformed or the type of <num> cannot be determined.
   */
  [[nodiscard]] static AccountId fromString(std::string_view id);

  /**
   * Construct an AccountId object from a string that represents an EvmAddress and, optionally, a shard and realm
   * number.
   *
   * @param evmAddress The string that represents an EvmAddress.
   * @param shard      The shard number.
   * @param realm      The realm number.
   * @return The constructed AccountId object.
   * @throws std::invalid_argument If the input string is malformed.
   * @throws OpenSSLException If OpenSSL is unable to convert the string to a byte array.
   */
  [[nodiscard]] static AccountId fromEvmAddress(std::string_view evmAddress,
                                                uint64_t shard = 0ULL,
                                                uint64_t realm = 0ULL);

  /**
   * Construct an AccountId object from an EvmAddress object and, optionally, a shard and realm number.
   *
   * @param evmAddress The EvmAddress from which to construct an AccountId.
   * @param shard      The shard number.
   * @param realm      The realm number.
   * @return The constructed AccountId object.
   */
  [[nodiscard]] static AccountId fromEvmAddress(const EvmAddress& evmAddress,
                                                uint64_t shard = 0ULL,
                                                uint64_t realm = 0ULL);

  /**
   * Construct an AccountId from a Solidity address.
   *
   * @param address The Solidity address from which to create an AccountId, in a string.
   * @return The constructed AccountId object.
   * @throws std::invalid_argument If a Solidity address cannot be realized from the input string.
   */
  [[nodiscard]] static AccountId fromSolidityAddress(std::string_view address);

  /**
   * Construct an AccountId object from an AccountID protobuf object.
   *
   * @param proto The AccountID protobuf object from which to create an AccountId object.
   * @return The constructed AccountId object.
   */
  [[nodiscard]] static AccountId fromProtobuf(const proto::AccountID& proto);

  /**
   * Construct an AccountId object from a representative byte array.
   *
   * @param bytes The byte array from which to construct an AccountId object.
   * @return The constructed AccountId object.
   */
  [[nodiscard]] static AccountId fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Verify the checksum of this AccountId using the input Client's network. Does nothing if this AccountId does not use
   * an account number (i.e. it contains a PublicKey or EvmAddress alias).
   *
   * @param client The Client with which to validate this AccountId's checksum.
   * @throws BadEntityException If the checksum of this AccountId is invalid.
   */
  void validateChecksum(const Client& client) const;

  /**
   * Construct an AccountID protobuf object from this AccountId object.
   *
   * @return A pointer to the created AccountID protobuf object filled with this AccountId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::AccountID> toProtobuf() const;

  /**
   * Get the Solidity address representation of this AccountId (Long-Zero address form).
   *
   * @return The Solidity address representation of this AccountId.
   * @throws IllegalStateException If this AccountId contains an alias.
   */
  [[nodiscard]] std::string toSolidityAddress() const;

  /**
   * Populates the EVM address for an Account using the Mirror Node. This function fetches the EVM address for an
   * Account from the Mirror Node.
   *
   * User Note: This Function requires a 3 second sleep if running on testnet environment as the MirrorNode does not
   * update on time for the MirrorNode request sent to it.
   *
   * @param client The Client object used for network access.
   * @return A reference to the modified AccountId object.
   * @throws IllegalStateException if mAccountNum is empty or if the account does not exist in the Mirror Network.
   */
  AccountId& populateAccountEvmAddress(const Client& client);

  /**
   * Get the string representation of this AccountId object.
   *
   * @return The string representation of this AccountId.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Get the string representation of this AccountId object with the checksum.
   *
   * @param client The Client with which to generate the checksum.
   * @return The string representation of this AccountId object with the checksum.
   * @throws IllegalStateException If this AccountId contains an alias.
   */
  [[nodiscard]] std::string toStringWithChecksum([[maybe_unused]] const Client& client) const;

  /**
   * Get a byte array representation of this AccountId object.
   *
   * @return A byte array representation of this AccountId object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Get the checksum of this AccountId.
   *
   * @return The checksum of this AccountId.
   */
  [[nodiscard]] inline std::string getChecksum() const { return mChecksum; }

  /**
   * The shard number.
   */
  uint64_t mShardNum = 0ULL;

  /**
   * The realm number.
   */
  uint64_t mRealmNum = 0ULL;

  /**
   * The account ID number.
   */
  std::optional<uint64_t> mAccountNum;

  /**
   * The public key to be used as the account's alias. Currently only primitive key bytes are supported as an alias
   * (ThresholdKey, KeyList, ContractID, and delegatable_contract_id are not supported)
   *
   * At most one account can ever have a given alias and it is used for account creation if it was automatically created
   * using a crypto transfer. It will be nullptr if an account is created normally. It is immutable once it is set for
   * an account.
   *
   * If a transaction auto-creates the account, any further transfers to that alias will simply be deposited in that
   * account, without creating anything, and with no creation fee being charged.
   */
  std::shared_ptr<PublicKey> mPublicKeyAlias = nullptr;

  /**
   * The ethereum account 20-byte EVM address to be used initially in place of the public key bytes. This EVM address
   * may be either the encoded form of the shard.realm.num or the keccak-256 hash of a ECDSA_SECP256K1 primitive key.
   *
   * If a transaction lazily-creates this account, a subsequent transaction will be required containing the public key
   * bytes that map to the EVM address bytes. Lazy account creates will only support the keccak-256 hash of a
   * ECDSA_SECP256K1 primitive key form.
   */
  std::optional<EvmAddress> mEvmAddressAlias;

private:
  /**
   * The checksum of this AccountId.
   */
  mutable std::string mChecksum;
};

} // namespace Hiero

namespace std
{
template<>
struct hash<Hiero::AccountId>
{
  /**
   * Operator override to enable use of AccountId as map key.
   */
  size_t operator()(const Hiero::AccountId& id) const { return hash<string>()(id.toString()); }
};

template<>
struct less<Hiero::AccountId>
{
  /**
   * Operator override to enable use of AccountId in a std::map, which requires fair ordering.
   */
  bool operator()(const Hiero::AccountId& lhs, const Hiero::AccountId& rhs) const
  {
    return lhs.toString() < rhs.toString();
  }
};

} // namespace std

#endif // HIERO_SDK_CPP_ACCOUNT_ID_H_
