/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef HEDERA_TCK_CPP_COMMON_TRANSACTION_PARAMS_H_
#define HEDERA_TCK_CPP_COMMON_TRANSACTION_PARAMS_H_

#include "json/JsonUtils.h"

#include <Client.h>
#include <Hbar.h>
#include <PrivateKey.h>
#include <Transaction.h>
#include <TransactionId.h>

#include <chrono>
#include <cstdint>
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Hedera::TCK
{
/**
 * Struct the contains the common parameters of a Transaction.
 */
struct CommonTransactionParams
{
  /**
   * Function that fills a Transaction with the parameters present in this CommonTransactionParams.
   *
   * @tparam T           The type of Transaction being filled out.
   * @param  transaction The Transaction to fill out.
   * @param  client      The Client being used to submit this Transaction.
   */
  template<typename T>
  void fillOutTransaction(Transaction<T>& transaction, const Client& client) const
  {
    if (mTransactionId.has_value())
    {
      transaction.setTransactionId(TransactionId::fromString(mTransactionId.value()));
    }

    if (mMaxTransactionFee.has_value())
    {
      transaction.setMaxTransactionFee(Hbar::fromTinybars(mMaxTransactionFee.value()));
    }

    if (mValidTransactionDuration.has_value())
    {
      transaction.setValidTransactionDuration(std::chrono::seconds(mValidTransactionDuration.value()));
    }

    if (mMemo.has_value())
    {
      transaction.setTransactionMemo(mMemo.value());
    }

    if (mRegenerateTransactionId.has_value())
    {
      transaction.setRegenerateTransactionIdPolicy(mRegenerateTransactionId.value());
    }

    if (mSigners.has_value())
    {
      transaction.freezeWith(&client);
      std::for_each(mSigners->cbegin(),
                    mSigners->cend(),
                    [&transaction](const std::string& key) { transaction.sign(PrivateKey::fromStringDer(key)); });
    }
  }

  /**
   * The ID of the transaction.
   */
  std::optional<std::string> mTransactionId;

  /**
   * The maximum amount willing to be paid to execute the transaction.
   */
  std::optional<int64_t> mMaxTransactionFee;

  /**
   * The length of time for which the transaction is valid.
   */
  std::optional<int64_t> mValidTransactionDuration;

  /**
   * The memo of the transaction.
   */
  std::optional<std::string> mMemo;

  /**
   * If expired, should the transaction regenerate it's transaction ID?
   */
  std::optional<bool> mRegenerateTransactionId;

  /**
   * The list of DER-encoded hex strings of private keys to sign the transaction.
   */
  std::optional<std::vector<std::string>> mSigners;
};

} // namespace Hedera::TCK

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CommonTransactionParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hedera::TCK::CommonTransactionParams>
{
  /**
   * Convert a JSON object to a CommonTransactionParams.
   *
   * @param jsonFrom The JSON object with which to fill the CommonTransactionParams.
   * @param params   The CommonTransactionParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hedera::TCK::CommonTransactionParams& params)
  {
    params.mTransactionId = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "transactionId");
    params.mMaxTransactionFee = Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "maxTransactionFee");
    params.mValidTransactionDuration =
      Hedera::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "validTransactionDuration");
    params.mMemo = Hedera::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mRegenerateTransactionId = Hedera::TCK::getOptionalJsonParameter<bool>(jsonFrom, "regenerateTransactionId");
    params.mSigners = Hedera::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "signers");
  }
};

} // namespace nlohmann

#endif // HEDERA_TCK_CPP_COMMON_TRANSACTION_PARAMS_H_
