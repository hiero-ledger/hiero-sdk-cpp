// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CUSTOM_FEE_SERIALIZER_H_
#define HIERO_TCK_CPP_CUSTOM_FEE_SERIALIZER_H_

#include "json/JsonRpcException.h"

#include <AccountId.h>
#include <CustomFee.h>
#include <CustomFeeBase.h>
#include <CustomFixedFee.h>
#include <CustomFractionalFee.h>
#include <CustomRoyaltyFee.h>
#include <impl/EntityIdHelper.h>

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CustomFee arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<std::shared_ptr<Hiero::CustomFee>>
{
  /**
   * Convert a CustomFee to a JSON object.
   *
   * @param jsonTo The JSON object to fill with the CustomFee.
   * @param fee    The CustomFee with which to fill the JSON object.
   */
  static void to_json(json& jsonTo, const std::shared_ptr<Hiero::CustomFee>& fee)
  {
    jsonTo["feeCollectorAccountId"] = fee->getFeeCollectorAccountId().toString();
    jsonTo["feeCollectorsExempt"] = fee->getAllCollectorsAreExempt();

    if (const std::shared_ptr<Hiero::CustomFixedFee> fixedFee = std::dynamic_pointer_cast<Hiero::CustomFixedFee>(fee);
        fixedFee)
    {
      jsonTo["fixedFee"] = {
        { "amount", fixedFee->getAmount() }
      };
      if (fixedFee->getDenominatingTokenId().has_value())
      {
        jsonTo["fixedFee"]["denominatingTokenId"] = fixedFee->getDenominatingTokenId()->toString();
      }
    }

    else if (const std::shared_ptr<Hiero::CustomFractionalFee> fractionalFee =
               std::dynamic_pointer_cast<Hiero::CustomFractionalFee>(fee);
             fractionalFee)
    {
      std::string assessmentMethod = Hiero::gFeeAssessmentMethodToString.at(fractionalFee->getAssessmentMethod());
      std::transform(assessmentMethod.begin(),
                     assessmentMethod.end(),
                     assessmentMethod.begin(),
                     [](unsigned char c) { return std::tolower(c); });
      jsonTo["fractionalFee"] = {
        { "numerator",        fractionalFee->getNumerator()     },
        { "denominator",      fractionalFee->getDenominator()   },
        { "minimumAmount",    fractionalFee->getMinimumAmount() },
        { "maximumAmount",    fractionalFee->getMaximumAmount() },
        { "assessmentMethod", assessmentMethod                  }
      };
    }

    else
    {
      const std::shared_ptr<Hiero::CustomRoyaltyFee> royaltyFee =
        std::dynamic_pointer_cast<Hiero::CustomRoyaltyFee>(fee);

      jsonTo["royaltyFee"] = {
        { "numerator",   royaltyFee->getNumerator()   },
        { "denominator", royaltyFee->getDenominator() }
      };

      if (royaltyFee->getFallbackFee().has_value())
      {
        jsonTo["royaltyFee"]["fallbackFee"] = {
          { "amount", royaltyFee->getFallbackFee()->getAmount() }
        };
        if (royaltyFee->getFallbackFee()->getDenominatingTokenId().has_value())
        {
          jsonTo["royaltyFee"]["fallbackFee"]["denominatingTokenId"] =
            royaltyFee->getFallbackFee()->getDenominatingTokenId()->toString();
        }
      }
    }
  }

  /**
   * Convert a JSON object to a CustomFee.
   *
   * @param jsonFrom The JSON object with which to fill the CustomFee.
   * @param fee      The CustomFee to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, std::shared_ptr<Hiero::CustomFee>& fee)
  {
    if (!jsonFrom.contains("feeCollectorAccountId"))
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: feeCollectorAccountId is REQUIRED.");
    }

    if (!jsonFrom["feeCollectorAccountId"].is_string())
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: feeCollectorAccountId MUST be a string.");
    }

    const Hiero::AccountId feeCollectorAccountId =
      Hiero::AccountId::fromString(jsonFrom["feeCollectorAccountId"].get<std::string>());

    if (!jsonFrom.contains("feeCollectorsExempt"))
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: feeCollectorsExempt is REQUIRED.");
    }

    if (!jsonFrom["feeCollectorsExempt"].is_boolean())
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: feeCollectorsExempt MUST be a boolean.");
    }

    const bool feeCollectorsExempt = jsonFrom["feeCollectorsExempt"].get<bool>();

    if (jsonFrom.contains("fixedFee"))
    {
      auto fixedFee = std::make_shared<Hiero::CustomFixedFee>();
      fixedFee->setFeeCollectorAccountId(feeCollectorAccountId);
      fixedFee->setAllCollectorsAreExempt(feeCollectorsExempt);

      if (!jsonFrom["fixedFee"].contains("amount"))
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: amount is REQUIRED for fixedFee fee types.");
      }

      if (!jsonFrom["fixedFee"]["amount"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: amount MUST be a string.");
      }

      try
      {
        fixedFee->setAmount(
          Hiero::internal::EntityIdHelper::getNum<int64_t>(jsonFrom["fixedFee"]["amount"].get<std::string>()));
      }
      catch (const std::invalid_argument&)
      {
        fixedFee->setAmount(Hiero::internal::EntityIdHelper::getNum(jsonFrom["fixedFee"]["amount"].get<std::string>()));
      }

      if (jsonFrom["fixedFee"].contains("denominatingTokenId"))
      {
        if (!jsonFrom["fixedFee"]["denominatingTokenId"].is_string())
        {
          throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                             "invalid parameters: denominatingTokenId MUST be a string.");
        }

        fixedFee->setDenominatingTokenId(
          Hiero::TokenId::fromString(jsonFrom["fixedFee"]["denominatingTokenId"].get<std::string>()));
      }

      fee = fixedFee;
    }

    else if (jsonFrom.contains("fractionalFee"))
    {
      auto fractionalFee = std::make_shared<Hiero::CustomFractionalFee>();
      fractionalFee->setFeeCollectorAccountId(feeCollectorAccountId);
      fractionalFee->setAllCollectorsAreExempt(feeCollectorsExempt);

      if (!jsonFrom["fractionalFee"].contains("numerator"))
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: numerator is REQUIRED for fractionalFee fee types.");
      }

      if (!jsonFrom["fractionalFee"]["numerator"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: numerator MUST be a string.");
      }

      try
      {
        fractionalFee->setNumerator(
          Hiero::internal::EntityIdHelper::getNum<int64_t>(jsonFrom["fractionalFee"]["numerator"].get<std::string>()));
      }
      catch (const std::invalid_argument&)
      {
        fractionalFee->setNumerator(
          Hiero::internal::EntityIdHelper::getNum(jsonFrom["fractionalFee"]["numerator"].get<std::string>()));
      }

      if (!jsonFrom["fractionalFee"].contains("denominator"))
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: denominator is REQUIRED for fractionalFee fee types.");
      }

      if (!jsonFrom["fractionalFee"]["denominator"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: denominator MUST be a string.");
      }

      try
      {
        fractionalFee->setDenominator(Hiero::internal::EntityIdHelper::getNum<int64_t>(
          jsonFrom["fractionalFee"]["denominator"].get<std::string>()));
      }
      catch (const std::invalid_argument&)
      {
        fractionalFee->setDenominator(
          Hiero::internal::EntityIdHelper::getNum(jsonFrom["fractionalFee"]["denominator"].get<std::string>()));
      }

      if (!jsonFrom["fractionalFee"].contains("minimumAmount"))
      {
        throw Hiero::TCK::JsonRpcException(
          Hiero::TCK::JsonErrorType::INVALID_PARAMS,
          "invalid parameters: minimumAmount is REQUIRED for fractionalFee fee types.");
      }

      if (!jsonFrom["fractionalFee"]["minimumAmount"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: minimumAmount MUST be a string.");
      }

      try
      {
        fractionalFee->setMinimumAmount(Hiero::internal::EntityIdHelper::getNum<int64_t>(
          jsonFrom["fractionalFee"]["minimumAmount"].get<std::string>()));
      }
      catch (const std::invalid_argument&)
      {
        fractionalFee->setMinimumAmount(
          Hiero::internal::EntityIdHelper::getNum(jsonFrom["fractionalFee"]["minimumAmount"].get<std::string>()));
      }

      if (!jsonFrom["fractionalFee"].contains("maximumAmount"))
      {
        throw Hiero::TCK::JsonRpcException(
          Hiero::TCK::JsonErrorType::INVALID_PARAMS,
          "invalid parameters: maximumAmount is REQUIRED for fractionalFee fee types.");
      }

      if (!jsonFrom["fractionalFee"]["maximumAmount"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: maximumAmount MUST be a string.");
      }

      try
      {
        fractionalFee->setMaximumAmount(Hiero::internal::EntityIdHelper::getNum<int64_t>(
          jsonFrom["fractionalFee"]["maximumAmount"].get<std::string>()));
      }
      catch (const std::invalid_argument&)
      {
        fractionalFee->setMaximumAmount(
          Hiero::internal::EntityIdHelper::getNum(jsonFrom["fractionalFee"]["maximumAmount"].get<std::string>()));
      }

      if (!jsonFrom["fractionalFee"].contains("assessmentMethod"))
      {
        throw Hiero::TCK::JsonRpcException(
          Hiero::TCK::JsonErrorType::INVALID_PARAMS,
          "invalid parameters: assessmentMethod is REQUIRED for fractionalFee fee types.");
      }

      if (!jsonFrom["fractionalFee"]["assessmentMethod"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: assessmentMethod MUST be a string.");
      }

      std::string assessmentMethod = jsonFrom["fractionalFee"]["assessmentMethod"].get<std::string>();
      std::transform(assessmentMethod.begin(),
                     assessmentMethod.end(),
                     assessmentMethod.begin(),
                     [](unsigned char c) { return std::toupper(c); });
      fractionalFee->setAssessmentMethod(Hiero::gStringToFeeAssessmentMethod.at(assessmentMethod));

      fee = fractionalFee;
    }

    else if (jsonFrom.contains("royaltyFee"))
    {
      auto royaltyFee = std::make_shared<Hiero::CustomRoyaltyFee>();
      royaltyFee->setFeeCollectorAccountId(feeCollectorAccountId);
      royaltyFee->setAllCollectorsAreExempt(feeCollectorsExempt);

      if (!jsonFrom["royaltyFee"].contains("numerator"))
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: numerator is REQUIRED for royaltyFee fee types.");
      }

      if (!jsonFrom["royaltyFee"]["numerator"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: numerator MUST be a string.");
      }

      try
      {
        royaltyFee->setNumerator(
          Hiero::internal::EntityIdHelper::getNum<int64_t>(jsonFrom["royaltyFee"]["numerator"].get<std::string>()));
      }
      catch (const std::invalid_argument&)
      {
        royaltyFee->setNumerator(
          Hiero::internal::EntityIdHelper::getNum(jsonFrom["royaltyFee"]["numerator"].get<std::string>()));
      }

      if (!jsonFrom["royaltyFee"].contains("denominator"))
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: denominator is REQUIRED for royaltyFee fee types.");
      }

      if (!jsonFrom["royaltyFee"]["denominator"].is_string())
      {
        throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                           "invalid parameters: denominator MUST be a string.");
      }

      try
      {
        royaltyFee->setDenominator(
          Hiero::internal::EntityIdHelper::getNum<int64_t>(jsonFrom["royaltyFee"]["denominator"].get<std::string>()));
      }
      catch (const std::invalid_argument&)
      {
        royaltyFee->setDenominator(
          Hiero::internal::EntityIdHelper::getNum(jsonFrom["royaltyFee"]["denominator"].get<std::string>()));
      }

      if (jsonFrom["royaltyFee"].contains("fallbackFee"))
      {
        Hiero::CustomFixedFee fallbackFee;

        if (!jsonFrom["royaltyFee"]["fallbackFee"].contains("amount"))
        {
          throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                             "invalid parameters: amount is REQUIRED for a fallback fee.");
        }

        if (!jsonFrom["royaltyFee"]["fallbackFee"]["amount"].is_string())
        {
          throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                             "invalid parameters: amount MUST be a string.");
        }

        try
        {
          fallbackFee.setAmount(Hiero::internal::EntityIdHelper::getNum(
            jsonFrom["royaltyFee"]["fallbackFee"]["amount"].get<std::string>()));
        }
        catch (const std::invalid_argument&)
        {
          fallbackFee.setAmount(Hiero::internal::EntityIdHelper::getNum<int64_t>(
            jsonFrom["royaltyFee"]["fallbackFee"]["amount"].get<std::string>()));
        }

        if (jsonFrom["royaltyFee"]["fallbackFee"].contains("denominatingTokenId"))
        {
          if (!jsonFrom["royaltyFee"]["fallbackFee"]["denominatingTokenId"].is_string())
          {
            throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                               "invalid parameters: denominatingTokenId MUST be a string.");
          }

          fallbackFee.setDenominatingTokenId(Hiero::TokenId::fromString(
            jsonFrom["royaltyFee"]["fallbackFee"]["denominatingTokenId"].get<std::string>()));
        }

        royaltyFee->setFallbackFee(fallbackFee);
      }

      fee = royaltyFee;
    }

    else
    {
      throw Hiero::TCK::JsonRpcException(Hiero::TCK::JsonErrorType::INVALID_PARAMS,
                                         "invalid parameters: fee MUST contain one of fixedFee, fractionalFee, or "
                                         "royaltyFee.");
    }
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CUSTOM_FEE_PARAMS_H_
