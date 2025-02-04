// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CLIENT_H_
#define HIERO_SDK_CPP_CLIENT_H_

#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Hiero
{
namespace internal
{
class MirrorNetwork;
class Network;
}
class AccountId;
class Hbar;
class LedgerId;
class Logger;
class NodeAddressBook;
class PrivateKey;
class PublicKey;
class SubscriptionHandle;
}

namespace Hiero
{
/**
 * Managed client for use on the Hiero network.
 */
class Client
{
public:
  /**
   * Constructor creates implementation object.
   */
  Client();

  /**
   * Destructor closes all network connections.
   */
  ~Client();

  /**
   * No copying allowed. Underlying connections should be moved instead of copied.
   */
  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;
  Client(Client&&) noexcept;
  Client& operator=(Client&&) noexcept;

  /**
   * Construct a Client pre-configured for a specific Hiero network.
   *
   * @param networkMap The map with string representation of node addresses with their corresponding accountId.
   * @return A reference to this Client object with the newly-set operator account ID from the map.
   */
  [[nodiscard]] static Client forNetwork(const std::unordered_map<std::string, AccountId>& networkMap);

  /**
   * Construct a Client pre-configured for a specific mirror network. This will attempt to pull the network address
   * book from the input mirror network and establish its consensus network with that.
   *
   * @param mirrorNetwork The mirror node network from which to grab the address book and initialize the Client's
   *                      consensus network.
   * @return A Client with the input mirror network and the corresponding address book consensus network
   */
  [[nodiscard]] static Client forMirrorNetwork(const std::vector<std::string>& mirrorNetwork);

  /**
   * Construct a Client by a name. The name must be one of "mainnet", "testnet", or "previewnet", otherwise this will
   * throw std::invalid_argument.
   *
   * @param name The name of the Client to construct.
   * @return A Client object that is set-up to communicate with the input network name.
   */
  [[nodiscard]] static Client forName(std::string_view name);

  /**
   * Construct a Client pre-configured for Hiero Mainnet access.
   *
   * @return A Client object that is set-up to communicate with the Hiero Mainnet.
   */
  [[nodiscard]] static Client forMainnet();

  /**
   * Construct a Client pre-configured for Hiero Testnet access.
   *
   * @return A Client object that is set-up to communicate with the Hiero Testnet.
   */
  [[nodiscard]] static Client forTestnet();

  /**
   * Construct a Client pre-configured for Hiero Previewnet access.
   *
   * @return A Client object that is set-up to communicate with the Hiero Previewnet.
   */
  [[nodiscard]] static Client forPreviewnet();

  /**
   * Construct a Client from a JSON configuration string.
   *
   * @param json The JSON configuration string.
   * @return A Client object initialized with the properties specified in the JSON configuration string.
   */
  [[nodiscard]] static Client fromConfig(std::string_view json);

  /**
   * Construct a Client from a JSON configuration object.
   *
   * @param json The JSON configuration object.
   * @return A Client object initialized with the properties specified in the JSON configuration object.
   */
  [[nodiscard]] static Client fromConfig(const nlohmann::json& json);

  /**
   * Construct a Client from a JSON configuration file.
   *
   * @param path The filepath to the JSON configuration file.
   * @return A Client object initialized with the properties specified in the JSON configuration file.
   */
  [[nodiscard]] static Client fromConfigFile(std::string_view path);

  /**
   * Send a ping to a given node.
   *
   * @param nodeAccountId The ID of the account of the node to ping.
   */
  void ping(const AccountId& nodeAccountId) const;

  /**
   * Send a ping to a given node with a specified timeout.
   *
   * @param nodeAccountId The ID of the account of the node to ping.
   * @param timeout       The desired timeout for the ping.
   */
  void ping(const AccountId& nodeAccountId, const std::chrono::system_clock::duration& timeout) const;

  /**
   * Send a ping to a given node asynchronously.
   *
   * @param nodeAccountId The ID of the account of the node to ping.
   */
  std::future<void> pingAsync(const AccountId& nodeAccountId) const;

  /**
   * Send a ping to a given node asynchronously with a specified timeout.
   *
   * @param nodeAccountId The ID of the account of the node to ping.
   * @param timeout       The desired timeout for the ping.
   */
  std::future<void> pingAsync(const AccountId& nodeAccountId, const std::chrono::system_clock::duration& timeout) const;

