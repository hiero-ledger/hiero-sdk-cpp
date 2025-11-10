// SPDX-License-Identifier: Apache-2.0
#include "AddressBookQuery.h"
#include "Client.h"
#include "FileId.h"
#include "NodeAddressBook.h"

#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  // Create a client for the Hiero network of which to get the address book.
  Client client = Client::forTestnet();

  // Query for the address book using the client.
  const NodeAddressBook nodeAddressBook = AddressBookQuery().setFileId(FileId::ADDRESS_BOOK).execute(client);

  // Print off the received addresses contained in the address book.
  for (const auto& nodeAddress : nodeAddressBook.getNodeAddresses())
  {
    std::cout << nodeAddress.toString() << std::endl;
  }

  return 0;
}
