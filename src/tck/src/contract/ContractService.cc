// SPDX-License-Identifier: Apache-2.0
#include "contract/ContractService.h"
#include "common/CommonTransactionParams.h"

#include "contract/params/ContractByteCodeQueryParams.h"
#include "contract/params/ContractCallQueryParams.h"
#include "contract/params/ContractInfoQueryParams.h"
#include "contract/params/CreateContractParams.h"
#include "contract/params/DeleteContractParams.h"
#include "contract/params/ExecuteContractParams.h"
#include "contract/params/UpdateContractParams.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"

#include <AccountId.h>
#include <ContractByteCodeQuery.h>
#include <ContractCallQuery.h>
#include <ContractCreateTransaction.h>
#include <ContractDeleteTransaction.h>
#include <ContractExecuteTransaction.h>
#include <ContractFunctionResult.h>
#include <ContractId.h>
#include <ContractInfo.h>
#include <ContractInfoQuery.h>
#include <ContractUpdateTransaction.h>
#include <FileId.h>
#include <Hbar.h>
#include <PublicKey.h>
#include <Status.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <impl/EntityIdHelper.h>
#include <impl/HexConverter.h>

#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace
{
std::string stripHexPrefix(std::string hex)
{
  if (hex.rfind("0x", 0) == 0 || hex.rfind("0X", 0) == 0)
  {
    hex = hex.substr(2);
  }

  return hex;
}
} // namespace

namespace Hiero::TCK::ContractService
{
//-----
nlohmann::json createContract(const CreateContractParams& params)
{
  ContractCreateTransaction contractCreateTransaction;
  contractCreateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mBytecodeFileId.has_value())
  {
    contractCreateTransaction.setBytecodeFileId(FileId::fromString(params.mBytecodeFileId.value()));
  }

  if (params.mGas.has_value())
  {
    contractCreateTransaction.setGas(static_cast<uint64_t>(params.mGas.value()));
  }

  if (params.mAdminKey.has_value())
  {
    contractCreateTransaction.setAdminKey(KeyService::getHieroKey(params.mAdminKey.value()));
  }

  if (params.mConstructorParameters.has_value())
  {
    std::string constructorParameters = stripHexPrefix(params.mConstructorParameters.value());

    contractCreateTransaction.setConstructorParameters(internal::HexConverter::hexToBytes(constructorParameters));
  }

  if (params.mInitialBalance.has_value())
  {
    contractCreateTransaction.setInitialBalance(
      Hbar::fromTinybars(internal::EntityIdHelper::getNum<int64_t>(params.mInitialBalance.value())));
  }

  if (params.mMemo.has_value())
  {
    contractCreateTransaction.setMemo(params.mMemo.value());
  }

  if (params.mAutoRenewAccountId.has_value())
  {
    contractCreateTransaction.setAutoRenewAccountId(AccountId::fromString(params.mAutoRenewAccountId.value()));
  }

