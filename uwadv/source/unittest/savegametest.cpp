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
/// \file savegametest.cpp
/// \brief Savegame test
//
#include "unittest.hpp"
#include "savegame.hpp"
#include "filesystem.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   /// \brief Savegame and SavegamesManager class tests
   /// Tests savegames and the savegames manager.
   TEST_CLASS(TestSavegame)
   {
      /// Tests saving and restoring a savegame.
      TEST_METHOD(TestSaveRestore)
      {
         TempFolder testFolder;
         std::string strSavegameFile = testFolder.GetPathName() + "/savegame.uas";

         Base::SavegameInfo info;
         info.strTitle = "title01";

         const std::string strTestStr("abcd 123 #!");

         // write savegame
         unsigned int uiVersion = 0;
         {
            Base::Savegame savegame(strSavegameFile, info);

            uiVersion = savegame.GetVersion();

            savegame.BeginSection("test");

            savegame.Write8(0x42);
            savegame.Write16(0xfffe);
            savegame.Write32(0x1234abcd);

            savegame.WriteString(strTestStr);

            savegame.EndSection();
         }

         // read back savegame
         {
            Base::Savegame savegame(strSavegameFile);

            Assert::IsTrue(uiVersion == savegame.GetVersion());

            savegame.BeginSection("test");

            Assert::IsTrue(0x42 == savegame.Read8());
            Assert::IsTrue(0xfffe == savegame.Read16());
            Assert::IsTrue(0x1234abcd == savegame.Read32());

            std::string strTestStr2;
            savegame.ReadString(strTestStr2);
            Assert::IsTrue(strTestStr2 == strTestStr);

            savegame.EndSection();

            Base::SavegameInfo& info2 = savegame.GetSavegameInfo();
            Assert::IsTrue(info.strTitle == info2.strTitle);
         }
      }

      /// Tests savegames manager functions.
      TEST_METHOD(TestSavegameManager_EmptySavegamesFolder)
      {
         TempFolder testFolder;
         std::string savegameFolder = testFolder.GetPathName();

         Base::Settings settings;
         settings.SetValue(Base::settingSavegameFolder, savegameFolder);
         settings.SetValue(Base::settingGamePrefix, std::string());

         // test empty savegames manager; no game prefix set
         Base::SavegamesManager savegamesManager(settings);
         savegamesManager.Rescan();
         Assert::IsTrue(0 == savegamesManager.GetSavegamesCount());
         Assert::IsTrue(false == savegamesManager.IsQuicksaveAvail());
      }

      // test savegame loading/saving and filename generation
      TEST_METHOD(TestSavegameManager_LoadingSaving)
      {
         TempFolder testFolder;
         std::string savegameFolder = testFolder.GetPathName();

         Base::Settings settings;
         settings.SetValue(Base::settingSavegameFolder, savegameFolder);

         const std::string c_strGamePrefix = "uw3";
         settings.SetValue(Base::settingGamePrefix, c_strGamePrefix);

         Base::SavegamesManager savegamesManager(settings);

         {
            Base::SavegameInfo info;
            Base::Savegame sg = savegamesManager.SaveSavegame(info);
            sg.Write8(0x42);
         }

         savegamesManager.Rescan();

         Assert::IsTrue(1 == savegamesManager.GetSavegamesCount());
         Assert::IsTrue(false == savegamesManager.IsQuicksaveAvail());

         Assert::IsTrue(savegamesManager.GetSavegameFilename(0).size() > 0);
         Assert::IsTrue(savegamesManager.GetSavegameFilename(0).find("uasave00000.uar") > 0);

         {
            Base::Savegame sg = savegamesManager.LoadSavegame(0);
            Assert::IsTrue(0x42 == sg.Read8());
            Assert::IsTrue(sg.GetSavegameInfo().strGamePrefix == c_strGamePrefix);
         }

         // clean up savegame again
         Base::FileSystem::RemoveFile(savegamesManager.GetSavegameFilename(0));
      }

      // test quicksave savegame loading/saving
      TEST_METHOD(TestSavegameManager_QuicksaveLoadingSaving)
      {
         TempFolder testFolder;
         std::string savegameFolder = testFolder.GetPathName();

         Base::Settings settings;
         settings.SetValue(Base::settingSavegameFolder, savegameFolder);

         const std::string c_strGamePrefix = "uw3";
         settings.SetValue(Base::settingGamePrefix, c_strGamePrefix);

         Base::SavegamesManager savegamesManager(settings);

         {
            Base::SavegameInfo info;
            Base::Savegame sg = savegamesManager.SaveQuicksaveSavegame(info);
            sg.Write8(0x42);
         }

         savegamesManager.Rescan();

         Assert::IsTrue(1 == savegamesManager.GetSavegamesCount());
         Assert::IsTrue(true == savegamesManager.IsQuicksaveAvail());

         Assert::IsTrue(savegamesManager.GetSavegameFilename(0).size() > 0);
         Assert::IsTrue(savegamesManager.GetSavegameFilename(0).find("quicksave") > 0);

         // check savegame info for quicksave
         {
            Base::SavegameInfo info;
            savegamesManager.GetSavegameInfo(0, info);
            Assert::IsTrue(info.strTitle == "Quicksave Savegame");
         }

         {
            Base::Savegame sg = savegamesManager.LoadQuicksaveSavegame();
            Assert::IsTrue(0x42 == sg.Read8());
            Assert::IsTrue(sg.GetSavegameInfo().strGamePrefix == c_strGamePrefix);
         }

         // clean up savegame again
         Base::FileSystem::RemoveFile(savegamesManager.GetSavegameFilename(0));
      }
   };
} // namespace UnitTest
