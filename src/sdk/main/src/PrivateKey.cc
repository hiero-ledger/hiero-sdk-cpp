// SPDX-License-Identifier: Apache-2.0
#include "PrivateKey.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "Transaction.h"
#include "WrappedTransaction.h"
#include "exceptions/BadKeyException.h"
#include "exceptions/OpenSSLException.h"
#include "impl/HexConverter.h"
#include "impl/PrivateKeyImpl.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <openssl/x509.h>
#include <stdexcept>
#include <transaction.pb.h>
#include <transaction_contents.pb.h>

namespace Hiero
{
//-----
PrivateKey::~PrivateKey() = default;

//-----
std::unique_ptr<PrivateKey> PrivateKey::fromStringDer(std::string_view key)
{
  if (const std::string_view prefix = "0x"; key.substr(0, prefix.size()) == prefix)
  {
    key.remove_prefix(prefix.size());
  }

  try
  {
    return PrivateKey::fromBytesDer(internal::HexConverter::hexToBytes(key));
  }
  catch (const OpenSSLException&)
  {
    throw BadKeyException(std::string("Unable to decode input key string ") + key.data());
  }
}

//-----
std::unique_ptr<PrivateKey> PrivateKey::fromBytesDer(const std::vector<std::byte>& bytes)
{
  if (internal::Utilities::isPrefixOf(bytes, ED25519PrivateKey::DER_ENCODED_PREFIX_BYTES))
  {
    return ED25519PrivateKey::fromBytes(bytes);
  }

  else if (internal::Utilities::isPrefixOf(bytes, ECDSAsecp256k1PrivateKey::DER_ENCODED_PREFIX_BYTES))
  {
    return ECDSAsecp256k1PrivateKey::fromBytes(bytes);
  }

  throw BadKeyException("Key type cannot be determined from input DER-encoded byte array");
}

//-----
template<typename SdkRequestType>
std::vector<std::byte> PrivateKey::signTransaction(Transaction<SdkRequestType>& transaction) const
{
  // Verify that the Transaction is only going to one node.
  transaction.requireOneNodeAccountId();

  // Freeze the transaction if not already frozen.
  if (!transaction.isFrozen())
  {
    transaction.freeze();
  }

  // This PrivateKey is only able to grab the built Transaction protobuf object, so make sure it's built (index is
  // guaranteed 0 since the one node account ID check has already passed).
  transaction.buildTransaction(0U);

  // Generate the signature.
  proto::SignedTransaction transactionToSign;
  transactionToSign.ParseFromString(transaction.getTransactionProtobufObject(0U).signedtransactionbytes());
  const std::vector<std::byte> signature = sign(internal::Utilities::stringToByteVector(transactionToSign.bodybytes()));

  // Add the signature to the Transaction.
  transaction.addSignature(getPublicKey(), signature);

  return signature;
}

//-----
std::vector<std::byte> PrivateKey::signTransaction(WrappedTransaction& transaction) const
{
  switch (transaction.getTransactionType())
  {
    case ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<AccountAllowanceApproveTransaction>());
    case ACCOUNT_ALLOWANCE_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<AccountAllowanceDeleteTransaction>());
    case ACCOUNT_CREATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<AccountCreateTransaction>());
    case ACCOUNT_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<AccountDeleteTransaction>());
    case ACCOUNT_UPDATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<AccountUpdateTransaction>());
    case CONTRACT_CREATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<ContractCreateTransaction>());
    case CONTRACT_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<ContractDeleteTransaction>());
    case CONTRACT_EXECUTE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<ContractExecuteTransaction>());
    case CONTRACT_UPDATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<ContractUpdateTransaction>());
    case ETHEREUM_TRANSACTION:
      return signTransaction(*transaction.getTransaction<EthereumTransaction>());
    case FILE_APPEND_TRANSACTION:
      return signTransaction(*transaction.getTransaction<FileAppendTransaction>());
    case FILE_CREATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<FileCreateTransaction>());
    case FILE_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<FileDeleteTransaction>());
    case FILE_UPDATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<FileUpdateTransaction>());
    case FREEZE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<FreezeTransaction>());
    case PRNG_TRANSACTION:
      return signTransaction(*transaction.getTransaction<PrngTransaction>());
    case SCHEDULE_CREATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<ScheduleCreateTransaction>());
    case SCHEDULE_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<ScheduleDeleteTransaction>());
    case SCHEDULE_SIGN_TRANSACTION:
      return signTransaction(*transaction.getTransaction<ScheduleSignTransaction>());
    case SYSTEM_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<SystemDeleteTransaction>());
    case SYSTEM_UNDELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<SystemUndeleteTransaction>());
    case TOKEN_ASSOCIATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenAssociateTransaction>());
    case TOKEN_BURN_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenBurnTransaction>());
    case TOKEN_CREATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenCreateTransaction>());
    case TOKEN_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenDeleteTransaction>());
    case TOKEN_DISSOCIATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenDissociateTransaction>());
    case TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenFeeScheduleUpdateTransaction>());
    case TOKEN_FREEZE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenFreezeTransaction>());
    case TOKEN_GRANT_KYC_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenGrantKycTransaction>());
    case TOKEN_MINT_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenMintTransaction>());
    case TOKEN_PAUSE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenPauseTransaction>());
    case TOKEN_REJECT_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenRejectTransaction>());
    case TOKEN_REVOKE_KYC_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenRevokeKycTransaction>());
    case TOKEN_UNFREEZE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenUnfreezeTransaction>());
    case TOKEN_UNPAUSE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenUnpauseTransaction>());
    case TOKEN_UPDATE_NFTS_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenUpdateNftsTransaction>());
    case TOKEN_UPDATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenUpdateTransaction>());
    case TOKEN_WIPE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TokenWipeTransaction>());
    case TOPIC_CREATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TopicCreateTransaction>());
    case TOPIC_DELETE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TopicDeleteTransaction>());
    case TOPIC_MESSAGE_SUBMIT_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TopicMessageSubmitTransaction>());
    case TOPIC_UPDATE_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TopicUpdateTransaction>());
    case TRANSFER_TRANSACTION:
      return signTransaction(*transaction.getTransaction<TransferTransaction>());
    case UNKNOWN_TRANSACTION:
    default:
    {
      throw std::invalid_argument("Unrecognized TransactionType");
    }
  }
}

