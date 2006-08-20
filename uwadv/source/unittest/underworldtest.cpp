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
#include "inventory.hpp"

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
      CPPUNIT_TEST(TestInventory)
      CPPUNIT_TEST(TestInventoryDropParent)
   CPPUNIT_TEST_SUITE_END()

protected:
   void TestSaveLoadSavegames();
   void TestObjectList();
   void TestInventory();
   void TestInventoryDropParent();
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

      ol.SetObject(uiPos, Underworld::ObjectPtr(new Underworld::Object));

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

      // to test compaction, delete every other element
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

using Underworld::c_uiInventorySlotNoItem;

/*! Tests inventory functions. */
void TestUnderworld::TestInventory()
{
   // simple object allocation
   {
      Underworld::Inventory inv;
      inv.Create();

      UaAssert(c_uiInventorySlotNoItem == inv.GetContainerPos());
      UaAssert(c_uiInventorySlotNoItem == inv.GetFloatingObjectPos());

      Uint16 uiPos = inv.Allocate();
      inv.GetObject(uiPos).m_uiItemID = 0x0001;

      inv.Free(uiPos);

      inv.Destroy();
   }

   // test opening / closing container
   {
      Underworld::Inventory inv;
      inv.Create();

      // add container
      Uint16 uiContainerPos = inv.Allocate();
      inv.GetObject(uiContainerPos).m_uiItemID = 0x0080; // a_sack
      inv.GetObject(uiContainerPos).m_bIsQuantity = false;

      // open it
      inv.OpenContainer(uiContainerPos);
      CPPUNIT_ASSERT(uiContainerPos == inv.GetContainerPos());
      CPPUNIT_ASSERT(c_uiInventorySlotNoItem == inv.GetParentContainerPos());

      // and close it
      inv.CloseContainer();
      CPPUNIT_ASSERT(c_uiInventorySlotNoItem == inv.GetContainerPos());
      CPPUNIT_ASSERT(c_uiInventorySlotNoItem == inv.GetParentContainerPos());
   }

   // test slot list building for topmost container
   {
      Underworld::Inventory inv;
      inv.Create();

      CPPUNIT_ASSERT(0 == inv.GetNumSlots());

      // add 4 objects
      Uint16 uiPos1 = inv.Allocate();
      inv.GetObject(uiPos1).m_uiItemID = 0x0001;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos1, c_uiInventorySlotNoItem));
      CPPUNIT_ASSERT(uiPos1 == 0); // move to position 0
      CPPUNIT_ASSERT(0 == inv.GetObject(uiPos1).m_uiLink); // assume link field is empty for topmost items

      Uint16 uiPos2 = inv.Allocate();
      inv.GetObject(uiPos2).m_uiItemID = 0x0002;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos2, c_uiInventorySlotNoItem));
      CPPUNIT_ASSERT(uiPos2 == 1); // move to position 1
      CPPUNIT_ASSERT(0 == inv.GetObject(uiPos2).m_uiLink);

      Uint16 uiPos3 = inv.Allocate();
      inv.GetObject(uiPos3).m_uiItemID = 0x0003;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos3, c_uiInventorySlotNoItem));
      CPPUNIT_ASSERT(uiPos3 == 2); // move to position 2
      CPPUNIT_ASSERT(0 == inv.GetObject(uiPos3).m_uiLink);

      Uint16 uiPos4 = inv.Allocate();
      inv.GetObject(uiPos4).m_uiItemID = 0x0004;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos4, c_uiInventorySlotNoItem));
      CPPUNIT_ASSERT(uiPos4 == 3); // move to position 3
      CPPUNIT_ASSERT(0 == inv.GetObject(uiPos4).m_uiLink);

      // in topmost container there always are 8 slots
      CPPUNIT_ASSERT(8 == inv.GetNumSlots());

      // remove some items from topmost container
      inv.RemoveFromContainer(uiPos1, c_uiInventorySlotNoItem);
      CPPUNIT_ASSERT(inv.GetObject(uiPos1).m_uiLink == 0);
      inv.Free(uiPos1);
      CPPUNIT_ASSERT(inv.GetObject(uiPos1).m_uiItemID == c_uiInventorySlotNoItem);

      inv.RemoveFromContainer(uiPos4, c_uiInventorySlotNoItem);
      CPPUNIT_ASSERT(inv.GetObject(uiPos4).m_uiLink == 0);
      inv.Free(uiPos4);
      CPPUNIT_ASSERT(inv.GetObject(uiPos4).m_uiItemID == c_uiInventorySlotNoItem);

      // still 8 slots in the topmost container
      CPPUNIT_ASSERT(8 == inv.GetNumSlots());
   }

   // test slot list building for container
   {
      Underworld::Inventory inv;
      inv.Create();

      Uint16 uiPosContainer = inv.Allocate();
      inv.GetObject(uiPosContainer).m_uiItemID = 0x0080; // a_sack
      inv.GetObject(uiPosContainer).m_bIsQuantity = false; // m_uiQuantity is a link
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPosContainer, c_uiInventorySlotNoItem));

      // always 8 slots in topmost container
      CPPUNIT_ASSERT(8 == inv.GetNumSlots());

      inv.OpenContainer(uiPosContainer);

      CPPUNIT_ASSERT(0 == inv.GetNumSlots()); // no object yet

      Uint16 uiPos1 = inv.Allocate();
      inv.GetObject(uiPos1).m_uiItemID = 0x0001;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos1, uiPosContainer));
      CPPUNIT_ASSERT(1 == inv.GetNumSlots());

      Uint16 uiPos2 = inv.Allocate();
      inv.GetObject(uiPos2).m_uiItemID = 0x0002;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos2, uiPosContainer));
      CPPUNIT_ASSERT(2 == inv.GetNumSlots());

      Uint16 uiPos3 = inv.Allocate();
      inv.GetObject(uiPos3).m_uiItemID = 0x0003;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos3, uiPosContainer));
      CPPUNIT_ASSERT(3 == inv.GetNumSlots());

      Uint16 uiPos4 = inv.Allocate();
      inv.GetObject(uiPos4).m_uiItemID = 0x0004;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos4, uiPosContainer));
      CPPUNIT_ASSERT(4 == inv.GetNumSlots());

      // remove from front
      inv.RemoveFromContainer(uiPos1, uiPosContainer);
      CPPUNIT_ASSERT(3 == inv.GetNumSlots());
      inv.Free(uiPos1);

      // remove from last
      inv.RemoveFromContainer(uiPos4, uiPosContainer);
      CPPUNIT_ASSERT(2 == inv.GetNumSlots());
      inv.Free(uiPos4);

      // remove from in between
      inv.RemoveFromContainer(uiPos3, uiPosContainer);
      CPPUNIT_ASSERT(1 == inv.GetNumSlots());
      inv.Free(uiPos3);

      inv.CloseContainer();

      // still 8 slots in the topmost container
      CPPUNIT_ASSERT(8 == inv.GetNumSlots());
   }

   // allocate all items, causing item list enlargement
   {
      Underworld::Inventory inv;
      inv.Create();

      for(unsigned int i=0; i<256; i++)
      {
         Uint16 uiPos = inv.Allocate();
         inv.GetObject(uiPos).m_uiItemID = 0x0001;
      }
   }
}

