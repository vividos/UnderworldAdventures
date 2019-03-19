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
/// \file ImportTest.cpp
/// \brief import functions test
//
#include "pch.hpp"
#include "Settings.hpp"
#include "ResourceManager.hpp"
#include "Import.hpp"
#include "LevelImporter.hpp"
#include "PlayerImporter.hpp"
#include "GameStrings.hpp"
#include "GameStringsImporter.hpp"
#include "Properties.hpp"
#include "LevelList.hpp"
#include "Player.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// Tests importing all kinds of original game data files.
   TEST_CLASS(ImportTest)
   {
      /// Tests importing object properties, uw1
      TEST_METHOD(TestObjectPropertiesImportUw1)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         // load properties
         Underworld::ObjectProperties objectProperties;

         Import::ImportProperties(resourceManager, objectProperties);

         // test if all property vectors are filled
         Assert::IsTrue(0x0200 == objectProperties.GetCommonObjectPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetMeleeWeaponPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetRangedWeaponPropertiesList().size());
         Assert::IsTrue(0x0020 == objectProperties.GetArmourAndWearablePropertiesList().size());
         Assert::IsTrue(0x0040 == objectProperties.GetCritterPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetContainerPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetLightSourcePropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetAnimatedObjectProperties().size());
      }

      /// Tests importing object properties, uw2
      TEST_METHOD(TestObjectPropertiesImportUw2)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

         Base::ResourceManager resourceManager{ settings };

         // load properties
         Underworld::ObjectProperties objectProperties;

         Import::ImportProperties(resourceManager, objectProperties);

         // test if all property vectors are filled
         Assert::IsTrue(0x0200 == objectProperties.GetCommonObjectPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetMeleeWeaponPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetRangedWeaponPropertiesList().size());
         Assert::IsTrue(0x0020 == objectProperties.GetArmourAndWearablePropertiesList().size());
         Assert::IsTrue(0x0040 == objectProperties.GetCritterPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetContainerPropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetLightSourcePropertiesList().size());
         Assert::IsTrue(0x0010 == objectProperties.GetAnimatedObjectProperties().size());
      }

      /// Tests loading level list, uw_demo
      TEST_METHOD(TestLevelListImportUwDemo)
      {
#if 0
         Base::Settings& settings = GetTestSettings();

         // uw_demo loading
         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUw1IsUwdemo, true);
         settings.SetValue(Base::settingUnderworldPath, "d:\\uwadv\\uw_demo\\");

         Base::ResourceManager resourceManager{ settings };
         Import::LevelImporter levelImporter(resourceManager);

         Underworld::LevelList levelList;
         levelImporter.LoadUwDemoLevel(levelList);

         UaAssert(levelList.GetNumLevels() == 1);

         settings.SetValue(Base::settingUw1IsUwdemo, false);
#endif
      }

      /// Tests loading level list, uw1
      TEST_METHOD(TestLevelListImportUw1)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };
         Import::LevelImporter levelImporter(resourceManager);

         Underworld::LevelList levelList;
         levelImporter.LoadUw1Levels(levelList);

         UaAssert(levelList.GetNumLevels() == 9);
      }

      /// Tests loading level list, uw2
      TEST_METHOD(TestLevelListImportUw2)
      {
         Base::Settings& settings = GetTestSettings();

         settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

         Base::ResourceManager resourceManager{ settings };
         Import::LevelImporter levelImporter(resourceManager);

         Underworld::LevelList levelList;
         levelImporter.LoadUw2Levels(levelList);

         UaAssert(levelList.GetNumLevels() == 80);
      }

      /// Tests loading player infos, uw1
      TEST_METHOD(TestPlayerImportUw1)
      {
         Base::Settings& settings = GetTestSettings();
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

         Base::ResourceManager resourceManager{ settings };

         Underworld::Player player;

         Import::PlayerImporter playerImporter(resourceManager);
         playerImporter.LoadPlayer(player, "data", true);
      }

      /// Tests loading player infos, uw2
      TEST_METHOD(TestPlayerImportUw2)
      {
         Base::Settings& settings = GetTestSettings();
         settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

         Base::ResourceManager resourceManager{ settings };

         Underworld::Player player;

         Import::PlayerImporter playerImporter(resourceManager);
         playerImporter.LoadPlayer(player, "data", true);
      }

      /// Tests loading game strings
      TEST_METHOD(TestGameStringsLoader)
      {
         Base::Settings& settings = GetTestSettings();

         Base::ResourceManager resourceManager{ settings };

         GameStrings gs;
         Import::GameStringsImporter importer(gs);
         Import::PlayerImporter playerImporter(resourceManager);
         importer.LoadDefaultStringsPakFile(resourceManager);

         Assert::IsTrue(gs.IsBlockAvail(0x0c00));
         Assert::IsTrue(gs.IsBlockAvail(0x0e01));
         Assert::IsTrue(gs.GetString(0x0001, 0).size() > 0);
      }
   };
} // namespace UnitTest
