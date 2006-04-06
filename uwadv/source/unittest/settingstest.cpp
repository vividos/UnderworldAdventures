/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file settingstest.cpp

   \brief Settings test

*/

// needed includes
#include "unittest.hpp"
#include "settings.hpp"

namespace UnitTest
{

//! ResourceManager class test
/*! Tests resource manager that loads resource files.
    \todo test search order of .uar files
*/
class TestSettings: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestSettings)
      CPPUNIT_TEST(TestSetValueStringBug)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestSetValueStringBug();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestSettings)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests a bug when calling SetValue with a const char* argument that gets
    implicitly converted to bool, calling the wrong function as result.
*/
void TestSettings::TestSetValueStringBug()
{
   Base::Settings settings;

   // fails by converting text to bool
   // note: when passing a variable of type const char*, at least a warning is printed:
   //       warning C4800: 'const char *' : forcing value to bool
   //       'true' or 'false' (performance warning)
   settings.SetValue(Base::settingUnderworldPath, "c:\\uwadv");
   CPPUNIT_ASSERT(settings.GetString(Base::settingUnderworldPath) == "c:\\uwadv");
}
