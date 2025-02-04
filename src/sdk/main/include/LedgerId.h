// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_LEDGER_ID_H_
#define HIERO_SDK_CPP_LEDGER_ID_H_

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace Hiero
{
class LedgerId
{
public:
  LedgerId() = default;

  /**
   * The Hiero Mainnet ledger ID.
   */
  static const LedgerId MAINNET;

  /**
   * The Hiero Testnet ledger ID.
   */
  static const LedgerId TESTNET;

  /**
   * The Hiero Previewnet ledger ID.
   */
  static const LedgerId PREVIEWNET;

  /**
   * Construct a LedgerId from a byte array.
   *
   * @param bytes The bytes from which to construct this LedgerId.
   */
  explicit LedgerId(std::vector<std::byte> bytes);

  /**
   * Compare this LedgerId to another LedgerId and determine if they represent the same ledger.
   *
   * @param other The other LedgerId with which to compare this LedgerId.
   * @return \c TRUE if this LedgerId is the same as the input LedgerId, otherwise \c FALSE.
   */
  bool operator==(const LedgerId& other) const;

  /**
   * Construct a LedgerId object from a string. The string can be either hex-encoded bytes, or "mainnet", "testnet", or
   * "previewnet".
   *
   * @param id The ledger ID string from which to construct.
   * @return The constructed LedgerId object.
   * @throws std::invalid_argument If a LedgerId cannot be realized from the input hex-encoded string.
   */
  [[nodiscard]] static LedgerId fromString(std::string_view id);

  /**
   * Get the string representation of this LedgerId. This will either return "mainnet", "testnet", "previewnet", or the
   * hex-encoded bytes.
   *
   * @return The string representation of this LedgerId.
   * @throws OpenSSLException If OpenSSL is unable to hex-encode the bytes.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Does this LedgerId represent the LedgerId of the Hiero Mainnet?
   *
   * @return \c TRUE if this LedgerId represent the LedgerId of the Hiero Mainnet, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool isMainnet() const { return *this == MAINNET; }

  /**
   * Does this LedgerId represent the LedgerId of the Hiero Testnet?
   *
   * @return \c TRUE if this LedgerId represent the LedgerId of the Hiero Testnet, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool isTestnet() const { return *this == TESTNET; }

  /**
   * Does this LedgerId represent the LedgerId of the Hiero Previewnet?
   *
   * @return \c TRUE if this LedgerId represent the LedgerId of the Hiero Previewnet, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool isPreviewnet() const { return *this == PREVIEWNET; }

  /**
   * Does this LedgerId represent the LedgerId of any Hiero network?
   *
   * @return \c TRUE if this LedgerId represents the LedgerId of any Hiero network, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool isKnownNetwork() const { return isMainnet() || isTestnet() || isPreviewnet(); }

  /**
   * Get the bytes of this LedgerId.
   *
   * @return The bytes of this LedgerId.
   */
  [[nodiscard]] inline std::vector<std::byte> toBytes() const { return mLedgerId; }

private:
  /**
   * The bytes of this LedgerId.
   */
  std::vector<std::byte> mLedgerId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_LEDGER_ID_H_
