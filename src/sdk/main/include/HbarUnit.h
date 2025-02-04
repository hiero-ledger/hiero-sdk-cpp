// SPDX-License-Identifier: Apache-2.0
#ifndef HBAR_UNIT_H_
#define HBAR_UNIT_H_

#include <map>
#include <string>

namespace Hiero
{
/**
 * Common units of Hbar; for the most part they follow SI prefix conventions.
 */
class HbarUnit
{
public:
  /**
   * The atomic (smallest) unit of hbar, used natively by the Hiero network. It is equivalent to 1/100,000,000 hbar.
   */
  static constexpr HbarUnit TINYBAR() { return { "tℏ", 1ULL }; }

  /**
   * Equivalent to 100 tinybar or 1/1,000,000 hbar.
   */
  static constexpr HbarUnit MICROBAR() { return { "μℏ", 100ULL }; }

  /**
   * Equivalent to 100,000 tinybar or 1/1,000 hbar.
   */
  static constexpr HbarUnit MILLIBAR() { return { "mℏ", 100000ULL }; }

  /**
   * The base unit of hbar, equivalent to 100 million tinybar.
   */
  static constexpr HbarUnit HBAR() { return { "ℏ", 100000000ULL }; }

  /**
   * Equivalent to 1 thousand hbar or 100 billion tinybar.
   */
  static constexpr HbarUnit KILOBAR() { return { "kℏ", 100000000000ULL }; }

  /**
   * Equivalent to 1 million hbar or 100 trillion tinybar.
   */
  static constexpr HbarUnit MEGABAR() { return { "Mℏ", 100000000000000ULL }; }

  /**
   * Equivalent to 1 billion hbar or 100 quadrillion tinybar. The maximum hbar amount supported by Hiero in any context
   * is ~92 gigabar (2^63 tinybar); use this unit sparingly.
   */
  static constexpr HbarUnit GIGABAR() { return { "Gℏ", 100000000000000000ULL }; }

  /**
   * Get the symbol of the current unit.
   *
   * @return The symbol of this unit.
   */
  [[nodiscard]] constexpr const char* getSymbol() const { return mSymbol; }

  /**
   * Get the value of this unit, in tinybars.
   *
   * @return The unit value, in tinybars.
   */
  [[nodiscard]] constexpr uint64_t getTinybars() const { return mTinybars; }

private:
  /**
   * Construct with a symbol and a value.
   * @param sym The symbol of the unit
   * @param val The value of the unit, in tinybars
   */
  constexpr HbarUnit(const char* sym, const uint64_t& val)
    : mSymbol(sym)
    , mTinybars(val)
  {
  }

  /**
   * The associated symbol of this unit
   */
  const char* mSymbol;

  /**
   * The value of this unit, in tinybars
   */
  const uint64_t mTinybars;
};

} // namespace Hiero

#endif // HBAR_UNIT_H_