// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "FeeEstimateMode.h"
#include "FeeEstimateQuery.h"
#include "FeeEstimateResponse.h"
#include "Hbar.h"
#include "TransferTransaction.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey =
    ECDSAsecp256k1PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  TransferTransaction transfer;
  transfer.addHbarTransfer(operatorAccountId, Hbar(-100LL, HbarUnit::TINYBAR()))
    .addHbarTransfer(AccountId::fromString("0.0.3"), Hbar(100LL, HbarUnit::TINYBAR()));

  const FeeEstimateResponse intrinsic = transfer.estimateFee().setMode(FeeEstimateMode::INTRINSIC).execute(client);
  const FeeEstimateResponse state = transfer.estimateFee().setMode(FeeEstimateMode::STATE).execute(client);

  std::cout << "INTRINSIC total: " << intrinsic.mTotal << " tinycents" << '\n';
  std::cout << "STATE total:     " << state.mTotal << " tinycents" << '\n';
  std::cout << "Difference:      " << static_cast<long long>(state.mTotal) - static_cast<long long>(intrinsic.mTotal)
            << " tinycents" << '\n';

  return 0;
}
