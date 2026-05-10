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

  // Build a small Hbar transfer to the operator's own account (no real movement of funds).
  TransferTransaction transfer;
  transfer.addHbarTransfer(operatorAccountId, Hbar(-1LL, HbarUnit::TINYBAR()))
    .addHbarTransfer(AccountId::fromString("0.0.3"), Hbar(1LL, HbarUnit::TINYBAR()));

  // Estimate fees using STATE mode so the mirror node accounts for state-dependent costs.
  const FeeEstimateResponse estimate = transfer.estimateFee().setMode(FeeEstimateMode::STATE).execute(client);

  std::cout << "STATE-mode fee estimate (tinycents):" << '\n';
  std::cout << "  network.subtotal = " << estimate.mNetwork.mSubtotal << '\n';
  std::cout << "  node.subtotal()  = " << estimate.mNode.subtotal() << '\n';
  std::cout << "  service.subtotal = " << estimate.mService.subtotal() << '\n';
  std::cout << "  total            = " << estimate.mTotal << '\n';
  std::cout << "  high_volume_multiplier = " << estimate.mHighVolumeMultiplier << '\n';

  return 0;
}