  /**
   * Send a ping to a given node asynchronously and consume the exception, if there is one, with a callback.
   *
   * @param nodeAccountId The ID of the account of the node to ping.
   * @param callback      The callback that should consume the exception, if there is one.
   */
  void pingAsync(const AccountId& nodeAccountId, const std::function<void(const std::exception&)>& callback) const;

  /**
   * Send a ping to a given node asynchronously with a specified timeout and consume the exception, if there is one,
   * with a callback.
   *
   * @param nodeAccountId The ID of the account of the node to ping.
   * @param timeout       The desired timeout for the ping.
   * @param callback      The callback that should consume the exception, if there is one.
   */
  void pingAsync(const AccountId& nodeAccountId,
                 const std::chrono::system_clock::duration& timeout,
                 const std::function<void(const std::exception&)>& callback) const;

  /**
   * Send a ping to all consensus nodes on this Client's network.
   */
  void pingAll() const;

  /**
   * Send a ping to all consensus nodes on this Client's network with a specified timeout.
   *
   * @param timeout The desired timeout for the ping.
   */
  void pingAll(const std::chrono::system_clock::duration& timeout) const;

  /**
   * Send a ping to all consensus nodes on this Client's network asynchronously.
   */
  std::future<void> pingAllAsync() const;

  /**
   * Send a ping to all consensus nodes on this Client's network asynchronously with a specified timeout.
   *
   * @param timeout The desired timeout for the ping.
   */
  std::future<void> pingAllAsync(const std::chrono::system_clock::duration& timeout) const;

  /**
   * Send a ping to all consensus nodes on this Client's network asynchronously and consume the exception, if there is
   * one, with a callback.
   *
   * @param callback The callback that should consume the exception, if there is one.
   */
  void pingAllAsync(const std::function<void(const std::exception&)>& callback) const;

  /**
   * Send a ping to all consensus nodes on this Client's network asynchronously with a specified timeout and consume the
   * exception, if there is one, with a callback.
   *
   * @param timeout  The desired timeout for the ping.
   * @param callback The callback that should consume the exception, if there is one.
   */
  void pingAllAsync(const std::chrono::system_clock::duration& timeout,
                    const std::function<void(const std::exception&)>& callback) const;

  /**
   * Set the account that will, by default, be paying for requests submitted by this Client. The operator account ID is
   * used to generate the default transaction ID for all transactions executed with this Client. The operator private
   * key is used to sign all transactions executed by this Client.
   *
   * @param accountId  The account ID of the operator.
   * @param privateKey The private key of the operator.
   * @return A reference to this Client object with the newly-set operator account ID and private key.
   */
  Client& setOperator(const AccountId& accountId, const std::shared_ptr<PrivateKey>& privateKey);

  /**
   * Set the account that will, by default, be paying for requests submitted by this Client. The operator account ID is
   * used to generate the default transaction ID for all transactions executed with this Client. The operator public
   * key and signer function is used to sign all transactions executed by this Client.
   *
   * @param accountId The account ID of the operator.
   * @param publicKey The public key of the operator.
   * @param signer    The function used to sign.
   * @return A reference to this Client object with the newly-set operator account ID, public key, and signer function.
   */
  Client& setOperatorWith(const AccountId& accountId,
                          const std::shared_ptr<PublicKey>& publicKey,
                          const std::function<std::vector<std::byte>(const std::vector<std::byte>&)>& signer);

  /**
   * Get the account ID of this Client's operator.
   *
   * @return The account ID of this Client's operator. Uninitialized if the operator has not been set.
   */
  [[nodiscard]] std::optional<AccountId> getOperatorAccountId() const;

  /**
   * Get the public key of this Client's operator.
   *
   * @return A pointer to the public key of this Client's operator. Nullptr if the operator has not been set.
   */
  [[nodiscard]] std::shared_ptr<PublicKey> getOperatorPublicKey() const;

  /**
   * Get the signer function of this Client's operator.
   *
   * @return The signer function of this Client's operator. Uninitialized if the operator has not been set.
   */
  [[nodiscard]] std::optional<std::function<std::vector<std::byte>(const std::vector<std::byte>&)>> getOperatorSigner()
    const;

