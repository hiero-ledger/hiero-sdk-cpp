// SPDX-License-Identifier: Apache-2.0
#include "AddressBookQuery.h"
#include "BaseIntegrationTest.h"
#include "FileId.h"
#include "NodeAddressBook.h"

#include <gtest/gtest.h>

using namespace Hiero;

class AddressBookQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
TEST_F(AddressBookQueryIntegrationTests, ExecuteAddressBookQuery)
{
  // Given / When / Then
  EXPECT_NO_THROW(const NodeAddressBook nodeAddressBook =
                    AddressBookQuery().setFileId(FileId::ADDRESS_BOOK).execute(getTestClient()));
}
