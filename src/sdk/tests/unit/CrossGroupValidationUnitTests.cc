// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "ED25519PrivateKey.h"
#include "FileAppendTransaction.h"
#include "Hbar.h"
#include "Transaction.h"
#include "TransactionId.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>
#include <services/crypto_transfer.pb.h>
#include <services/file_append.pb.h>
#include <services/transaction.pb.h>
#include <services/transaction_contents.pb.h>
#include <transaction_list.pb.h>

#include <string>
#include <vector>

using namespace Hiero;

class CrossGroupValidationUnitTests : public ::testing::Test
{
protected:
  // Accounts
  static constexpr int64_t VICTIM_NUM = 100;
  static constexpr int64_t ATTACKER_NUM = 200;
  static constexpr int64_t BENIGN_AMOUNT = 1;                  // 1 tinybar
  static constexpr int64_t MALICIOUS_AMOUNT = 100000000000LL;  // 1,000 HBAR

  // Nodes
  static const std::vector<int64_t>& getNodeNums()
  {
    static const std::vector<int64_t> nodes = { 3, 4, 5 };
    return nodes;
  }

  /**
   * Build a proto::AccountID from an account number.
   */
  static proto::AccountID makeAccountId(int64_t num)
  {
    proto::AccountID id;
    id.set_shardnum(0);
    id.set_realmnum(0);
    id.set_accountnum(num);
    return id;
  }

  /**
   * Build a proto::TransactionID.
   */
  static proto::TransactionID makeTransactionId(int64_t accountNum, int64_t seconds)
  {
    proto::TransactionID txId;
    *txId.mutable_accountid() = makeAccountId(accountNum);
    txId.mutable_transactionvalidstart()->set_seconds(seconds);
    txId.mutable_transactionvalidstart()->set_nanos(0);
    return txId;
  }

  /**
   * Build a CryptoTransfer body with the given amount.
   */
  static proto::CryptoTransferTransactionBody makeCryptoTransfer(int64_t fromAccount,
                                                                  int64_t toAccount,
                                                                  int64_t amount)
  {
    proto::CryptoTransferTransactionBody transfer;
    auto* aa1 = transfer.mutable_transfers()->add_accountamounts();
    *aa1->mutable_accountid() = makeAccountId(fromAccount);
    aa1->set_amount(-amount);

    auto* aa2 = transfer.mutable_transfers()->add_accountamounts();
    *aa2->mutable_accountid() = makeAccountId(toAccount);
    aa2->set_amount(amount);

    return transfer;
  }

  /**
   * Build a signed proto::Transaction from a TransactionBody.
   * If signerKey is provided, the body bytes are signed.
   */
  static proto::Transaction buildSignedProtoTx(const proto::TransactionBody& body,
                                                const std::shared_ptr<PrivateKey>& signerKey = nullptr)
  {
    std::string bodyBytes = body.SerializeAsString();

    proto::SignedTransaction signedTx;
    signedTx.set_bodybytes(bodyBytes);

    if (signerKey)
    {
      auto signature = signerKey->sign(internal::Utilities::stringToByteVector(bodyBytes));
      auto* sigPair = signedTx.mutable_sigmap()->add_sigpair();
      sigPair->set_pubkeyprefix(
        internal::Utilities::byteVectorToString(signerKey->getPublicKey()->toBytesRaw()));
      sigPair->set_ed25519(internal::Utilities::byteVectorToString(signature));
    }

    proto::Transaction tx;
    tx.set_signedtransactionbytes(signedTx.SerializeAsString());
    return tx;
  }

  /**
   * Build a malicious TransactionList with two CryptoTransfer groups:
   *   Group 1: benign (1 tinybar)
   *   Group 2: malicious (1,000 HBAR)
   */
  static std::vector<std::byte> buildMaliciousPayload(const std::shared_ptr<PrivateKey>& attackerKey)
  {
    proto::TransactionList txList;

    auto txId1 = makeTransactionId(VICTIM_NUM, 1234567890);
    auto txId2 = makeTransactionId(VICTIM_NUM, 1234567891);

    auto benignTransfer = makeCryptoTransfer(VICTIM_NUM, ATTACKER_NUM, BENIGN_AMOUNT);
    auto maliciousTransfer = makeCryptoTransfer(VICTIM_NUM, ATTACKER_NUM, MALICIOUS_AMOUNT);

    // Group 1: benign transfers (one per node)
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId1;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      *body.mutable_cryptotransfer() = benignTransfer;

      *txList.add_transaction_list() = buildSignedProtoTx(body, attackerKey);
    }

