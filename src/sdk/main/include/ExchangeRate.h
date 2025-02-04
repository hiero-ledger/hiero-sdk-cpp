// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EXCHANGE_RATE_H_
#define HIERO_SDK_CPP_EXCHANGE_RATE_H_

#include <chrono>
#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class ExchangeRate;
}

namespace Hiero
{
/**
 * An exchange rate between hbar and cents (USD) and the time at which the exchange rate will expire, and be superseded
 * by a new exchange rate.
 */
class ExchangeRate
{
public:
  ExchangeRate() = default;

  /**
   * Construct from an Hbar amount, cents amount, and an expiration time.
   *
   * @param hbar  The amount of Hbar.
   * @param cents The amount of USD (in cents).
   * @param expirationTime The time this ExchangeRate expires.
   */
  ExchangeRate(int hbar, int cents, const std::chrono::system_clock::time_point& expirationTime);

  /**
   * Construct an ExchangeRate object from an ExchangeRate protobuf object.
   *
   * @param proto The ExchangeRate protobuf object from which to construct an ExchangeRate object.
   * @return The constructed ExchangeRate object.
   */
  [[nodiscard]] static ExchangeRate fromProtobuf(const proto::ExchangeRate& proto);

  /**
   * Construct an ExchangeRate object from a byte array.
   *
   * @param bytes The byte array from which to construct an ExchangeRate object.
   * @return The constructed ExchangeRate object.
   */
  [[nodiscard]] static ExchangeRate fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct an ExchangeRate protobuf object from this ExchangeRate object.
   *
   * @return A pointer to the created ExchangeRate protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::ExchangeRate> toProtobuf() const;

  /**
   * Construct a representative byte array from this ExchangeRate object.
   *
   * @return The byte array representing this ExchangeRate object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this ExchangeRate object.
   *
   * @return The string representation of this ExchangeRate object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this ExchangeRate to an output stream.
   *
   * @param os   The output stream.
   * @param rate The ExchangeRate to print.
   * @return The output stream with this ExchangeRate written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const ExchangeRate& rate);

  /**
   * Denotes Hbar equivalent to cents.
   */
  int mHbars = 0;

  /**
   * Denotes cents equivalent to Hbar.
   */
  int mCents = 0;

  /**
   * Expiration time of this ExchangeRate.
   */
  std::chrono::system_clock::time_point mExpirationTime;

  /**
   * Calculated exchange rate.
   */
  double mExchangeRateInCents = 0.0;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EXCHANGE_RATE_H_