  /**
   * Initiate an orderly close of communications with the networks with which this Client was configured to
   * communicate. Preexisting transactions or queries continue but subsequent calls would be immediately cancelled.
   *
   * After this method returns, this Client can be re-used. All network communication can be re-established as needed.
   */
  void close();

  /**
   * Set the maximum transaction fee willing to be paid for transactions executed by this Client. Every request
   * submitted with this Client will have its maximum transaction fee overwritten by this Client's maximum transaction
   * fee if and only if it has not been set manually in the request itself.
   *
   * @param fee The desired maximum transaction fee willing to be paid for transactions submitted by this Client.
   * @return A reference to this Client object with the newly-set maximum transaction fee.
   * @throws std::invalid_argument If the transaction fee is negative.
   */
  Client& setMaxTransactionFee(const Hbar& fee);

  /**
   * Get the maximum transaction fee willing to be paid for transactions submitted by this Client.
   *
   * @return The maximum transaction fee willing to be paid by this Client. Uninitialized if the fee has not been set.
   */
  [[nodiscard]] std::optional<Hbar> getMaxTransactionFee() const;

  /**
   * Set the maximum query payment willing to be paid for transactions executed by this Client. Every request submitted
   * with this Client will have its maximum query payment overwritten by this Client's maximum query payment if and only
   * if it has not been set manually in the request itself.
   *
   * @param payment The desired maximum query payment willing to be paid for queries submitted by this Client.
   * @return A reference to this Client object with the newly-set maximum query payment.
   * @throws std::invalid_argument If the query payment is negative.
   */
  Client& setMaxQueryPayment(const Hbar& payment);

  /**
   * Get the maximum payment willing to be paid for queries submitted by this Client.
   *
   * @return The maximum query payment willing to be paid by this Client. Uninitialized if the fee has not been set.
   */
  [[nodiscard]] std::optional<Hbar> getMaxQueryPayment() const;

  /**
   * Set the transaction ID regeneration policy for transactions executed by this Client. Every transaction submitted
   * with this Client will have its transaction ID regeneration policy overwritten by this Client's transaction ID
   * regeneration policy if and only if it has not been set manually in the request itself.
   *
   * @param regenerate The desired transaction ID regeneration policy for transactions submitted by this Client.
   * @return A reference to this Client object with the newly-set transaction ID regeneration policy.
   */
  Client& setTransactionIdRegenerationPolicy(bool regenerate);

  /**
   * Get the transaction ID regeneration policy for transactions submitted by this Client.
   *
   * @return The default transaction ID regeneration policy for this Client. Uninitialized if the policy has not been
   *         set.
   */
  [[nodiscard]] std::optional<bool> getTransactionIdRegenerationPolicy() const;

  /**
   * Set the automatic entity checksum validation policy.
   *
   * @param validate \c TRUE if this Client should validate entity checksums when submitting Executables, otherwise \c
   *                 FALSE.
   * @return A reference to this Client with the newly-set automatic entity checksum validation policy.
   */
  Client& setAutoValidateChecksums(bool validate);

  /**
   * Is automatic entity checksum validation turned on for this Client?
   *
   * @return \c TRUE if automatic entity checksum validation is turned on, otherwise \c FALSE.
   */
  [[nodiscard]] bool isAutoValidateChecksumsEnabled() const;

  /**
   * Replace the network being used by this Client with nodes contained in an address book.
   *
   * @param addressBook The NodeAddressBook which contains the new nodes to be used by this Client's network.
   * @return A reference to this Client with the newly-set network.
   */
  Client& setNetworkFromAddressBook(const NodeAddressBook& addressBook);

  /**
   * Set the consensus network with which this Client should communicate.
   *
   * @param network The map IPs and ports to the account IDs of the consensus nodes with which this Client should
   *                communicate.
   * @return A reference to this Client object with the newly-set consensus network.
   */
  Client& setNetwork(const std::unordered_map<std::string, AccountId>& network);

  /**
   * Get the map of consensus nodes on this Client's network.
   *
   * @return The map of consensus nodes on this Client's network.
   */
  [[nodiscard]] std::unordered_map<std::string, AccountId> getNetwork() const;

