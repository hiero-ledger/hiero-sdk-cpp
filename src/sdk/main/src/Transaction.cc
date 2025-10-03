// SPDX-License-Identifier: Apache-2.0
#include "Transaction.h"
#include "AccountAllowanceApproveTransaction.h"
#include "AccountAllowanceDeleteTransaction.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountUpdateTransaction.h"
#include "BatchTransaction.h"
#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractUpdateTransaction.h"
#include "Defaults.h"
#include "EthereumTransaction.h"
#include "FileAppendTransaction.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileUpdateTransaction.h"
#include "FreezeTransaction.h"
#include "NodeCreateTransaction.h"
#include "NodeDeleteTransaction.h"
#include "NodeUpdateTransaction.h"
#include "PrivateKey.h"
#include "PrngTransaction.h"
#include "PublicKey.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleDeleteTransaction.h"
#include "ScheduleSignTransaction.h"
#include "Status.h"
#include "SystemDeleteTransaction.h"
#include "SystemUndeleteTransaction.h"
#include "TokenAirdropTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenBurnTransaction.h"
#include "TokenCancelAirdropTransaction.h"
#include "TokenClaimAirdropTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenDissociateTransaction.h"
#include "TokenFeeScheduleUpdateTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TokenGrantKycTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenPauseTransaction.h"
#include "TokenRejectTransaction.h"
#include "TokenRevokeKycTransaction.h"
#include "TokenUnfreezeTransaction.h"
#include "TokenUnpauseTransaction.h"
#include "TokenUpdateNftsTransaction.h"
#include "TokenUpdateTransaction.h"
#include "TokenWipeTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicMessageSubmitTransaction.h"
#include "TopicUpdateTransaction.h"
#include "TransactionId.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "exceptions/UninitializedException.h"
#include "impl/DurationConverter.h"
#include "impl/Network.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <services/basic_types.pb.h>
#include <services/transaction.pb.h>
#include <services/transaction_contents.pb.h>
#include <services/transaction_response.pb.h>

#include <transaction_list.pb.h>

#include <vector>

namespace Hiero
{
//-----
template<typename SdkRequestType>
struct Transaction<SdkRequestType>::TransactionImpl
{
  // The source TransactionBody protobuf object from which derived transactions
  // should use to construct themselves. The Transaction base class will use
  // this to get the Transaction-specific fields, and then pass it to the
  // derived class to pick up its own data. It also acts as the "source of
  // truth" when generating SignedTransaction and Transaction protobuf objects
  // to send to the network.
  proto::TransactionBody mSourceTransactionBody;

  // List of completed Transaction protobuf objects ready to be sent. These are
  // functionally identical, the only difference is the node to which they are
  // sent.
  std::vector<proto::Transaction> mTransactions;

  // List of SignedTransaction protobuf objects. The index of these
  // SignedTransactions match up with their corresponding Transaction protobuf
  // object in mTransactions.
  std::vector<proto::SignedTransaction> mSignedTransactions;

  // When submitting a Transaction, the index into mSignedTransactions and
  // mTransactions must be tracked so that a proper TransactionResponse can be
  // generated (which must grab the transaction hash and node account ID).
  unsigned int mTransactionIndex = 0U;

  // A list of PublicKeys with their signer functions that should sign the
  // TransactionBody protobuf objects this Transaction creates. If the signer
  // function associated with a public key is empty, that means that the private
  // key associated with that public key has already contributed a signature,
  // but the signer is not available (probably because this Transaction was
  // created fromBytes(), or the signature was contributed manually via
  // addSignature()).
  std::unordered_map<std::shared_ptr<PublicKey>, std::function<std::vector<std::byte>(const std::vector<std::byte>&)>>
    mSignatories;

  // Keep a map of PublicKeys to their associated PrivateKeys. If the
  // Transaction is signed with a PrivateKey, the Transaction must make sure the
  // PrivateKey does not go out of scope, otherwise it will crash when trying to
  // generate a signature.
  std::unordered_map<std::shared_ptr<PublicKey>, std::shared_ptr<PrivateKey>> mPrivateKeys;

  // Is this Transaction frozen?
  bool mIsFrozen = false;

  // The ID of this Transaction. No value if it has not yet been set.
  std::optional<TransactionId> mTransactionId;

  // The maximum transaction fee willing to be paid to execute this Transaction.
  // If not set, this Transaction will use the Client's set maximum transaction
  // fee. If that's not set, mDefaultMaxTransactionFee is used.
  std::optional<Hbar> mMaxTransactionFee;

  // The default maximum transaction fee. This can be adjusted by derived
  // Transaction classes if those Transactions generally cost more.
  Hbar mDefaultMaxTransactionFee = DEFAULT_MAX_TRANSACTION_FEE;

  // The length of time this Transaction will remain valid.
  std::chrono::system_clock::duration mTransactionValidDuration = DEFAULT_TRANSACTION_VALID_DURATION;

  // The memo to be associated with this Transaction.
  std::string mTransactionMemo;

  // Should this Transaction regenerate its TransactionId upon a
  // TRANSACTION_EXPIRED response from the network? If not set, this Transaction
  // will use the Client's set transaction ID regeneration policy. If that's not
  // set, the default behavior is captured in DEFAULT_REGENERATE_TRANSACTION_ID.
  std::optional<bool> mTransactionIdRegenerationPolicy;

