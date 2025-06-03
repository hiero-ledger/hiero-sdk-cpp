// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_BASE_UNIT_TEST_H_
#define HIERO_SDK_CPP_BASE_UNIT_TEST_H_

#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "TransactionId.h"

#include <gtest/gtest.h>
#include <string>

namespace Hiero
{

class BaseUnitTest : public testing::Test
{
protected:
  /**  Get the test Client mock used in unit tests.
   *
   * @return The test Client mock.
   */
  [[nodiscard]] inline const Client& getTestClientMock() const { return mClientMock; }

  /**  Get a transaction ID with a valid start time.
   *
   * @return A TransactionId with a valid start time.
   */
  [[nodiscard]] inline const TransactionId& getTestTransactionIdMock() const { return mTransactionIdMock; }

  /** Set up the test environment.
   *
   * This method is called before each test case to initialize the test Client and file content.
   */
  void SetUp() override;

private:
  Client mClientMock;
  TransactionId mTransactionIdMock;
  const AccountId mOperatorMockAccountId = AccountId::fromString("0.0.2");
  const std::shared_ptr<PrivateKey> mOperatorMockPrivateKey = ED25519PrivateKey::fromString(
    "302e020100300506032b65700422042091132178e72057a1d7528025956fe39b0b847f200ab59b2fdd367017f3087137");
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_BASE_UNIT_TEST_H_