// SPDX-License-Identifier: Apache-2.0
#include "BaseUnitTest.h"

namespace Hiero
{
//-----
void BaseUnitTest::SetUp()
{
  // Set up the test Client mock with a default operator account ID and private key.
  mClientMock = Client::forNetwork({
    { "0.client.mock.com:50211", AccountId(3ULL) },
    { "1.client.mock.com:50211", AccountId(4ULL) }
  });
  mClientMock.setMirrorNetwork({ { "0.mirror.mock.com:5551" } });
  mClientMock.setOperator(mOperatorMockAccountId, mOperatorMockPrivateKey);

  // Set up a mock TransactionId with a valid start time.
  mTransactionIdMock = TransactionId::withValidStart(mOperatorMockAccountId, std::chrono::system_clock::now());
}

} // namespace Hiero
