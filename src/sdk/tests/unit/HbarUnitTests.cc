// SPDX-License-Identifier: Apache-2.0
#include "Hbar.h"

#include <gtest/gtest.h>

using namespace Hiero;

class HbarUnitTests : public ::testing::Test
{
protected:
  int64_t amount = 6ULL;
};

//-----
TEST_F(HbarUnitTests, TinybarUnit)
{
  EXPECT_EQ(HbarUnit::TINYBAR().getTinybars(), 1ULL);
  EXPECT_EQ(strcmp(HbarUnit::TINYBAR().getSymbol(), "tℏ"), 0);
}

//-----
TEST_F(HbarUnitTests, MicrobarUnit)
{
  EXPECT_EQ(HbarUnit::MICROBAR().getTinybars(), 100ULL);
  EXPECT_EQ(strcmp(HbarUnit::MICROBAR().getSymbol(), "μℏ"), 0);
}

//-----
TEST_F(HbarUnitTests, MillibarUnit)
{
  EXPECT_EQ(HbarUnit::MILLIBAR().getTinybars(), 100000ULL);
  EXPECT_EQ(strcmp(HbarUnit::MILLIBAR().getSymbol(), "mℏ"), 0);
}

//-----
TEST_F(HbarUnitTests, HbarUnit)
{
  EXPECT_EQ(HbarUnit::HBAR().getTinybars(), 100000000ULL);
  EXPECT_EQ(strcmp(HbarUnit::HBAR().getSymbol(), "ℏ"), 0);
}

//-----
TEST_F(HbarUnitTests, KilobarUnit)
{
  EXPECT_EQ(HbarUnit::KILOBAR().getTinybars(), 100000000000ULL);
  EXPECT_EQ(strcmp(HbarUnit::KILOBAR().getSymbol(), "kℏ"), 0);
}

//-----
TEST_F(HbarUnitTests, MegabarUnit)
{
  EXPECT_EQ(HbarUnit::MEGABAR().getTinybars(), 100000000000000ULL);
  EXPECT_EQ(strcmp(HbarUnit::MEGABAR().getSymbol(), "Mℏ"), 0);
}

//-----
TEST_F(HbarUnitTests, GigabarUnit)
{
  EXPECT_EQ(HbarUnit::GIGABAR().getTinybars(), 100000000000000000ULL);
  EXPECT_EQ(strcmp(HbarUnit::GIGABAR().getSymbol(), "Gℏ"), 0);
}

//-----
TEST_F(HbarUnitTests, DefaultConstructor)
{
  Hbar hbar;
  EXPECT_EQ(hbar.toTinybars(), 0ULL);
}

//-----
TEST_F(HbarUnitTests, AmountConstructor)
{
  Hbar hbar(amount);
  EXPECT_EQ(hbar.toTinybars(), amount * HbarUnit::HBAR().getTinybars());
}

//-----
TEST_F(HbarUnitTests, AmountUnitConstructor)
{
  Hbar tinybar(amount, HbarUnit::TINYBAR());
  Hbar microbar(amount, HbarUnit::MICROBAR());
  Hbar millibar(amount, HbarUnit::MILLIBAR());
  Hbar kilobar(amount, HbarUnit::KILOBAR());
  Hbar megabar(amount, HbarUnit::MEGABAR());
  Hbar gigabar(amount, HbarUnit::GIGABAR());

  EXPECT_EQ(tinybar.toTinybars(), amount * HbarUnit::TINYBAR().getTinybars());
  EXPECT_EQ(microbar.toTinybars(), amount * HbarUnit::MICROBAR().getTinybars());
  EXPECT_EQ(millibar.toTinybars(), amount * HbarUnit::MILLIBAR().getTinybars());
  EXPECT_EQ(kilobar.toTinybars(), amount * HbarUnit::KILOBAR().getTinybars());
  EXPECT_EQ(megabar.toTinybars(), amount * HbarUnit::MEGABAR().getTinybars());
  EXPECT_EQ(gigabar.toTinybars(), amount * HbarUnit::GIGABAR().getTinybars());
}

//-----
TEST_F(HbarUnitTests, AddOperator)
{
  EXPECT_EQ(Hbar(amount) + Hbar(amount), Hbar(amount + amount));
}

//-----
TEST_F(HbarUnitTests, AddEqualsOperator)
{
  Hbar hbar(amount, HbarUnit::TINYBAR());
  hbar += Hbar(amount, HbarUnit::MICROBAR());
  EXPECT_EQ(hbar.toTinybars(), amount * HbarUnit::MICROBAR().getTinybars() + amount);
}

//-----
TEST_F(HbarUnitTests, Negated)
{
  Hbar hbar(amount);
  Hbar hbarNegated(-amount);
  EXPECT_EQ(hbar.negated().toTinybars(), hbarNegated.toTinybars());
  EXPECT_EQ(hbar.toTinybars(), hbarNegated.negated().toTinybars());
}
