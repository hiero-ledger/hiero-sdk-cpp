// SPDX-License-Identifier: Apache-2.0
#include "EthereumFlow.h"
#include "Client.h"
#include "EthereumTransaction.h"
#include "FileAppendTransaction.h"
#include "FileCreateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "exceptions/IllegalStateException.h"

namespace Hiero
{
//-----
TransactionResponse EthereumFlow::execute(const Client& client)
{
  return execute(client, client.getRequestTimeout());
}

//-----
TransactionResponse EthereumFlow::execute(const Client& client, const std::chrono::system_clock::duration& timeout)
{
  // Make sure the ethereum data has been set. There is nothing to do if there is no ethereum data.
  if (!mEthereumData)
  {
    throw IllegalStateException("Cannot execute EthereumTransaction with no EthereumTransactionData");
  }

  // Create the transaction.
  EthereumTransaction ethereumTransaction;
  if (mMaxGasAllowance.has_value())
  {
    ethereumTransaction.setMaxGasAllowance(mMaxGasAllowance.value());
  }

  // Create a file to hold the Ethereum call data if the data is too large.
  if (const std::vector<std::byte> ethereumDataBytes = mEthereumData->toBytes();
      ethereumDataBytes.size() > MAX_ETHEREUM_DATA_SIZE)
  {
    // Put the Ethereum call data into a file.
    const FileId fileId =
      FileCreateTransaction()
        .setContents({ mEthereumData->mCallData.cbegin(),
                       mEthereumData->mCallData.cbegin() + FileAppendTransaction::DEFAULT_CHUNK_SIZE })
        .execute(client, timeout)
        .getReceipt(client, timeout)
        .mFileId.value();

    FileAppendTransaction()
      .setFileId(fileId)
      .setContents({ mEthereumData->mCallData.cbegin() + FileAppendTransaction::DEFAULT_CHUNK_SIZE,
                     mEthereumData->mCallData.cend() })
      .execute(client, timeout);

    // Set the ethereum data without the call data, and reference the file ID that contains the call data.
    mEthereumData->mCallData.clear();
    ethereumTransaction.setEthereumData(mEthereumData->toBytes()).setCallDataFileId(fileId);
  }
  else
  {
    ethereumTransaction.setEthereumData(ethereumDataBytes);
  }

  return ethereumTransaction.execute(client, timeout);
}

//-----
EthereumFlow& EthereumFlow::setEthereumData(const std::vector<std::byte>& data)
{
  mEthereumData = std::shared_ptr<EthereumTransactionData>(EthereumTransactionData::fromBytes(data).release());
  return *this;
}

//-----
EthereumFlow& EthereumFlow::setMaxGasAllowance(const Hbar& maxGasAllowance)
{
  mMaxGasAllowance = maxGasAllowance;
  return *this;
}

} // namespace Hiero
