// SPDX-License-Identifier: Apache-2.0
#include "WrappedTransaction.h"
#include "exceptions/UninitializedException.h"

#include <memory>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
WrappedTransaction::WrappedTransaction(AnyPossibleTransaction transaction)
  : mTransaction(std::move(transaction))
{
}

//-----
WrappedTransaction WrappedTransaction::fromProtobuf(const proto::TransactionBody& proto)
{
  if (proto.has_cryptoapproveallowance())
  {
    return WrappedTransaction(AccountAllowanceApproveTransaction(proto));
  }
  else if (proto.has_cryptodeleteallowance())
  {
    return WrappedTransaction(AccountAllowanceDeleteTransaction(proto));
  }
  else if (proto.has_cryptocreateaccount())
  {
    return WrappedTransaction(AccountCreateTransaction(proto));
  }
  else if (proto.has_cryptodelete())
  {
    return WrappedTransaction(AccountDeleteTransaction(proto));
  }
  else if (proto.has_cryptoupdateaccount())
  {
    return WrappedTransaction(AccountUpdateTransaction(proto));
  }
  else if (proto.has_contractcreateinstance())
  {
    return WrappedTransaction(ContractCreateTransaction(proto));
  }
  else if (proto.has_contractdeleteinstance())
  {
    return WrappedTransaction(ContractDeleteTransaction(proto));
  }
  else if (proto.has_contractcall())
  {
    return WrappedTransaction(ContractExecuteTransaction(proto));
  }
  else if (proto.has_contractupdateinstance())
  {
    return WrappedTransaction(ContractUpdateTransaction(proto));
  }
  else if (proto.has_ethereumtransaction())
  {
    return WrappedTransaction(EthereumTransaction(proto));
  }
  else if (proto.has_fileappend())
  {
    return WrappedTransaction(FileAppendTransaction(proto));
  }
  else if (proto.has_filecreate())
  {
    return WrappedTransaction(FileCreateTransaction(proto));
  }
  else if (proto.has_filedelete())
  {
    return WrappedTransaction(FileDeleteTransaction(proto));
  }
  else if (proto.has_fileupdate())
  {
    return WrappedTransaction(FileUpdateTransaction(proto));
  }
  else if (proto.has_freeze())
  {
    return WrappedTransaction(FreezeTransaction(proto));
  }
  else if (proto.has_nodecreate())
  {
    return WrappedTransaction(NodeCreateTransaction(proto));
  }
  else if (proto.has_nodedelete())
  {
    return WrappedTransaction(NodeDeleteTransaction(proto));
  }
  else if (proto.has_nodeupdate())
  {
    return WrappedTransaction(NodeUpdateTransaction(proto));
  }
  else if (proto.has_util_prng())
  {
    return WrappedTransaction(PrngTransaction(proto));
  }
  else if (proto.has_schedulecreate())
  {
    return WrappedTransaction(ScheduleCreateTransaction(proto));
  }
  else if (proto.has_scheduledelete())
  {
    return WrappedTransaction(ScheduleDeleteTransaction(proto));
  }
  else if (proto.has_schedulesign())
  {
    return WrappedTransaction(ScheduleSignTransaction(proto));
  }
  else if (proto.has_systemdelete())
  {
    return WrappedTransaction(SystemDeleteTransaction(proto));
  }
  else if (proto.has_systemundelete())
  {
    return WrappedTransaction(SystemUndeleteTransaction(proto));
  }
  else if (proto.has_tokenassociate())
  {
    return WrappedTransaction(TokenAssociateTransaction(proto));
  }
  else if (proto.has_tokenburn())
  {
    return WrappedTransaction(TokenBurnTransaction(proto));
  }
  else if (proto.has_tokencreation())
  {
    return WrappedTransaction(TokenCreateTransaction(proto));
  }
  else if (proto.has_tokendeletion())
  {
    return WrappedTransaction(TokenDeleteTransaction(proto));
  }
  else if (proto.has_tokendissociate())
  {
    return WrappedTransaction(TokenDissociateTransaction(proto));
  }
  else if (proto.has_token_fee_schedule_update())
  {
    return WrappedTransaction(TokenFeeScheduleUpdateTransaction(proto));
  }
  else if (proto.has_tokenfreeze())
  {
    return WrappedTransaction(TokenFreezeTransaction(proto));
  }
  else if (proto.has_tokengrantkyc())
  {
    return WrappedTransaction(TokenGrantKycTransaction(proto));
  }
  else if (proto.has_tokenmint())
  {
    return WrappedTransaction(TokenMintTransaction(proto));
  }
  else if (proto.has_token_pause())
  {
    return WrappedTransaction(TokenPauseTransaction(proto));
  }
  else if (proto.has_tokenreject())
  {
    return WrappedTransaction(TokenRejectTransaction(proto));
  }
  else if (proto.has_tokenrevokekyc())
  {
    return WrappedTransaction(TokenRevokeKycTransaction(proto));
  }
  else if (proto.has_tokenunfreeze())
  {
    return WrappedTransaction(TokenUnfreezeTransaction(proto));
  }
  else if (proto.has_token_unpause())
  {
    return WrappedTransaction(TokenUnpauseTransaction(proto));
  }
  else if (proto.has_token_update_nfts())
  {
    return WrappedTransaction(TokenUpdateNftsTransaction(proto));
  }
  else if (proto.has_tokenwipe())
  {
    return WrappedTransaction(TokenWipeTransaction(proto));
  }
  else if (proto.has_consensuscreatetopic())
  {
    return WrappedTransaction(TopicCreateTransaction(proto));
  }
  else if (proto.has_consensusdeletetopic())
  {
    return WrappedTransaction(TopicDeleteTransaction(proto));
  }
  else if (proto.has_consensussubmitmessage())
  {
    return WrappedTransaction(TopicMessageSubmitTransaction(proto));
  }
  else if (proto.has_consensusupdatetopic())
  {
    return WrappedTransaction(TopicUpdateTransaction(proto));
  }
  else if (proto.has_cryptotransfer())
  {
    return WrappedTransaction(TransferTransaction(proto));
  }
  else
  {
    throw std::invalid_argument("TransactionBody does not contain a valid Transaction");
  }
}

