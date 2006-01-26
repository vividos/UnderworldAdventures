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
/*! \file savegametest.cpp

   \brief Savegame test

*/

// needed includes
#include "unittest.hpp"
#include "savegame.hpp"
#include "filesystem.hpp"

namespace UnitTest
{

//! Savegame and SavegamesManager class test
/*! Tests savegames and the savegames manager. */
class TestSavegame: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestSavegame)
      CPPUNIT_TEST(TestSaveRestore)
      CPPUNIT_TEST(TestSavegameManager)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestSaveRestore();
   void TestSavegameManager();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestSavegame)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests saving and restoring a savegame. */
void TestSavegame::TestSaveRestore()
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

      CPPUNIT_ASSERT(uiVersion == savegame.GetVersion());

      savegame.BeginSection("test");

      CPPUNIT_ASSERT(0x42 == savegame.Read8());
      CPPUNIT_ASSERT(0xfffe == savegame.Read16());
      CPPUNIT_ASSERT(0x1234abcd == savegame.Read32());

      std::string strTestStr2;
      savegame.ReadString(strTestStr2);
      CPPUNIT_ASSERT(strTestStr2 == strTestStr);

      savegame.EndSection();

      Base::SavegameInfo& info2 = savegame.GetSavegameInfo();
      CPPUNIT_ASSERT(info.strTitle == info2.strTitle);
   }
}

/*! Tests savegames manager functions. */
void TestSavegame::TestSavegameManager()
{
   TempFolder testFolder;
   std::string strSavegameFolder = testFolder.GetPathName();

   Base::Settings settings;
   settings.SetValue(Base::settingSavegameFolder, strSavegameFolder);
   settings.SetValue(Base::settingGamePrefix, std::string(""));

   // test empty savegames manager; no game prefix set
   {
      Base::SavegamesManager sgMgr(settings);
      sgMgr.Rescan();
      CPPUNIT_ASSERT(0 == sgMgr.GetSavegamesCount());
      CPPUNIT_ASSERT(false == sgMgr.IsQuicksaveAvail());
   }

   // now set prefix for tests with prefix needed
   const std::string c_strGamePrefix = "uw3";
   settings.SetValue(Base::settingGamePrefix, c_strGamePrefix);

   // test savegame loading/saving and filename generation
   {
      Base::SavegamesManager sgMgr(settings);

      {
         Base::SavegameInfo info;
         Base::Savegame sg = sgMgr.SaveSavegame(info);
         sg.Write8(0x42);
      }

      sgMgr.Rescan();

      CPPUNIT_ASSERT(1 == sgMgr.GetSavegamesCount());
      CPPUNIT_ASSERT(false == sgMgr.IsQuicksaveAvail());

      CPPUNIT_ASSERT(sgMgr.GetSavegameFilename(0).size() > 0);
      CPPUNIT_ASSERT(sgMgr.GetSavegameFilename(0).find("uasave00000.uar") > 0);

      {
         Base::Savegame sg = sgMgr.LoadSavegame(0);
         CPPUNIT_ASSERT(0x42 == sg.Read8());
         CPPUNIT_ASSERT(sg.GetSavegameInfo().strGamePrefix == c_strGamePrefix);
      }

      // clean up savegame again
      Base::FileSystem::RemoveFile(sgMgr.GetSavegameFilename(0));
   }

   // test quicksave savegame loading/saving
   {
      Base::SavegamesManager sgMgr(settings);

      {
         Base::SavegameInfo info;
         Base::Savegame sg = sgMgr.SaveQuicksaveSavegame(info);
         sg.Write8(0x42);
      }

      sgMgr.Rescan();

      CPPUNIT_ASSERT(1 == sgMgr.GetSavegamesCount());
      CPPUNIT_ASSERT(true == sgMgr.IsQuicksaveAvail());

      CPPUNIT_ASSERT(sgMgr.GetSavegameFilename(0).size() > 0);
      CPPUNIT_ASSERT(sgMgr.GetSavegameFilename(0).find("quicksave") > 0);

      // check savegame info for quicksave
      {
         Base::SavegameInfo info;
         sgMgr.GetSavegameInfo(0, info);
         CPPUNIT_ASSERT(info.strTitle == "Quicksave Savegame");
      }

      {
         Base::Savegame sg = sgMgr.LoadQuicksaveSavegame();
         CPPUNIT_ASSERT(0x42 == sg.Read8());
         CPPUNIT_ASSERT(sg.GetSavegameInfo().strGamePrefix == c_strGamePrefix);
      }

      // clean up savegame again
      Base::FileSystem::RemoveFile(sgMgr.GetSavegameFilename(0));
   }
}
