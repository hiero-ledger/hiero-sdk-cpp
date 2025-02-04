// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountAllowanceDeleteTransaction.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountUpdateTransaction.h"
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
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>
#include <transaction_contents.pb.h>
#include <transaction_list.pb.h>

using namespace Hiero;

class TransactionUnitTests : public ::testing::Test
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