//-----
WrappedTransaction WrappedTransaction::fromProtobuf(const proto::SchedulableTransactionBody& proto)
{
  proto::TransactionBody txBody;
  txBody.set_memo(proto.memo());
  txBody.set_transactionfee(proto.transactionfee());

  if (proto.has_cryptoapproveallowance())
  {
    *txBody.mutable_cryptoapproveallowance() = proto.cryptoapproveallowance();
    return WrappedTransaction(AccountAllowanceApproveTransaction(txBody));
  }
  else if (proto.has_cryptodeleteallowance())
  {
    *txBody.mutable_cryptodeleteallowance() = proto.cryptodeleteallowance();
    return WrappedTransaction(AccountAllowanceDeleteTransaction(txBody));
  }
  else if (proto.has_cryptocreateaccount())
  {
    *txBody.mutable_cryptocreateaccount() = proto.cryptocreateaccount();
    return WrappedTransaction(AccountCreateTransaction(txBody));
  }
  else if (proto.has_cryptodeleteallowance())
  {
    *txBody.mutable_cryptodelete() = proto.cryptodelete();
    return WrappedTransaction(AccountDeleteTransaction(txBody));
  }
  else if (proto.has_cryptoupdateaccount())
  {
    *txBody.mutable_cryptoupdateaccount() = proto.cryptoupdateaccount();
    return WrappedTransaction(AccountUpdateTransaction(txBody));
  }
  else if (proto.has_contractcreateinstance())
  {
    *txBody.mutable_contractcreateinstance() = proto.contractcreateinstance();
    return WrappedTransaction(ContractCreateTransaction(txBody));
  }
  else if (proto.has_contractdeleteinstance())
  {
    *txBody.mutable_contractdeleteinstance() = proto.contractdeleteinstance();
    return WrappedTransaction(ContractDeleteTransaction(txBody));
  }
  else if (proto.has_contractcall())
  {
    *txBody.mutable_contractcall() = proto.contractcall();
    return WrappedTransaction(ContractExecuteTransaction(txBody));
  }
  else if (proto.has_contractupdateinstance())
  {
    *txBody.mutable_contractupdateinstance() = proto.contractupdateinstance();
    return WrappedTransaction(ContractUpdateTransaction(txBody));
  }
  else if (proto.has_fileappend())
  {
    *txBody.mutable_fileappend() = proto.fileappend();
    return WrappedTransaction(FileAppendTransaction(txBody));
  }
  else if (proto.has_filecreate())
  {
    *txBody.mutable_filecreate() = proto.filecreate();
    return WrappedTransaction(FileCreateTransaction(txBody));
  }
  else if (proto.has_filedelete())
  {
    *txBody.mutable_filedelete() = proto.filedelete();
    return WrappedTransaction(FileDeleteTransaction(txBody));
  }
  else if (proto.has_fileupdate())
  {
    *txBody.mutable_fileupdate() = proto.fileupdate();
    return WrappedTransaction(FileUpdateTransaction(txBody));
  }
  else if (proto.has_freeze())
  {
    *txBody.mutable_freeze() = proto.freeze();
    return WrappedTransaction(FreezeTransaction(txBody));
  }
  else if (proto.has_nodecreate())
  {
    *txBody.mutable_nodecreate() = proto.nodecreate();
    return WrappedTransaction(NodeCreateTransaction(txBody));
  }
  else if (proto.has_nodedelete())
  {
    *txBody.mutable_nodedelete() = proto.nodedelete();
    return WrappedTransaction(NodeDeleteTransaction(txBody));
  }
  else if (proto.has_nodeupdate())
  {
    *txBody.mutable_nodeupdate() = proto.nodeupdate();
    return WrappedTransaction(NodeUpdateTransaction(txBody));
  }
  else if (proto.has_util_prng())
  {
    *txBody.mutable_util_prng() = proto.util_prng();
    return WrappedTransaction(PrngTransaction(txBody));
  }
  else if (proto.has_scheduledelete())
  {
    *txBody.mutable_scheduledelete() = proto.scheduledelete();
    return WrappedTransaction(ScheduleCreateTransaction(txBody));
  }
  else if (proto.has_systemdelete())
  {
    *txBody.mutable_systemdelete() = proto.systemdelete();
    return WrappedTransaction(SystemDeleteTransaction(txBody));
  }
  else if (proto.has_systemundelete())
  {
    *txBody.mutable_systemundelete() = proto.systemundelete();
    return WrappedTransaction(SystemUndeleteTransaction(txBody));
  }
  else if (proto.has_tokenairdrop())
  {
    *txBody.mutable_tokenairdrop() = proto.tokenairdrop();
    return WrappedTransaction(TokenAirdropTransaction(txBody));
  }
  else if (proto.has_tokenassociate())
  {
    *txBody.mutable_tokenassociate() = proto.tokenassociate();
    return WrappedTransaction(TokenAssociateTransaction(txBody));
  }
  else if (proto.has_tokenburn())
  {
    *txBody.mutable_tokenburn() = proto.tokenburn();
    return WrappedTransaction(TokenBurnTransaction(txBody));
  }
  else if (proto.has_tokencancelairdrop())
  {
    *txBody.mutable_tokencancelairdrop() = proto.tokencancelairdrop();
    return WrappedTransaction(TokenCancelAirdropTransaction(txBody));
  }
  else if (proto.has_tokenclaimairdrop())
  {
    *txBody.mutable_tokenclaimairdrop() = proto.tokenclaimairdrop();
    return WrappedTransaction(TokenClaimAirdropTransaction(txBody));
  }
  else if (proto.has_tokencreation())
  {
    *txBody.mutable_tokencreation() = proto.tokencreation();
    return WrappedTransaction(TokenCreateTransaction(txBody));
  }
  else if (proto.has_tokendeletion())
  {
    *txBody.mutable_tokendeletion() = proto.tokendeletion();
    return WrappedTransaction(TokenDeleteTransaction(txBody));
  }
  else if (proto.has_tokendissociate())
  {
    *txBody.mutable_tokendissociate() = proto.tokendissociate();
    return WrappedTransaction(TokenDissociateTransaction(txBody));
  }
  else if (proto.has_token_fee_schedule_update())
  {
    *txBody.mutable_token_fee_schedule_update() = proto.token_fee_schedule_update();
    return WrappedTransaction(TokenFeeScheduleUpdateTransaction(txBody));
  }
  else if (proto.has_tokenfreeze())
  {
    *txBody.mutable_tokenfreeze() = proto.tokenfreeze();
    return WrappedTransaction(TokenFreezeTransaction(txBody));
  }
  else if (proto.has_tokengrantkyc())
  {
    *txBody.mutable_tokengrantkyc() = proto.tokengrantkyc();
    return WrappedTransaction(TokenGrantKycTransaction(txBody));
  }
  else if (proto.has_tokenmint())
  {
    *txBody.mutable_tokenmint() = proto.tokenmint();
    return WrappedTransaction(TokenMintTransaction(txBody));
  }
  else if (proto.has_token_pause())
  {
    *txBody.mutable_token_pause() = proto.token_pause();
    return WrappedTransaction(TokenPauseTransaction(txBody));
  }
  else if (proto.has_tokenreject())
  {
    *txBody.mutable_tokenreject() = proto.tokenreject();
    return WrappedTransaction(TokenRejectTransaction(txBody));
  }
  else if (proto.has_tokenrevokekyc())
  {
    *txBody.mutable_tokenrevokekyc() = proto.tokenrevokekyc();
    return WrappedTransaction(TokenRevokeKycTransaction(txBody));
  }
  else if (proto.has_tokenunfreeze())
  {
    *txBody.mutable_tokenunfreeze() = proto.tokenunfreeze();
    return WrappedTransaction(TokenFreezeTransaction(txBody));
  }
  else if (proto.has_token_unpause())
  {
    *txBody.mutable_token_unpause() = proto.token_unpause();
    return WrappedTransaction(TokenUnpauseTransaction(txBody));
  }
  else if (proto.has_token_update_nfts())
  {
    *txBody.mutable_token_update_nfts() = proto.token_update_nfts();
    return WrappedTransaction(TokenUpdateNftsTransaction(txBody));
  }
  else if (proto.has_tokenwipe())
  {
    *txBody.mutable_tokenwipe() = proto.tokenwipe();
    return WrappedTransaction(TokenWipeTransaction(txBody));
  }
  else if (proto.has_consensuscreatetopic())
  {
    *txBody.mutable_consensuscreatetopic() = proto.consensuscreatetopic();
    return WrappedTransaction(TopicCreateTransaction(txBody));
  }
  else if (proto.has_consensusdeletetopic())
  {
    *txBody.mutable_consensusdeletetopic() = proto.consensusdeletetopic();
    return WrappedTransaction(TopicDeleteTransaction(txBody));
  }
  else if (proto.has_consensussubmitmessage())
  {
    *txBody.mutable_consensussubmitmessage() = proto.consensussubmitmessage();
    return WrappedTransaction(TopicMessageSubmitTransaction(txBody));
  }
  else if (proto.has_consensusupdatetopic())
  {
    *txBody.mutable_consensusupdatetopic() = proto.consensusupdatetopic();
    return WrappedTransaction(TopicUpdateTransaction(txBody));
  }
  else if (proto.has_cryptotransfer())
  {
    *txBody.mutable_cryptotransfer() = proto.cryptotransfer();
    return WrappedTransaction(TransferTransaction(txBody));
  }
  else
  {
    throw std::invalid_argument("TransactionBody contains no transaction");
  }
}

