// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_NODE_H_
#define HIERO_SDK_CPP_IMPL_NODE_H_

#include <address_book_service.grpc.pb.h>
#include <consensus_service.grpc.pb.h>
#include <crypto_service.grpc.pb.h>
#include <file_service.grpc.pb.h>
#include <freeze_service.grpc.pb.h>
#include <network_service.grpc.pb.h>
#include <schedule_service.grpc.pb.h>
#include <smart_contract_service.grpc.pb.h>
#include <token_service.grpc.pb.h>
#include <util_service.grpc.pb.h>

#include "AccountId.h"
#include "BaseNode.h"

#include <cstddef>
#include <memory>
#include <string_view>
#include <vector>

namespace Hiero::internal
{
class BaseNodeAddress;
}

namespace Hiero::internal
{
/**
 * Internal utility class used to represent a node on a Hiero network.
 */
class Node : public BaseNode<Node, AccountId>
{
public:
  /**
   * Construct with the AccountId that's running this Node and the address of the Node.
   *
   * @param accountId The AccountId that runs the remote node represented by this Node.
   * @param address   The address of the Node.
   */
  explicit Node(AccountId accountId, const BaseNodeAddress& address);

  /**
   * Construct with the AccountId that's running this Node and the address of the Node as a string.
   *
   * @param accountId The AccountId that runs the remote node represented by this Node.
   * @param address   The address of the Node as a string.
   */
  explicit Node(const AccountId& accountId, std::string_view address);

  /**
   * Submit a Query protobuf to the remote node with which this Node is communicating.
   *
   * @param funcEnum The enumeration specifying which gRPC function to call for this specific Query.
   * @param query    The Query protobuf object to send.
   * @param deadline The deadline for submitting this Query.
   * @param response Pointer to the Response protobuf object to fill with the gRPC server's response.
   * @return The gRPC status response of the function call from the gRPC server.
   */
  grpc::Status submitQuery(proto::Query::QueryCase funcEnum,
                           const proto::Query& query,
                           const std::chrono::system_clock::time_point& deadline,
                           proto::Response* response);

  /**
   * Submit a Transaction protobuf to the remote node with which this Node is communicating.
   *
   * @param funcEnum    The enumeration specifying which gRPC function to call for this specific Transaction.
   * @param transaction The Transaction protobuf object to send.
   * @param deadline    The deadline for submitting this Transaction.
   * @param response    Pointer to the TransactionResponse protobuf object to fill with the gRPC server's response.
   * @return The gRPC status response of the function call from the gRPC server.
   */
  grpc::Status submitTransaction(proto::TransactionBody::DataCase funcEnum,
                                 const proto::Transaction& transaction,
                                 const std::chrono::system_clock::time_point& deadline,
                                 proto::TransactionResponse* response);

  /**
   * Construct an insecure version of this Node. This will close the Node's current connection.
   *
   * @return A reference to this Node, now using an insecure connection.
   */
  Node& toInsecure();

  /**
   * Construct a secure version of this Node. This will close the Node's current connection.
   *
   * @return A reference to this Node, now using a secure connection.
   */
  Node& toSecure();

  /**
   * Set the node certificate hash of this Node.
   *
   * @param hash The node certificate hash to set.
   * @return A reference to this Node with the newly-set node certificate hash.
   */
  Node& setNodeCertificateHash(const std::vector<std::byte>& hash);

  /**
   * Set the certificate verification policy of this Node.
   *
   * @param verify \c TRUE if this Node should verify certificates, otherwise \c FALSE.
   * @return A reference to this Node with the newly-set certificate verification policy.
   */
  Node& setVerifyCertificates(bool verify);

  /**
   * Derived from BaseNode. Get this Node's key, which is its AccountId.
   *
   * @return This Node's key, which is its AccountId.
   */
  [[nodiscard]] inline AccountId getKey() const override { return getAccountId(); };

  /**
   * Get the AccountId of this Node.
   *
   * @return The AccountId of this Node.
   */
  [[nodiscard]] inline AccountId getAccountId() const
  {
    std::unique_lock lock(*getLock());
    return mAccountId;
  };

  /**
   * Get the node certificate hash of this Node.
   *
   * @return The node certificate hash of this Node.
   */
  [[nodiscard]] inline std::vector<std::byte> getNodeCertificateHash() const
  {
    std::unique_lock lock(*getLock());
    return mNodeCertificateHash;
  }

  /**
   * Get the certificate verification policy of this Node.
   *
   * @return \c TRUE if this Node is currently configured to verify certificates, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getVerifyCertificates() const
  {
    std::unique_lock lock(*getLock());
    return mVerifyCertificates;
  }

private:
  /**
   * Construct from another Node and a BaseNodeAddress.
   *
   * @param node    The Node from which to construct.
   * @param address The address of the Node.
   */
  explicit Node(const Node& node, const BaseNodeAddress& address);

  /**
   * Derived from BaseNode. Get the TLS credentials of this Node's gRPC channel.
   *
   * @return A pointer to the TLS credentials for this Node's gRPC channel.
   */
  [[nodiscard]] std::shared_ptr<grpc::ChannelCredentials> getTlsChannelCredentials() const override;

  /**
   * Derived from BaseNode. Initialize the stubs in this Node with this Node's gRPC channel.
   */
  void initializeStubs() override;

  /**
   * Derived from BaseNode. Close the stubs in this Node.
   */
  void closeStubs() override;

  /**
   * Pointer to the gRPC stub used to communicate with the consensus service living on the remote node.
   */
  std::unique_ptr<proto::ConsensusService::Stub> mConsensusStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the cryptography service living on the remote node.
   */
  std::unique_ptr<proto::CryptoService::Stub> mCryptoStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the file service living on the remote node.
   */
  std::unique_ptr<proto::FileService::Stub> mFileStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the freeze service living on the remote node.
   */
  std::unique_ptr<proto::FreezeService::Stub> mFreezeStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the network service living on the remote node.
   */
  std::unique_ptr<proto::NetworkService::Stub> mNetworkStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the schedule service living on the remote node.
   */
  std::unique_ptr<proto::ScheduleService::Stub> mScheduleStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the smart contract service living on the remote node.
   */
  std::unique_ptr<proto::SmartContractService::Stub> mSmartContractStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the token service living on the remote node.
   */
  std::unique_ptr<proto::TokenService::Stub> mTokenStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the utility service living on the remote node.
   */
  std::unique_ptr<proto::UtilService::Stub> mUtilStub = nullptr;

  /**
   * Pointer to the gRPC stub used to communicate with the address book service living on the remote node.
   */
  std::unique_ptr<proto::AddressBookService::Stub> mAddressBookStub = nullptr;

  /**
   * The AccountId that runs the remote node represented by this Node.
   */
  AccountId mAccountId;

  /**
   * The node certificate hash of this Node.
   */
  std::vector<std::byte> mNodeCertificateHash;

  /**
   * Should this Node verify the certificates coming from the remote node?
   */
  bool mVerifyCertificates = false;
};

} // namespace Hiero::internal

#endif // HIERO_SDK_CPP_IMPL_NODE_H_
