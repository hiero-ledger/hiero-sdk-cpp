// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_COMMON_TRANSACTION_PARAMS_H_
#define HIERO_TCK_CPP_COMMON_TRANSACTION_PARAMS_H_

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

namespace Hiero::TCK
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

} // namespace Hiero::TCK

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CommonTransactionParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::CommonTransactionParams>
{
  /**
   * Convert a JSON object to a CommonTransactionParams.
   *
   * @param jsonFrom The JSON object with which to fill the CommonTransactionParams.
   * @param params   The CommonTransactionParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::CommonTransactionParams& params)
  {
    params.mTransactionId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "transactionId");
    params.mMaxTransactionFee = Hiero::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "maxTransactionFee");
    params.mValidTransactionDuration =
      Hiero::TCK::getOptionalJsonParameter<int64_t>(jsonFrom, "validTransactionDuration");
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");
    params.mRegenerateTransactionId = Hiero::TCK::getOptionalJsonParameter<bool>(jsonFrom, "regenerateTransactionId");
    params.mSigners = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "signers");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_COMMON_TRANSACTION_PARAMS_H_
