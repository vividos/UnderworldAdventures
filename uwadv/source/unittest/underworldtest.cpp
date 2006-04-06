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
/*! \file underworldtest.cpp

   \brief Underworld classes test

*/

// needed includes
#include "unittest.hpp"
#include "savegame.hpp"
#include "filesystem.hpp"
#include "underworld.hpp"
#include "objectlist.hpp"

namespace UnitTest
{

//! Underworld classes test
/*! Tests underworld main functionality. */
class TestUnderworld: public UnitTestCase
{
public:
   // generate test suite
   CPPUNIT_TEST_SUITE(TestUnderworld)
      CPPUNIT_TEST(TestSaveLoadSavegames)
      CPPUNIT_TEST(TestObjectList)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestSaveLoadSavegames();
   void TestObjectList();
};

// register test suite
CPPUNIT_TEST_SUITE_REGISTRATION(TestUnderworld)

} // namespace UnitTest

// methods

using namespace UnitTest;

/*! Tests saving and restoring underworld.
    \todo insert data into various underworld objects and check if the data
          is available after loading.
*/
void TestUnderworld::TestSaveLoadSavegames()
{
   TempFolder testFolder;
   std::string strSavegameFile = testFolder.GetPathName() + "/savegame.uas";

   Base::SavegameInfo info;
   info.strTitle = "title01";

   // write savegame
   {
      Base::Savegame savegame(strSavegameFile, info);

      Underworld::Underworld uw;
      uw.Save(savegame);
   }

   // read back savegame
   {
      Base::Savegame savegame(strSavegameFile);

      Underworld::Underworld uw;
      uw.Load(savegame);
   }
}

/*! Tests object list functions. */
void TestUnderworld::TestObjectList()
{
   // simple allocation/free
   {
      Underworld::ObjectList ol;
      ol.Create();

      Uint16 uiPos = ol.Allocate();
      CPPUNIT_ASSERT(uiPos != 0);

      ol.Free(uiPos);
      ol.Destroy();
   }

   // tile list: add as first object, remove first object
   {
      Underworld::ObjectList ol;
      ol.Create();
      CPPUNIT_ASSERT(Underworld::g_uiObjectListPosNone == ol.GetListStart(32,16));

      Uint16 uiPos = ol.Allocate();
      ol.SetObject(uiPos, Underworld::ObjectPtr(new Underworld::Object));

      ol.AddObjectToTileList(uiPos, 32, 16);
      CPPUNIT_ASSERT(uiPos == ol.GetListStart(32,16));

      ol.RemoveObjectFromTileList(uiPos, 32, 16);
      CPPUNIT_ASSERT(Underworld::g_uiObjectListPosNone == ol.GetListStart(32,16));
   }

   // tile list: add some objects, remove from front, remove from end, remove in between
   {
      Underworld::ObjectList ol;
      ol.Create();

      Uint16 uiPos1 = ol.Allocate();
      ol.SetObject(uiPos1, Underworld::ObjectPtr(new Underworld::Object));

      Uint16 uiPos2 = ol.Allocate();
      ol.SetObject(uiPos2, Underworld::ObjectPtr(new Underworld::Object));

      Uint16 uiPos3 = ol.Allocate();
      ol.SetObject(uiPos3, Underworld::ObjectPtr(new Underworld::Object));

      Uint16 uiPos4 = ol.Allocate();
      ol.SetObject(uiPos4, Underworld::ObjectPtr(new Underworld::Object));

      Uint16 uiPos5 = ol.Allocate();
      ol.SetObject(uiPos5, Underworld::ObjectPtr(new Underworld::Object));

      CPPUNIT_ASSERT(uiPos1 != uiPos2);
      CPPUNIT_ASSERT(uiPos1 != uiPos3);
      CPPUNIT_ASSERT(uiPos1 != uiPos4);
      CPPUNIT_ASSERT(uiPos1 != uiPos5);
      CPPUNIT_ASSERT(uiPos2 != uiPos3);
      CPPUNIT_ASSERT(uiPos2 != uiPos4);
      CPPUNIT_ASSERT(uiPos2 != uiPos5);
      CPPUNIT_ASSERT(uiPos3 != uiPos4);
      CPPUNIT_ASSERT(uiPos5 != uiPos4);

      ol.AddObjectToTileList(uiPos1, 32, 16);
      ol.AddObjectToTileList(uiPos2, 32, 16);
      ol.AddObjectToTileList(uiPos3, 32, 16);
      ol.AddObjectToTileList(uiPos4, 32, 16);
      ol.AddObjectToTileList(uiPos5, 32, 16);

      // remove ...
      ol.RemoveObjectFromTileList(uiPos1, 32, 16); // from front
      CPPUNIT_ASSERT(ol.GetObject(uiPos1)->GetObjectInfo().m_uiLink == Underworld::g_uiObjectListPosNone);

      ol.RemoveObjectFromTileList(uiPos5, 32, 16); // from back
      CPPUNIT_ASSERT(ol.GetObject(uiPos5)->GetObjectInfo().m_uiLink == Underworld::g_uiObjectListPosNone);

      ol.RemoveObjectFromTileList(uiPos3, 32, 16); // in the middle
      CPPUNIT_ASSERT(ol.GetObject(uiPos3)->GetObjectInfo().m_uiLink == Underworld::g_uiObjectListPosNone);

      ol.RemoveObjectFromTileList(uiPos4, 32, 16); // again, from back
      CPPUNIT_ASSERT(ol.GetObject(uiPos4)->GetObjectInfo().m_uiLink == Underworld::g_uiObjectListPosNone);

      // only object 2 is in list now
      CPPUNIT_ASSERT(uiPos2 == ol.GetListStart(32,16));
      CPPUNIT_ASSERT(ol.GetObject(uiPos2)->GetObjectInfo().m_uiLink == Underworld::g_uiObjectListPosNone);

      // remove last
      ol.RemoveObjectFromTileList(uiPos2, 32, 16);
      CPPUNIT_ASSERT(ol.GetObject(uiPos2)->GetObjectInfo().m_uiLink == Underworld::g_uiObjectListPosNone);
      CPPUNIT_ASSERT(Underworld::g_uiObjectListPosNone == ol.GetListStart(32,16));
   }

   // allocate 0x401 entries, exceeding default object list size
   {
      Underworld::ObjectList ol;
      ol.Create();

      std::vector<Uint16> vecAllPos;
      vecAllPos.reserve(0x401);

      for (unsigned int ui=0; ui<0x401; ui++)
      {
         Uint16 uiPos = ol.Allocate();
         CPPUNIT_ASSERT(uiPos != 0);
         ol.SetObject(uiPos, Underworld::ObjectPtr(new Underworld::Object));

         ol.AddObjectToTileList(uiPos, 32, 16);

         vecAllPos.push_back(uiPos);
      }

      // to test compcation, delete every other element
      {
         unsigned int uiMax = vecAllPos.size();
         for (unsigned int ui=0; ui<uiMax; ui+=2)
         {
            ol.RemoveObjectFromTileList(vecAllPos[ui], 32, 16);
            ol.Free(vecAllPos[ui]);
         }

         ol.Compact();
      }

      ol.Destroy();
   }
}
