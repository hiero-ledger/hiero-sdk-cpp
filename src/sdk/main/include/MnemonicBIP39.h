// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_MNEMONIC_BIP39_H_
#define HIERO_SDK_CPP_MNEMONIC_BIP39_H_

#include "Mnemonic.h"
#include "PrivateKey.h"

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace Hiero
{
/**
 * A standard BIP39 mnemonic.
 */
class MnemonicBIP39 : public Mnemonic
{
public:
  /**
   * Initialize a MnemonicBIP39 from a vector of word indices.
   *
   * @param wordIndices The indices of the words from the BIP39 word list to use to create a MnemonicBIP39.
   * @return An initialized MnemonicBIP39.
   * @throws BadMnemonicException If the input indices are not valid or there is an invalid checksum.
   */
  [[nodiscard]] static MnemonicBIP39 initializeBIP39Mnemonic(const std::vector<uint16_t>& wordIndices);

  /**
   * Initialize a MnemonicBIP39 from a vector of word strings.
   *
   * @param words The words from which to create a MnemonicBIP39.
   * @return An initialized MnemonicBIP39.
   * @throws BadMnemonicException If any of the input words are not part of the BIP39 standard word list or if the
   *                              checksum is invalid.
   */
  [[nodiscard]] static MnemonicBIP39 initializeBIP39Mnemonic(const std::vector<std::string>& words);

  /**
   * Initialize a MnemonicBIP39 from a string containing all the words of a mnemonic phrase.
   *
   * @param fullMnemonic The string that contains all the words from which to create a MnemonicBIP39.
   * @param delimiter    The delimiting string for the mnemonic string.
   * @return An initialized MnemonicBIP39.
   * @throws BadMnemonicException If any of the input words are not part of the BIP39 standard word list or if the
   *                              checksum is invalid.
   */
  [[nodiscard]] static MnemonicBIP39 initializeBIP39Mnemonic(std::string_view fullMnemonic,
                                                             std::string_view delimiter = " ");

  /**
   * Generate a MnemonicBIP39 phrase with 12 words.
   *
   * @return The newly-generated MnemonicBIP39.
   */
  [[nodiscard]] static MnemonicBIP39 generate12WordBIP39Mnemonic();

  /**
   * Generate a MnemonicBIP39 phrase with 24 words.
   *
   * @return The newly-generated MnemonicBIP39.
   */
  [[nodiscard]] static MnemonicBIP39 generate24WordBIP39Mnemonic();

  /**
   * Generate a SLIP44-standard ED25519PrivateKey from this Mnemonic using the input passphrase at the specified
   * unhardened index.
   *
   * @param passphrase The passphrase to use to generate the ED25519PrivateKey.
   * @param index      The unhardened index in the derivation path from which to derive the ED25519PrivateKey.
   * @return A pointer to the derived ED25519PrivateKey.
   * @throws OpenSSLException If OpenSSL is unable to generate a key from this MnemonicBIP39.
   * @throws std::invalid_argument If the index is already hardened.
   */
  [[nodiscard]] std::unique_ptr<PrivateKey> toStandardEd25519PrivateKey(std::string_view passphrase = "",
                                                                        uint32_t index = 0) const;

  /**
   * Generate a SLIP44-standard ECDSAsecp256k1PrivateKey from this Mnemonic using the input passphrase at the specified
   * index.
   *
   * @param passphrase The passphrase to use to generate the ECDSAsecp256k1PrivateKey.
   * @param index      The index in the derivation path from which to derive the ECDSAsecp256k1PrivateKey.
   * @return A pointer to the derived ECDSAsecp256k1PrivateKey.
   * @throws OpenSSLException If OpenSSL is unable to generate a key from this MnemonicBIP39.
   */
  [[nodiscard]] std::unique_ptr<PrivateKey> toStandardECDSAsecp256k1PrivateKey(std::string_view passphrase = "",
                                                                               uint32_t index = 0) const;

  /**
   * Compute a seed that results from this MnemonicBIP39.
   *
   * @param passphrase The passphrase to use in seed generation.
   * @return This MnemonicBIP39's seed, given the input passphrase.
   * @throws OpenSSLException If OpenSSL is unable to compute a seed.
   */
  [[nodiscard]] std::vector<std::byte> toSeed(std::string_view passphrase = "") const;

  /**
   * Compute the word indices that result from the input entropy.
   *
   * @param entropy The entropy from which to compute the word indices.
   * @return A vector containing the word indices.
   */
  [[nodiscard]] static std::vector<uint16_t> entropyToWordIndices(const std::vector<std::byte>& entropy);

private:
  /**
   * Hidden constructor. Static initialization/generation functions should be used to create MnemonicBIP39 objects.
   */
  MnemonicBIP39() = default;

  /**
   * Derived from Mnemonic. Get a reference to the BIP39 word list.
   *
   * @return The BIP39 word list.
   */
  [[nodiscard]] const std::vector<std::string_view>& getWordList() const override;

  /**
   * Get the set of acceptable word counts. In the case of a BIP39 passphrase, this set will include 12 and 24.
   *
   * @return The set of acceptable word counts.
   */
  [[nodiscard]] const std::set<unsigned long>& getAcceptableWordCounts() const override;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_MNEMONIC_BIP39_H_