//-----
std::unique_ptr<proto::TransactionBody> WrappedTransaction::toProtobuf() const
{
  switch (getTransactionType())
  {
    case ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION:
    {
      const auto transaction = getTransaction<AccountAllowanceApproveTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case ACCOUNT_ALLOWANCE_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<AccountAllowanceDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case ACCOUNT_CREATE_TRANSACTION:
    {
      const auto transaction = getTransaction<AccountCreateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case ACCOUNT_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<AccountDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case ACCOUNT_UPDATE_TRANSACTION:
    {
      const auto transaction = getTransaction<AccountUpdateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case CONTRACT_CREATE_TRANSACTION:
    {
      const auto transaction = getTransaction<ContractCreateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case CONTRACT_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<ContractDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case CONTRACT_EXECUTE_TRANSACTION:
    {
      const auto transaction = getTransaction<ContractExecuteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case CONTRACT_UPDATE_TRANSACTION:
    {
      const auto transaction = getTransaction<ContractUpdateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case ETHEREUM_TRANSACTION:
    {
      const auto transaction = getTransaction<EthereumTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case FILE_APPEND_TRANSACTION:
    {
      const auto transaction = getTransaction<FileAppendTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case FILE_CREATE_TRANSACTION:
    {
      const auto transaction = getTransaction<FileCreateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case FILE_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<FileDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case FILE_UPDATE_TRANSACTION:
    {
      const auto transaction = getTransaction<FileUpdateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case FREEZE_TRANSACTION:
    {
      const auto transaction = getTransaction<FreezeTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case NODE_CREATE_TRANSACTION:
    {
      const auto transaction = getTransaction<NodeCreateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case NODE_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<NodeDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case NODE_UPDATE_TRANSACTION:
    {
      const auto transaction = getTransaction<NodeUpdateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case PRNG_TRANSACTION:
    {
      const auto transaction = getTransaction<PrngTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case SCHEDULE_CREATE_TRANSACTION:
    {
      const auto transaction = getTransaction<ScheduleCreateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case SCHEDULE_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<ScheduleDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case SCHEDULE_SIGN_TRANSACTION:
    {
      const auto transaction = getTransaction<ScheduleSignTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case SYSTEM_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<SystemDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case SYSTEM_UNDELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<SystemUndeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_AIRDROP_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenAirdropTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_ASSOCIATE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenAssociateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_BURN_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenBurnTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_CANCEL_AIRDROP_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenCancelAirdropTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_CLAIM_AIRDROP_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenClaimAirdropTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_CREATE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenCreateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_DISSOCIATE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenDissociateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenFeeScheduleUpdateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_FREEZE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenFreezeTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_GRANT_KYC_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenGrantKycTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_MINT_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenMintTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_PAUSE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenPauseTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_REJECT_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenRejectTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_REVOKE_KYC_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenRevokeKycTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_UNFREEZE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenUnfreezeTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_UNPAUSE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenUnpauseTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_UPDATE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenUpdateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_UPDATE_NFTS_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenUpdateNftsTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOKEN_WIPE_TRANSACTION:
    {
      const auto transaction = getTransaction<TokenWipeTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOPIC_CREATE_TRANSACTION:
    {
      const auto transaction = getTransaction<TopicCreateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOPIC_DELETE_TRANSACTION:
    {
      const auto transaction = getTransaction<TopicDeleteTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOPIC_MESSAGE_SUBMIT_TRANSACTION:
    {
      const auto transaction = getTransaction<TopicMessageSubmitTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TOPIC_UPDATE_TRANSACTION:
    {
      const auto transaction = getTransaction<TopicUpdateTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    case TRANSFER_TRANSACTION:
    {
      const auto transaction = getTransaction<TransferTransaction>();
      transaction->updateSourceTransactionBody(nullptr);
      return std::make_unique<proto::TransactionBody>(transaction->getSourceTransactionBody());
    }
    default:
    {
      throw UninitializedException("WrappedTransaction doesn't contain a Transaction");
    }
  }
}

//-----
std::unique_ptr<proto::SchedulableTransactionBody> WrappedTransaction::toSchedulableProtobuf() const
{
  proto::TransactionBody txBody = *toProtobuf();

  auto schedulableTxBody = std::make_unique<proto::SchedulableTransactionBody>();
  schedulableTxBody->set_transactionfee(txBody.transactionfee());
  schedulableTxBody->set_memo(txBody.memo());

  if (txBody.has_cryptoapproveallowance())
  {
    schedulableTxBody->set_allocated_cryptoapproveallowance(txBody.release_cryptoapproveallowance());
  }
  else if (txBody.has_cryptodeleteallowance())
  {
    schedulableTxBody->set_allocated_cryptodeleteallowance(txBody.release_cryptodeleteallowance());
  }
  else if (txBody.has_cryptocreateaccount())
  {
    schedulableTxBody->set_allocated_cryptocreateaccount(txBody.release_cryptocreateaccount());
  }
  else if (txBody.has_cryptodeleteallowance())
  {
    schedulableTxBody->set_allocated_cryptodelete(txBody.release_cryptodelete());
  }
  else if (txBody.has_cryptoupdateaccount())
  {
    schedulableTxBody->set_allocated_cryptoupdateaccount(txBody.release_cryptoupdateaccount());
  }
  else if (txBody.has_contractcreateinstance())
  {
    schedulableTxBody->set_allocated_contractcreateinstance(txBody.release_contractcreateinstance());
  }
  else if (txBody.has_contractdeleteinstance())
  {
    schedulableTxBody->set_allocated_contractdeleteinstance(txBody.release_contractdeleteinstance());
  }
  else if (txBody.has_contractcall())
  {
    schedulableTxBody->set_allocated_contractcall(txBody.release_contractcall());
  }
  else if (txBody.has_contractupdateinstance())
  {
    schedulableTxBody->set_allocated_contractupdateinstance(txBody.release_contractupdateinstance());
  }
  else if (txBody.has_fileappend())
  {
    schedulableTxBody->set_allocated_fileappend(txBody.release_fileappend());
  }
  else if (txBody.has_filecreate())
  {
    schedulableTxBody->set_allocated_filecreate(txBody.release_filecreate());
  }
  else if (txBody.has_filedelete())
  {
    schedulableTxBody->set_allocated_filedelete(txBody.release_filedelete());
  }
  else if (txBody.has_fileupdate())
  {
    schedulableTxBody->set_allocated_fileupdate(txBody.release_fileupdate());
  }
  else if (txBody.has_freeze())
  {
    schedulableTxBody->set_allocated_freeze(txBody.release_freeze());
  }
  else if (txBody.has_nodecreate())
  {
    schedulableTxBody->set_allocated_nodecreate(txBody.release_nodecreate());
  }
  else if (txBody.has_nodedelete())
  {
    schedulableTxBody->set_allocated_nodedelete(txBody.release_nodedelete());
  }
  else if (txBody.has_nodeupdate())
  {
    schedulableTxBody->set_allocated_nodeupdate(txBody.release_nodeupdate());
  }
  else if (txBody.has_util_prng())
  {
    schedulableTxBody->set_allocated_util_prng(txBody.release_util_prng());
  }
  else if (txBody.has_scheduledelete())
  {
    schedulableTxBody->set_allocated_scheduledelete(txBody.release_scheduledelete());
  }
  else if (txBody.has_systemdelete())
  {
    schedulableTxBody->set_allocated_systemdelete(txBody.release_systemdelete());
  }
  else if (txBody.has_systemundelete())
  {
    schedulableTxBody->set_allocated_systemundelete(txBody.release_systemundelete());
  }
  else if (txBody.has_tokenairdrop())
  {
    schedulableTxBody->set_allocated_tokenairdrop(txBody.release_tokenairdrop());
  }
  else if (txBody.has_tokenassociate())
  {
    schedulableTxBody->set_allocated_tokenassociate(txBody.release_tokenassociate());
  }
  else if (txBody.has_tokenburn())
  {
    schedulableTxBody->set_allocated_tokenburn(txBody.release_tokenburn());
  }
  else if (txBody.has_tokencancelairdrop())
  {
    schedulableTxBody->set_allocated_tokencancelairdrop(txBody.release_tokencancelairdrop());
  }
  else if (txBody.has_tokenclaimairdrop())
  {
    schedulableTxBody->set_allocated_tokenclaimairdrop(txBody.release_tokenclaimairdrop());
  }
  else if (txBody.has_tokencreation())
  {
    schedulableTxBody->set_allocated_tokencreation(txBody.release_tokencreation());
  }
  else if (txBody.has_tokendeletion())
  {
    schedulableTxBody->set_allocated_tokendeletion(txBody.release_tokendeletion());
  }
  else if (txBody.has_tokendissociate())
  {
    schedulableTxBody->set_allocated_tokendissociate(txBody.release_tokendissociate());
  }
  else if (txBody.has_token_fee_schedule_update())
  {
    schedulableTxBody->set_allocated_token_fee_schedule_update(txBody.release_token_fee_schedule_update());
  }
  else if (txBody.has_tokenfreeze())
  {
    schedulableTxBody->set_allocated_tokenfreeze(txBody.release_tokenfreeze());
  }
  else if (txBody.has_tokengrantkyc())
  {
    schedulableTxBody->set_allocated_tokengrantkyc(txBody.release_tokengrantkyc());
  }
  else if (txBody.has_tokenmint())
  {
    schedulableTxBody->set_allocated_tokenmint(txBody.release_tokenmint());
  }
  else if (txBody.has_token_pause())
  {
    schedulableTxBody->set_allocated_token_pause(txBody.release_token_pause());
  }
  else if (txBody.has_tokenrevokekyc())
  {
    schedulableTxBody->set_allocated_tokenrevokekyc(txBody.release_tokenrevokekyc());
  }
  else if (txBody.has_tokenunfreeze())
  {
    schedulableTxBody->set_allocated_tokenunfreeze(txBody.release_tokenunfreeze());
  }
  else if (txBody.has_token_unpause())
  {
    schedulableTxBody->set_allocated_token_unpause(txBody.release_token_unpause());
  }
  else if (txBody.has_tokenwipe())
  {
    schedulableTxBody->set_allocated_tokenwipe(txBody.release_tokenwipe());
  }
  else if (txBody.has_consensuscreatetopic())
  {
    schedulableTxBody->set_allocated_consensuscreatetopic(txBody.release_consensuscreatetopic());
  }
  else if (txBody.has_consensusdeletetopic())
  {
    schedulableTxBody->set_allocated_consensusdeletetopic(txBody.release_consensusdeletetopic());
  }
  else if (txBody.has_consensussubmitmessage())
  {
    schedulableTxBody->set_allocated_consensussubmitmessage(txBody.release_consensussubmitmessage());
  }
  else if (txBody.has_consensusupdatetopic())
  {
    schedulableTxBody->set_allocated_consensusupdatetopic(txBody.release_consensusupdatetopic());
  }
  else if (txBody.has_cryptotransfer())
  {
    schedulableTxBody->set_allocated_cryptotransfer(txBody.release_cryptotransfer());
  }
  else
  {
    throw UninitializedException("WrappedTransaction doesn't contain a valid schedulable Transaction");
  }

  return schedulableTxBody;
}

//-----
WrappedTransaction& WrappedTransaction::setTransaction(const AnyPossibleTransaction& transaction)
{
  mTransaction = transaction;
  return *this;
}

} // namespace Hiero