    // Group 2: malicious transfers (one per node)
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId2;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      *body.mutable_cryptotransfer() = maliciousTransfer;

      *txList.add_transaction_list() = buildSignedProtoTx(body, attackerKey);
    }

    return internal::Utilities::stringToByteVector(txList.SerializeAsString());
  }

  /**
   * Build a malicious TransactionList with two CryptoTransfer groups that have
   * IDENTICAL bodies (only TransactionID differs). This tests that the SDK
   * rejects multi-group non-chunked payloads even when bodies are identical.
   */
  static std::vector<std::byte> buildIdenticalBodyPayload()
  {
    proto::TransactionList txList;

    auto txId1 = makeTransactionId(VICTIM_NUM, 1234567890);
    auto txId2 = makeTransactionId(VICTIM_NUM, 1234567891);

    auto transfer = makeCryptoTransfer(VICTIM_NUM, ATTACKER_NUM, BENIGN_AMOUNT);

    // Group 1
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId1;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      *body.mutable_cryptotransfer() = transfer;

      *txList.add_transaction_list() = buildSignedProtoTx(body);
    }

    // Group 2 (identical body, different TransactionID)
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId2;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      *body.mutable_cryptotransfer() = transfer;

      *txList.add_transaction_list() = buildSignedProtoTx(body);
    }

    return internal::Utilities::stringToByteVector(txList.SerializeAsString());
  }

  /**
   * Build a single-group TransactionList with one CryptoTransfer (legitimate).
   */
  static std::vector<std::byte> buildSingleGroupPayload()
  {
    proto::TransactionList txList;

    auto txId = makeTransactionId(VICTIM_NUM, 1234567890);
    auto transfer = makeCryptoTransfer(VICTIM_NUM, ATTACKER_NUM, BENIGN_AMOUNT);

    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      *body.mutable_cryptotransfer() = transfer;

      *txList.add_transaction_list() = buildSignedProtoTx(body);
    }

    return internal::Utilities::stringToByteVector(txList.SerializeAsString());
  }

  /**
   * Build a multi-group FileAppend TransactionList (legitimate chunked transaction).
   */
  static std::vector<std::byte> buildLegitimateChunkedPayload()
  {
    proto::TransactionList txList;

    auto txId1 = makeTransactionId(VICTIM_NUM, 1234567890);
    auto txId2 = makeTransactionId(VICTIM_NUM, 1234567891);

    proto::FileID fileId;
    fileId.set_shardnum(0);
    fileId.set_realmnum(0);
    fileId.set_filenum(150);

    // Group 1: FileAppend chunk 1
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId1;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      auto* fileAppend = body.mutable_fileappend();
      *fileAppend->mutable_fileid() = fileId;
      fileAppend->set_contents("chunk1-data");

      *txList.add_transaction_list() = buildSignedProtoTx(body);
    }

    // Group 2: FileAppend chunk 2 (same FileID, different contents)
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId2;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      auto* fileAppend = body.mutable_fileappend();
      *fileAppend->mutable_fileid() = fileId;
      fileAppend->set_contents("chunk2-data");

      *txList.add_transaction_list() = buildSignedProtoTx(body);
    }

    return internal::Utilities::stringToByteVector(txList.SerializeAsString());
  }

  /**
   * Build a multi-group FileAppend TransactionList with DIVERGENT file IDs
   * (should be rejected by Layer 2).
   */
  static std::vector<std::byte> buildDivergentChunkedPayload()
  {
    proto::TransactionList txList;

    auto txId1 = makeTransactionId(VICTIM_NUM, 1234567890);
    auto txId2 = makeTransactionId(VICTIM_NUM, 1234567891);

    proto::FileID fileId1;
    fileId1.set_shardnum(0);
    fileId1.set_realmnum(0);
    fileId1.set_filenum(150);

    proto::FileID fileId2;
    fileId2.set_shardnum(0);
    fileId2.set_realmnum(0);
    fileId2.set_filenum(999);  // Different file!

    // Group 1: FileAppend to file 150
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId1;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      auto* fileAppend = body.mutable_fileappend();
      *fileAppend->mutable_fileid() = fileId1;
      fileAppend->set_contents("chunk1-data");

      *txList.add_transaction_list() = buildSignedProtoTx(body);
    }

    // Group 2: FileAppend to file 999 (divergent!)
    for (auto nodeNum : getNodeNums())
    {
      proto::TransactionBody body;
      *body.mutable_transactionid() = txId2;
      *body.mutable_nodeaccountid() = makeAccountId(nodeNum);
      body.set_transactionfee(100000000ULL);
      body.mutable_transactionvalidduration()->set_seconds(120);
      auto* fileAppend = body.mutable_fileappend();
      *fileAppend->mutable_fileid() = fileId2;
      fileAppend->set_contents("chunk2-data");

      *txList.add_transaction_list() = buildSignedProtoTx(body);
    }

    return internal::Utilities::stringToByteVector(txList.SerializeAsString());
  }
};

