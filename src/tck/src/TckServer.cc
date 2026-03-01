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
#include "file/FileService.h"
#include "file/params/CreateFileParams.h"
#include "file/params/UpdateFileParams.h"
#include "key/KeyService.h"
#include "key/params/GenerateKeyParams.h"
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

  // Add the FileService functions.
  mJsonRpcParser.addMethod("createFile", getHandle(FileService::createFile));
  mJsonRpcParser.addMethod("updateFile", getHandle(FileService::updateFile));

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
template TckServer::MethodHandle TckServer::getHandle<FileService::CreateFileParams>(
  nlohmann::json (*method)(const FileService::CreateFileParams&));
template TckServer::MethodHandle TckServer::getHandle<FileService::UpdateFileParams>(
  nlohmann::json (*method)(const FileService::UpdateFileParams&));

} // namespace Hiero::TCK
