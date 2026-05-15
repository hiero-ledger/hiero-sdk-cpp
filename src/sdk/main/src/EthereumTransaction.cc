// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransaction.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "EthereumTransactionData.h"
#include "EthereumTransactionDataEip1559.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <services/ethereum_transaction.pb.h>
#include <services/transaction.pb.h>

#include <stdexcept>

namespace Hiero
{
//-----
EthereumTransaction::EthereumTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<EthereumTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
EthereumTransaction::EthereumTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<EthereumTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
EthereumTransaction& EthereumTransaction::setEthereumData(const std::vector<std::byte>& ethereumData)
{
  requireNotFrozen();
  mEthereumData = ethereumData;
  return *this;
}

//-----
EthereumTransaction& EthereumTransaction::setCallDataFileId(const FileId& fileId)
{
  requireNotFrozen();
  mCallDataFileId = fileId;
  return *this;
}

//-----
EthereumTransaction& EthereumTransaction::setMaxGasAllowance(const Hbar& maxGasAllowance)
{
  requireNotFrozen();
  mMaxGasAllowance = maxGasAllowance;
  return *this;
}

//-----
EthereumTransaction& EthereumTransaction::sign(const std::shared_ptr<PrivateKey>& key)
{
  if (!key)
  {
    throw std::invalid_argument("Signing key must be set.");
  }

  EthereumTransaction& signedTransaction =
    static_cast<EthereumTransaction&>(Transaction<EthereumTransaction>::sign(key));

  const auto ecdsaKey = std::dynamic_pointer_cast<ECDSAsecp256k1PrivateKey>(key);
  if (!ecdsaKey || mEthereumData.empty())
  {
    return signedTransaction;
  }

  std::unique_ptr<EthereumTransactionData> ethData = EthereumTransactionData::fromBytes(mEthereumData);
  auto* eip1559Data = dynamic_cast<EthereumTransactionDataEip1559*>(ethData.get());
  if (!eip1559Data)
  {
    return signedTransaction;
  }

  const std::vector<std::byte> messageToSign = eip1559Data->toSignBytes();
  const std::vector<std::byte> signatureBytes = ecdsaKey->sign(messageToSign);
  if (signatureBytes.size() != ECDSAsecp256k1PrivateKey::RAW_SIGNATURE_SIZE)
  {
    throw std::runtime_error("Unexpected ECDSA signature size while signing Ethereum transaction");
  }

  std::vector<std::byte> r(signatureBytes.cbegin(), signatureBytes.cbegin() + ECDSAsecp256k1PrivateKey::R_SIZE);
  std::vector<std::byte> s(signatureBytes.cbegin() + ECDSAsecp256k1PrivateKey::R_SIZE, signatureBytes.cend());

  const int recId = ecdsaKey->getRecoveryId(r, s, messageToSign);
  if (recId < 0)
  {
    throw std::runtime_error("Failed to compute recovery ID during Ethereum transaction signing");
  }

  eip1559Data->mR = std::move(r);
  eip1559Data->mS = std::move(s);
  eip1559Data->mRecoveryId = { static_cast<std::byte>(recId) };
  mEthereumData = eip1559Data->toBytes();

  regenerateSignedTransactions(nullptr);
  return signedTransaction;
}

//-----
grpc::Status EthereumTransaction::submitRequest(const proto::Transaction& request,
                                                const std::shared_ptr<internal::Node>& node,
                                                const std::chrono::system_clock::time_point& deadline,
                                                proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kEthereumTransaction, request, deadline, response);
}

//-----
void EthereumTransaction::validateChecksums(const Client& client) const
{
  if (mCallDataFileId.has_value())
  {
    mCallDataFileId->validateChecksum(client);
  }
}

//-----
void EthereumTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_ethereumtransaction(build());
}

//-----
void EthereumTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_ethereumtransaction())
  {
    throw std::invalid_argument("Transaction body doesn't contain EthereumTransaction data");
  }

  const proto::EthereumTransactionBody& body = transactionBody.ethereumtransaction();

  mEthereumData = internal::Utilities::stringToByteVector(body.ethereum_data());

  if (body.has_call_data())
  {
    mCallDataFileId = FileId::fromProtobuf(body.call_data());
  }

  mMaxGasAllowance = Hbar(body.max_gas_allowance(), HbarUnit::TINYBAR());
}

//-----
proto::EthereumTransactionBody* EthereumTransaction::build() const
{
  auto body = std::make_unique<proto::EthereumTransactionBody>();

  body->set_ethereum_data(internal::Utilities::byteVectorToString(mEthereumData));

  if (mCallDataFileId.has_value())
  {
    body->set_allocated_call_data(mCallDataFileId->toProtobuf().release());
  }

  body->set_max_gas_allowance(mMaxGasAllowance.toTinybars());

  return body.release();
}

} // namespace Hiero