  if (params.mAutoRenewPeriod.has_value())
  {
    contractCreateTransaction.setAutoRenewPeriod(
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(params.mAutoRenewPeriod.value())));
  }

  if (params.mStakedAccountId.has_value())
  {
    contractCreateTransaction.setStakedAccountId(AccountId::fromString(params.mStakedAccountId.value()));
  }

  if (params.mStakedNodeId.has_value())
  {
    contractCreateTransaction.setStakedNodeId(internal::EntityIdHelper::getNum<uint64_t>(params.mStakedNodeId.value()));
  }

  if (params.mDeclineStakingReward.has_value())
  {
    contractCreateTransaction.setDeclineStakingReward(params.mDeclineStakingReward.value());
  }

  if (params.mMaxAutomaticTokenAssociations.has_value())
  {
    contractCreateTransaction.setMaxAutomaticTokenAssociations(params.mMaxAutomaticTokenAssociations.value());
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(contractCreateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    contractCreateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  return {
    {"contractId", txReceipt.mContractId.value().toString()},
    { "status",    gStatusToString.at(txReceipt.mStatus)   }
  };
}

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

//-----
nlohmann::json updateContract(const UpdateContractParams& params)
{
  ContractUpdateTransaction contractUpdateTransaction;
  contractUpdateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mContractId.has_value())
  {
    contractUpdateTransaction.setContractId(ContractId::fromString(params.mContractId.value()));
  }

  if (params.mAdminKey.has_value())
  {
    contractUpdateTransaction.setAdminKey(KeyService::getHieroKey(params.mAdminKey.value()));
  }

  if (params.mExpirationTime.has_value())
  {
    contractUpdateTransaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(params.mExpirationTime.value())));
  }

  if (params.mAutoRenewAccountId.has_value())
  {
    contractUpdateTransaction.setAutoRenewAccountId(AccountId::fromString(params.mAutoRenewAccountId.value()));
  }

  if (params.mAutoRenewPeriod.has_value())
  {
    contractUpdateTransaction.setAutoRenewPeriod(
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(params.mAutoRenewPeriod.value())));
  }

  if (params.mMemo.has_value())
  {
    contractUpdateTransaction.setContractMemo(params.mMemo.value());
  }

  if (params.mMaxAutomaticTokenAssociations.has_value())
  {
    contractUpdateTransaction.setMaxAutomaticTokenAssociations(params.mMaxAutomaticTokenAssociations.value());
  }

  if (params.mStakedAccountId.has_value())
  {
    contractUpdateTransaction.setStakedAccountId(AccountId::fromString(params.mStakedAccountId.value()));
  }

  if (params.mStakedNodeId.has_value())
  {
    contractUpdateTransaction.setStakedNodeId(internal::EntityIdHelper::getNum<int64_t>(params.mStakedNodeId.value()));
  }

  if (params.mDeclineStakingReward.has_value())
  {
    contractUpdateTransaction.setDeclineStakingReward(params.mDeclineStakingReward.value());
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(contractUpdateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    contractUpdateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  return {
    {"status", gStatusToString.at(txReceipt.mStatus)}
  };
}

//-----
nlohmann::json executeContract(const ExecuteContractParams& params)
{
  ContractExecuteTransaction contractExecuteTransaction;
  contractExecuteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mContractId.has_value())
  {
    contractExecuteTransaction.setContractId(ContractId::fromString(params.mContractId.value()));
  }

  if (params.mGas.has_value())
  {
    contractExecuteTransaction.setGas(static_cast<uint64_t>(params.mGas.value()));
  }

  if (params.mAmount.has_value())
  {
    contractExecuteTransaction.setPayableAmount(
      Hbar::fromTinybars(internal::EntityIdHelper::getNum<int64_t>(params.mAmount.value())));
  }

  if (params.mFunctionParameters.has_value())
  {
    std::string functionParameters = stripHexPrefix(params.mFunctionParameters.value());
    contractExecuteTransaction.setFunctionParameters(internal::HexConverter::hexToBytes(functionParameters));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(contractExecuteTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    contractExecuteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  return {
    {"status", gStatusToString.at(txReceipt.mStatus)}
  };
}

//-----
nlohmann::json contractByteCodeQuery(const ContractByteCodeQueryParams& params)
{
  ContractByteCodeQuery contractByteCodeQuery;
  contractByteCodeQuery.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mContractId.has_value())
  {
    contractByteCodeQuery.setContractId(ContractId::fromString(params.mContractId.value()));
  }

  if (params.mQueryPayment.has_value())
  {
    contractByteCodeQuery.setQueryPayment(Hbar::fromTinybars(std::stoll(params.mQueryPayment.value())));
  }

  if (params.mMaxQueryPayment.has_value())
  {
    contractByteCodeQuery.setMaxQueryPayment(Hbar::fromTinybars(std::stoll(params.mMaxQueryPayment.value())));
  }

  const ContractByteCode byteCode = contractByteCodeQuery.execute(SdkClient::getClient());

  return {
    {"bytecode", internal::HexConverter::bytesToHex(byteCode)}
  };
}

//-----
nlohmann::json contractCallQuery(const ContractCallQueryParams& params)
{
  ContractCallQuery contractCallQuery;
  contractCallQuery.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mContractId.has_value())
  {
    contractCallQuery.setContractId(ContractId::fromString(params.mContractId.value()));
  }

  if (params.mGas.has_value())
  {
    contractCallQuery.setGas(static_cast<uint64_t>(params.mGas.value()));
  }

  if (params.mFunctionName.has_value())
  {
    contractCallQuery.setFunction(params.mFunctionName.value());

    if (params.mFunctionParameters.has_value())
    {
      std::string functionParameters = stripHexPrefix(params.mFunctionParameters.value());

      std::vector<std::byte> callData = contractCallQuery.getFunctionParameters();
      std::vector<std::byte> encodedParameters = internal::HexConverter::hexToBytes(functionParameters);
      callData.insert(callData.end(), encodedParameters.begin(), encodedParameters.end());

      contractCallQuery.setFunctionParameters(callData);
    }
  }
  else if (params.mFunctionParameters.has_value())
  {
    std::string functionParameters = stripHexPrefix(params.mFunctionParameters.value());

    contractCallQuery.setFunctionParameters(internal::HexConverter::hexToBytes(functionParameters));
  }

  if (params.mQueryPayment.has_value())
  {
    contractCallQuery.setQueryPayment(Hbar::fromTinybars(std::stoll(params.mQueryPayment.value())));
  }

  if (params.mMaxQueryPayment.has_value())
  {
    contractCallQuery.setMaxQueryPayment(Hbar::fromTinybars(std::stoll(params.mMaxQueryPayment.value())));
  }

  const ContractFunctionResult result = contractCallQuery.execute(SdkClient::getClient());

  nlohmann::json response = {
    {"contractId",    result.mContractId.toString()                                 },
    { "rawResult",    internal::HexConverter::bytesToHex(result.mContractCallResult)},
    { "gasUsed",      std::to_string(result.mGasUsed)                               },
    { "errorMessage", result.mErrorMessage                                          },
    { "bloom",        internal::HexConverter::bytesToHex(result.mBloom)             },
    { "gas",          std::to_string(result.mGas)                                   },
    { "amount",       std::to_string(result.mHbarAmount.toTinybars())               }
  };

  try
  {
    response["address"] = result.getAddress(0);
  }
  catch (const std::exception&)
  {
    // result.getAddress(0) throws if the result contains no addresses; omit the field in that case
  }

  return response;
}

