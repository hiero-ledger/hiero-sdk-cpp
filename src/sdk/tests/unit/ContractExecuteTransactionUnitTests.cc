// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "ContractExecuteTransaction.h"
#include "ContractId.h"
#include "ED25519PrivateKey.h"
#include "exceptions/IllegalStateException.h"
#include "impl/Utilities.h"

#include <cstddef>
#include <gtest/gtest.h>
#include <transaction_body.pb.h>
#include <vector>

using namespace Hiero;

class ContractExecuteTransactionUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const ContractId& getTestContractId() const { return mTestContractId; }
  [[nodiscard]] inline const uint64_t& getTestGas() const { return mTestGas; }
  [[nodiscard]] inline const Hbar& getTestPayableAmount() const { return mTestPayableAmount; }
  [[nodiscard]] inline const std::vector<std::byte>& getTestFunctionParameters() const
  {
    return mTestFunctionParameters;
  }

private:
  const ContractId mTestContractId = ContractId(1ULL);
  const uint64_t mTestGas = 2ULL;
  const Hbar mTestPayableAmount = Hbar(3LL);
  const std::vector<std::byte> mTestFunctionParameters = { std::byte(0x04), std::byte(0x05), std::byte(0x06) };
};

//-----
TEST_F(ContractExecuteTransactionUnitTests, ConstructContractExecuteTransactionFromTransactionBodyProtobuf)
{
  // Given
  auto body = std::make_unique<proto::ContractCallTransactionBody>();
  body->set_allocated_contractid(getTestContractId().toProtobuf().release());
  body->set_gas(static_cast<int64_t>(getTestGas()));
  body->set_amount(getTestPayableAmount().toTinybars());
  body->set_functionparameters(internal::Utilities::byteVectorToString(getTestFunctionParameters()));

  proto::TransactionBody txBody;
  txBody.set_allocated_contractcall(body.release());

  // When
  const ContractExecuteTransaction contractExecuteTransaction(txBody);

  // Then
  EXPECT_EQ(contractExecuteTransaction.getContractId(), getTestContractId());
  EXPECT_EQ(contractExecuteTransaction.getGas(), getTestGas());
  EXPECT_EQ(contractExecuteTransaction.getPayableAmount(), getTestPayableAmount());
  EXPECT_EQ(contractExecuteTransaction.getFunctionParameters(), getTestFunctionParameters());
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetContractId)
{
  // Given
  ContractExecuteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setContractId(getTestContractId()));

  // Then
  EXPECT_EQ(transaction.getContractId(), getTestContractId());
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetContractIdFrozen)
{
  // Given
  ContractExecuteTransaction transaction = ContractExecuteTransaction()
                                             .setNodeAccountIds({ AccountId(1ULL) })
                                             .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setContractId(getTestContractId()), IllegalStateException);
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetGas)
{
  // Given
  ContractExecuteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setGas(getTestGas()));

  // Then
  EXPECT_EQ(transaction.getGas(), getTestGas());
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetGasFrozen)
{
  // Given
  ContractExecuteTransaction transaction = ContractExecuteTransaction()
                                             .setNodeAccountIds({ AccountId(1ULL) })
                                             .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setGas(getTestGas()), IllegalStateException);
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetPayableAmount)
{
  // Given
  ContractExecuteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setPayableAmount(getTestPayableAmount()));

  // Then
  EXPECT_EQ(transaction.getPayableAmount(), getTestPayableAmount());
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetPayableAmountFrozen)
{
  // Given
  ContractExecuteTransaction transaction = ContractExecuteTransaction()
                                             .setNodeAccountIds({ AccountId(1ULL) })
                                             .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setPayableAmount(getTestPayableAmount()), IllegalStateException);
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetFunctionParameters)
{
  // Given
  ContractExecuteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFunctionParameters(getTestFunctionParameters()));

  // Then
  EXPECT_EQ(transaction.getFunctionParameters(), getTestFunctionParameters());
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetFunctionParametersFrozen)
{
  // Given
  ContractExecuteTransaction transaction = ContractExecuteTransaction()
                                             .setNodeAccountIds({ AccountId(1ULL) })
                                             .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFunctionParameters(getTestFunctionParameters()), IllegalStateException);
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetFunctionName)
{
  // Given
  ContractExecuteTransaction transaction;

  // When
  EXPECT_NO_THROW(transaction.setFunction("functionName"));

  // Then
  EXPECT_EQ(transaction.getFunctionParameters().size(), 4);
}

//-----
TEST_F(ContractExecuteTransactionUnitTests, GetSetFunctionNameFrozen)
{
  // Given
  ContractExecuteTransaction transaction = ContractExecuteTransaction()
                                             .setNodeAccountIds({ AccountId(1ULL) })
                                             .setTransactionId(TransactionId::generate(AccountId(1ULL)));
  ASSERT_NO_THROW(transaction.freeze());

  // When / Then
  EXPECT_THROW(transaction.setFunction("functionName"), IllegalStateException);
}