  /**
   * Set the mirror network with which this Client should communicate.
   *
   * @param network The list of IPs and ports of the mirror nodes with which this Client should communicate.
   * @return A reference to this Client object with the newly-set mirror network.
   */
  Client& setMirrorNetwork(const std::vector<std::string>& network);

  /**
   * Get the list of mirror nodes on this Client's network.
   *
   * @return The list of mirror nodes on this Client's network.
   */
  [[nodiscard]] std::vector<std::string> getMirrorNetwork() const;

  /**
   * Set the Logger to be used by this Client.
   *
   * @param logger The Logger to be used by this Client.
   * @return A reference to this Client with the newly-set logger.
   */
  Client& setLogger(const Logger& logger);

  /**
   * Get the Logger used by this Client.
   *
   * @return The Logger used by this Client.
   */
  [[nodiscard]] Logger getLogger() const;

  /**
   * Set the period of time this Client wait between updating its network. This will immediately cancel any scheduled
   * network updates and start a new waiting period.
   *
   * @param update The period of time this Client wait between updating its network.
   * @return A reference to this Client with the newly-set network update period.
   */
  Client& setNetworkUpdatePeriod(const std::chrono::system_clock::duration& update);

  /**
   * Get the period of time this Client wait between updating its network.
   *
   * @return The period of time this Client wait between updating its network.
   */
  [[nodiscard]] std::chrono::system_clock::duration getNetworkUpdatePeriod() const;

  /**
   * Set the ID of the ledger of this Client's Network. Useful when constructing a Network which is a subset of an
   * existing known network. Does nothing if the Client's Network is not yet initialized.
   *
   * @param ledgerId The LedgerId to set.
   * @return A reference to this Client with the newly-set ledger ID.
   */
  Client& setLedgerId(const LedgerId& ledgerId);

  /**
   * Get the ID of the ledger on which this Client's Network is running.
   *
   * @returns The ID of the ledger on which this Client's Network is running.
   * @throws UninitializedException If this Client does not have a Network initialized.
   */
  [[nodiscard]] LedgerId getLedgerId() const;

  /**
   * Set this Client's consensus node network's TLS policy.
   *
   * @param enable \c TRUE to enable TLS for consensus nodes, otherwise \c FALSE.
   * @return A reference to this Client with the newly-set TLS policy.
   */
  Client& setTransportSecurity(bool enable);

  /**
   * Is TLS currently enabled for this Client's consensus nodes?
   *
   * @return \c TRUE if TLS is currently enabled, otherwise \c FALSE.
   */
  [[nodiscard]] bool isTransportSecurity() const;

  /**
   * Set this Client's consensus node network's certificate verification policy.
   *
   * @param verify \c TRUE to enable certification verification for consensus nodes, otherwise \c FALSE.
   * @return A reference to this Client with the newly-set certificate verification policy.
   */
  Client& setVerifyCertificates(bool verify);

  /**
   * Is certificate verification enabled?
   *
   * @return \c TRUE if certification verification is enabled, otherwise \c FALSE.
   */
  [[nodiscard]] bool isVerifyCertificates() const;

  /**
   * Set the length of time a request sent by this Client can be processed before it times out.
   *
   * @param timeout The desired timeout for requests submitted by this Client.
   * @return A reference to this Client object with the newly-set request timeout.
   */
  Client& setRequestTimeout(const std::chrono::system_clock::duration& timeout);

  /**
   * Get the length of time a request sent by this Client can be processed before it times out.
   *
   * @return The request timeout duration.
   */
  [[nodiscard]] std::chrono::system_clock::duration getRequestTimeout() const;

  /**
   * Set the maximum number of times this Client is willing to resubmit failed requests before considering the execution
   * of the request a failure. Every request submitted with this Client will have its maximum number of attempts
   * overwritten by this Client's maximum number of attempts if and only if it has not been set manually in the request
   * itself.
   *
   * @param attempts The desired maximum number of execution attempts for requests submitted by this Client.
   * @return A reference to this Client with the newly-set maximum number of execution attempts.
   */
  Client& setMaxAttempts(unsigned int attempts);

  /**
   * Get the maximum number of times this Client is willing to resubmit failed requests before considering the execution
   * of the request a failure.
   *
   * @return The maximum number of execution attempts. Uninitialized value if not previously set.
   */
  [[nodiscard]] std::optional<unsigned int> getMaxAttempts() const;