  /**
   * The public key of the trusted batch assembler.
   */
  std::shared_ptr<Key> mBatchKey = nullptr;
  /**
 * 
 * This flag is used to determine whether a Transaction's TransactionId
 * should be regenerated if the Transaction expires. 
 * 
 * - If `mTransactionIdManualSet` is true, the TransactionId was set manually by the user,
 *   and it **must not be regenerated**, regardless of the client-wide or transaction-specific
 *   regeneration policies.
 * - If `mTransactionIdManualSet` is false (default), the TransactionId may be regenerated
 *   based on the transaction's own `mTransactionIdRegenerationPolicy` or the client's policy.
 */
  bool mTransactionIdManualSet = false;
};

//-----
template<typename SdkRequestType>
WrappedTransaction Transaction<SdkRequestType>::fromBytes(const std::vector<std::byte>& bytes)
{
  // Keep a list of all transactions that were serialized.
  std::map<TransactionId, std::map<AccountId, proto::Transaction>> transactions;

  proto::Transaction tx;
  proto::SignedTransaction signedTx;
  proto::TransactionBody txBody;

  bool batchified = false;

  // Check if batchified
  tx.set_signedtransactionbytes(internal::Utilities::byteVectorToString(bytes));
  signedTx.ParseFromArray(tx.signedtransactionbytes().data(), static_cast<int>(tx.signedtransactionbytes().size()));
  txBody.ParseFromArray(signedTx.bodybytes().data(), static_cast<int>(signedTx.bodybytes().size()));
  if (txBody.has_batch_key())
  {
    batchified = true;
    transactions[txBody.has_transactionid() ? TransactionId::fromProtobuf(txBody.transactionid())
                                            : DUMMY_TRANSACTION_ID]
                [txBody.has_nodeaccountid() ? AccountId::fromProtobuf(txBody.nodeaccountid()) : DUMMY_ACCOUNT_ID] = tx;
  }

  if (!batchified)
  {
    // Serialized object is a TransactionList protobuf object.
    if (proto::TransactionList txList;
        txList.ParseFromArray(bytes.data(), static_cast<int>(bytes.size())) && txList.transaction_list_size() > 0)
    {
      for (int i = 0; i < txList.transaction_list_size(); ++i)
      {
        tx = txList.transaction_list(i);
        signedTx.ParseFromArray(tx.signedtransactionbytes().data(),
                                static_cast<int>(tx.signedtransactionbytes().size()));
        txBody.ParseFromArray(signedTx.bodybytes().data(), static_cast<int>(signedTx.bodybytes().size()));

        transactions[txBody.has_transactionid() ? TransactionId::fromProtobuf(txBody.transactionid())
                                                : DUMMY_TRANSACTION_ID]
                    [txBody.has_nodeaccountid() ? AccountId::fromProtobuf(txBody.nodeaccountid()) : DUMMY_ACCOUNT_ID] =
                      tx;
      }
    }

    // Transaction protobuf object.
    else if (txBody.data_case() == proto::TransactionBody::DataCase::DATA_NOT_SET &&
             tx.ParseFromArray(bytes.data(), static_cast<int>(bytes.size())) && !tx.signedtransactionbytes().empty())
    {
      signedTx.ParseFromArray(tx.signedtransactionbytes().data(), static_cast<int>(tx.signedtransactionbytes().size()));
      txBody.ParseFromArray(signedTx.bodybytes().data(), static_cast<int>(signedTx.bodybytes().size()));

      transactions[txBody.has_transactionid() ? TransactionId::fromProtobuf(txBody.transactionid())
                                              : DUMMY_TRANSACTION_ID]
                  [txBody.has_nodeaccountid() ? AccountId::fromProtobuf(txBody.nodeaccountid()) : DUMMY_ACCOUNT_ID] =
                    tx;
    }

    // TransactionBody protobuf object.
    else if (txBody.data_case() == proto::TransactionBody::DataCase::DATA_NOT_SET &&
             txBody.ParseFromArray(bytes.data(), static_cast<int>(bytes.size())))
    {
      signedTx.set_bodybytes(txBody.SerializeAsString());
      tx.set_signedtransactionbytes(signedTx.SerializeAsString());

      transactions[txBody.has_transactionid() ? TransactionId::fromProtobuf(txBody.transactionid())
                                              : DUMMY_TRANSACTION_ID]
                  [txBody.has_nodeaccountid() ? AccountId::fromProtobuf(txBody.nodeaccountid()) : DUMMY_ACCOUNT_ID] =
                    tx;
    }

    // If not any Transaction, throw.
    else
    {
      throw std::invalid_argument("Unable to construct Transaction from input bytes.");
    }
  }

  switch (txBody.data_case())
  {
    case proto::TransactionBody::kCryptoApproveAllowance:
      return WrappedTransaction(AccountAllowanceApproveTransaction(transactions));
    case proto::TransactionBody::kCryptoDeleteAllowance:
      return WrappedTransaction(AccountAllowanceDeleteTransaction(transactions));
    case proto::TransactionBody::kCryptoCreateAccount:
      return WrappedTransaction(AccountCreateTransaction(transactions));
    case proto::TransactionBody::kCryptoDelete:
      return WrappedTransaction(AccountDeleteTransaction(transactions));
    case proto::TransactionBody::kCryptoUpdateAccount:
      return WrappedTransaction(AccountUpdateTransaction(transactions));
    case proto::TransactionBody::kAtomicBatch:
      return WrappedTransaction(BatchTransaction(transactions));
    case proto::TransactionBody::kContractCreateInstance:
      return WrappedTransaction(ContractCreateTransaction(transactions));
    case proto::TransactionBody::kContractDeleteInstance:
      return WrappedTransaction(ContractDeleteTransaction(transactions));
    case proto::TransactionBody::kContractCall:
      return WrappedTransaction(ContractExecuteTransaction(transactions));
    case proto::TransactionBody::kContractUpdateInstance:
      return WrappedTransaction(ContractUpdateTransaction(transactions));
    case proto::TransactionBody::kEthereumTransaction:
      return WrappedTransaction(EthereumTransaction(transactions));
    case proto::TransactionBody::kFileAppend:
      return WrappedTransaction(FileAppendTransaction(transactions));
    case proto::TransactionBody::kFileCreate:
      return WrappedTransaction(FileCreateTransaction(transactions));
    case proto::TransactionBody::kFileDelete:
      return WrappedTransaction(FileDeleteTransaction(transactions));
    case proto::TransactionBody::kFileUpdate:
      return WrappedTransaction(FileUpdateTransaction(transactions));
    case proto::TransactionBody::kFreeze:
      return WrappedTransaction(FreezeTransaction(transactions));
    case proto::TransactionBody::kNodeCreate:
      return WrappedTransaction(NodeCreateTransaction(transactions));
    case proto::TransactionBody::kNodeDelete:
      return WrappedTransaction(NodeDeleteTransaction(transactions));
    case proto::TransactionBody::kNodeUpdate:
      return WrappedTransaction(NodeUpdateTransaction(transactions));
    case proto::TransactionBody::kUtilPrng:
      return WrappedTransaction(PrngTransaction(transactions));
    case proto::TransactionBody::kScheduleCreate:
      return WrappedTransaction(ScheduleCreateTransaction(transactions));
    case proto::TransactionBody::kScheduleDelete:
      return WrappedTransaction(ScheduleDeleteTransaction(transactions));
    case proto::TransactionBody::kScheduleSign:
      return WrappedTransaction(ScheduleSignTransaction(transactions));
    case proto::TransactionBody::kSystemDelete:
      return WrappedTransaction(SystemDeleteTransaction(transactions));
    case proto::TransactionBody::kSystemUndelete:
      return WrappedTransaction(SystemUndeleteTransaction(transactions));
    case proto::TransactionBody::kTokenAirdrop:
      return WrappedTransaction(TokenAirdropTransaction(transactions));
    case proto::TransactionBody::kTokenAssociate:
      return WrappedTransaction(TokenAssociateTransaction(transactions));
    case proto::TransactionBody::kTokenBurn:
      return WrappedTransaction(TokenBurnTransaction(transactions));
    case proto::TransactionBody::kTokenCancelAirdrop:
      return WrappedTransaction(TokenCancelAirdropTransaction(transactions));
    case proto::TransactionBody::kTokenClaimAirdrop:
      return WrappedTransaction(TokenClaimAirdropTransaction(transactions));
    case proto::TransactionBody::kTokenCreation:
      return WrappedTransaction(TokenCreateTransaction(transactions));
    case proto::TransactionBody::kTokenDeletion:
      return WrappedTransaction(TokenDeleteTransaction(transactions));
    case proto::TransactionBody::kTokenDissociate:
      return WrappedTransaction(TokenDissociateTransaction(transactions));
    case proto::TransactionBody::kTokenFeeScheduleUpdate:
      return WrappedTransaction(TokenFeeScheduleUpdateTransaction(transactions));
    case proto::TransactionBody::kTokenFreeze:
      return WrappedTransaction(TokenFreezeTransaction(transactions));
    case proto::TransactionBody::kTokenGrantKyc:
      return WrappedTransaction(TokenGrantKycTransaction(transactions));
    case proto::TransactionBody::kTokenMint:
      return WrappedTransaction(TokenMintTransaction(transactions));
    case proto::TransactionBody::kTokenPause:
      return WrappedTransaction(TokenPauseTransaction(transactions));
    case proto::TransactionBody::kTokenReject:
      return WrappedTransaction(TokenRejectTransaction(transactions));
    case proto::TransactionBody::kTokenRevokeKyc:
      return WrappedTransaction(TokenRevokeKycTransaction(transactions));
    case proto::TransactionBody::kTokenUnfreeze:
      return WrappedTransaction(TokenUnfreezeTransaction(transactions));
    case proto::TransactionBody::kTokenUnpause:
      return WrappedTransaction(TokenUnpauseTransaction(transactions));
    case proto::TransactionBody::kTokenUpdateNfts:
      return WrappedTransaction(TokenUpdateNftsTransaction(transactions));
    case proto::TransactionBody::kTokenUpdate:
      return WrappedTransaction(TokenUpdateTransaction(transactions));
    case proto::TransactionBody::kTokenWipe:
      return WrappedTransaction(TokenWipeTransaction(transactions));
    case proto::TransactionBody::kConsensusCreateTopic:
      return WrappedTransaction(TopicCreateTransaction(transactions));
    case proto::TransactionBody::kConsensusDeleteTopic:
      return WrappedTransaction(TopicDeleteTransaction(transactions));
    case proto::TransactionBody::kConsensusSubmitMessage:
      return WrappedTransaction(TopicMessageSubmitTransaction(transactions));
    case proto::TransactionBody::kConsensusUpdateTopic:
      return WrappedTransaction(TopicUpdateTransaction(transactions));
    case proto::TransactionBody::kCryptoTransfer:
      return WrappedTransaction(TransferTransaction(transactions));
    default:
      throw std::invalid_argument("Type of transaction cannot be determined from input bytes.");
  }
}

//-----
template<typename SdkRequestType>
std::vector<std::byte> Transaction<SdkRequestType>::toBytes() const
{
  // If no nodes have been selected yet, the mSourceTransactionBody can be used
  // to build a Transaction protobuf object.
  proto::TransactionList txList;
  if (Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
        getNodeAccountIds()
          .empty())
  {
    // Make sure the Transaction has, if any, all recent changes.
    updateSourceTransactionBody(nullptr);

    proto::SignedTransaction signedTx;
    signedTx.set_bodybytes(mImpl->mSourceTransactionBody.SerializeAsString());

    proto::Transaction tx;
    tx.set_signedtransactionbytes(signedTx.SerializeAsString());

    *txList.add_transaction_list() = tx;
  }
  else
  {
    // Generate the SignedTransaction protobuf objects if the Transaction's not
    // frozen.
    if (!isFrozen())
    {
      regenerateSignedTransactions(nullptr);
    }

    // Build all the Transaction protobuf objects and add them to the
    // TransactionList protobuf object.
    buildAllTransactions();
    for (const auto& tx : mImpl->mTransactions)
    {
      *txList.add_transaction_list() = tx;
    }
  }

  return internal::Utilities::stringToByteVector(txList.SerializeAsString());
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::sign(const std::shared_ptr<PrivateKey>& key)
{
  // clang-format off
  if (key == nullptr)
  {
    throw std::invalid_argument("Signing key must be set.");
  }

  return signInternal(key->getPublicKey(), [key](const std::vector<std::byte>& vec) { return key->sign(vec); }, key);
  // clang-format on
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::signWith(
  const std::shared_ptr<PublicKey>& key,
  const std::function<std::vector<std::byte>(const std::vector<std::byte>&)>& signer)
{
  return signInternal(key, signer);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::signWithOperator(const Client& client)
{
  if (!client.getOperatorAccountId().has_value())
  {
    throw UninitializedException("Input client has no operator.");
  }

  freezeWith(&client);

  return signInternal(client.getOperatorPublicKey(), client.getOperatorSigner().value());
}

//------
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::batchify(const Client& client, const std::shared_ptr<Key>& batchKey)
{
  // Set the provided batch key
  if (batchKey)
  {
    mImpl->mBatchKey = batchKey;
  }

  // HIP-551 States that batchified transactions should always have the zero account ID for the node
  Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::setNodeAccountIds(
    { DUMMY_ACCOUNT_ID });

  // Sign the transaction with the client’s operator, if applicable
  if (client.getOperatorAccountId().has_value())
  {
    signWithOperator(client);
  }

  // Build the transaction sigmap for the signed body bytes
  buildTransaction(0);

  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::setBatchKey(const std::shared_ptr<Key>& batchKey)
{
  requireNotFrozen();

  if (batchKey)
  {
    mImpl->mBatchKey = batchKey;
  }

  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::addSignature(const std::shared_ptr<PublicKey>& publicKey,
                                                          const std::vector<std::byte>& signature)
{
  // A signature can only be added for Transactions being sent to exactly one
  // node.
  requireOneNodeAccountId();

  // A signature can only be added to frozen Transactions.
  if (!isFrozen())
  {
    throw IllegalStateException("Adding a signature to a Transaction requires "
                                "the Transaction to be frozen");
  }

  // If this PublicKey has already signed this Transaction, the signature
  // doesn't need to be added again.
  if (keyAlreadySigned(publicKey))
  {
    return static_cast<SdkRequestType&>(*this);
  }

  // Adding a signature will require all Transaction protobuf objects to be
  // regenerated.
  mImpl->mTransactions.clear();
  mImpl->mTransactions.resize(mImpl->mSignedTransactions.size());
  mImpl->mSignatories.emplace(publicKey, std::function<std::vector<std::byte>(const std::vector<std::byte>&)>());
  mImpl->mPrivateKeys.emplace(publicKey, nullptr);

  // Add the signature to the SignedTransaction protobuf object. Since there's
  // only one node account ID, there's only one SignedTransaction protobuf
  // object in the vector.
  *mImpl->mSignedTransactions.begin()->mutable_sigmap()->add_sigpair() = *publicKey->toSignaturePairProtobuf(signature);

  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
std::map<AccountId, std::map<std::shared_ptr<PublicKey>, std::vector<std::byte>>>
Transaction<SdkRequestType>::getSignatures() const
{
  if (!isFrozen())
  {
    throw IllegalStateException("Transaction must be frozen in order to have signatures.");
  }

  if (mImpl->mSignatories.empty())
  {
    return {};
  }

  // Build all the Transaction protobuf objects to generate the signatures for
  // each key.
  buildAllTransactions();
  return getSignaturesInternal();
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::freeze()
{
  return freezeWith(nullptr);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::freezeWith(const Client* client)
{
  if (isFrozen())
  {
    return static_cast<SdkRequestType&>(*this);
  }

  if (!mImpl->mTransactionId.has_value())
  {
    if (!client)
    {
      throw IllegalStateException("If no client is provided to freeze transaction, the transaction ID "
                                  "must be manually set.");
    }

    if (!client->getOperatorAccountId().has_value())
    {
      throw UninitializedException("Client operator has not been initialized "
                                   "and cannot freeze transaction.");
    }

    // Generate a transaction ID with the client.
    mImpl->mTransactionId = TransactionId::generate(client->getOperatorAccountId().value());
  }

  if (Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
        getNodeAccountIds()
          .empty())
  {
    if (!client)
    {
      throw IllegalStateException("If no client is provided to freeze transaction, the node account "
                                  "ID(s) must be manually set.");
    }

    // Make sure the client has a valid network.
    if (!client->getClientNetwork())
    {
      throw UninitializedException("Client has not been initialized with a valid network.");
    }

    // Have the Client's network generate the node account IDs to which to send
    // this Transaction.
    Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::setNodeAccountIds(
      client->getClientNetwork()->getNodeAccountIdsForExecute());
  }

  // Regenerate the final SignedTransaction protobuf objects.
  regenerateSignedTransactions(client);

  mImpl->mIsFrozen = true;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
ScheduleCreateTransaction Transaction<SdkRequestType>::schedule() const
{
  requireNotFrozen();
  if (!Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
         getNodeAccountIds()
           .empty())
  {
    throw IllegalStateException("Underlying transaction for a scheduled transaction cannot have node "
                                "account IDs set.");
  }

  updateSourceTransactionBody(nullptr);
  return ScheduleCreateTransaction().setScheduledTransaction(
    WrappedTransaction::fromProtobuf(mImpl->mSourceTransactionBody));
}

//-----
template<typename SdkRequestType>
std::vector<std::byte> Transaction<SdkRequestType>::getTransactionHash() const
{
  if (!isFrozen())
  {
    throw IllegalStateException("Transaction must be frozen in order to calculate the hash.");
  }

  // Use the first transaction's hash.
  buildTransaction(0U);
  return internal::OpenSSLUtils::computeSHA384(
    internal::Utilities::stringToByteVector(getTransactionProtobufObject(0U).signedtransactionbytes()));
}

//-----
template<typename SdkRequestType>
std::map<AccountId, std::vector<std::byte>> Transaction<SdkRequestType>::getTransactionHashPerNode() const
{
  if (!isFrozen())
  {
    throw IllegalStateException("Transaction must be frozen in order to calculate the hash.");
  }

  buildAllTransactions();
  const std::vector<AccountId> nodeAccountIds =
    Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
      getNodeAccountIds();

  std::map<AccountId, std::vector<std::byte>> hashes;
  for (unsigned int i = 0; i < mImpl->mTransactions.size(); ++i)
  {
    hashes[nodeAccountIds.at(i)] = internal::OpenSSLUtils::computeSHA384(
      internal::Utilities::stringToByteVector(getTransactionProtobufObject(i).signedtransactionbytes()));
  }

  return hashes;
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::requireOneNodeAccountId() const
{
  if (Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
        getNodeAccountIds()
          .size() != 1ULL)
  {
    throw IllegalStateException("Transaction does not have exactly one node account ID set.");
  }
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::setTransactionId(const TransactionId& id)
{
  requireNotFrozen();
  mImpl->mTransactionId = id;
  mImpl->mTransactionIdManualSet = true; 
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::setNodeAccountIds(std::vector<AccountId> nodeAccountIds)
{
  requireNotFrozen();
  Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::setNodeAccountIds(
    std::move(nodeAccountIds));
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::setMaxTransactionFee(const Hbar& fee)
{
  requireNotFrozen();
  mImpl->mMaxTransactionFee = fee;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::setValidTransactionDuration(
  const std::chrono::system_clock::duration& duration)
{
  requireNotFrozen();
  mImpl->mTransactionValidDuration = duration;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::setTransactionMemo(const std::string& memo)
{
  requireNotFrozen();
  mImpl->mTransactionMemo = memo;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::setRegenerateTransactionIdPolicy(bool regenerate)
{
  requireNotFrozen();
  mImpl->mTransactionIdRegenerationPolicy = regenerate;
  return static_cast<SdkRequestType&>(*this);
}

//-----
template<typename SdkRequestType>
TransactionId Transaction<SdkRequestType>::getTransactionId() const
{
  if (!mImpl->mTransactionId.has_value())
  {
    throw UninitializedException("No transaction ID generated yet. Try "
                                 "freezing the transaction or manually setting "
                                 "the transaction ID.");
  }

  return mImpl->mTransactionId.value();
}

//-----
template<typename SdkRequestType>
Hbar Transaction<SdkRequestType>::getMaxTransactionFee() const
{
  return mImpl->mMaxTransactionFee.value_or(mImpl->mDefaultMaxTransactionFee);
}

//-----
template<typename SdkRequestType>
Hbar Transaction<SdkRequestType>::getDefaultMaxTransactionFee() const
{
  return mImpl->mDefaultMaxTransactionFee;
}

//-----
template<typename SdkRequestType>
std::chrono::system_clock::duration Transaction<SdkRequestType>::getValidTransactionDuration() const
{
  return mImpl->mTransactionValidDuration;
}

//-----
template<typename SdkRequestType>
std::string Transaction<SdkRequestType>::getTransactionMemo() const
{
  return mImpl->mTransactionMemo;
}

//-----
template<typename SdkRequestType>
std::optional<bool> Transaction<SdkRequestType>::getRegenerateTransactionIdPolicy() const
{
  return mImpl->mTransactionIdRegenerationPolicy;
}

//-----
template<typename SdkRequestType>
size_t Transaction<SdkRequestType>::getTransactionSize() const
{
  if (!isFrozen())
  {
    throw IllegalStateException("Transaction must be frozen in order to calculate its size.");
  }
  return makeRequest(0).ByteSizeLong();
}

//-----
template<typename SdkRequestType>
size_t Transaction<SdkRequestType>::getTransactionBodySize() const
{
  if (!isFrozen())
  {
    throw IllegalStateException("Transaction must be frozen in order to calculate its body size.");
  }
  return mImpl->mSourceTransactionBody.ByteSizeLong();
}

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>::Transaction()
  : Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>()
  , mImpl(std::make_unique<TransactionImpl>())
{
}

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>::~Transaction() = default;

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>::Transaction(const Transaction<SdkRequestType>& other)
  : Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>(other)
  , mImpl(std::make_unique<TransactionImpl>(*other.mImpl))
{
}

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>& Transaction<SdkRequestType>::operator=(const Transaction<SdkRequestType>& other)
{
  if (this != &other)
  {
    Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::operator=(other);
    mImpl = std::make_unique<TransactionImpl>(*other.mImpl);
  }

  return *this;
}

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>::Transaction(Transaction<SdkRequestType>&& other) noexcept
  : Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>(std::move(other))
  , mImpl(std::move(other.mImpl)) // NOLINT
{
  // Leave moved-from object in a valid state.
  other.mImpl = std::make_unique<TransactionImpl>(); // NOLINT
}

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>& Transaction<SdkRequestType>::operator=(Transaction<SdkRequestType>&& other) noexcept
{
  if (this != &other)
  {
    Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::operator=(
      std::move(other));
    mImpl = std::move(other.mImpl); // NOLINT

    // Leave moved-from object in a valid state.
    other.mImpl = std::make_unique<TransactionImpl>();
  }

  return *this;
}

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>::Transaction(const proto::TransactionBody& txBody)
  : Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>()
  , mImpl(std::make_unique<TransactionImpl>())
{
  if (txBody.has_transactionid())
  {
    mImpl->mTransactionId = TransactionId::fromProtobuf(txBody.transactionid());
  }

  if (txBody.transactionfee() > 0ULL)
  {
    mImpl->mMaxTransactionFee = Hbar(static_cast<int64_t>(txBody.transactionfee()), HbarUnit::TINYBAR());
  }

  if (txBody.has_transactionvalidduration())
  {
    mImpl->mTransactionValidDuration = internal::DurationConverter::fromProtobuf(txBody.transactionvalidduration());
  }

  mImpl->mTransactionMemo = txBody.memo();

  if (txBody.has_batch_key())
  {
    mImpl->mBatchKey = Key::fromProtobuf(txBody.batch_key());
  }

  mImpl->mSourceTransactionBody = txBody;
}

//-----
template<typename SdkRequestType>
Transaction<SdkRequestType>::Transaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>()
  , mImpl(std::make_unique<TransactionImpl>())
{
  if (transactions.empty())
  {
    return;
  }

  // Set the TransactionId of this Transaction. Transactions only care about the
  // first TransactionId in the map, so grab the first TransactionId and set it
  // as this Transaction's TransactionId if it's not a dummy TransactionId. If
  // it's a dummy TransactionId, the Transaction should remain incomplete. The
  // other TransactionIds will be looked at by the ChunkedTransaction
  // constructor if this Transaction is a ChunkedTransaction. If this
  // Transaction is not a ChunkedTransaction or if it's an incomplete
  // Transaction (i.e. has a dummy account ID in its transaction ID), there
  // should be only one transaction ID anyway.
  if (!(transactions.cbegin()->first == DUMMY_TRANSACTION_ID))
  {
    mImpl->mTransactionId = transactions.cbegin()->first;
  }

  // If the first account ID is a dummy account ID, then only the source
  // TransactionBody needs to be copied.
  const std::map<AccountId, proto::Transaction>& transactionMap = transactions.cbegin()->second;
  if (!transactionMap.empty() && transactionMap.cbegin()->first == DUMMY_ACCOUNT_ID)
  {
    proto::SignedTransaction signedTx;
    signedTx.ParseFromString(transactionMap.cbegin()->second.signedtransactionbytes());

    proto::TransactionBody txBody;
    txBody.ParseFromString(signedTx.bodybytes());

    // If the transaction was a batchified transaction we need a full transaction proto object
    if (txBody.has_batch_key())
    {
      mImpl->mTransactions.push_back(transactionMap.cbegin()->second);
    }

    mImpl->mSourceTransactionBody = txBody;
  }

  else
  {
    // The node account IDs get added as a batch so just add them to a separate
    // vector for now.
    std::vector<AccountId> nodeAccountIds;

    // A standard Transaction can only hold information for one Transaction. If
    // this Transaction is a ChunkedTransaction, the additional Transaction
    // protobuf objects will be processed there.
    nodeAccountIds.reserve(transactions.cbegin()->second.size());

    bool gotSignatures = false;
    for (const auto& [accountId, protoTx] : transactionMap)
    {
      nodeAccountIds.push_back(accountId);
      addTransaction(protoTx);

      // Only need to grab the signatures once.
      if (!gotSignatures)
      {
        const proto::SignedTransaction& signedTx = mImpl->mSignedTransactions.back();

        // If there are signatures, grab them.
        if (signedTx.sigmap().sigpair_size() != 0)
        {
          for (int i = 0; i < signedTx.sigmap().sigpair_size(); ++i)
          {
            const std::shared_ptr<PublicKey> publicKey = PublicKey::fromBytes(
              internal::Utilities::stringToByteVector(signedTx.sigmap().sigpair(i).pubkeyprefix()));
            mImpl->mSignatories.emplace(publicKey,
                                        std::function<std::vector<std::byte>(const std::vector<std::byte>&)>());
            mImpl->mPrivateKeys.emplace(publicKey, nullptr);
          }

          // The presence of signatures implies the Transaction should be
          // frozen.
          mImpl->mIsFrozen = true;
        }

        gotSignatures = true;
      }
    }

    // Set the source TransactionBody based on the generated SignedTransaction
    // protobuf objects.
    proto::TransactionBody txBody;
    txBody.ParseFromArray(mImpl->mSignedTransactions.cbegin()->bodybytes().data(),
                          static_cast<int>(mImpl->mSignedTransactions.cbegin()->bodybytes().size()));
    mImpl->mSourceTransactionBody = txBody;

    // Now that all node account IDs have been seen, they can all be added at
    // once.
    Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::setNodeAccountIds(
      nodeAccountIds);
  }

  // Set the remaining fields based on the source TransactionBody.
  if (mImpl->mSourceTransactionBody.transactionfee() > 0LL)
  {
    mImpl->mMaxTransactionFee =
      Hbar(static_cast<int64_t>(mImpl->mSourceTransactionBody.transactionfee()), HbarUnit::TINYBAR());
  }

  if (mImpl->mSourceTransactionBody.has_transactionvalidduration())
  {
    mImpl->mTransactionValidDuration =
      internal::DurationConverter::fromProtobuf(mImpl->mSourceTransactionBody.transactionvalidduration());
  }

  mImpl->mTransactionMemo = mImpl->mSourceTransactionBody.memo();

  if (mImpl->mSourceTransactionBody.has_batch_key())
  {
    mImpl->mBatchKey = Key::fromProtobuf(mImpl->mSourceTransactionBody.batch_key());
  }
}

//-----
template<typename SdkRequestType>
proto::Transaction Transaction<SdkRequestType>::makeRequest(unsigned int index) const
{
  mImpl->mTransactionIndex = index;
  buildTransaction(index);
  return getTransactionProtobufObject(index);
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::buildAllTransactions() const
{
  // Go through each SignedTransaction protobuf object and add all signatures to
  // its SignatureMap protobuf object.
  for (unsigned int i = 0; i < mImpl->mSignedTransactions.size(); ++i)
  {
    buildTransaction(i);
  }
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::regenerateSignedTransactions(const Client* client) const
{
  // Update this Transaction's source TransactionBody protobuf object.
  updateSourceTransactionBody(client);

  // Clear out any stale SignedTransaction and/or Transaction protobuf objects.
  clearTransactions();

  // Add a SignedTransaction protobuf object for each node account ID based off
  // of this Transaction's mSourceTransactionBody.
  addSignedTransactionForEachNode(mImpl->mSourceTransactionBody);
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::updateSourceTransactionBody(const Client* client) const
{
  if (mImpl->mTransactionId.has_value())
  {
    mImpl->mSourceTransactionBody.set_allocated_transactionid(mImpl->mTransactionId->toProtobuf().release());
  }

  if (mImpl->mMaxTransactionFee.has_value())
  {
    mImpl->mSourceTransactionBody.set_transactionfee(static_cast<uint64_t>(mImpl->mMaxTransactionFee->toTinybars()));
  }
  else if (client && client->getMaxTransactionFee().has_value())
  {
    mImpl->mSourceTransactionBody.set_transactionfee(
      static_cast<uint64_t>(client->getMaxTransactionFee()->toTinybars()));
  }
  else
  {
    mImpl->mSourceTransactionBody.set_transactionfee(
      static_cast<uint64_t>(mImpl->mDefaultMaxTransactionFee.toTinybars()));
  }

  mImpl->mSourceTransactionBody.set_allocated_transactionvalidduration(
    internal::DurationConverter::toProtobuf(mImpl->mTransactionValidDuration));
  mImpl->mSourceTransactionBody.set_memo(mImpl->mTransactionMemo);

  if (mImpl->mBatchKey)
  {
    mImpl->mSourceTransactionBody.set_allocated_batch_key(mImpl->mBatchKey->toProtobufKey().release());
  }

  // Add derived Transaction fields to mSourceTransactionBody.
  addToBody(mImpl->mSourceTransactionBody);
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::addTransaction(const proto::Transaction& transaction) const
{
  // Add the Transaction protobuf object to the Transaction protobuf object
  // list.
  mImpl->mTransactions.push_back(transaction);

  // Parse the Transaction protobuf object into a SignedTransaction protobuf
  // object.
  proto::SignedTransaction signedTx;
  signedTx.ParseFromArray(transaction.signedtransactionbytes().data(),
                          static_cast<int>(transaction.signedtransactionbytes().size()));

  // Add the SignedTransaction protobuf object to the SignedTransaction protobuf
  // object list.
  mImpl->mSignedTransactions.push_back(signedTx);
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::addTransaction(const proto::SignedTransaction& transaction) const
{
  mImpl->mTransactions.push_back(proto::Transaction());
  mImpl->mSignedTransactions.push_back(transaction);
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::addSignedTransactionForEachNode(proto::TransactionBody transactionBody) const
{
  // For each node account ID, generate the SignedTransaction protobuf object.
  for (const AccountId& accountId :
       Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
         getNodeAccountIds())
  {
    transactionBody.set_allocated_nodeaccountid(accountId.toProtobuf().release());

    proto::SignedTransaction signedTransaction;
    signedTransaction.set_bodybytes(transactionBody.SerializeAsString());
    addTransaction(signedTransaction);
  }

  transactionBody.clear_nodeaccountid();
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::clearTransactions() const
{
  mImpl->mSignedTransactions.clear();
  mImpl->mTransactions.clear();
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::requireNotFrozen() const
{
  if (isFrozen())
  {
    throw IllegalStateException("Transaction is immutable and cannot be edited");
  }
}

//-----
template<typename SdkRequestType>
bool Transaction<SdkRequestType>::isFrozen() const
{
  return mImpl->mIsFrozen;
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::setDefaultMaxTransactionFee(const Hbar& fee)
{
  mImpl->mDefaultMaxTransactionFee = fee;
}

//-----
template<typename SdkRequestType>
std::map<AccountId, std::map<std::shared_ptr<PublicKey>, std::vector<std::byte>>>
Transaction<SdkRequestType>::getSignaturesInternal(size_t offset) const
{
  // Get each node account ID that the Transaction protobuf objects will be
  // sent.
  const std::vector<AccountId> nodeAccountIds =
    Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
      getNodeAccountIds();

  std::map<AccountId, std::map<std::shared_ptr<PublicKey>, std::vector<std::byte>>> signatures;
  for (int i = 0; i < nodeAccountIds.size(); ++i)
  {
    const proto::SignatureMap& signatureMap =
      mImpl->mSignedTransactions.at(offset * nodeAccountIds.size() + i).sigmap();
    for (const auto& [key, signer] : mImpl->mSignatories)
    {
      const std::string rawPublicKeyBytes = internal::Utilities::byteVectorToString(key->toBytesRaw());
      for (int j = 0; j < signatureMap.sigpair_size(); ++j)
      {
        const proto::SignaturePair& signaturePair = signatureMap.sigpair(j);
        if (signaturePair.has_ed25519() && signaturePair.ed25519() == rawPublicKeyBytes)
        {
          signatures[nodeAccountIds.at(i)][key] = internal::Utilities::stringToByteVector(signaturePair.ed25519());
        }
        else if (signaturePair.has_ecdsa_secp256k1() && signaturePair.ecdsa_secp256k1() == rawPublicKeyBytes)
        {
          signatures[nodeAccountIds.at(i)][key] =
            internal::Utilities::stringToByteVector(signaturePair.ecdsa_secp256k1());
        }
      }
    }
  }

  return signatures;
}

//-----
template<typename SdkRequestType>
proto::Transaction Transaction<SdkRequestType>::getTransactionProtobufObject(unsigned int index) const
{
  return mImpl->mTransactions.at(index);
}

//-----
template<typename SdkRequestType>
proto::TransactionBody Transaction<SdkRequestType>::getSourceTransactionBody() const
{
  // mSourceTransactionBody should not be updated in this call because
  // updateSourceTransactionBody() makes a virtual call to addBody(), which will
  // produce undefined behavior in the construction of derived Transactions. In
  // the constructors of derived Transactions, mSourceTransactionBody already
  // contains all the correct data and doesn't need an update. If this function
  // is called anywhere else, a call to updateSourceTransactionBody() should be
  // made before calling this to make sure any and all recent changes to this
  // Transaction are grabbed and used to update mSourceTransactionBody.
  return mImpl->mSourceTransactionBody;
}

//-----
template<typename SdkRequestType>
TransactionId Transaction<SdkRequestType>::getCurrentTransactionId() const
{
  return mImpl->mTransactionId.value_or(TransactionId());
}

//-----
template<typename SdkRequestType>
TransactionResponse Transaction<SdkRequestType>::mapResponse(const proto::TransactionResponse&) const
{
  return TransactionResponse(
    Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::getNodeAccountIds()
      .at(mImpl->mTransactionIndex %
          Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
            getNodeAccountIds()
              .size()),
    getCurrentTransactionId(),
    internal::OpenSSLUtils::computeSHA384(internal::Utilities::stringToByteVector(
      getTransactionProtobufObject(mImpl->mTransactionIndex).signedtransactionbytes())));
}

//-----
template<typename SdkRequestType>
Status Transaction<SdkRequestType>::mapResponseStatus(const proto::TransactionResponse& response) const
{
  return gProtobufResponseCodeToStatus.at(response.nodetransactionprecheckcode());
}

//-----
template<typename SdkRequestType>
typename Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
  ExecutionStatus
  Transaction<SdkRequestType>::determineStatus(Status status,
                                               const Client& client,
                                               const proto::TransactionResponse& response)
{
  // If the Transaction's not expired, forward to the base class.
  if (status != Status::TRANSACTION_EXPIRED)
  {
    return Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
      determineStatus(status, client, response);
  }

  bool shouldRegenerate = DEFAULT_REGENERATE_TRANSACTION_ID;
  if (mImpl->mTransactionIdManualSet)
  {
    shouldRegenerate = false;
  }

  // Follow this Transaction's policy if it has been explicitly set.
  else if (mImpl->mTransactionIdRegenerationPolicy.has_value())
  {
    shouldRegenerate = mImpl->mTransactionIdRegenerationPolicy.value();
  }

  // Follow the Client's policy if this Transaction's policy hasn't been
  // explicitly set and the Client's policy has been.
  else if (client.getTransactionIdRegenerationPolicy().has_value())
  {
    shouldRegenerate = client.getTransactionIdRegenerationPolicy().value();
  }

  if (shouldRegenerate)
  {
    // If transaction IDs are allowed to be regenerated, regenerate the
    // transaction ID and the Transaction protobuf objects.
    mImpl->mTransactionId = TransactionId::generate(mImpl->mTransactionId->mAccountId);

    // Regenerate the SignedTransaction protobuf objects.
    regenerateSignedTransactions(&client);

    // Retry execution with the new transaction ID.
    return Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
      ExecutionStatus::RETRY;
  }

  // Return REQUEST_ERROR if the transaction expired but transaction IDs aren't
  // allowed to be regenerated.
  return Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
    ExecutionStatus::REQUEST_ERROR;
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::onExecute(const Client& client)
{
  if (!isFrozen())
  {
    freezeWith(&client);
  }

  // Validate checksums if that option is enabled.
  if (client.isAutoValidateChecksumsEnabled())
  {
    validateChecksums(client);
  }

  // Sign with the operator if the operator's present, and if it's paying for
  // the Transaction.
  if (client.getOperatorAccountId().has_value() &&
      client.getOperatorAccountId().value() == mImpl->mTransactionId->mAccountId)
  {
    signWithOperator(client);
  }
}

//-----
template<typename SdkRequestType>
void Transaction<SdkRequestType>::buildTransaction(unsigned int index) const
{
  // If the Transaction protobuf object is already built for this index, there's
  // no need to do anything else.
  if (!getTransactionProtobufObject(index).signedtransactionbytes().empty())
  {
    return;
  }

  // For each PublicKey and signer function, generate a signature of the
  // TransactionBody protobuf object bytes held in the SignedTransaction
  // protobuf object at the provided index.
  proto::SignedTransaction& signedTransaction = mImpl->mSignedTransactions[index];
  for (const auto& [publicKey, signer] : mImpl->mSignatories)
  {
    // If there is no signer function, the signature has already been generated
    // for the SignedTransaction (either added manually with addSignature() or
    // this Transaction came from fromBytes()).
    if (signer)
    {
      *signedTransaction.mutable_sigmap()->add_sigpair() = *publicKey->toSignaturePairProtobuf(
        signer(internal::Utilities::stringToByteVector(signedTransaction.bodybytes())));
    }
  }

  mImpl->mTransactions[index].set_signedtransactionbytes(signedTransaction.SerializeAsString());
}

//-----
template<typename SdkRequestType>
std::optional<TransactionId> Transaction<SdkRequestType>::getTransactionIdInternal() const
{
  return mImpl->mTransactionId;
}

//-----
template<typename SdkRequestType>
bool Transaction<SdkRequestType>::keyAlreadySigned(const std::shared_ptr<PublicKey>& publicKey) const
{
  const std::vector<std::byte> publicKeyBytes = publicKey->toBytesDer();
  return std::any_of(
    mImpl->mSignatories.cbegin(),
    mImpl->mSignatories.cend(),
    [&publicKeyBytes](
      const std::pair<std::shared_ptr<PublicKey>, std::function<std::vector<std::byte>(const std::vector<std::byte>&)>>&
        toSign) { return toSign.first->toBytesDer() == publicKeyBytes; });
}

//-----
template<typename SdkRequestType>
SdkRequestType& Transaction<SdkRequestType>::signInternal(
  const std::shared_ptr<PublicKey>& publicKey,
  const std::function<std::vector<std::byte>(const std::vector<std::byte>&)>& signer,
  const std::shared_ptr<PrivateKey>& privateKey)
{
  // The Transaction must be frozen in order to sign.
  if (!isFrozen())
  {
    throw IllegalStateException("Transaction must be frozen in order to sign.");
  }

  if (!keyAlreadySigned(publicKey))
  {
    // Adding a signature will require all Transaction protobuf objects to be
    // regenerated.
    mImpl->mTransactions.clear();
    mImpl->mTransactions.resize(mImpl->mSignedTransactions.size());
    mImpl->mSignatories.emplace(publicKey, signer);
    mImpl->mPrivateKeys.emplace(publicKey, privateKey);
  }

  return static_cast<SdkRequestType&>(*this);
}

/**
 * Explicit template instantiations.
 */
template class Transaction<AccountAllowanceApproveTransaction>;
template class Transaction<AccountAllowanceDeleteTransaction>;
template class Transaction<AccountCreateTransaction>;
template class Transaction<AccountDeleteTransaction>;
template class Transaction<AccountUpdateTransaction>;
template class Transaction<BatchTransaction>;
template class Transaction<ContractCreateTransaction>;
template class Transaction<ContractDeleteTransaction>;
template class Transaction<ContractExecuteTransaction>;
template class Transaction<ContractUpdateTransaction>;
template class Transaction<EthereumTransaction>;
template class Transaction<FileAppendTransaction>;
template class Transaction<FileCreateTransaction>;
template class Transaction<FileDeleteTransaction>;
template class Transaction<FileUpdateTransaction>;
template class Transaction<FreezeTransaction>;
template class Transaction<NodeCreateTransaction>;
template class Transaction<NodeDeleteTransaction>;
template class Transaction<NodeUpdateTransaction>;
template class Transaction<PrngTransaction>;
template class Transaction<ScheduleCreateTransaction>;
template class Transaction<ScheduleDeleteTransaction>;
template class Transaction<ScheduleSignTransaction>;
template class Transaction<SystemDeleteTransaction>;
template class Transaction<SystemUndeleteTransaction>;
template class Transaction<TokenAirdropTransaction>;
template class Transaction<TokenAssociateTransaction>;
template class Transaction<TokenBurnTransaction>;
template class Transaction<TokenCancelAirdropTransaction>;
template class Transaction<TokenClaimAirdropTransaction>;
template class Transaction<TokenCreateTransaction>;
template class Transaction<TokenDeleteTransaction>;
template class Transaction<TokenDissociateTransaction>;
template class Transaction<TokenFeeScheduleUpdateTransaction>;
template class Transaction<TokenFreezeTransaction>;
template class Transaction<TokenGrantKycTransaction>;
template class Transaction<TokenMintTransaction>;
template class Transaction<TokenPauseTransaction>;
template class Transaction<TokenRejectTransaction>;
template class Transaction<TokenRevokeKycTransaction>;
template class Transaction<TokenUnfreezeTransaction>;
template class Transaction<TokenUnpauseTransaction>;
template class Transaction<TokenUpdateNftsTransaction>;
template class Transaction<TokenUpdateTransaction>;
template class Transaction<TokenWipeTransaction>;
template class Transaction<TopicCreateTransaction>;
template class Transaction<TopicDeleteTransaction>;
template class Transaction<TopicMessageSubmitTransaction>;
template class Transaction<TopicUpdateTransaction>;
template class Transaction<TransferTransaction>;

} // namespace Hiero