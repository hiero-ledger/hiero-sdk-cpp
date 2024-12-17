// SPDX-License-Identifier: Apache-2.0
#include "ContractCreateFlow.h"
#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ContractFunctionParameters.h"
#include "FileAppendTransaction.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileId.h"
#include "PrivateKey.h"
#include "TransactionReceipt.h"
#include "exceptions/UninitializedException.h"
#include "impl/Utilities.h"

namespace Hiero
{
//-----
TransactionResponse ContractCreateFlow::execute(const Client& client)
{
  return execute(client, client.getRequestTimeout());
}

//-----
TransactionResponse ContractCreateFlow::execute(const Client& client,
                                                const std::chrono::system_clock::duration& timeout)
{
  // First determine if the input bytecode needs to be split.
  std::vector<std::byte> appendedByteCode;
  if (mBytecode.size() > 2048ULL)
  {
    appendedByteCode = internal::Utilities::removePrefix(mBytecode, 2048);
    mBytecode = { mBytecode.cbegin(), mBytecode.cbegin() + 2048 };
  }

  // Create the file.
  FileCreateTransaction fileCreateTransaction =
    FileCreateTransaction().setKeys({ client.getOperatorPublicKey() }).setContents(mBytecode);

  if (!mNodeAccountIds.empty())
  {
    fileCreateTransaction.setNodeAccountIds(mNodeAccountIds);
  }

  const FileId fileId = fileCreateTransaction.execute(client, timeout).getReceipt(client, timeout).mFileId.value();

  // Append to the file if needed.
  if (!appendedByteCode.empty())
  {
    FileAppendTransaction fileAppendTransaction =
      FileAppendTransaction().setFileId(fileId).setContents(appendedByteCode).setMaxChunks(mMaxChunks);

    if (!mNodeAccountIds.empty())
    {
      fileAppendTransaction.setNodeAccountIds(mNodeAccountIds);
    }

    fileAppendTransaction.execute(client, timeout);
  }

  // Create the smart contract instance using the bytecode in the file.
  ContractCreateTransaction contractCreateTransaction =
    ContractCreateTransaction()
      .setBytecodeFileId(fileId)
      .setConstructorParameters(mConstructorParameters)
      .setGas(mGas)
      .setInitialBalance(mInitialBalance)
      .setAutoRenewPeriod(mAutoRenewPeriod)
      .setMaxAutomaticTokenAssociations(mMaxAutomaticTokenAssociations)
      .setDeclineStakingReward(mDeclineStakingReward);

  if (mAdminKey)
  {
    contractCreateTransaction.setAdminKey(mAdminKey);
  }

  if (mAutoRenewAccountId.has_value())
  {
    contractCreateTransaction.setAutoRenewAccountId(mAutoRenewAccountId.value());
  }

  if (!mMemo.empty())
  {
    contractCreateTransaction.setMemo(mMemo);
  }

  if (mStakedAccountId.has_value())
  {
    contractCreateTransaction.setStakedAccountId(mStakedAccountId.value());
  }
  else if (mStakedNodeId.has_value())
  {
    contractCreateTransaction.setStakedNodeId(mStakedNodeId.value());
  }

  if (!mNodeAccountIds.empty())
  {
    contractCreateTransaction.setNodeAccountIds(mNodeAccountIds);
  }

  if (mFreezeWithClient != nullptr)
  {
    contractCreateTransaction.freezeWith(mFreezeWithClient);
  }

  if (mPrivateKey)
  {
    contractCreateTransaction.sign(mPrivateKey);
  }
  else if (mPublicKey && mSigner.has_value())
  {
    contractCreateTransaction.signWith(mPublicKey, mSigner.value());
  }

  // Submit the ContractCreateTransaction.
  TransactionResponse txResponse = contractCreateTransaction.execute(client, timeout);

  // Make sure the transaction reaches consensus.
  const TransactionReceipt txReceipt = txResponse.getReceipt(client, timeout);

  // Delete the bytecode file.
  FileDeleteTransaction().setFileId(fileId).execute(client, timeout);

  return txResponse;
}

//-----
ContractCreateFlow& ContractCreateFlow::setBytecode(const std::vector<std::byte>& initCode)
{
  mBytecode = initCode;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setBytecode(std::string_view byteCode)
{
  mBytecode = internal::Utilities::stringToByteVector(byteCode);
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setAdminKey(const std::shared_ptr<Key>& key)
{
  mAdminKey = key;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setGas(const uint64_t& gas)
{
  mGas = gas;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setInitialBalance(const Hbar& initialBalance)
{
  mInitialBalance = initialBalance;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenewPeriod)
{
  mAutoRenewPeriod = autoRenewPeriod;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setConstructorParameters(
  const ContractFunctionParameters& constructorParameters)
{
  return setConstructorParameters(constructorParameters.toBytes());
}

//-----
ContractCreateFlow& ContractCreateFlow::setConstructorParameters(const std::vector<std::byte>& constructorParameters)
{
  mConstructorParameters = constructorParameters;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setMemo(std::string_view memo)
{
  mMemo = memo;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setMaxAutomaticTokenAssociations(int32_t associations)
{
  mMaxAutomaticTokenAssociations = associations;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setAutoRenewAccountId(const AccountId& autoRenewAccountId)
{
  mAutoRenewAccountId = autoRenewAccountId;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setStakedAccountId(const AccountId& stakedAccountId)
{
  mStakedAccountId = stakedAccountId;
  mStakedNodeId.reset();
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setStakedNodeId(const uint64_t& stakedNodeId)
{
  mStakedNodeId = stakedNodeId;
  mStakedAccountId.reset();
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setDeclineStakingReward(bool declineReward)
{
  mDeclineStakingReward = declineReward;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setNodeAccountIds(const std::vector<AccountId>& nodeAccountIds)
{
  mNodeAccountIds = nodeAccountIds;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::setMaxChunks(unsigned int chunks)
{
  mMaxChunks = chunks;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::freezeWith(const Client& client)
{
  mFreezeWithClient = &client;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::sign(const std::shared_ptr<PrivateKey>& key)
{
  mPrivateKey = key;
  mPublicKey = nullptr;
  mSigner.reset();
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::signWith(
  const std::shared_ptr<PublicKey>& key,
  const std::function<std::vector<std::byte>(const std::vector<std::byte>&)>& signer)
{
  mPrivateKey = nullptr;
  mPublicKey = key;
  mSigner = signer;
  return *this;
}

//-----
ContractCreateFlow& ContractCreateFlow::signWithOperator(const Client& client)
{
  if (!client.getOperatorPublicKey())
  {
    throw UninitializedException("Client operator has not yet been set");
  }

  mPrivateKey = nullptr;
  mPublicKey = client.getOperatorPublicKey();
  mSigner = client.getOperatorSigner();
  return *this;
}

} // namespace Hiero