//-----
nlohmann::json contractInfoQuery(const ContractInfoQueryParams& params)
{
  ContractInfoQuery contractInfoQuery;
  contractInfoQuery.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mContractId.has_value())
  {
    contractInfoQuery.setContractId(ContractId::fromString(params.mContractId.value()));
  }

  if (params.mQueryPayment.has_value())
  {
    contractInfoQuery.setQueryPayment(Hbar::fromTinybars(std::stoll(params.mQueryPayment.value())));
  }

  if (params.mMaxQueryPayment.has_value())
  {
    contractInfoQuery.setMaxQueryPayment(Hbar::fromTinybars(std::stoll(params.mMaxQueryPayment.value())));
  }

  const ContractInfo info = contractInfoQuery.execute(SdkClient::getClient());

  nlohmann::json response;

  response["contractId"] = info.mContractId.toString();
  response["accountId"] = info.mAccountId.toString();
  response["contractAccountId"] = info.mContractAccountId;

  if (const auto* pubKey = dynamic_cast<const PublicKey*>(info.mAdminKey.get()))
  {
    response["adminKey"] = pubKey->toStringDer();
  }
  else if (info.mAdminKey)
  {
    response["adminKey"] = internal::HexConverter::bytesToHex(info.mAdminKey->toBytes());
  }

  const auto expirationTimeSeconds =
    std::chrono::duration_cast<std::chrono::seconds>(info.mExpirationTime.time_since_epoch()).count();
  response["expirationTime"] = std::to_string(expirationTimeSeconds);

  const auto autoRenewPeriodSeconds = std::chrono::duration_cast<std::chrono::seconds>(info.mAutoRenewPeriod).count();
  response["autoRenewPeriod"] = std::to_string(autoRenewPeriodSeconds);

  if (info.mAutoRenewAccountId.has_value())
  {
    response["autoRenewAccountId"] = info.mAutoRenewAccountId->toString();
  }

  response["storage"] = std::to_string(info.mStorage);
  response["contractMemo"] = info.mMemo;
  response["balance"] = std::to_string(info.mBalance.toTinybars());
  response["isDeleted"] = info.mIsDeleted;
  response["maxAutomaticTokenAssociations"] = std::to_string(info.mMaxAutomaticTokenAssociations);
  response["ledgerId"] = info.mLedgerId.toString();

  nlohmann::json stakingInfo;
  stakingInfo["declineStakingReward"] = info.mStakingInfo.mDeclineRewards;

  if (info.mStakingInfo.mStakePeriodStart.has_value())
  {
    const auto stakePeriodStartSeconds =
      std::chrono::duration_cast<std::chrono::seconds>(info.mStakingInfo.mStakePeriodStart->time_since_epoch()).count();
    stakingInfo["stakePeriodStart"] = std::to_string(stakePeriodStartSeconds);
  }
  else
  {
    stakingInfo["stakePeriodStart"] = "0";
  }

  stakingInfo["pendingReward"] = std::to_string(info.mStakingInfo.mPendingReward.toTinybars());
  stakingInfo["stakedToMe"] = std::to_string(info.mStakingInfo.mStakedToMe.toTinybars());

  if (info.mStakingInfo.mStakedAccountId.has_value())
  {
    stakingInfo["stakedAccountId"] = info.mStakingInfo.mStakedAccountId->toString();
  }
  else
  {
    stakingInfo["stakedAccountId"] = nullptr;
  }

  if (info.mStakingInfo.mStakedNodeId.has_value())
  {
    stakingInfo["stakedNodeId"] = std::to_string(info.mStakingInfo.mStakedNodeId.value());
  }
  else
  {
    stakingInfo["stakedNodeId"] = nullptr;
  }

  response["stakingInfo"] = stakingInfo;

  return response;
}

} // namespace Hiero::TCK::ContractService
