// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountAllowanceDeleteTransaction.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountUpdateTransaction.h"
#include "BaseUnitTest.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractUpdateTransaction.h"
#include "EthereumTransaction.h"
#include "FileAppendTransaction.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileUpdateTransaction.h"
#include "FreezeTransaction.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleDeleteTransaction.h"
#include "ScheduleSignTransaction.h"
#include "SystemDeleteTransaction.h"
#include "SystemUndeleteTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenBurnTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenDissociateTransaction.h"
#include "TokenFeeScheduleUpdateTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TokenGrantKycTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenPauseTransaction.h"
#include "TokenRevokeKycTransaction.h"
#include "TokenUnfreezeTransaction.h"
#include "TokenUnpauseTransaction.h"
#include "TokenUpdateTransaction.h"
#include "TokenWipeTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicMessageSubmitTransaction.h"
#include "TopicUpdateTransaction.h"
#include "Transaction.h"
#include "TransactionType.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/transaction.pb.h>

#include <services/transaction_contents.pb.h>
#include <transaction_list.pb.h>

using namespace Hiero;

class TransactionUnitTests : public BaseUnitTest
{
};

//-----
TEST_F(TransactionUnitTests, AccountApproveAllowanceFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoapproveallowance(new proto::CryptoApproveAllowanceTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountAllowanceApproveTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountAllowanceApproveTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountApproveAllowanceFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoapproveallowance(new proto::CryptoApproveAllowanceTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountAllowanceApproveTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountAllowanceApproveTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountApproveAllowanceFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoapproveallowance(new proto::CryptoApproveAllowanceTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountAllowanceApproveTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountAllowanceApproveTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountDeleteAllowanceFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodeleteallowance(new proto::CryptoDeleteAllowanceTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountAllowanceDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_ALLOWANCE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountAllowanceDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountDeleteAllowanceFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodeleteallowance(new proto::CryptoDeleteAllowanceTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountAllowanceDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_ALLOWANCE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountAllowanceDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountDeleteAllowanceFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodeleteallowance(new proto::CryptoDeleteAllowanceTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountAllowanceDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_ALLOWANCE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountAllowanceDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountCreateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptocreateaccount(new proto::CryptoCreateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountCreateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountCreateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptocreateaccount(new proto::CryptoCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<AccountCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountCreateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptocreateaccount(new proto::CryptoCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountCreateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountDeleteTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodelete(new proto::CryptoDeleteTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountDeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodelete(new proto::CryptoDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<AccountDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountDeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptodelete(new proto::CryptoDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountUpdateTransactionFromTransactionBodyByte)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoupdateaccount(new proto::CryptoUpdateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountUpdateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountUpdateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoupdateaccount(new proto::CryptoUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<AccountUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, AccountUpdateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptoupdateaccount(new proto::CryptoUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<AccountUpdateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ACCOUNT_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<AccountUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractCreateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractcreateinstance(new proto::ContractCreateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractCreateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractCreateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractcreateinstance(new proto::ContractCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<ContractCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractCreateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractcreateinstance(new proto::ContractCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractCreateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractDeleteTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractdeleteinstance(new proto::ContractDeleteTransactionBody());

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractDeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractdeleteinstance(new proto::ContractDeleteTransactionBody());

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<ContractDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractDeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractdeleteinstance(new proto::ContractDeleteTransactionBody());

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractExecuteTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractcall(new proto::ContractCallTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractExecuteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_EXECUTE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractExecuteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractExecuteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractcall(new proto::ContractCallTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<ContractExecuteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_EXECUTE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractExecuteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractExecuteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractcall(new proto::ContractCallTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractExecuteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_EXECUTE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractExecuteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractUpdateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractupdateinstance(new proto::ContractUpdateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractUpdateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractUpdateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractupdateinstance(new proto::ContractUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<ContractUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ContractUpdateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_contractupdateinstance(new proto::ContractUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<ContractUpdateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::CONTRACT_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ContractUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, EthereumTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_ethereumtransaction(new proto::EthereumTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<EthereumTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ETHEREUM_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<EthereumTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, EthereumTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_ethereumtransaction(new proto::EthereumTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<EthereumTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ETHEREUM_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<EthereumTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, EthereumTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_ethereumtransaction(new proto::EthereumTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<EthereumTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::ETHEREUM_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<EthereumTransaction>(), nullptr);
}

TEST_F(TransactionUnitTests, FileAppendTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_fileappend(new proto::FileAppendTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileAppendTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_APPEND_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileAppendTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileAppendTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_fileappend(new proto::FileAppendTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileAppendTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_APPEND_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileAppendTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileAppendTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_fileappend(new proto::FileAppendTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileAppendTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_APPEND_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileAppendTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileCreateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_filecreate(new proto::FileCreateTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileCreateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_filecreate(new proto::FileCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileCreateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_filecreate(new proto::FileCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileDeleteTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_filedelete(new proto::FileDeleteTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileDeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_filedelete(new proto::FileDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileDeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_filedelete(new proto::FileDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileUpdateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_fileupdate(new proto::FileUpdateTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileUpdateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_fileupdate(new proto::FileUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FileUpdateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_fileupdate(new proto::FileUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FileUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FILE_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FileUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FreezeTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_freeze(new proto::FreezeTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FreezeTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FreezeTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_freeze(new proto::FreezeTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FreezeTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, FreezeTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_freeze(new proto::FreezeTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<FreezeTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::FREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<FreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, PrngTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_util_prng(new proto::UtilPrngTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<PrngTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::PRNG_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<PrngTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, PrngTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_util_prng(new proto::UtilPrngTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<PrngTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::PRNG_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<PrngTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, PrngTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_util_prng(new proto::UtilPrngTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<PrngTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::PRNG_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<PrngTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleCreateTransactionTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_schedulecreate(new proto::ScheduleCreateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<ScheduleCreateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleCreateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_schedulecreate(new proto::ScheduleCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<ScheduleCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleCreateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_schedulecreate(new proto::ScheduleCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<ScheduleCreateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleDeleteTransactionTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_scheduledelete(new proto::ScheduleDeleteTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<ScheduleDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleDeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_scheduledelete(new proto::ScheduleDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<ScheduleDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleDeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_scheduledelete(new proto::ScheduleDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<ScheduleDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleSignTransactionTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_schedulesign(new proto::ScheduleSignTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<ScheduleSignTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_SIGN_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleSignTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleSignTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_schedulesign(new proto::ScheduleSignTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<ScheduleSignTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_SIGN_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleSignTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, ScheduleSignTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_schedulesign(new proto::ScheduleSignTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<ScheduleSignTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SCHEDULE_SIGN_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<ScheduleSignTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, SystemDeleteTransactionTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_systemdelete(new proto::SystemDeleteTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<SystemDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SYSTEM_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<SystemDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, SystemDeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_systemdelete(new proto::SystemDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<SystemDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SYSTEM_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<SystemDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, SystemDeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_systemdelete(new proto::SystemDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<SystemDeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SYSTEM_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<SystemDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, SystemUndeleteTransactionTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_systemundelete(new proto::SystemUndeleteTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<SystemUndeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SYSTEM_UNDELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<SystemUndeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, SystemUndeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_systemundelete(new proto::SystemUndeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<SystemUndeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SYSTEM_UNDELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<SystemUndeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, SystemUndeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_systemundelete(new proto::SystemUndeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<SystemUndeleteTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::SYSTEM_UNDELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<SystemUndeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenAssociateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenassociate(new proto::TokenAssociateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenAssociateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_ASSOCIATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenAssociateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenAssociateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenassociate(new proto::TokenAssociateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenAssociateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_ASSOCIATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenAssociateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenAssociateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenassociate(new proto::TokenAssociateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenAssociateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_ASSOCIATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenAssociateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenBurnTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenburn(new proto::TokenBurnTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenBurnTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_BURN_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenBurnTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenBurnTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenburn(new proto::TokenBurnTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenBurnTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_BURN_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenBurnTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenBurnTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenburn(new proto::TokenBurnTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenBurnTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_BURN_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenBurnTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenCreateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokencreation(new proto::TokenCreateTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenCreateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokencreation(new proto::TokenCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenCreateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokencreation(new proto::TokenCreateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenDeleteTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokendeletion(new proto::TokenDeleteTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenDeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokendeletion(new proto::TokenDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenDeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokendeletion(new proto::TokenDeleteTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenDissociateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokendissociate(new proto::TokenDissociateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenDissociateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_DISSOCIATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenDissociateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenDissociateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokendissociate(new proto::TokenDissociateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenDissociateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_DISSOCIATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenDissociateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenDissociateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokendissociate(new proto::TokenDissociateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenDissociateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_DISSOCIATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenDissociateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenFeeScheduleUpdateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_fee_schedule_update(new proto::TokenFeeScheduleUpdateTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenFeeScheduleUpdateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenFeeScheduleUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenFeeScheduleUpdateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_fee_schedule_update(new proto::TokenFeeScheduleUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenFeeScheduleUpdateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenFeeScheduleUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenFeeScheduleUpdateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_fee_schedule_update(new proto::TokenFeeScheduleUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenFeeScheduleUpdateTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenFeeScheduleUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenFreezeTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenfreeze(new proto::TokenFreezeAccountTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenFreezeTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_FREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenFreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenFreezeTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenfreeze(new proto::TokenFreezeAccountTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenFreezeTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_FREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenFreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenFreezeTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenfreeze(new proto::TokenFreezeAccountTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenFreezeTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_FREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenFreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenGrantKycTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokengrantkyc(new proto::TokenGrantKycTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenGrantKycTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_GRANT_KYC_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenGrantKycTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenGrantKycTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokengrantkyc(new proto::TokenGrantKycTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenGrantKycTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_GRANT_KYC_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenGrantKycTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenGrantKycTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokengrantkyc(new proto::TokenGrantKycTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenGrantKycTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_GRANT_KYC_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenGrantKycTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenMintTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenmint(new proto::TokenMintTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenMintTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_MINT_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenMintTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenMintTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenmint(new proto::TokenMintTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenMintTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_MINT_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenMintTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenMintTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenmint(new proto::TokenMintTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenMintTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_MINT_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenMintTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenPauseTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_pause(new proto::TokenPauseTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenPauseTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_PAUSE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenPauseTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenPauseTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_pause(new proto::TokenPauseTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenPauseTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_PAUSE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenPauseTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenPauseTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_pause(new proto::TokenPauseTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenPauseTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_PAUSE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenPauseTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenRevokeKycTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenrevokekyc(new proto::TokenRevokeKycTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenRevokeKycTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_REVOKE_KYC_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenRevokeKycTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenRevokeKycTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenrevokekyc(new proto::TokenRevokeKycTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenRevokeKycTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_REVOKE_KYC_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenRevokeKycTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenRevokeKycTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenrevokekyc(new proto::TokenRevokeKycTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenRevokeKycTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_REVOKE_KYC_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenRevokeKycTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUnfreezeTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenunfreeze(new proto::TokenUnfreezeAccountTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenUnfreezeTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UNFREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUnfreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUnfreezeTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenunfreeze(new proto::TokenUnfreezeAccountTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenUnfreezeTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UNFREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUnfreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUnfreezeTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenunfreeze(new proto::TokenUnfreezeAccountTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenUnfreezeTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UNFREEZE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUnfreezeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUnpauseTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_unpause(new proto::TokenUnpauseTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenUnpauseTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UNPAUSE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUnpauseTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUnpauseTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_unpause(new proto::TokenUnpauseTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenUnpauseTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UNPAUSE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUnpauseTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUnpauseTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_token_unpause(new proto::TokenUnpauseTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TokenUnpauseTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UNPAUSE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUnpauseTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUpdateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenupdate(new proto::TokenUpdateTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUpdateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenupdate(new proto::TokenUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenUpdateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenupdate(new proto::TokenUpdateTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenWipeTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenwipe(new proto::TokenWipeAccountTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenWipeTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_WIPE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenWipeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenWipeTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenwipe(new proto::TokenWipeAccountTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenWipeTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_WIPE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenWipeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TokenWipeTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_tokenwipe(new proto::TokenWipeAccountTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TokenWipeTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOKEN_WIPE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TokenWipeTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicCreateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensuscreatetopic(new proto::ConsensusCreateTopicTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicCreateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensuscreatetopic(new proto::ConsensusCreateTopicTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicCreateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensuscreatetopic(new proto::ConsensusCreateTopicTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicCreateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_CREATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicCreateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicDeleteTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensusdeletetopic(new proto::ConsensusDeleteTopicTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicDeleteTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensusdeletetopic(new proto::ConsensusDeleteTopicTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicDeleteTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensusdeletetopic(new proto::ConsensusDeleteTopicTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicDeleteTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_DELETE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicDeleteTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicMessageSubmitTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensussubmitmessage(new proto::ConsensusSubmitMessageTransactionBody);

  // When
  const WrappedTransaction wrappedTx = Transaction<TopicMessageSubmitTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_MESSAGE_SUBMIT_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicMessageSubmitTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicMessageSubmitTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensussubmitmessage(new proto::ConsensusSubmitMessageTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx = Transaction<TopicMessageSubmitTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_MESSAGE_SUBMIT_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicMessageSubmitTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicMessageSubmitTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensussubmitmessage(new proto::ConsensusSubmitMessageTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx = Transaction<TopicMessageSubmitTransaction>::fromBytes(
    internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_MESSAGE_SUBMIT_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicMessageSubmitTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicUpdateTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensusupdatetopic(new proto::ConsensusUpdateTopicTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicUpdateTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensusupdatetopic(new proto::ConsensusUpdateTopicTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TopicUpdateTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_consensusupdatetopic(new proto::ConsensusUpdateTopicTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TopicUpdateTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TOPIC_UPDATE_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TopicUpdateTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TransferTransactionFromTransactionBodyBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TransferTransaction>::fromBytes(internal::Utilities::stringToByteVector(txBody.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TRANSFER_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TransferTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TransferTransactionFromTransactionBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TransferTransaction>::fromBytes(internal::Utilities::stringToByteVector(tx.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TRANSFER_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TransferTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, TransferTransactionFromTransactionListBytes)
{
  // Given
  proto::TransactionBody txBody;
  txBody.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  proto::SignedTransaction signedTx;
  signedTx.set_bodybytes(txBody.SerializeAsString());
  // SignatureMap not required

  proto::Transaction tx;
  tx.set_signedtransactionbytes(signedTx.SerializeAsString());

  proto::TransactionList txList;
  *txList.add_transaction_list() = tx;

  // When
  const WrappedTransaction wrappedTx =
    Transaction<TransferTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString()));

  // Then
  ASSERT_EQ(wrappedTx.getTransactionType(), TransactionType::TRANSFER_TRANSACTION);
  EXPECT_NE(wrappedTx.getTransaction<TransferTransaction>(), nullptr);
}

//-----
TEST_F(TransactionUnitTests, GetTransactionAndTransactionBodySize)
{
  // Given
  AccountCreateTransaction accountCreateTransaction;
  ASSERT_NO_THROW(accountCreateTransaction = AccountCreateTransaction()
                                               .setTransactionMemo("Test Memo")
                                               .setNodeAccountIds({ AccountId(3) })
                                               .setTransactionId(getTestTransactionIdMock())
                                               .freezeWith(&getTestClientMock()));

  // When
  size_t transactionBodySize;
  ASSERT_NO_THROW(transactionBodySize = accountCreateTransaction.getTransactionBodySize());

  size_t transactionSize;
  ASSERT_NO_THROW(transactionSize = accountCreateTransaction.getTransactionSize());

  // Then
  ASSERT_GT(transactionSize, transactionBodySize);
}

//-----
TEST_F(TransactionUnitTests, CannotGetTransactionAndTransactionBodySizeWhenNotFrozen)
{
  // Given / When
  AccountCreateTransaction accountCreateTransaction;
  ASSERT_NO_THROW(accountCreateTransaction = AccountCreateTransaction()
                                               .setTransactionMemo("Test Memo")
                                               .setNodeAccountIds({ AccountId(3) })
                                               .setTransactionId(getTestTransactionIdMock()));

  // Then
  size_t transactionBodySize;
  EXPECT_THROW(transactionBodySize = accountCreateTransaction.getTransactionBodySize(), IllegalStateException);

  size_t transactionSize;
  EXPECT_THROW(transactionSize = accountCreateTransaction.getTransactionSize(), IllegalStateException);
}

//-----
TEST_F(TransactionUnitTests, GetTransactionSizeWithSignatures)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();

  AccountCreateTransaction accountCreateTransaction;
  ASSERT_NO_THROW(accountCreateTransaction = AccountCreateTransaction()
                                               .setTransactionMemo("Test Memo")
                                               .setNodeAccountIds({ AccountId(3) })
                                               .setTransactionId(getTestTransactionIdMock())
                                               .setKey(privateKey->getPublicKey())
                                               .freezeWith(&getTestClientMock()));

  // When
  size_t transactionSizeBeforeSign;
  ASSERT_NO_THROW(transactionSizeBeforeSign = accountCreateTransaction.getTransactionSize());

  ASSERT_NO_THROW(accountCreateTransaction.sign(privateKey));

  size_t transactionSizeAfterSign;
  ASSERT_NO_THROW(transactionSizeAfterSign = accountCreateTransaction.getTransactionSize());

  // Then
  ASSERT_GT(transactionSizeAfterSign, transactionSizeBeforeSign);
}

//-----
TEST_F(TransactionUnitTests, GetTransactionBodySizeForBigAndSmallData)
{
  // Given
  AccountCreateTransaction bigBodyTransaction;
  ASSERT_NO_THROW(
    bigBodyTransaction =
      AccountCreateTransaction()
        .setTransactionMemo("This is a very long memo that exceeds the typical size limit for a transaction memo. "
                            "It is designed to test the handling of larger transaction bodies.")
        .setNodeAccountIds({ AccountId(3) })
        .setTransactionId(getTestTransactionIdMock())
        .freezeWith(&getTestClientMock()));

  AccountCreateTransaction smallBodyTransaction;
  ASSERT_NO_THROW(smallBodyTransaction = AccountCreateTransaction()
                                           .setTransactionMemo("memo")
                                           .setNodeAccountIds({ AccountId(3) })
                                           .setTransactionId(getTestTransactionIdMock())
                                           .freezeWith(&getTestClientMock()));

  // When
  size_t transactionBigBodySize;
  ASSERT_NO_THROW(transactionBigBodySize = bigBodyTransaction.getTransactionBodySize());

  size_t transactionSmallBodySize;
  ASSERT_NO_THROW(transactionSmallBodySize = smallBodyTransaction.getTransactionBodySize());

  // Then
  ASSERT_GT(transactionBigBodySize, transactionSmallBodySize);
}

//-----
TEST_F(TransactionUnitTests, FromBytesThrowsWhenTransactionListHasInconsistentBodyBytes)
{
  // Given
  // Create two bodies with different content
  proto::TransactionBody txBody1;
  txBody1.set_memo("Transaction 1");
  txBody1.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  proto::TransactionBody txBody2;
  txBody2.set_memo("Transaction 2"); // Differs from txBody1
  txBody2.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  // Wrap them in SignedTransaction -> Transaction
  proto::SignedTransaction signedTx1;
  signedTx1.set_bodybytes(txBody1.SerializeAsString());
  proto::Transaction tx1;
  tx1.set_signedtransactionbytes(signedTx1.SerializeAsString());

  proto::SignedTransaction signedTx2;
  signedTx2.set_bodybytes(txBody2.SerializeAsString());
  proto::Transaction tx2;
  tx2.set_signedtransactionbytes(signedTx2.SerializeAsString());

  // Add both to a TransactionList
  proto::TransactionList txList;
  *txList.add_transaction_list() = tx1;
  *txList.add_transaction_list() = tx2;

  // When / Then
  EXPECT_THROW(
    Transaction<TransferTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString())),
    std::invalid_argument);
}

//-----
TEST_F(TransactionUnitTests, FromBytesSuccessWhenTransactionListHasConsistentBodyBytesButDifferentNodeAccountIds)
{
  // Given
  // Create a base body with common data
  proto::TransactionBody txBodyPrototype;
  txBodyPrototype.set_memo("Common Memo");
  txBodyPrototype.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  // Transaction 1: Targeted at Node 0.0.3
  proto::TransactionBody txBody1 = txBodyPrototype;
  txBody1.set_allocated_nodeaccountid(AccountId(3).toProtobuf().release());

  proto::SignedTransaction signedTx1;
  signedTx1.set_bodybytes(txBody1.SerializeAsString());
  proto::Transaction tx1;
  tx1.set_signedtransactionbytes(signedTx1.SerializeAsString());

  // Transaction 2: Targeted at Node 0.0.4 (Same content, different Node ID)
  proto::TransactionBody txBody2 = txBodyPrototype;
  txBody2.set_allocated_nodeaccountid(AccountId(4).toProtobuf().release());

  proto::SignedTransaction signedTx2;
  signedTx2.set_bodybytes(txBody2.SerializeAsString());
  proto::Transaction tx2;
  tx2.set_signedtransactionbytes(signedTx2.SerializeAsString());

  // Add both to a TransactionList
  proto::TransactionList txList;
  *txList.add_transaction_list() = tx1;
  *txList.add_transaction_list() = tx2;

  // When / Then
  // This is expected to not throw the error as the sanitised bodies, not the NodeID are identical
  EXPECT_NO_THROW(
    Transaction<TransferTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString())));
}

//-----
TEST_F(TransactionUnitTests, RemoveSignatureFromSingleSignerTransaction)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountCreateTransaction tx;

  tx.setNodeAccountIds({ AccountId(3) }).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  tx.sign(privateKey);
  const size_t sizeSigned = tx.getTransactionSize();

  // When
  std::vector<std::vector<std::byte>> removedSigs;
  ASSERT_NO_THROW(removedSigs = tx.removeSignature(privateKey->getPublicKey()));

  // Then
  EXPECT_EQ(removedSigs.size(), 1);
  EXPECT_FALSE(removedSigs[0].empty());

  // getTransactionSize() rebuilds the transaction. Since the signature was removed,
  // the rebuilt (unsigned) transaction should be strictly smaller.
  EXPECT_LT(tx.getTransactionSize(), sizeSigned);
}

//-----
TEST_F(TransactionUnitTests, RemoveSignatureFromSingleSignerTransactionWithECDSAsecp256k1Key)
{
  // Given
  const std::shared_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  AccountCreateTransaction tx;

  tx.setNodeAccountIds({ AccountId(3) }).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  tx.sign(privateKey);
  const size_t sizeSigned = tx.getTransactionSize();

  // When
  std::vector<std::vector<std::byte>> removedSigs;
  ASSERT_NO_THROW(removedSigs = tx.removeSignature(privateKey->getPublicKey()));

  // Then
  EXPECT_EQ(removedSigs.size(), 1);
  EXPECT_FALSE(removedSigs[0].empty());

  // getTransactionSize() rebuilds the transaction. Since the signature was removed,
  // the rebuilt (unsigned) transaction should be strictly smaller.
  EXPECT_LT(tx.getTransactionSize(), sizeSigned);
}

//-----
TEST_F(TransactionUnitTests, RemoveOneSignatureFromMultiSignerTransaction)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> privateKey1 = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ED25519PrivateKey> privateKey2 = ED25519PrivateKey::generatePrivateKey();

  AccountCreateTransaction tx;
  tx.setNodeAccountIds({ AccountId(3) }).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  tx.sign(privateKey1);
  tx.sign(privateKey2);

  const size_t sizeTwoSigs = tx.getTransactionSize();

  // When
  std::vector<std::vector<std::byte>> removedSigs;
  ASSERT_NO_THROW(removedSigs = tx.removeSignature(privateKey1->getPublicKey()));

  // Then
  EXPECT_EQ(removedSigs.size(), 1);
  EXPECT_FALSE(removedSigs[0].empty());

  // Transaction size should reflect the removal of the first signature
  EXPECT_LT(tx.getTransactionSize(), sizeTwoSigs);

  // Ensure that the second signature is still perfectly intact and can also be removed
  ASSERT_NO_THROW(tx.removeSignature(privateKey2->getPublicKey()));
}

//-----
TEST_F(TransactionUnitTests, RemoveSignatureThatDoesNotExistThrows)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> signerKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ED25519PrivateKey> nonSignerKey = ED25519PrivateKey::generatePrivateKey();

  AccountCreateTransaction tx;
  tx.setNodeAccountIds({ AccountId(3) }).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  tx.sign(signerKey);

  // When / Then
  // Attempting to remove a signature for a key that hasn't signed should throw
  EXPECT_THROW(tx.removeSignature(nonSignerKey->getPublicKey()), IllegalStateException);
  // The signer key can still be removed
  ASSERT_NO_THROW(tx.removeSignature(signerKey->getPublicKey()));
}

//-----
TEST_F(TransactionUnitTests, RemoveSignatureFromUnfrozenTransactionThrows)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  AccountCreateTransaction tx;
  // Transaction is intentionally left unfrozen

  // Then
  // Signature map modifications require a frozen transaction state
  EXPECT_THROW(tx.removeSignature(privateKey->getPublicKey()), IllegalStateException);
}

//-----
TEST_F(TransactionUnitTests, RemoveAllSignaturesFromUnfrozenTransactionThrows)
{
  // Given
  AccountCreateTransaction tx;
  // Transaction is intentionally left unfrozen

  // Then
  // Signature map modifications require a frozen transaction state
  EXPECT_THROW(tx.removeAllSignatures(), IllegalStateException);
}

//-----
TEST_F(TransactionUnitTests, RemoveAllSignaturesAndVerifyTransactionState)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> privateKey1 = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ED25519PrivateKey> privateKey2 = ED25519PrivateKey::generatePrivateKey();

  AccountCreateTransaction tx;
  tx.setNodeAccountIds({ AccountId(3) }).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  const size_t sizeUnsigned = tx.getTransactionSize();

  // Act: Sign with two different keys
  tx.sign(privateKey1);
  tx.sign(privateKey2);
  EXPECT_GT(tx.getTransactionSize(), sizeUnsigned);

  // When (First Removal)
  std::map<std::shared_ptr<PublicKey>, std::vector<std::vector<std::byte>>> removedSigsMap;
  ASSERT_NO_THROW(removedSigsMap = tx.removeAllSignatures());

  // Then
  // Verify both signatures were extracted and the transaction size reverted to baseline
  EXPECT_EQ(removedSigsMap.size(), 2);
  EXPECT_EQ(tx.getTransactionSize(), sizeUnsigned);

  // When (Second Removal)
  // Attempting to remove all signatures again from a now-empty transaction
  std::map<std::shared_ptr<PublicKey>, std::vector<std::vector<std::byte>>> secondRemoval;
  ASSERT_NO_THROW(secondRemoval = tx.removeAllSignatures());

  // Then
  // A second pass should safely return an empty map
  EXPECT_EQ(secondRemoval.size(), 0);
}

//-----
TEST_F(TransactionUnitTests, RemoveAllSignaturesAndSignWithDifferentKey)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> privateKey1 = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<ED25519PrivateKey> privateKey2 = ED25519PrivateKey::generatePrivateKey();

  AccountCreateTransaction tx;
  tx.setNodeAccountIds({ AccountId(3) }).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  const size_t sizeUnsigned = tx.getTransactionSize();

  // When (First Signing)
  tx.sign(privateKey1);
  const size_t sizeWithKey1 = tx.getTransactionSize();

  // Then
  EXPECT_GT(sizeWithKey1, sizeUnsigned);

  // When (First Removal)
  std::map<std::shared_ptr<PublicKey>, std::vector<std::vector<std::byte>>> firstRemovalMap;
  ASSERT_NO_THROW(firstRemovalMap = tx.removeAllSignatures());

  // Then
  // Verify Key 1 was extracted and the transaction returned to baseline size
  EXPECT_EQ(firstRemovalMap.size(), 1);
  EXPECT_NE(firstRemovalMap.find(privateKey1->getPublicKey()), firstRemovalMap.end());
  EXPECT_EQ(tx.getTransactionSize(), sizeUnsigned);

  // When (Second Signing)
  // Sign the now-empty transaction with a different key
  tx.sign(privateKey2);
  const size_t sizeWithKey2 = tx.getTransactionSize();

  // Then
  EXPECT_GT(sizeWithKey2, sizeUnsigned);

  // When (Final Removal)
  std::map<std::shared_ptr<PublicKey>, std::vector<std::vector<std::byte>>> finalRemovalMap;
  ASSERT_NO_THROW(finalRemovalMap = tx.removeAllSignatures());

  // Then
  // Verify that only Key 2 was present
  EXPECT_EQ(finalRemovalMap.size(), 1);
  EXPECT_NE(finalRemovalMap.find(privateKey2->getPublicKey()), finalRemovalMap.end());
  EXPECT_EQ(finalRemovalMap.find(privateKey1->getPublicKey()), finalRemovalMap.end());
  EXPECT_EQ(tx.getTransactionSize(), sizeUnsigned);
}

//-----
TEST_F(TransactionUnitTests, VerifyReturnedSignaturesMatchWhatWasAdded)
{
  // Given
  const auto privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  // Create a predictable dummy signature to test byte extraction accuracy
  const std::vector<std::byte> dummySignature(64, std::byte{ 0xAB });

  AccountCreateTransaction tx;
  tx.setNodeAccountIds({ AccountId(3) }).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  tx.addSignature(publicKey, dummySignature);

  // When
  std::vector<std::vector<std::byte>> removedSigs;
  ASSERT_NO_THROW(removedSigs = tx.removeSignature(publicKey));

  // Then
  // The bytes returned by the removal function should match the injected dummy bytes
  ASSERT_EQ(removedSigs.size(), 1);
  EXPECT_EQ(removedSigs[0], dummySignature);
}

//-----
TEST_F(TransactionUnitTests, RemoveSignatureFromMultiNodeTransaction)
{
  // Given
  const std::shared_ptr<ED25519PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  // Define multiple nodes
  const std::vector<AccountId> nodeIds = { AccountId(3), AccountId(4), AccountId(5) };

  AccountCreateTransaction tx;
  tx.setNodeAccountIds(nodeIds).setTransactionId(getTestTransactionIdMock()).freezeWith(&getTestClientMock());

  const size_t sizeUnsigned = tx.getTransactionSize();

  // When (Signing)
  tx.sign(privateKey);
  const size_t sizeSigned = tx.getTransactionSize();

  // Then
  EXPECT_GT(sizeSigned, sizeUnsigned);

  // When (Removal)
  std::vector<std::vector<std::byte>> removedSigs;
  ASSERT_NO_THROW(removedSigs = tx.removeSignature(publicKey));

  // Then
  // The SDK maintains a single template SignedTransaction while in a frozen state;
  // therefore, exactly one signature should be removed.
  EXPECT_EQ(removedSigs.size(), 1);

  // The transaction size should return exactly the same as the original unsigned baseline size.
  EXPECT_EQ(tx.getTransactionSize(), sizeUnsigned);

  // Verify the extracted signature is non-empty and has the correct ED25519 length (64 bytes)
  ASSERT_FALSE(removedSigs.empty());
  EXPECT_EQ(removedSigs[0].size(), 64);

  // When (Final Verification)
  std::map<std::shared_ptr<PublicKey>, std::vector<std::vector<std::byte>>> finalRemoval;
  ASSERT_NO_THROW(finalRemoval = tx.removeAllSignatures());

  // Then
  // Verify that all internal signatory tracking and protobuf signature maps are completely clear
  EXPECT_EQ(finalRemoval.size(), 0);
}

//-----
// Security Test: Prevent transaction smuggling attacks (CVE fix)
// This test verifies that fromBytes() rejects TransactionLists containing entries
// with different transaction types, which could be used to smuggle a malicious
// transaction that gets signed but not displayed to the user.
TEST_F(TransactionUnitTests, FromBytesRejectsTransactionListWithMixedTransactionTypes)
{
  // Given
  // Entry 1: A benign TransferTransaction (CryptoTransfer)
  proto::TransactionBody benignBody;
  benignBody.set_memo("Benign transfer");
  benignBody.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  proto::SignedTransaction signedTx1;
  signedTx1.set_bodybytes(benignBody.SerializeAsString());
  proto::Transaction tx1;
  tx1.set_signedtransactionbytes(signedTx1.SerializeAsString());

  // Entry 2: A different transaction type (AccountCreate) - this simulates
  // an attacker trying to smuggle a different transaction type
  proto::TransactionBody maliciousBody;
  maliciousBody.set_memo("Hidden transaction");
  maliciousBody.set_allocated_cryptocreateaccount(new proto::CryptoCreateTransactionBody);

  proto::SignedTransaction signedTx2;
  signedTx2.set_bodybytes(maliciousBody.SerializeAsString());
  proto::Transaction tx2;
  tx2.set_signedtransactionbytes(signedTx2.SerializeAsString());

  // Create a TransactionList with mixed types
  proto::TransactionList txList;
  *txList.add_transaction_list() = tx1;
  *txList.add_transaction_list() = tx2;

  // When / Then
  // This should throw because entries have different transaction types
  EXPECT_THROW(
    Transaction<TransferTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString())),
    std::invalid_argument);
}

//-----
// Security Test: Prevent transaction smuggling via multiple transactionIds
// For non-chunked transaction types, multiple transactionIds are not allowed
// as they could be used to hide malicious transactions.
TEST_F(TransactionUnitTests, FromBytesRejectsNonChunkedTransactionWithMultipleTransactionIds)
{
  // Given
  // Entry 1: TransferTransaction with TransactionId A
  proto::TransactionBody txBody1;
  txBody1.set_memo("Transaction 1");
  txBody1.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  proto::TransactionID txId1;
  txId1.mutable_accountid()->set_accountnum(201);
  txId1.mutable_transactionvalidstart()->set_seconds(1700000010);
  txId1.mutable_transactionvalidstart()->set_nanos(1);
  txBody1.set_allocated_transactionid(new proto::TransactionID(txId1));

  proto::SignedTransaction signedTx1;
  signedTx1.set_bodybytes(txBody1.SerializeAsString());
  proto::Transaction tx1;
  tx1.set_signedtransactionbytes(signedTx1.SerializeAsString());

  // Entry 2: Same type but different TransactionId (different timestamp)
  // This simulates an attacker hiding a second transaction
  proto::TransactionBody txBody2;
  txBody2.set_memo("Transaction 1"); // Same memo to look identical
  txBody2.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  proto::TransactionID txId2;
  txId2.mutable_accountid()->set_accountnum(201);
  txId2.mutable_transactionvalidstart()->set_seconds(1700000011); // Different timestamp!
  txId2.mutable_transactionvalidstart()->set_nanos(1);
  txBody2.set_allocated_transactionid(new proto::TransactionID(txId2));

  proto::SignedTransaction signedTx2;
  signedTx2.set_bodybytes(txBody2.SerializeAsString());
  proto::Transaction tx2;
  tx2.set_signedtransactionbytes(signedTx2.SerializeAsString());

  // Create a TransactionList with two different transactionIds
  proto::TransactionList txList;
  *txList.add_transaction_list() = tx1;
  *txList.add_transaction_list() = tx2;

  // When / Then
  // This should throw because TransferTransaction (non-chunked) cannot have multiple transactionIds
  EXPECT_THROW(
    Transaction<TransferTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString())),
    std::invalid_argument);
}

//-----
// Security Test: FileAppend (chunked transaction) can have multiple transactionIds
// but they must all have the same transaction type.
TEST_F(TransactionUnitTests, FromBytesAllowsChunkedTransactionWithMultipleTransactionIdsOfSameType)
{
  // Given
  // Entry 1: FileAppend chunk 1
  proto::TransactionBody txBody1;
  txBody1.set_memo("FileAppend chunk 1");
  txBody1.set_allocated_fileappend(new proto::FileAppendTransactionBody);

  proto::TransactionID txId1;
  txId1.mutable_accountid()->set_accountnum(201);
  txId1.mutable_transactionvalidstart()->set_seconds(1700000010);
  txId1.mutable_transactionvalidstart()->set_nanos(1);
  txBody1.set_allocated_transactionid(new proto::TransactionID(txId1));

  proto::SignedTransaction signedTx1;
  signedTx1.set_bodybytes(txBody1.SerializeAsString());
  proto::Transaction tx1;
  tx1.set_signedtransactionbytes(signedTx1.SerializeAsString());

  // Entry 2: FileAppend chunk 2 (same type, different transactionId - this is valid for chunks)
  proto::TransactionBody txBody2;
  txBody2.set_memo("FileAppend chunk 2");
  txBody2.set_allocated_fileappend(new proto::FileAppendTransactionBody);

  proto::TransactionID txId2;
  txId2.mutable_accountid()->set_accountnum(201);
  txId2.mutable_transactionvalidstart()->set_seconds(1700000011); // Different timestamp for chunk 2
  txId2.mutable_transactionvalidstart()->set_nanos(1);
  txBody2.set_allocated_transactionid(new proto::TransactionID(txId2));

  proto::SignedTransaction signedTx2;
  signedTx2.set_bodybytes(txBody2.SerializeAsString());
  proto::Transaction tx2;
  tx2.set_signedtransactionbytes(signedTx2.SerializeAsString());

  // Create a TransactionList
  proto::TransactionList txList;
  *txList.add_transaction_list() = tx1;
  *txList.add_transaction_list() = tx2;

  // When / Then
  // FileAppend is a chunked transaction type, so multiple transactionIds are allowed
  // as long as they all have the same transaction type
  EXPECT_NO_THROW(
    Transaction<FileAppendTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString())));
}

//-----
// Security Test: Even FileAppend (chunked) must reject mixed transaction types
TEST_F(TransactionUnitTests, FromBytesRejectsChunkedTransactionWithMixedTypes)
{
  // Given
  // Entry 1: FileAppend
  proto::TransactionBody txBody1;
  txBody1.set_memo("FileAppend");
  txBody1.set_allocated_fileappend(new proto::FileAppendTransactionBody);

  proto::TransactionID txId1;
  txId1.mutable_accountid()->set_accountnum(201);
  txId1.mutable_transactionvalidstart()->set_seconds(1700000010);
  txId1.mutable_transactionvalidstart()->set_nanos(1);
  txBody1.set_allocated_transactionid(new proto::TransactionID(txId1));

  proto::SignedTransaction signedTx1;
  signedTx1.set_bodybytes(txBody1.SerializeAsString());
  proto::Transaction tx1;
  tx1.set_signedtransactionbytes(signedTx1.SerializeAsString());

  // Entry 2: Malicious CryptoTransfer hidden as "chunk 2"
  proto::TransactionBody txBody2;
  txBody2.set_memo("Hidden drain");
  txBody2.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody); // Different type!

  proto::TransactionID txId2;
  txId2.mutable_accountid()->set_accountnum(201);
  txId2.mutable_transactionvalidstart()->set_seconds(1700000011);
  txId2.mutable_transactionvalidstart()->set_nanos(1);
  txBody2.set_allocated_transactionid(new proto::TransactionID(txId2));

  proto::SignedTransaction signedTx2;
  signedTx2.set_bodybytes(txBody2.SerializeAsString());
  proto::Transaction tx2;
  tx2.set_signedtransactionbytes(signedTx2.SerializeAsString());

  // Create a TransactionList
  proto::TransactionList txList;
  *txList.add_transaction_list() = tx1;
  *txList.add_transaction_list() = tx2;

  // When / Then
  // Even for FileAppend, mixed transaction types must be rejected
  EXPECT_THROW(
    Transaction<FileAppendTransaction>::fromBytes(internal::Utilities::stringToByteVector(txList.SerializeAsString())),
    std::invalid_argument);
}

//-----
// Security Test: Directly exercise ChunkedTransaction constructor validation.
// Build a mixed-type transactions map and ensure chunked constructor rejects it.
TEST_F(TransactionUnitTests, ChunkedTransactionConstructorRejectsMixedTypes)
{
  // Given
  // Entry 1: FileAppend (expected chunked type)
  proto::TransactionBody txBody1;
  txBody1.set_memo("FileAppend chunk 1");
  txBody1.set_allocated_fileappend(new proto::FileAppendTransactionBody);

  proto::TransactionID txId1;
  txId1.mutable_accountid()->set_accountnum(201);
  txId1.mutable_transactionvalidstart()->set_seconds(1700000020);
  txId1.mutable_transactionvalidstart()->set_nanos(1);
  txBody1.set_allocated_transactionid(new proto::TransactionID(txId1));

  proto::SignedTransaction signedTx1;
  signedTx1.set_bodybytes(txBody1.SerializeAsString());
  proto::Transaction tx1;
  tx1.set_signedtransactionbytes(signedTx1.SerializeAsString());

  // Entry 2: Different TransactionId and different type (CryptoTransfer)
  // This simulates a smuggled chunk that should be rejected by ChunkedTransaction.
  proto::TransactionBody txBody2;
  txBody2.set_memo("Smuggled transfer");
  txBody2.set_allocated_cryptotransfer(new proto::CryptoTransferTransactionBody);

  proto::TransactionID txId2;
  txId2.mutable_accountid()->set_accountnum(201);
  txId2.mutable_transactionvalidstart()->set_seconds(1700000021);
  txId2.mutable_transactionvalidstart()->set_nanos(1);
  txBody2.set_allocated_transactionid(new proto::TransactionID(txId2));

  proto::SignedTransaction signedTx2;
  signedTx2.set_bodybytes(txBody2.SerializeAsString());
  proto::Transaction tx2;
  tx2.set_signedtransactionbytes(signedTx2.SerializeAsString());

  std::map<TransactionId, std::map<AccountId, proto::Transaction>> transactions;
  const auto txIdKey1 = TransactionId::withValidStart(AccountId(201), std::chrono::system_clock::from_time_t(1700000020));
  const auto txIdKey2 = TransactionId::withValidStart(AccountId(201), std::chrono::system_clock::from_time_t(1700000021));
  transactions[txIdKey1][AccountId(3)] = tx1;
  transactions[txIdKey2][AccountId(3)] = tx2;

  // When / Then
  EXPECT_THROW(FileAppendTransaction{ transactions }, std::invalid_argument);
}
