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
/*! \file resourcemanagertest.cpp

   \brief ResourceManager test

*/

// needed includes
#include "unittest.hpp"
#include "resourcemanager.hpp"
#include "settings.hpp"
#include "file.hpp"

namespace UnitTest
{

//! ResourceManager class test
/*! Tests resource manager that loads resource files.
    \todo test search order of .uar files
*/
class TestResourceManager: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestResourceManager)
      CPPUNIT_TEST(TestResourceLoading)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestResourceLoading();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestResourceManager)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests opening of resource files that may be zipped into an .uar file.
    Assumes that a valid "uwadv.cfg" file can be found where Underworld
    Adventures would expect the file.
*/
void TestResourceManager::TestResourceLoading()
{
   Base::Settings settings;
   Base::LoadSettings(settings);

   Base::ResourceManager resManager(settings);
   SDL_RWops* rwops = resManager.GetResourceFile("uw1/keymap.cfg");
   CPPUNIT_ASSERT(rwops != NULL);

   Base::File file(rwops);
   CPPUNIT_ASSERT(file.FileLength() > 0);
}
