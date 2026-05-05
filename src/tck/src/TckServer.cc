// SPDX-License-Identifier: Apache-2.0
// Windows build requires this to be included first for some reason.
#include <Transaction.h> // NOLINT

#include "TckServer.h"
#include "account/AccountService.h"
#include "account/params/ApproveAllowanceParams.h"
#include "account/params/CreateAccountParams.h"
#include "account/params/DeleteAccountParams.h"
#include "account/params/DeleteAllowanceParams.h"
#include "account/params/GetAccountBalanceParams.h"
#include "account/params/GetAccountInfoParams.h"
#include "account/params/TransferCryptoParams.h"
#include "account/params/UpdateAccountParams.h"
#include "contract/ContractService.h"
#include "contract/params/ContractByteCodeQueryParams.h"
#include "contract/params/ContractCallQueryParams.h"
#include "contract/params/ContractInfoQueryParams.h"
#include "contract/params/CreateContractParams.h"
#include "contract/params/DeleteContractParams.h"
#include "contract/params/ExecuteContractParams.h"
#include "contract/params/UpdateContractParams.h"
#include "file/FileService.h"
#include "file/params/AppendFileParams.h"
#include "file/params/CreateFileParams.h"
#include "file/params/DeleteFileParams.h"
#include "file/params/GetFileContentsParams.h"
#include "file/params/GetFileInfoParams.h"
#include "file/params/UpdateFileParams.h"
#include "key/KeyService.h"
#include "key/params/GenerateKeyParams.h"
#include "schedule/ScheduleService.h"
#include "schedule/params/CreateScheduleParams.h"
#include "schedule/params/DeleteScheduleParams.h"
#include "sdk/SdkClient.h"
#include "sdk/params/ResetParams.h"
#include "sdk/params/SetupParams.h"
#include "token/TokenService.h"
#include "token/params/AirdropTokenParams.h"
#include "token/params/AssociateTokenParams.h"
#include "token/params/BurnTokenParams.h"
#include "token/params/CancelAirdropParams.h"
#include "token/params/ClaimAirdropParams.h"
#include "token/params/CreateTokenParams.h"
#include "token/params/DeleteTokenParams.h"
#include "token/params/DissociateTokenParams.h"
#include "token/params/FreezeTokenParams.h"
#include "token/params/GrantTokenKycParams.h"
#include "token/params/MintTokenParams.h"
#include "token/params/PauseTokenParams.h"
#include "token/params/RevokeTokenKycParams.h"
#include "token/params/TokenRejectParams.h"
#include "token/params/UnfreezeTokenParams.h"
#include "token/params/UnpauseTokenParams.h"
#include "token/params/UpdateTokenFeeScheduleParams.h"
#include "token/params/UpdateTokenParams.h"
#include "token/params/WipeTokenParams.h"
#include "topic/TopicService.h"
#include "topic/params/CreateTopicParams.h"
#include "topic/params/DeleteTopicParams.h"
#include "topic/params/GetTopicInfoQueryParams.h"
#include "topic/params/TopicMessageSubmitParams.h"
#include "json/JsonUtils.h"