  /**
   * Set the minimum amount of time this Client should wait before attempting to resubmit a previously failed request to
   * the network. Every request submitted with this Client will have its minimum backoff time overwritten by this
   * Client's minimum backoff time if and only if it has not been set manually in the request itself.
   *
   * @param backoff The desired minimum amount of time this Client should wait before attempting to resubmit a
   *                previously failed to a particular node.
   * @return A reference to this Client with the newly-set minimum backoff time.
   * @throws std::invalid_argument If the desired minimum backoff duration is longer than the set maximum backoff time
   *                               (DEFAULT_MAX_BACKOFF if the maximum backoff time has not been set).
   */
  Client& setMinBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Get the minimum amount of time this Client should wait before attempting to resubmit a previously failed request to
   * the network.
   *
   * @return The minimum backoff time for Nodes for unsuccessful requests sent by this Client. Uninitialized value if
   *         not previously set.
   */
  [[nodiscard]] std::optional<std::chrono::system_clock::duration> getMinBackoff() const;

  /**
   * Set the maximum amount of time this Client should wait before attempting to resubmit a previously failed request to
   * the network. Every request submitted with this Client will have its maximum backoff time overwritten by this
   * Client's maximum backoff time if and only if it has not been set manually in the request itself.
   *
   * @param backoff The desired maximum amount of time requests submitted by this Client should wait before attempting
   *                to resubmit themselves to a particular node.
   * @return A reference to this Client with the newly-set maximum backoff time.
   * @throws std::invalid_argument If the desired maximum backoff duration is shorter than the set minimum backoff time
   *                               (DEFAULT_MIN_BACKOFF if the minimum backoff time has not been set).
   */
  Client& setMaxBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Get the maximum amount of time this Client should wait before attempting to resubmit a previously failed request to
   * the network.
   *
   * @return The maximum backoff time for Nodes for unsuccessful requests sent by this Client. Uninitialized value if
   *         not previously set.
   */
  [[nodiscard]] std::optional<std::chrono::system_clock::duration> getMaxBackoff() const;

  /**
   * Set the maximum amount of time this Client should spend trying to execute a request before giving up on that
   * request attempt. Every request submitted with this Client will have its gRPC deadline overwritten by this Client's
   * gRPC deadline if and only if it has not been set manually in the request itself.
   *
   * @param deadline The desired maximum amount of time this requests submitted by this Client should spend trying to
   *                 execute.
   * @return A reference to this Client with the newly-set gRPC deadline.
   */
  Client& setGrpcDeadline(const std::chrono::system_clock::duration& deadline);

  /**
   * Get the maximum amount of time this Client should spend trying to execute a request before giving up on that
   * request attempt.
   *
   * @return The maximum amount of time this requests submitted by this Client should spend trying to execute.
   *         Uninitialized value if not previously set.
   */
  [[nodiscard]] std::optional<std::chrono::system_clock::duration> getGrpcDeadline() const;

  /**
   * Set the maximum number of times any node in the network can received a bad gRPC status before being removed from
   * the network.
   *
   * @param attempts The number of times any node can receive a bad gRPC status before being removed from the network.
   * @return A reference to this Client with the newly-set max node attempts.
   */
  Client& setMaxNodeAttempts(unsigned int attempts);

  /**
   * Get the maximum number of times any node in the network can received a bad gRPC status before being removed from
   * the network.
   *
   * @return The maximum number of times any node in the network can received a bad gRPC status before being removed
   *         from the network.
   */
  [[nodiscard]] unsigned int getMaxNodeAttempts() const;

  /**
   * Set the minimum backoff time for nodes in this Client's network.
   *
   * @param backoff The desired minimum backoff time to set for nodes in this Client's network.
   * @return A reference to this Client with the newly-set minimum node backoff time.
   */
  Client& setNodeMinBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Get the minimum backoff time for nodes in this Client's network.
   *
   * @return The minimum backoff time to set for nodes in this Client's network.
   */
  [[nodiscard]] std::chrono::system_clock::duration getNodeMinBackoff() const;

