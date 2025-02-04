// SPDX-License-Identifier: Apache-2.0
#include "impl/Node.h"
#include "impl/BaseNodeAddress.h"
#include "impl/HieroCertificateVerifier.h"

#include <algorithm>
#include <utility>

namespace Hiero::internal
{
//-----
Node::Node(AccountId accountId, const BaseNodeAddress& address)
  : BaseNode<Node, AccountId>(address)
  , mAccountId(std::move(accountId))
{
}

//-----
Node::Node(const AccountId& accountId, std::string_view address)
  : Node(accountId, BaseNodeAddress::fromString(address))
{
}

//-----
grpc::Status Node::submitQuery(proto::Query::QueryCase funcEnum,
                               const proto::Query& query,
                               const std::chrono::system_clock::time_point& deadline,
                               proto::Response* response)
{
  std::unique_lock lock(*getLock());

  grpc::ClientContext context;
  context.set_deadline(deadline);

  switch (funcEnum)
  {
    case proto::Query::QueryCase::kConsensusGetTopicInfo:
      return mConsensusStub->getTopicInfo(&context, query, response);
    case proto::Query::QueryCase::kContractCallLocal:
      return mSmartContractStub->contractCallLocalMethod(&context, query, response);
    case proto::Query::QueryCase::kContractGetBytecode:
      return mSmartContractStub->ContractGetBytecode(&context, query, response);
    case proto::Query::QueryCase::kContractGetInfo:
      return mSmartContractStub->getContractInfo(&context, query, response);
    case proto::Query::QueryCase::kCryptogetAccountBalance:
      return mCryptoStub->cryptoGetBalance(&context, query, response);
    case proto::Query::QueryCase::kCryptoGetAccountRecords:
      return mCryptoStub->getAccountRecords(&context, query, response);
    case proto::Query::QueryCase::kCryptoGetInfo:
      return mCryptoStub->getAccountInfo(&context, query, response);
    case proto::Query::QueryCase::kCryptoGetLiveHash:
      return mCryptoStub->getLiveHash(&context, query, response);
    case proto::Query::QueryCase::kCryptoGetProxyStakers:
      return mCryptoStub->getStakersByAccountID(&context, query, response);
    case proto::Query::QueryCase::kFileGetContents:
      return mFileStub->getFileContent(&context, query, response);
    case proto::Query::QueryCase::kFileGetInfo:
      return mFileStub->getFileInfo(&context, query, response);
    case proto::Query::QueryCase::kNetworkGetVersionInfo:
      return mNetworkStub->getVersionInfo(&context, query, response);
    case proto::Query::QueryCase::kScheduleGetInfo:
      return mScheduleStub->getScheduleInfo(&context, query, response);
    case proto::Query::QueryCase::kTokenGetInfo:
      return mTokenStub->getTokenInfo(&context, query, response);
    case proto::Query::QueryCase::kTokenGetNftInfo:
      return mTokenStub->getTokenNftInfo(&context, query, response);
    case proto::Query::QueryCase::kTransactionGetReceipt:
      return mCryptoStub->getTransactionReceipts(&context, query, response);
    case proto::Query::QueryCase::kTransactionGetRecord:
      return mCryptoStub->getTxRecordByTxID(&context, query, response);
    default:
      // This should never happen
      throw std::invalid_argument("Unrecognized gRPC query method case");
  }
}

//-----
grpc::Status Node::submitTransaction(proto::TransactionBody::DataCase funcEnum,
                                     const proto::Transaction& transaction,
                                     const std::chrono::system_clock::time_point& deadline,
                                     proto::TransactionResponse* response)
{
  std::unique_lock lock(*getLock());

  grpc::ClientContext context;
  context.set_deadline(deadline);

  switch (funcEnum)
  {
    case proto::TransactionBody::DataCase::kNodeCreate:
      return mAddressBookStub->createNode(&context, transaction, response);
    case proto::TransactionBody::DataCase::kNodeDelete:
      return mAddressBookStub->createNode(&context, transaction, response);
    case proto::TransactionBody::DataCase::kNodeUpdate:
      return mAddressBookStub->createNode(&context, transaction, response);
    case proto::TransactionBody::DataCase::kConsensusCreateTopic:
      return mConsensusStub->createTopic(&context, transaction, response);
    case proto::TransactionBody::DataCase::kConsensusDeleteTopic:
      return mConsensusStub->deleteTopic(&context, transaction, response);
    case proto::TransactionBody::DataCase::kConsensusSubmitMessage:
      return mConsensusStub->submitMessage(&context, transaction, response);
    case proto::TransactionBody::DataCase::kConsensusUpdateTopic:
      return mConsensusStub->updateTopic(&context, transaction, response);
    case proto::TransactionBody::DataCase::kContractCall:
      return mSmartContractStub->contractCallMethod(&context, transaction, response);
    case proto::TransactionBody::DataCase::kContractCreateInstance:
      return mSmartContractStub->createContract(&context, transaction, response);
    case proto::TransactionBody::DataCase::kContractDeleteInstance:
      return mSmartContractStub->deleteContract(&context, transaction, response);
    case proto::TransactionBody::DataCase::kContractUpdateInstance:
      return mSmartContractStub->updateContract(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoAddLiveHash:
      return mCryptoStub->addLiveHash(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoApproveAllowance:
      return mCryptoStub->approveAllowances(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoDeleteAllowance:
      return mCryptoStub->deleteAllowances(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoCreateAccount:
      return mCryptoStub->createAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoDelete:
      return mCryptoStub->cryptoDelete(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoDeleteLiveHash:
      return mCryptoStub->deleteLiveHash(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoTransfer:
      return mCryptoStub->cryptoTransfer(&context, transaction, response);
    case proto::TransactionBody::DataCase::kCryptoUpdateAccount:
      return mCryptoStub->updateAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kEthereumTransaction:
      return mSmartContractStub->callEthereum(&context, transaction, response);
    case proto::TransactionBody::DataCase::kFileAppend:
      return mFileStub->appendContent(&context, transaction, response);
    case proto::TransactionBody::DataCase::kFileCreate:
      return mFileStub->createFile(&context, transaction, response);
    case proto::TransactionBody::DataCase::kFileDelete:
      return mFileStub->deleteFile(&context, transaction, response);
    case proto::TransactionBody::DataCase::kFileUpdate:
      return mFileStub->updateFile(&context, transaction, response);
    case proto::TransactionBody::DataCase::kFreeze:
      return mFreezeStub->freeze(&context, transaction, response);
    case proto::TransactionBody::DataCase::kScheduleCreate:
      return mScheduleStub->createSchedule(&context, transaction, response);
    case proto::TransactionBody::DataCase::kScheduleDelete:
      return mScheduleStub->deleteSchedule(&context, transaction, response);
    case proto::TransactionBody::DataCase::kScheduleSign:
      return mScheduleStub->signSchedule(&context, transaction, response);
    case proto::TransactionBody::DataCase::kSystemDelete:
      return mFileStub->systemDelete(&context, transaction, response);
    case proto::TransactionBody::DataCase::kSystemUndelete:
      return mFileStub->systemUndelete(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenAirdrop:
      return mTokenStub->airdropTokens(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenAssociate:
      return mTokenStub->associateTokens(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenBurn:
      return mTokenStub->burnToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenCancelAirdrop:
      return mTokenStub->burnToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenClaimAirdrop:
      return mTokenStub->burnToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenCreation:
      return mTokenStub->createToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenDeletion:
      return mTokenStub->deleteToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenDissociate:
      return mTokenStub->dissociateTokens(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenFeeScheduleUpdate:
      return mTokenStub->updateTokenFeeSchedule(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenFreeze:
      return mTokenStub->freezeTokenAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenGrantKyc:
      return mTokenStub->grantKycToTokenAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenMint:
      return mTokenStub->mintToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenPause:
      return mTokenStub->pauseToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenReject:
      return mTokenStub->revokeKycFromTokenAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenRevokeKyc:
      return mTokenStub->revokeKycFromTokenAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenUnfreeze:
      return mTokenStub->unfreezeTokenAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenUnpause:
      return mTokenStub->unpauseToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenUpdate:
      return mTokenStub->updateToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenUpdateNfts:
      return mTokenStub->updateToken(&context, transaction, response);
    case proto::TransactionBody::DataCase::kTokenWipe:
      return mTokenStub->wipeTokenAccount(&context, transaction, response);
    case proto::TransactionBody::DataCase::kUtilPrng:
      return mUtilStub->prng(&context, transaction, response);
    default:
      // This should never happen
      throw std::invalid_argument("Unrecognized gRPC transaction method case");
  }
}

//-----
Node& Node::toInsecure()
{
  const BaseNodeAddress address = getAddress().toInsecure();

  std::unique_lock lock(*getLock());
  return BaseNode<Node, AccountId>::setAddress(address);
}

//-----
Node& Node::toSecure()
{
  const BaseNodeAddress address = getAddress().toSecure();

  std::unique_lock lock(*getLock());
  return BaseNode<Node, AccountId>::setAddress(address);
}

//-----
Node& Node::setNodeCertificateHash(const std::vector<std::byte>& hash)
{
  std::unique_lock lock(*getLock());
  mNodeCertificateHash = hash;
  return *this;
}

//-----
Node& Node::setVerifyCertificates(bool verify)
{
  std::unique_lock lock(*getLock());
  mVerifyCertificates = verify;
  return *this;
}

//-----
Node::Node(const Node& node, const BaseNodeAddress& address)
  : BaseNode<Node, AccountId>(address)
  , mAccountId(node.mAccountId)
  , mNodeCertificateHash(node.mNodeCertificateHash)
  , mVerifyCertificates(node.mVerifyCertificates)
{
}

//-----
std::shared_ptr<grpc::ChannelCredentials> Node::getTlsChannelCredentials() const
{
  grpc::experimental::TlsChannelCredentialsOptions tlsChannelCredentialsOptions;

  // Custom verification using the node's certificate chain hash
  tlsChannelCredentialsOptions.set_verify_server_certs(false);
  tlsChannelCredentialsOptions.set_check_call_host(false);
  tlsChannelCredentialsOptions.set_certificate_verifier(
    grpc::experimental::ExternalCertificateVerifier::Create<HieroCertificateVerifier>(mNodeCertificateHash));

  // Feed in the root CA's file manually for Windows (this is a bug in the gRPC implementation and says here
  // https://deploy-preview-763--grpc-io.netlify.app/docs/guides/auth/#using-client-side-ssltls that this needs to be
  // specified manually).
#ifdef _WIN32
  tlsChannelCredentialsOptions.set_certificate_provider(
    std::make_shared<grpc::experimental::FileWatcherCertificateProvider>("roots.pem", -1));
  tlsChannelCredentialsOptions.watch_root_certs();
#endif

  return grpc::experimental::TlsCredentials(tlsChannelCredentialsOptions);
}

//-----
void Node::initializeStubs()
{
  // clang-format off
  if (!mConsensusStub)     mConsensusStub     = proto::ConsensusService::NewStub(getChannel());
  if (!mCryptoStub)        mCryptoStub        = proto::CryptoService::NewStub(getChannel());
  if (!mFileStub)          mFileStub          = proto::FileService::NewStub(getChannel());
  if (!mFreezeStub)        mFreezeStub        = proto::FreezeService::NewStub(getChannel());
  if (!mNetworkStub)       mNetworkStub       = proto::NetworkService::NewStub(getChannel());
  if (!mScheduleStub)      mScheduleStub      = proto::ScheduleService::NewStub(getChannel());
  if (!mSmartContractStub) mSmartContractStub = proto::SmartContractService::NewStub(getChannel());
  if (!mTokenStub)         mTokenStub         = proto::TokenService::NewStub(getChannel());
  if (!mUtilStub)          mUtilStub          = proto::UtilService::NewStub(getChannel());
  if (!mAddressBookStub)   mAddressBookStub   = proto::AddressBookService::NewStub(getChannel());
  // clang-format on
}

//-----
void Node::closeStubs()
{
  mConsensusStub = nullptr;
  mCryptoStub = nullptr;
  mFileStub = nullptr;
  mFreezeStub = nullptr;
  mNetworkStub = nullptr;
  mScheduleStub = nullptr;
  mSmartContractStub = nullptr;
  mTokenStub = nullptr;
  mUtilStub = nullptr;
  mAddressBookStub = nullptr;
}

} // namespace Hiero::internal