namespace Hiero::TCK
{

//-----
TckServer::TckServer()
  : TckServer(DEFAULT_HTTP_PORT)
{
}

//-----
TckServer::TckServer(int port)
{
  // Register Methods with the new JsonRpcParser

  // Add the SDK client functions.
  mJsonRpcParser.addMethod("setup", getHandle(SdkClient::setup));
  mJsonRpcParser.addMethod("reset", getHandle(SdkClient::reset));

  // Add the KeyService functions.
  mJsonRpcParser.addMethod("generateKey", getHandle(KeyService::generateKey));

  // Add the AccountService functions.
  mJsonRpcParser.addMethod("approveAllowance", getHandle(AccountService::approveAllowance));
  mJsonRpcParser.addMethod("createAccount", getHandle(AccountService::createAccount));
  mJsonRpcParser.addMethod("deleteAllowance", getHandle(AccountService::deleteAllowance));
  mJsonRpcParser.addMethod("deleteAccount", getHandle(AccountService::deleteAccount));
  mJsonRpcParser.addMethod("getAccountBalance", getHandle(AccountService::getAccountBalance));
  mJsonRpcParser.addMethod("getAccountInfo", getHandle(AccountService::getAccountInfo));
  mJsonRpcParser.addMethod("transferCrypto", getHandle(AccountService::transferCrypto));
  mJsonRpcParser.addMethod("updateAccount", getHandle(AccountService::updateAccount));

  mJsonRpcParser.addMethod("airdropToken", getHandle(TokenService::airdropToken));
  mJsonRpcParser.addMethod("associateToken", getHandle(TokenService::associateToken));
  mJsonRpcParser.addMethod("burnToken", getHandle(TokenService::burnToken));
  mJsonRpcParser.addMethod("cancelAirdrop", getHandle(TokenService::cancelAirdrop));
  mJsonRpcParser.addMethod("claimAirdrop", getHandle(TokenService::claimAirdrop));
  mJsonRpcParser.addMethod("createToken", getHandle(TokenService::createToken));
  mJsonRpcParser.addMethod("deleteToken", getHandle(TokenService::deleteToken));
  mJsonRpcParser.addMethod("dissociateToken", getHandle(TokenService::dissociateToken));
  mJsonRpcParser.addMethod("freezeToken", getHandle(TokenService::freezeToken));
  mJsonRpcParser.addMethod("grantTokenKyc", getHandle(TokenService::grantTokenKyc));
  mJsonRpcParser.addMethod("mintToken", getHandle(TokenService::mintToken));
  mJsonRpcParser.addMethod("pauseToken", getHandle(TokenService::pauseToken));
  mJsonRpcParser.addMethod("rejectToken", getHandle(TokenService::rejectToken));
  mJsonRpcParser.addMethod("revokeTokenKyc", getHandle(TokenService::revokeTokenKyc));
  mJsonRpcParser.addMethod("unfreezeToken", getHandle(TokenService::unfreezeToken));
  mJsonRpcParser.addMethod("unpauseToken", getHandle(TokenService::unpauseToken));
  mJsonRpcParser.addMethod("updateToken", getHandle(TokenService::updateToken));
  mJsonRpcParser.addMethod("updateTokenFeeSchedule", getHandle(TokenService::updateTokenFeeSchedule));
  mJsonRpcParser.addMethod("wipeToken", getHandle(TokenService::wipeToken));

  // Topic Service
  mJsonRpcParser.addMethod("createTopic", getHandle(TopicService::createTopic));
  mJsonRpcParser.addMethod("deleteTopic", getHandle(TopicService::deleteTopic));
  mJsonRpcParser.addMethod("getTopicInfo", getHandle(TopicService::getTopicInfo));
  mJsonRpcParser.addMethod("submitTopicMessage", getHandle(TopicService::submitTopicMessage));

  // Contract Service
  mJsonRpcParser.addMethod("createContract", getHandle(ContractService::createContract));
  mJsonRpcParser.addMethod("contractByteCodeQuery", getHandle(ContractService::contractByteCodeQuery));
  mJsonRpcParser.addMethod("contractCallQuery", getHandle(ContractService::contractCallQuery));
  mJsonRpcParser.addMethod("contractInfoQuery", getHandle(ContractService::contractInfoQuery));
  mJsonRpcParser.addMethod("deleteContract", getHandle(ContractService::deleteContract));
  mJsonRpcParser.addMethod("executeContract", getHandle(ContractService::executeContract));
  mJsonRpcParser.addMethod("updateContract", getHandle(ContractService::updateContract));

  // Add the FileService functions.
  mJsonRpcParser.addMethod("appendFile", getHandle(FileService::appendFile));
  mJsonRpcParser.addMethod("createFile", getHandle(FileService::createFile));
  mJsonRpcParser.addMethod("deleteFile", getHandle(FileService::deleteFile));
  mJsonRpcParser.addMethod("getFileContents", getHandle(FileService::getFileContents));
  mJsonRpcParser.addMethod("getFileInfo", getHandle(FileService::getFileInfo));
  mJsonRpcParser.addMethod("updateFile", getHandle(FileService::updateFile));

  // Schedule Service
  mJsonRpcParser.addMethod("createSchedule", getHandle(ScheduleService::createSchedule));
  mJsonRpcParser.addMethod("deleteSchedule", getHandle(ScheduleService::deleteSchedule));

  setupHttpHandler();
  mServer.listen("localhost", port);
}

//-----
template<typename ParamsType>
TckServer::MethodHandle TckServer::getHandle(nlohmann::json (*method)(const ParamsType&))
{
  return [method](const nlohmann::json& params) { return method(params.get<ParamsType>()); };
}

//-----
template<typename ParamsType>
TckServer::NotificationHandle TckServer::getHandle(void (*notification)(const ParamsType&))
{
  return [notification](const nlohmann::json& params) { notification(params.get<ParamsType>()); };
}

//-----
void TckServer::setupHttpHandler()
{
  mServer.Post("/",
               [this](const httplib::Request& request, httplib::Response& response)
               { handleHttpRequest(request, response); });
}

//-----
void TckServer::handleHttpRequest(const httplib::Request& request, httplib::Response& response)
{
  const std::string jsonResponse = handleJsonRequest(request.body);
  if (jsonResponse.empty())
  {
    response.status = 204;
  }
  else
  {
    response.status = 200;
    response.set_content(jsonResponse, "application/json");
  }
}

//-----
std::string TckServer::handleJsonRequest(const std::string& request)
{
  return mJsonRpcParser.handle(request);
}

// Explicit template instantiations
template TckServer::MethodHandle TckServer::getHandle<SdkClient::SetupParams>(
  nlohmann::json (*method)(const SdkClient::SetupParams&));
template TckServer::MethodHandle TckServer::getHandle<SdkClient::ResetParams>(
  nlohmann::json (*method)(const SdkClient::ResetParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::CreateAccountParams>(
  nlohmann::json (*method)(const AccountService::CreateAccountParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::UpdateAccountParams>(
  nlohmann::json (*method)(const AccountService::UpdateAccountParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::DeleteAccountParams>(
  nlohmann::json (*method)(const AccountService::DeleteAccountParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::GetAccountBalanceParams>(
  nlohmann::json (*method)(const AccountService::GetAccountBalanceParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::GetAccountInfoParams>(
  nlohmann::json (*method)(const AccountService::GetAccountInfoParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::TransferCryptoParams>(
  nlohmann::json (*method)(const AccountService::TransferCryptoParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::ApproveAllowanceParams>(
  nlohmann::json (*method)(const AccountService::ApproveAllowanceParams&));
template TckServer::MethodHandle TckServer::getHandle<AccountService::DeleteAllowanceParams>(
  nlohmann::json (*method)(const AccountService::DeleteAllowanceParams&));
template TckServer::MethodHandle TckServer::getHandle<ContractService::CreateContractParams>(
  nlohmann::json (*method)(const ContractService::CreateContractParams&));
template TckServer::MethodHandle TckServer::getHandle<ContractService::ContractByteCodeQueryParams>(
  nlohmann::json (*method)(const ContractService::ContractByteCodeQueryParams&));
template TckServer::MethodHandle TckServer::getHandle<ContractService::ContractCallQueryParams>(
  nlohmann::json (*method)(const ContractService::ContractCallQueryParams&));
template TckServer::MethodHandle TckServer::getHandle<ContractService::ContractInfoQueryParams>(
  nlohmann::json (*method)(const ContractService::ContractInfoQueryParams&));
template TckServer::MethodHandle TckServer::getHandle<ContractService::DeleteContractParams>(
  nlohmann::json (*method)(const ContractService::DeleteContractParams&));
template TckServer::MethodHandle TckServer::getHandle<ContractService::ExecuteContractParams>(
  nlohmann::json (*method)(const ContractService::ExecuteContractParams&));
template TckServer::MethodHandle TckServer::getHandle<ContractService::UpdateContractParams>(
  nlohmann::json (*method)(const ContractService::UpdateContractParams&));
template TckServer::MethodHandle TckServer::getHandle<KeyService::GenerateKeyParams>(
  nlohmann::json (*method)(const KeyService::GenerateKeyParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::CreateTokenParams>(
  nlohmann::json (*method)(const TokenService::CreateTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::UpdateTokenParams>(
  nlohmann::json (*method)(const TokenService::UpdateTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::MintTokenParams>(
  nlohmann::json (*method)(const TokenService::MintTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::BurnTokenParams>(
  nlohmann::json (*method)(const TokenService::BurnTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::AssociateTokenParams>(
  nlohmann::json (*method)(const TokenService::AssociateTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::DissociateTokenParams>(
  nlohmann::json (*method)(const TokenService::DissociateTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::UpdateTokenFeeScheduleParams>(
  nlohmann::json (*method)(const TokenService::UpdateTokenFeeScheduleParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::UnpauseTokenParams>(
  nlohmann::json (*method)(const TokenService::UnpauseTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::PauseTokenParams>(
  nlohmann::json (*method)(const TokenService::PauseTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::FreezeTokenParams>(
  nlohmann::json (*method)(const TokenService::FreezeTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::UnfreezeTokenParams>(
  nlohmann::json (*method)(const TokenService::UnfreezeTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::WipeTokenParams>(
  nlohmann::json (*method)(const TokenService::WipeTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::DeleteTokenParams>(
  nlohmann::json (*method)(const TokenService::DeleteTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::GrantTokenKycParams>(
  nlohmann::json (*method)(const TokenService::GrantTokenKycParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::RevokeTokenKycParams>(
  nlohmann::json (*method)(const TokenService::RevokeTokenKycParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::RejectTokenParams>(
  nlohmann::json (*method)(const TokenService::RejectTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::AirdropTokenParams>(
  nlohmann::json (*method)(const TokenService::AirdropTokenParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::CancelAirdropParams>(
  nlohmann::json (*method)(const TokenService::CancelAirdropParams&));
template TckServer::MethodHandle TckServer::getHandle<TokenService::ClaimAirdropParams>(
  nlohmann::json (*method)(const TokenService::ClaimAirdropParams&));
template TckServer::MethodHandle TckServer::getHandle<FileService::AppendFileParams>(
  nlohmann::json (*method)(const FileService::AppendFileParams&));
template TckServer::MethodHandle TckServer::getHandle<FileService::CreateFileParams>(
  nlohmann::json (*method)(const FileService::CreateFileParams&));
template TckServer::MethodHandle TckServer::getHandle<FileService::DeleteFileParams>(
  nlohmann::json (*method)(const FileService::DeleteFileParams&));
template TckServer::MethodHandle TckServer::getHandle<FileService::GetFileContentsParams>(
  nlohmann::json (*method)(const FileService::GetFileContentsParams&));
template TckServer::MethodHandle TckServer::getHandle<FileService::GetFileInfoParams>(
  nlohmann::json (*method)(const FileService::GetFileInfoParams&));
template TckServer::MethodHandle TckServer::getHandle<FileService::UpdateFileParams>(
  nlohmann::json (*method)(const FileService::UpdateFileParams&));
template TckServer::MethodHandle TckServer::getHandle<TopicService::CreateTopicParams>(
  nlohmann::json (*method)(const TopicService::CreateTopicParams&));
template TckServer::MethodHandle TckServer::getHandle<TopicService::DeleteTopicParams>(
  nlohmann::json (*method)(const TopicService::DeleteTopicParams&));
template TckServer::MethodHandle TckServer::getHandle<TopicService::GetTopicInfoQueryParams>(
  nlohmann::json (*method)(const TopicService::GetTopicInfoQueryParams&));
template TckServer::MethodHandle TckServer::getHandle<TopicService::TopicMessageSubmitParams>(
  nlohmann::json (*method)(const TopicService::TopicMessageSubmitParams&));
template TckServer::MethodHandle TckServer::getHandle<ScheduleService::CreateScheduleParams>(
  nlohmann::json (*method)(const ScheduleService::CreateScheduleParams&));
template TckServer::MethodHandle TckServer::getHandle<ScheduleService::DeleteScheduleParams>(
  nlohmann::json (*method)(const ScheduleService::DeleteScheduleParams&));

} // namespace Hiero::TCK
