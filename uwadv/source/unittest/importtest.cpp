/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006,2019 Michael Fink

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
/*! \file importtest.cpp

   \brief import functions test

*/

// needed includes
#include "unittest.hpp"
#include "settings.hpp"
#include "resourcemanager.hpp"
#include "import.hpp"
#include "properties.hpp"
#include "levellist.hpp"
#include "player.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{

//! Import test
/*! Tests importing all kinds of original game data files. */
TEST_CLASS(TestImport)
{
/*! Tests importing object properties */
TEST_METHOD(TestObjectPropertiesImport)
{
   Base::Settings& settings = GetTestSettings();

   settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

   {
      Base::ResourceManager resourceManager(settings);

      // load properties
      Underworld::ObjectProperties objectProperties;

      Import::ImportProperties(resourceManager, objectProperties);

      // test if all property vectors are filled
      Assert::IsTrue(0x0200 == objectProperties.GetVectorCommonObjectProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorMeleeWeaponProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorRangedWeaponProperties().size());
      Assert::IsTrue(0x0020 == objectProperties.GetVectorArmourAndWearableProperties().size());
      Assert::IsTrue(0x0040 == objectProperties.GetVectorCritterProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorContainerProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorLightSourceProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorAnimatedObjectProperties().size());
   }

   settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

   {
      Base::ResourceManager resourceManager(settings);

      // load properties
      Underworld::ObjectProperties objectProperties;

      Import::ImportProperties(resourceManager, objectProperties);

      // test if all property vectors are filled
      Assert::IsTrue(0x0200 == objectProperties.GetVectorCommonObjectProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorMeleeWeaponProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorRangedWeaponProperties().size());
      Assert::IsTrue(0x0020 == objectProperties.GetVectorArmourAndWearableProperties().size());
      Assert::IsTrue(0x0040 == objectProperties.GetVectorCritterProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorContainerProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorLightSourceProperties().size());
      Assert::IsTrue(0x0010 == objectProperties.GetVectorAnimatedObjectProperties().size());
   }
}

/*! Tests loading level list */
TEST_METHOD(TestLevelListImport)
{
   Base::Settings& settings = GetTestSettings();

#if 0
   // uw_demo loading
   settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
   settings.SetValue(Base::settingUw1IsUwdemo, true);
   settings.SetValue(Base::settingUnderworldPath, "d:\\uwadv\\uw_demo\\");

   {
      Base::ResourceManager resourceManager(settings);
      Import::LevelImporter levelImporter(resourceManager);

      Underworld::LevelList levelList;
      levelImporter.LoadUwDemoLevel(levelList);

      UaAssert(levelList.GetNumLevels() == 1);
   }

   settings.SetValue(Base::settingUw1IsUwdemo, false);
#endif

   settings.SetValue(Base::settingGamePrefix, std::string("uw1"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw1Path));

   {
      Base::ResourceManager resourceManager(settings);
      Import::LevelImporter levelImporter(resourceManager);

      Underworld::LevelList levelList;
      levelImporter.LoadUw1Levels(levelList);

      UaAssert(levelList.GetNumLevels() == 9);
   }

   settings.SetValue(Base::settingGamePrefix, std::string("uw2"));
   settings.SetValue(Base::settingUnderworldPath, settings.GetString(Base::settingUw2Path));

   {
      Base::ResourceManager resourceManager(settings);
      Import::LevelImporter levelImporter(resourceManager);

      Underworld::LevelList levelList;
      levelImporter.LoadUw2Levels(levelList);

      UaAssert(levelList.GetNumLevels() == 80);
   }
}

/*! Tests loading player infos */
TEST_METHOD(TestPlayerImport)
{
   Base::Settings& settings = GetTestSettings();

   {
      Base::ResourceManager resourceManager(settings);

      Underworld::Player player;

      Import::PlayerImporter playerImporter(resourceManager);
      playerImporter.LoadPlayer(player, settings.GetString(Base::settingUw1Path));
   }

   {
      Base::ResourceManager resourceManager(settings);

      Underworld::Player player;

      Import::PlayerImporter playerImporter(resourceManager);
      playerImporter.LoadPlayer(player, settings.GetString(Base::settingUw2Path));
   }
}
};
} // namespace UnitTest
