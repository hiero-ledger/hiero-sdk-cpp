// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "FileId.h"
#include "FreezeTransaction.h"
#include "NodeDeleteTransaction.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>

using namespace Hiero;

class NodeDeleteTransactionIntegrationTests : public BaseIntegrationTest
{
protected:
  [[nodiscard]] const uint64_t& getNodeId() const { return mNodeId; }
  [[nodiscard]] const FileId& getFileId() const { return mFileId; }
  [[nodiscard]] const std::vector<std::byte> getFileHash() const
  {
    return internal::HexConverter::hexToBytes(mFileHash);
  }

private:
  const uint64_t mNodeId = 2;
  const FileId mFileId = FileId::fromString("0.0.150");
  // The file hash needs to be taken from the network context to be correct
  const std::string mFileHash =
    "f933337c1585674b4e273072f48b140fc0aa81aea962c8cfa1f0cef5e04981bcd7c596c1df0ec6a26f0610940a5de5f9";
};

//-----
TEST_F(NodeDeleteTransactionIntegrationTests, DISABLED_CanExecuteNodeDeleteTransaction)
{
  // Given / When / Then
  TransactionResponse txResponse;

  ASSERT_NO_THROW(
    txResponse = NodeDeleteTransaction().setNodeId(getNodeId()).freezeWith(&getTestClient()).execute(getTestClient()));

  ASSERT_NO_THROW(txResponse = FreezeTransaction()
                                 .setFreezeType(FreezeType::PREPARE_UPGRADE)
                                 .setFileId(getFileId())
                                 .setFileHash(getFileHash())
                                 .freezeWith(&getTestClient())
                                 .execute(getTestClient()));

  ASSERT_NO_THROW(txResponse = FreezeTransaction()
                                 .setFreezeType(FreezeType::FREEZE_UPGRADE)
                                 .setStartTime(std::chrono::system_clock::now() + std::chrono::seconds(5))
                                 .setFileId(getFileId())
                                 .setFileHash(getFileHash())
                                 .freezeWith(&getTestClient())
                                 .execute(getTestClient()));

  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
}