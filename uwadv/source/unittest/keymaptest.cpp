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
/*! \file keymaptest.cpp

   \brief Keymap test

*/

// needed includes
#include "unittest.hpp"
#include "keymap.hpp"
#include "settings.hpp"
#include "textfile.hpp"
#include "filesystem.hpp"

namespace UnitTest
{

//! Keymap class test
/*! Tests loading keymaps and custom keymaps and checks loaded key mappings. */
class TestKeymap: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestKeymap)
      CPPUNIT_TEST(TestKeymapInsert)
      CPPUNIT_TEST(TestKeymapLoading)
      CPPUNIT_TEST(TestCustomKeymapLoading)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestKeymapInsert();
   void TestKeymapLoading();
   void TestCustomKeymapLoading();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestKeymap)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests inserting a key mapping into a Keymap and checking if it can be
    found again.
*/
void TestKeymap::TestKeymapInsert()
{
   // empty keymap
   Base::Keymap keymap;

   keymap.InsertKeyMapping(SDLK_a, static_cast<SDLMod>(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT), Base::keyMenuTopOfList2);
   CPPUNIT_ASSERT(keymap.FindKey(SDLK_a, static_cast<SDLMod>(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT)) == Base::keyMenuTopOfList2);
}

/*! Tests keymap loading; keymap.cfg is loaded from current uwadv.cfg settings. */
void TestKeymap::TestKeymapLoading()
{
   Base::Settings settings;
   Base::LoadSettings(settings);
   settings.SetValue(Base::settingGamePrefix, std::string("uw1"));

   Base::Keymap keymap;
   keymap.Init(settings);
}

/*! Tests loading custom keymaps and checking that keys mappings are properly
    saved in the Keymap object.
*/
void TestKeymap::TestCustomKeymapLoading()
{
   // write custom keymap file
   TempFolder testFolder;
   std::string strCustomKeymapFile;
   {
      strCustomKeymapFile = testFolder.GetPathName() + "/custom.cfg";

      Base::TextFile keymapFile(strCustomKeymapFile, Base::modeWrite);
      keymapFile.WriteLine("menu-top-of-list2 shift alt ctrl A");
      keymapFile.WriteLine("ua-debug ctrl F12");
   }

   // create default keymap.cfg
   {
      Base::FileSystem::MakeFolder(testFolder.GetPathName() + "/uw1");
      Base::TextFile defaultKeymapCfg(testFolder.GetPathName() + "/uw1/keymap.cfg", Base::modeWrite);
      defaultKeymapCfg.WriteLine("#");
   }

   // set up settings
   Base::Settings settings;
   settings.SetValue(Base::settingUadataPath, testFolder.GetPathName());
   settings.SetValue(Base::settingCustomKeymap, strCustomKeymapFile);
   settings.SetValue(Base::settingGamePrefix, std::string("uw1"));

   // load custom keymap
   Base::Keymap keymap;
   keymap.Init(settings);

   // verify settings
   CPPUNIT_ASSERT(keymap.FindKey(SDLK_a, static_cast<SDLMod>(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT)) == Base::keyMenuTopOfList2);
   CPPUNIT_ASSERT(keymap.FindKey(SDLK_F12, static_cast<SDLMod>(KMOD_CTRL)) == Base::keyUaDebug);
   CPPUNIT_ASSERT(keymap.FindKey(SDLK_F12, KMOD_LCTRL) == Base::keyNone);
}
