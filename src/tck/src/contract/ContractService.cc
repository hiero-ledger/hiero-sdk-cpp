// SPDX-License-Identifier: Apache-2.0
#include "contract/ContractService.h"
#include "common/CommonTransactionParams.h"
#include "contract/params/DeleteContractParams.h"
#include "sdk/SdkClient.h"

#include <AccountId.h>
#include <ContractDeleteTransaction.h>
#include <ContractId.h>
#include <Status.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>

#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK::ContractService
{
//-----
nlohmann::json deleteContract(const DeleteContractParams& params)
{
  ContractDeleteTransaction contractDeleteTransaction;
  contractDeleteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mContractId.has_value())
  {
    contractDeleteTransaction.setContractId(ContractId::fromString(params.mContractId.value()));
  }

  if (params.mTransferAccountId.has_value())
  {
    contractDeleteTransaction.setTransferAccountId(AccountId::fromString(params.mTransferAccountId.value()));
  }

  if (params.mTransferContractId.has_value())
  {
    contractDeleteTransaction.setTransferContractId(ContractId::fromString(params.mTransferContractId.value()));
  }

  if (params.mPermanentRemoval.has_value())
  {
    contractDeleteTransaction.setPermanentRemoval(params.mPermanentRemoval.value());
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(contractDeleteTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        contractDeleteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

} // namespace Hiero::TCK::ContractService
