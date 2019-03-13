//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file KeymapTest.cpp
/// \brief Keymap test
//
#include "UnitTest.hpp"
#include "Keymap.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include "TextFile.hpp"
#include "FileSystem.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief Keymap class tests
   /// Tests loading keymaps and custom keymaps and checks loaded key mappings.
   TEST_CLASS(KeymapTest)
   {
      /// Tests inserting a key mapping into a Keymap and checking if it can be
      /// found again.
      TEST_METHOD(TestKeymapInsert)
      {
         // empty keymap
         Base::Keymap keymap;

         keymap.InsertKeyMapping(SDLK_a, static_cast<SDL_Keymod>(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT), Base::keyMenuTopOfList2);
         Assert::IsTrue(keymap.FindKey(SDLK_a, static_cast<SDL_Keymod>(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT)) == Base::keyMenuTopOfList2);
      }

      /// Tests keymap loading; keymap.cfg is loaded from current uwadv.cfg settings.
      TEST_METHOD(TestKeymapLoading)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));

         Base::ResourceManager resourceManager{ settings };

         Base::Keymap keymap;
         keymap.Init(settings, resourceManager);
      }

      /// Tests loading custom keymaps and checking that keys mappings are properly
      /// saved in the Keymap object.
      TEST_METHOD(TestCustomKeymapLoading)
      {
         // write custom keymap file
         TempFolder testFolder;
         std::string customKeymapFile;
         {
            customKeymapFile = testFolder.GetPathName() + "/custom.cfg";

            Base::TextFile keymapFile(customKeymapFile, Base::modeWrite);
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
         settings.SetValue(Base::settingCustomKeymap, customKeymapFile);
         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));

         Base::ResourceManager resourceManager{ settings };

         // load custom keymap
         Base::Keymap keymap;
         keymap.Init(settings, resourceManager);

         // verify settings
         Assert::IsTrue(keymap.FindKey(SDLK_a, static_cast<SDL_Keymod>(KMOD_CTRL | KMOD_SHIFT | KMOD_ALT)) == Base::keyMenuTopOfList2);
         Assert::IsTrue(keymap.FindKey(SDLK_F12, static_cast<SDL_Keymod>(KMOD_CTRL)) == Base::keyUaDebug);
         Assert::IsTrue(keymap.FindKey(SDLK_F12, KMOD_LCTRL) == Base::keyNone);
      }
   };
} // namespace UnitTest