//-----
std::vector<std::byte> PrivateKey::getChainCode() const
{
  return mImpl->mChainCode;
}

//-----
std::shared_ptr<PublicKey> PrivateKey::getPublicKey() const
{
  return mImpl->mPublicKey;
}

//-----
std::ostream& operator<<(std::ostream& os, const PrivateKey& key)
{
  os << key.toStringDer();
  return os;
}

//-----
PrivateKey::PrivateKey(const PrivateKey& other)
  : mImpl(std::make_unique<PrivateKeyImpl>(*other.mImpl))
{
}

//-----
PrivateKey& PrivateKey::operator=(const PrivateKey& other)
{
  if (this != &other)
  {
    mImpl = std::make_unique<PrivateKeyImpl>(*other.mImpl);
  }

  return *this;
}

//-----
PrivateKey::PrivateKey(PrivateKey&& other) noexcept
  : mImpl(std::move(other.mImpl))
{
  // Leave the moved-from PrivateKey in a valid state.
  other.mImpl = std::make_unique<PrivateKeyImpl>();
}

//-----
PrivateKey& PrivateKey::operator=(PrivateKey&& other) noexcept
{
  if (this != &other)
  {
    mImpl = std::move(other.mImpl);

    // Leave the moved-from PrivateKey in a valid state.
    other.mImpl = std::make_unique<PrivateKeyImpl>();
  }

  return *this;
}

//-----
PrivateKey::PrivateKey(internal::OpenSSLUtils::EVP_PKEY&& key, std::vector<std::byte> chainCode)
  : mImpl(std::make_unique<PrivateKeyImpl>())
{
  mImpl->mKey = std::move(key);
  mImpl->mChainCode = std::move(chainCode);

  if (!mImpl->mChainCode.empty() && mImpl->mChainCode.size() != CHAIN_CODE_SIZE)
  {
    throw BadKeyException("Key chain code malformed");
  }

  std::vector<std::byte> keyBytes(i2d_PUBKEY(mImpl->mKey.get(), nullptr));

  if (auto* rawPublicKeyBytes = internal::Utilities::toTypePtr<unsigned char>(keyBytes.data());
      i2d_PUBKEY(mImpl->mKey.get(), &rawPublicKeyBytes) <= 0)
  {
    throw OpenSSLException(internal::OpenSSLUtils::getErrorMessage("i2d_PUBKEY"));
  }

  mImpl->mPublicKey = PublicKey::fromBytesDer(keyBytes);
}

//-----
internal::OpenSSLUtils::EVP_PKEY PrivateKey::getInternalKey() const
{
  return mImpl->mKey;
}

} // namespace Hiero
