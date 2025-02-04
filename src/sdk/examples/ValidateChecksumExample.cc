// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountId.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "exceptions/BadEntityIdException.h"

#include <dotenv.h>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  /*
   * Entity IDs, such as TokenId and AccountId, can be constructed from strings. For example, the
   * AccountId::fromString(inputString) static method will attempt to parse the input string and construct the expected
   * AccountId object, and will throw a std::invalid_argument if the string is incorrectly formatted.
   *
   * From here on, we'll talk about methods on AccountId, but equivalent methods exist on every entity ID type.
   *
   * fromString() expects the input to look something like this: "1.2.3-asdfg". Here, 1 is the shard, 2 is the realm, 3
   * is the number, and "asdfg" is the checksum.
   *
   * The checksum can be used to ensure that an entity ID was input correctly. For example, if the string being parsed
   * is from a config file, or from user input, it could contain typos.
   *
   * You can use accountId.getChecksum() to get the checksum of an AccountId object that was constructed using
   * fromString(). This will be the checksum from the input string. fromString() will merely parse the string and
   * create an AccountId object with the expected shard, realm, num, and checksum values. fromString() will NOT verify
   * that the AccountId maps to a valid account on the Hiero network, and it will not verify the checksum.
   *
   * To verify a checksum, call accountId.validateChecksum(client).  If the checksum is invalid, validateChecksum() will
   * throw a BadEntityIdException, otherwise it will return normally.
   *
   * The validity of a checksum depends on the network to which the client is connected (EG mainnet or testnet or
   * previewnet). For example, a checksum that is valid for a particular shard/realm/num on mainnet will be INVALID for
   * the same shard/realm/num on testnet.
   *
   * As far as fromString() is concerned, the checksum is optional. If you use fromString() to generate an AccountId
   * from a string that does not include a checksum, such as "1.2.3", fromString() will work, but a call to the
   * getChecksum() method on the resulting AccountId object will return an empty string.
   *
   * Generally speaking, AccountId objects can come from three places:
   * 1) AccountId::fromString(str)
   * 2) AccountId(shard, realm, num)
   * 3) From the result of a query
   *
   * In the first case, the AccountId object will have a checksum (getChecksum() will not return null) if the input
   * string included a checksum, and it will not have a checksum if the string did not include a checksum.
   *
   * In the second and third cases, the AccountId object will not have a checksum.
   *
   * If you call accountId.validateChecksum(client) and accountId has no checksum to validate, validateChecksum() will
   * silently pass, and will not throw an exception.
   *
   * accountId.toString() will stringify the account ID with no checksum, accountId.toStringWithChecksum(client) will
   * stringify the account ID with the correct checksum for that shard/realm/num on the client's network.
   */
  std::cout << "Manual checksum validation" << std::endl << "--------------------------" << std::endl;
  while (true)
  {
    try
    {
      // Get an account ID from user input.
      std::string inputId;
      std::cout << "Enter an account ID with a checksum: ";
      std::cin >> inputId;

      const AccountId accountId = AccountId::fromString(inputId);

      std::cout << "Account ID without checksum: " << accountId.toString() << std::endl;
      std::cout << "Account ID with checksum: " << accountId.toStringWithChecksum(client) << std::endl;

      if (accountId.getChecksum().empty())
      {
        std::cout << "You must enter a checksum" << std::endl;
        continue;
      }
      else
      {
        std::cout << "The checksum entered was " << accountId.getChecksum() << std::endl;
      }

      // Validate the checksum.
      accountId.validateChecksum(client);

      break;
    }
    catch (const std::invalid_argument& invalidArgument)
    {
      std::cout << invalidArgument.what() << std::endl;
    }
    catch (const BadEntityIdException& badEntityIdException)
    {
      std::cout << badEntityIdException.what() << std::endl;
    }
  }

  /*
   * It is also possible to perform automatic checksum validation.
   *
   * Automatic checksum validation is disabled by default, but it can be enabled with
   * client.setAutoValidateChecksums(true). You can check whether automatic checksum validation is enabled with
   * client.isAutoValidateChecksumsEnabled().
   *
   * When this feature is enabled, the execute() method of a transaction or query will automatically check the validity
   * of checksums on any IDs in the transaction or query. It will throw an BadEntityIdException if an invalid checksum
   * is encountered.
   */
  client.setAutoValidateChecksums(true);

  std::cout << std::endl
            << "Automatic checksum validation" << std::endl
            << "-----------------------------" << std::endl;
  while (true)
  {
    try
    {
      // Get an account ID from user input.
      std::string inputId;
      std::cout << "Enter an account ID with a checksum: ";
      std::cin >> inputId;

      const AccountId accountId = AccountId::fromString(inputId);

      if (accountId.getChecksum().empty())
      {
        std::cout << "You must enter a checksum" << std::endl;
        continue;
      }

      // Get the account's balance.
      std::cout << "Balance of account: "
                << AccountBalanceQuery().setAccountId(accountId).execute(client).mBalance.toString() << std::endl;

      break;
    }
    catch (const std::invalid_argument& invalidArgument)
    {
      std::cout << invalidArgument.what() << std::endl;
    }
    catch (const BadEntityIdException& badEntityIdException)
    {
      std::cout << badEntityIdException.what() << std::endl;
    }
  }

  return 0;
}
