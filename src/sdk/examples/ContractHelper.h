// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EXAMPLES_CONTRACT_HELPER_H_
#define HIERO_SDK_CPP_EXAMPLES_CONTRACT_HELPER_H_

#include "AccountId.h"
#include "Client.h"
#include "ContractCreateFlow.h"
#include "ContractExecuteTransaction.h"
#include "ContractFunctionParameters.h"
#include "ContractFunctionResult.h"
#include "ContractId.h"
#include "Hbar.h"
#include "PrivateKey.h"
#include "Status.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "impl/HexConverter.h"

#include <basic_types.pb.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Hiero
{
/**
 * ContractHelper de-clutters SolidityPrecompileExample.
 *
 * When we instantiate a ContractHelper, we provide it with the JSON of a compiled solidity contract which is assumed to
 * have functions named "step0()" through "stepN()".
 *
 * Each of these step functions is assumed to take no function parameters, and to return a Hiero ResponseCode which
 * ought to be SUCCESS -- in other words, an int32 with value 22. See
 * examples/precompile-example/HederaResponseCodes.sol.
 *
 * If a step takes function parameters, or if its ContractFunctionResult should be validated with a different method,
 * the user can specify a supplier for a particular step with setParameterSupplier(stepIndex, parametersSupplier), and
 * can specify an alternative validation method with setResultValidator(stepIndex, validateFunction)
 *
 * The contract is created on the Hiero network in the ContractHelper constructor, and when the user is ready to
 * execute the step functions in the contract, they should call executeSteps(firstStepToExecute, lastStepToExecute).
 */
class ContractHelper
{
public:
  /**
   * Initialize this ContractHelper with the JSON file that contains the contract bytecode, the constructor parameters,
   * and the Client to use to create the contract.
   *
   * @param filename              The JSON file that contains the contract bytecode.
   * @param constructorParameters The constructor parameters for the contract.
   * @param client                The Client to use the create the contract.
   */
  ContractHelper(std::string_view filename,
                 const ContractFunctionParameters& constructorParameters,
                 const Client& client)
    : mContractId(ContractCreateFlow()
                    .setBytecode(getBytecodeHex(filename))
                    .setMaxChunks(30U)
                    .setGas(8'000'000)
                    .setConstructorParameters(constructorParameters)
                    .execute(client)
                    .getReceipt(client)
                    .mContractId.value())
  {
  }

  /**
   * Add a result validator for a step.
   *
   * @param step The step the validator should take effect.
   * @param func The validator function to run.
   * @return A reference to this ContractHelper with the newly-added step result validator.
   */
  ContractHelper& setResultValidatorForStep(int step, const std::function<bool(const ContractFunctionResult&)>& func)
  {
    mStepResultValidators.try_emplace(step, func);
    return *this;
  }

  /**
   * Add a parameter supplier for a step.
   *
   * @param step The step the parameter should be supplied.
   * @param func The parameter supplier to run.
   * @return A reference to this ContractHelper with the newly-added step parameter supplier.
   */
  ContractHelper& setParameterSupplierForStep(int step, const std::function<ContractFunctionParameters(void)>& func)
  {
    mStepParameterSupplier.try_emplace(step, func);
    return *this;
  }

  /**
   * Add a payable amount for a step.
   *
   * @param step   The step to which the payable amount should be added.
   * @param amount The payable amount.
   * @return A reference to this ContractHelper with the newly-added payable amount.
   */
  ContractHelper& setPayableAmountForStep(int step, const Hbar& amount)
  {
    mStepPayableAmounts.try_emplace(step, amount);
    return *this;
  }

  /**
   * Add a signer for a step.
   *
   * @param step The step to which the signer should sign.
   * @param key  The PrivateKey with which to sign.
   * @return A reference to this ContractHelper with the newly-added signer key.
   */
  ContractHelper& addSignerForStep(int step, const std::shared_ptr<PrivateKey>& key)
  {
    mStepSigners[step].push_back(key);
    return *this;
  }

  /**
   * Add a fee payer for a step.
   *
   * @param step      The step to which the fee payer should be added.
   * @param accountId The ID of the account paying.
   * @param key       The private key of the fee payer account.
   * @return A reference to this ContractHelper with the newly-added payable amount.
   */
  ContractHelper& setFeePayerForStep(int step, const AccountId& accountId, const std::shared_ptr<PrivateKey>& key)
  {
    mStepFeePayers.try_emplace(step, accountId);
    return addSignerForStep(step, key);
  }

  /**
   * Execute all steps.
   *
   * @param first  The first step to execute.
   * @param last   The last step to execute.
   * @param client The Client to use to execute the steps.
   */
  ContractHelper& executeSteps(int first, int last, const Client& client)
  {
    for (int step = first; step < last + 1; ++step)
    {
      std::cout << "Attempting to execute step " << step << std::endl;

      ContractExecuteTransaction transaction =
        ContractExecuteTransaction().setContractId(mContractId).setGas(10'000'000);

      const bool hasPayableAmount = mStepPayableAmounts.find(step) != mStepPayableAmounts.end();
      const bool hasParameters = mStepParameterSupplier.find(step) != mStepParameterSupplier.end();
      const bool hasFeePayer = mStepFeePayers.find(step) != mStepFeePayers.end();
      const bool hasSigners = mStepSigners.find(step) != mStepSigners.end();
      const bool hasResultValidator = mStepResultValidators.find(step) != mStepResultValidators.end();

      if (hasPayableAmount)
      {
        transaction.setPayableAmount(mStepPayableAmounts.at(step));
      }

      transaction.setFunction("step" + std::to_string(step),
                              hasParameters ? mStepParameterSupplier.at(step)() : ContractFunctionParameters());

      if (hasFeePayer)
      {
        transaction.setTransactionId(TransactionId::generate(mStepFeePayers.at(step)));
      }

      transaction.freezeWith(&client);
      if (hasSigners)
      {
        std::for_each(mStepSigners.at(step).cbegin(),
                      mStepSigners.at(step).cend(),
                      [&transaction](const std::shared_ptr<PrivateKey>& key) { transaction.sign(key); });
      }

      const TransactionRecord txRecord = transaction.execute(client).setValidateStatus(false).getRecord(client);

      const ContractFunctionResult result = txRecord.mContractFunctionResult.value();
      const std::function<bool(const ContractFunctionResult&)> validatorFunc =
        hasResultValidator ? mStepResultValidators.at(step)
                           : [](const ContractFunctionResult& stepResult)
      {
        // Assume no custom validator means the function should return a success.
        const Status status =
          gProtobufResponseCodeToStatus.at(static_cast<proto::ResponseCodeEnum>(stepResult.getInt32(0)));
        return status == Status::SUCCESS;
      };

      if (validatorFunc(result))
      {
        std::cout << "Step " << step
                  << " completed and returned a valid result. TransactionId=" << txRecord.mTransactionId->toString()
                  << std::endl;
      }
      else
      {
        std::cout << "Encountered invalid response status" << std::endl;
      }
    }

    return *this;
  }

private:
  /**
   * Get the hex-encoded bytecode in the input JSON file.
   *
   * @param file The JSON file from which to grab the bytecode.
   * @return The bytecode encoded into a hex string.
   */
  static std::string getBytecodeHex(std::string_view file)
  {
    std::ifstream infile(file.data());
    nlohmann::json json = nlohmann::json::parse(infile);

    if (json.contains("object") && json["object"].is_string())
    {
      return json["object"].get<std::string>();
    }
    else
    {
      return json["bytecode"].get<std::string>();
    }
  }

  /**
   * The ID of the created contract.
   */
  ContractId mContractId;

  /**
   * Map of steps to the step's validator function.
   */
  std::unordered_map<int, std::function<bool(const ContractFunctionResult&)>> mStepResultValidators;

  /**
   * Map of steps to the step's parameter supplier.
   */
  std::unordered_map<int, std::function<ContractFunctionParameters(void)>> mStepParameterSupplier;

  /**
   * Map of steps to the step's payable amount.
   */
  std::unordered_map<int, Hbar> mStepPayableAmounts;

  /**
   * Map of steps to the step's signers.
   */
  std::unordered_map<int, std::vector<std::shared_ptr<PrivateKey>>> mStepSigners;

  /**
   * Map of steps to the ID of the fee payer account.
   */
  std::unordered_map<int, AccountId> mStepFeePayers;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EXAMPLES_CONTRACT_HELPER_H_