/*
    - dropping objects to the symbol of the currently open container (the parent)
    - dropping objects to an empty place in the container, basically adding
      it to the end of the slot list
    - dropping objects to the paperdoll or paperdoll slots
    - dropping object to an item in the container; this generates three further
      possibilities:
      - when the object is a container, drop item in this container (if possible)
      - when the objects can be combined, the new object is dropped (there still
        may be a floating object afterwards, e.g. lighting incense with a torch)
      - when the objects can't be combined, the objects just swap
*/
void TestUnderworld::TestInventoryDropParent()
{
   // test dropping object to parent container
   {
      Underworld::Inventory inv;
      inv.Create();

      // add container
      Uint16 uiContainerPos1 = inv.Allocate();
      inv.GetObject(uiContainerPos1).m_uiItemID = 0x0080; // a_sack
      inv.GetObject(uiContainerPos1).m_bIsQuantity = false;
      inv.AddToContainer(uiContainerPos1, c_uiInventorySlotNoItem);

      // open it
      inv.OpenContainer(uiContainerPos1);
      CPPUNIT_ASSERT(uiContainerPos1 == inv.GetContainerPos());
      CPPUNIT_ASSERT(c_uiInventorySlotNoItem == inv.GetParentContainerPos());

      // add floating object
      {
         Uint16 uiFloatObjPos1 = inv.Allocate();
         inv.GetObject(uiFloatObjPos1).m_uiItemID = 0x0001;
         CPPUNIT_ASSERT(true == inv.FloatObject(uiFloatObjPos1));
      }

      // drop to parent container, the topmost one
      CPPUNIT_ASSERT(true == inv.DropFloatingObject(inv.GetParentContainerPos()));
      CPPUNIT_ASSERT(0 == inv.GetNumSlots()); // current slot list is untouched

      // check if object was really added to topmost container
      // must be after first container
      CPPUNIT_ASSERT(0x0080 == inv.GetObject(0).m_uiItemID);
      CPPUNIT_ASSERT(0x0001 == inv.GetObject(1).m_uiItemID);
      CPPUNIT_ASSERT(0 == inv.GetObject(1).m_uiLink); // must have no link

      // add another container
      Uint16 uiContainerPos2 = inv.Allocate();
      inv.GetObject(uiContainerPos2).m_uiItemID = 0x0082; // a_sack
      inv.GetObject(uiContainerPos2).m_bIsQuantity = false;
      inv.AddToContainer(uiContainerPos2, uiContainerPos1);

      // open it
      inv.OpenContainer(uiContainerPos2);
      CPPUNIT_ASSERT(uiContainerPos2 == inv.GetContainerPos());
      CPPUNIT_ASSERT(uiContainerPos1 == inv.GetParentContainerPos());

      // add another floating object
      {
         Uint16 uiFloatObjPos2 = inv.Allocate();
         inv.GetObject(uiFloatObjPos2).m_uiItemID = 0x0002;
         CPPUNIT_ASSERT(true == inv.FloatObject(uiFloatObjPos2));
      }

      // drop to parent container, the parent one
      CPPUNIT_ASSERT(true == inv.DropFloatingObject(inv.GetParentContainerPos()));
      CPPUNIT_ASSERT(0 == inv.GetNumSlots()); // current slot list is untouched

      // check if item was dropped to parent container
      // two objects in list; first, the second container, then the dropped object
      {
         const Underworld::ObjectInfo& objInfo = inv.GetObject(inv.GetParentContainerPos());
         CPPUNIT_ASSERT(uiContainerPos2 == objInfo.m_uiQuantity);

         Uint16 uiNonFloatObjectPos2 = inv.GetObject(objInfo.m_uiQuantity).m_uiLink;
         CPPUNIT_ASSERT(0x0002 == inv.GetObject(uiNonFloatObjectPos2).m_uiItemID);
         CPPUNIT_ASSERT(0 == inv.GetObject(uiNonFloatObjectPos2).m_uiLink); // must have no link
      }

      // check previous container
      // has two objects; container 1 and dropped item 1
      inv.CloseContainer();
      CPPUNIT_ASSERT(2 == inv.GetNumSlots());
      CPPUNIT_ASSERT(uiContainerPos2 == inv.GetSlotListPos(0));
      CPPUNIT_ASSERT(0x0082 == inv.GetObject(inv.GetSlotListPos(0)).m_uiItemID);
      CPPUNIT_ASSERT(0x0002 == inv.GetObject(inv.GetSlotListPos(1)).m_uiItemID);

      // check topmost container
      inv.CloseContainer();
      CPPUNIT_ASSERT(8 == inv.GetNumSlots());
      CPPUNIT_ASSERT(uiContainerPos1 == inv.GetSlotListPos(0));
      CPPUNIT_ASSERT(0x0080 == inv.GetObject(inv.GetSlotListPos(0)).m_uiItemID);
      CPPUNIT_ASSERT(0x0001 == inv.GetObject(inv.GetSlotListPos(1)).m_uiItemID);
      CPPUNIT_ASSERT(Underworld::c_uiItemIDNone == inv.GetObject(inv.GetSlotListPos(2)).m_uiItemID);
   }
/*
   // test floating object functions
   {
      Underworld::Inventory inv;
      inv.Create();

      // add item
      Uint16 uiPos1 = inv.Allocate();
      inv.GetObject(uiPos1).m_uiItemID = 0x0001;

      // make floating
      inv.FloatObject(uiPos1);

      // add some more items to topmost container
      Uint16 uiPos2 = inv.Allocate();
      inv.GetObject(uiPos2).m_uiItemID = 0x0002;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos2, c_uiInventorySlotNoItem));

      Uint16 uiPos3 = inv.Allocate();
      inv.GetObject(uiPos3).m_uiItemID = 0x0003;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos3, c_uiInventorySlotNoItem));

      Uint16 uiPos4 = inv.Allocate();
      inv.GetObject(uiPos4).m_uiItemID = 0x0004;
      CPPUNIT_ASSERT(true == inv.AddToContainer(uiPos4, c_uiInventorySlotNoItem));

      // drop item in topmost container
      CPPUNIT_ASSERT(true == inv.DropFloatingObject(c_uiInventorySlotNoItem));

   }
*/
}