//-----
TEST_F(CrossGroupValidationUnitTests, RejectMultiGroupNonChunkedDifferentBodies)
{
  // Given: a malicious TransactionList with two CryptoTransfer groups with different amounts
  auto attackerKey = ED25519PrivateKey::generatePrivateKey();
  auto payload = buildMaliciousPayload(attackerKey);

  // When / Then: fromBytes() must reject this payload
  EXPECT_THROW(
    Transaction<TransferTransaction>::fromBytes(payload),
    std::invalid_argument
  ) << "Multi-group non-chunked TransactionList with different bodies must be rejected";
}

//-----
TEST_F(CrossGroupValidationUnitTests, RejectMultiGroupNonChunkedIdenticalBodies)
{
  // Given: a TransactionList with two CryptoTransfer groups with identical bodies
  // but different TransactionIDs
  auto payload = buildIdenticalBodyPayload();

  // When / Then: fromBytes() must reject this payload even with identical bodies
  EXPECT_THROW(
    Transaction<TransferTransaction>::fromBytes(payload),
    std::invalid_argument
  ) << "Multi-group non-chunked TransactionList must be rejected even with identical bodies";
}

//-----
TEST_F(CrossGroupValidationUnitTests, AcceptSingleGroupTransfer)
{
  // Given: a legitimate single-group CryptoTransfer TransactionList
  auto payload = buildSingleGroupPayload();

  // When / Then: fromBytes() must accept this payload
  EXPECT_NO_THROW(Transaction<TransferTransaction>::fromBytes(payload))
    << "Single-group TransactionList must be accepted";
}

//-----
TEST_F(CrossGroupValidationUnitTests, AcceptLegitimateChunkedMultiGroup)
{
  // Given: a legitimate multi-group FileAppend TransactionList (chunked)
  auto payload = buildLegitimateChunkedPayload();

  // When / Then: fromBytes() must accept this payload
  EXPECT_NO_THROW(Transaction<FileAppendTransaction>::fromBytes(payload))
    << "Legitimate chunked multi-group TransactionList must be accepted";
}

//-----
TEST_F(CrossGroupValidationUnitTests, RejectDivergentChunkedMultiGroup)
{
  // Given: a multi-group FileAppend TransactionList with divergent file IDs
  auto payload = buildDivergentChunkedPayload();

  // When / Then: fromBytes() must reject this payload (Layer 2 protection)
  EXPECT_THROW(
    Transaction<FileAppendTransaction>::fromBytes(payload),
    std::invalid_argument
  ) << "Chunked TransactionList with divergent non-chunk fields must be rejected";
}

//-----
TEST_F(CrossGroupValidationUnitTests, FullKillChainBlocked)
{
  // This test verifies the full attack chain is blocked.
  // Before the fix, the attacker could:
  //   1. Construct a malicious payload
  //   2. Victim deserializes it (silent acceptance)
  //   3. Victim inspects via getters (sees only benign data)
  //   4. Victim signs (all groups get signed)
  //   5. Attacker extracts Group 2 with victim's signature
  //
  // After the fix, step 2 must fail with an exception.

  auto attackerKey = ED25519PrivateKey::generatePrivateKey();
  auto victimKey = ED25519PrivateKey::generatePrivateKey();
  auto payload = buildMaliciousPayload(attackerKey);

  // The kill chain must be broken at fromBytes()
  try
  {
    auto wrappedTx = Transaction<TransferTransaction>::fromBytes(payload);
    FAIL() << "fromBytes() should have thrown std::invalid_argument for multi-group non-chunked payload";
  }
  catch (const std::invalid_argument& e)
  {
    // Expected -- the attack is blocked
    std::string msg = e.what();
    EXPECT_FALSE(msg.empty()) << "Exception message should describe the rejection reason";
  }
}