  /**
   * Set the maximum backoff time for nodes in this Client's network.
   *
   * @param backoff The desired maximum backoff time to set for nodes in this Client's network.
   * @return A reference to this Client with the newly-set maximum node backoff time.
   */
  Client& setNodeMaxBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Get the maximum backoff time for nodes in this Client's network.
   *
   * @return The maximum backoff time to set for nodes in this Client's network.
   */
  [[nodiscard]] std::chrono::system_clock::duration getNodeMaxBackoff() const;

  /**
   * Set the minimum amount of time for a node to wait after it receives a bad gRPC status for it to be deemed
   * "healthy".
   *
   * @param time The desired minimum amount of time for a node to wait before it's considered "healthy" again.
   * @return A reference to this Client with the newly-set minimum node readmit time.
   */
  Client& setMinNodeReadmitTime(const std::chrono::system_clock::duration& time);

  /**
   * Get the minimum amount of time for a node to wait after it receives a bad gRPC status for it to be deemed
   * "healthy".
   *
   * @return The minimum amount of time for a node to wait before it's considered "healthy" again.
   */
  [[nodiscard]] std::chrono::system_clock::duration getMinNodeReadmitTime() const;

  /**
   * Set the maximum amount of time for a node to wait after it receives a bad gRPC status for it to be deemed
   * "healthy".
   *
   * @param time The desired maximum amount of time for a node to wait before it's considered "healthy" again.
   * @return A reference to this Client with the newly-set maximum node readmit time.
   */
  Client& setMaxNodeReadmitTime(const std::chrono::system_clock::duration& time);

  /**
   * Get the maximum amount of time for a node to wait after it receives a bad gRPC status for it to be deemed
   * "healthy".
   *
   * @return The maximum amount of time for a node to wait before it's considered "healthy" again.
   */
  [[nodiscard]] std::chrono::system_clock::duration getMaxNodeReadmitTime() const;

  /**
   * Set the maximum number of nodes to be chosen per request. By default, this Client will select 1/3 of the total
   * available nodes per request.
   *
   * @param maxNodes The desired maximum number of nodes this Client should use per request.
   * @return A reference to this Client with the newly-set maximum nodes per request.
   */
  Client& setMaxNodesPerTransaction(unsigned int maxNodes);

  /**
   * Get the maximum number of nodes to be chosen per request. By default, this Client will select 1/3 of the total
   * available nodes per request.
   *
   * @return The maximum number of nodes this Client should use per request.
   */
  [[nodiscard]] unsigned int getMaxNodesPerTransaction() const;

  /**
   * Add a subscription for this Client to track.
   *
   * @param subscription The subscription for this Client to track.
   */
  void trackSubscription(const std::shared_ptr<SubscriptionHandle>& subscription) const;

  /**
   * Remove a subscription from this Client to track.
   *
   * @param subscription The subscription for this Client to untrack.
   */
  void untrackSubscription(const std::shared_ptr<SubscriptionHandle>& subscription) const;

  /**
   * Get a pointer to the Network this Client is using to communicate with consensus nodes.
   *
   * @return A pointer to the Network this Client is using to communicate with consensus nodes.
   */
  [[nodiscard]] std::shared_ptr<internal::Network> getClientNetwork() const;

  /**
   * Get a pointer to the MirrorNetwork this Client is using to communicate with mirror nodes.
   *
   * @return A pointer to the MirrorNetwork this Client is using to communicate with mirror nodes.
   */
  [[nodiscard]] std::shared_ptr<internal::MirrorNetwork> getClientMirrorNetwork() const;

private:
  /**
   * Replace the network being used by this Client with nodes contained in an address book.
   *
   * @param addressBook The NodeAddressBook which contains the new nodes to be used by this Client's network.
   */
  void setNetworkFromAddressBookInternal(const NodeAddressBook& addressBook);

  /**
   * Start the network update thread.
   *
   * @param period The period of time to wait before a network update is performed.
   */
  void startNetworkUpdateThread(const std::chrono::system_clock::duration& period);

  /**
   * Schedule a network update.
   */
  void scheduleNetworkUpdate();

  /**
   * Cancel any scheduled network updates.
   */
  void cancelScheduledNetworkUpdate();

  /**
   * Helper function used for moving a Client implementation into this Client, as well as doing network update thread
   * handling.
   *
   * @param other The Client to move into this Client.
   */
  void moveClient(Client&& other);

  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct ClientImpl;
  std::unique_ptr<ClientImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CLIENT_H_
