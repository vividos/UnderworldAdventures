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
/// \file UnderworldTest.cpp
/// \brief Underworld classes test
//
#include "UnitTest.hpp"
#include "Savegame.hpp"
#include "FileSystem.hpp"
#include "Underworld.hpp"
#include "ObjectList.hpp"
#include "Inventory.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
   using Underworld::c_inventorySlotNoItem;

   /// \brief Underworld classes test
   /// Tests underworld main functionality.
   TEST_CLASS(UnderworldTest)
   {
      /// Tests saving and restoring underworld.
      /// \todo insert data into various underworld objects and check if the data
      ///       is available after loading.
      TEST_METHOD(TestSaveLoadSavegames)
      {
         TempFolder testFolder;
         std::string strSavegameFile = testFolder.GetPathName() + "/savegame.uas";

         Base::SavegameInfo info;
         info.m_title = "title01";

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

      /// Tests object list functions; simple allocation/free
      TEST_METHOD(TestObjectList_AllocFree)
      {
         Underworld::ObjectList ol;
         ol.Create();

         Uint16 uiPos = ol.Allocate();
         Assert::IsTrue(uiPos != 0);

         ol.SetObject(uiPos, Underworld::ObjectPtr(new Underworld::Object));

         ol.Free(uiPos);
         ol.Destroy();
      }

      /// Tests object list functions; tile list: add as first object, remove first object
      TEST_METHOD(TestObjectList_AddRemoveFirstObject)
      {
         Underworld::ObjectList ol;
         ol.Create();
         Assert::IsTrue(Underworld::g_objectListPosNone == ol.GetListStart(32, 16));

         Uint16 uiPos = ol.Allocate();
         ol.SetObject(uiPos, Underworld::ObjectPtr(new Underworld::Object));

         ol.AddObjectToTileList(uiPos, 32, 16);
         Assert::IsTrue(uiPos == ol.GetListStart(32, 16));

         ol.RemoveObjectFromTileList(uiPos, 32, 16);
         Assert::IsTrue(Underworld::g_objectListPosNone == ol.GetListStart(32, 16));
      }

      /// Tests object list functions; tile list: add some objects, remove from front, remove from
      /// end, remove in between
      TEST_METHOD(TestObjectList_RemoveObjects)
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

         Assert::IsTrue(uiPos1 != uiPos2);
         Assert::IsTrue(uiPos1 != uiPos3);
         Assert::IsTrue(uiPos1 != uiPos4);
         Assert::IsTrue(uiPos1 != uiPos5);
         Assert::IsTrue(uiPos2 != uiPos3);
         Assert::IsTrue(uiPos2 != uiPos4);
         Assert::IsTrue(uiPos2 != uiPos5);
         Assert::IsTrue(uiPos3 != uiPos4);
         Assert::IsTrue(uiPos5 != uiPos4);

         ol.AddObjectToTileList(uiPos1, 32, 16);
         ol.AddObjectToTileList(uiPos2, 32, 16);
         ol.AddObjectToTileList(uiPos3, 32, 16);
         ol.AddObjectToTileList(uiPos4, 32, 16);
         ol.AddObjectToTileList(uiPos5, 32, 16);

         // remove ...
         ol.RemoveObjectFromTileList(uiPos1, 32, 16); // from front
         Assert::IsTrue(ol.GetObject(uiPos1)->GetObjectInfo().m_link == Underworld::g_objectListPosNone);

         ol.RemoveObjectFromTileList(uiPos5, 32, 16); // from back
         Assert::IsTrue(ol.GetObject(uiPos5)->GetObjectInfo().m_link == Underworld::g_objectListPosNone);

         ol.RemoveObjectFromTileList(uiPos3, 32, 16); // in the middle
         Assert::IsTrue(ol.GetObject(uiPos3)->GetObjectInfo().m_link == Underworld::g_objectListPosNone);

         ol.RemoveObjectFromTileList(uiPos4, 32, 16); // again, from back
         Assert::IsTrue(ol.GetObject(uiPos4)->GetObjectInfo().m_link == Underworld::g_objectListPosNone);

         // only object 2 is in list now
         Assert::IsTrue(uiPos2 == ol.GetListStart(32, 16));
         Assert::IsTrue(ol.GetObject(uiPos2)->GetObjectInfo().m_link == Underworld::g_objectListPosNone);

         // remove last
         ol.RemoveObjectFromTileList(uiPos2, 32, 16);
         Assert::IsTrue(ol.GetObject(uiPos2)->GetObjectInfo().m_link == Underworld::g_objectListPosNone);
         Assert::IsTrue(Underworld::g_objectListPosNone == ol.GetListStart(32, 16));
      }

      /// Tests object list functions; allocate 0x401 entries, exceeding default object list size
      TEST_METHOD(TestObjectList_ExceedDefaultSize)
      {
         Underworld::ObjectList ol;
         ol.Create();

         std::vector<Uint16> vecAllPos;
         vecAllPos.reserve(0x401);

         for (unsigned int ui = 0; ui < 0x401; ui++)
         {
            Uint16 uiPos = ol.Allocate();
            Assert::IsTrue(uiPos != 0);
            ol.SetObject(uiPos, Underworld::ObjectPtr(new Underworld::Object));

            ol.AddObjectToTileList(uiPos, 32, 16);

            vecAllPos.push_back(uiPos);
         }

         // to test compaction, delete every other element
         {
            unsigned int uiMax = vecAllPos.size();
            for (unsigned int ui = 0; ui < uiMax; ui += 2)
            {
               ol.RemoveObjectFromTileList(vecAllPos[ui], 32, 16);
               ol.Free(vecAllPos[ui]);
            }

            ol.Compact();
         }

         ol.Destroy();
      }

      /// Tests inventory functions; simple object allocation
      TEST_METHOD(TestInventory_ObjectAlloc)
      {
         Underworld::Inventory inv;
         inv.Create();

         UaAssert(c_inventorySlotNoItem == inv.GetContainerPos());
         UaAssert(c_inventorySlotNoItem == inv.GetFloatingObjectPos());

         Uint16 uiPos = inv.Allocate();
         inv.GetObjectInfo(uiPos).m_itemID = 0x0001;

         inv.Free(uiPos);

         inv.Destroy();
      }

      /// Tests inventory functions; test opening / closing container
      TEST_METHOD(TestInventory_OpenCloseContainer)
      {
         Underworld::Inventory inv;
         inv.Create();

         // add container
         Uint16 uiContainerPos = inv.Allocate();
         inv.GetObjectInfo(uiContainerPos).m_itemID = 0x0080; // a_sack
         inv.GetObjectInfo(uiContainerPos).m_isQuantity = false;

         // open it
         inv.OpenContainer(uiContainerPos);
         Assert::IsTrue(uiContainerPos == inv.GetContainerPos());
         Assert::IsTrue(c_inventorySlotNoItem == inv.GetParentContainerPos());

         // and close it
         inv.CloseContainer();
         Assert::IsTrue(c_inventorySlotNoItem == inv.GetContainerPos());
         Assert::IsTrue(c_inventorySlotNoItem == inv.GetParentContainerPos());
      }

      /// Tests inventory functions; test slot list building for topmost container
      TEST_METHOD(TestInventory_SlotListBuildingTopmostContainer)
      {
         Underworld::Inventory inv;
         inv.Create();

         Assert::IsTrue(0 == inv.GetNumSlots());

         // add 4 objects
         Uint16 uiPos1 = inv.Allocate();
         inv.GetObjectInfo(uiPos1).m_itemID = 0x0001;
         Assert::IsTrue(true == inv.AddToContainer(uiPos1, c_inventorySlotNoItem));
         Assert::IsTrue(uiPos1 == 0); // move to position 0
         Assert::IsTrue(0 == inv.GetObjectInfo(uiPos1).m_link); // assume link field is empty for topmost items

         Uint16 uiPos2 = inv.Allocate();
         inv.GetObjectInfo(uiPos2).m_itemID = 0x0002;
         Assert::IsTrue(true == inv.AddToContainer(uiPos2, c_inventorySlotNoItem));
         Assert::IsTrue(uiPos2 == 1); // move to position 1
         Assert::IsTrue(0 == inv.GetObjectInfo(uiPos2).m_link);

         Uint16 uiPos3 = inv.Allocate();
         inv.GetObjectInfo(uiPos3).m_itemID = 0x0003;
         Assert::IsTrue(true == inv.AddToContainer(uiPos3, c_inventorySlotNoItem));
         Assert::IsTrue(uiPos3 == 2); // move to position 2
         Assert::IsTrue(0 == inv.GetObjectInfo(uiPos3).m_link);

         Uint16 uiPos4 = inv.Allocate();
         inv.GetObjectInfo(uiPos4).m_itemID = 0x0004;
         Assert::IsTrue(true == inv.AddToContainer(uiPos4, c_inventorySlotNoItem));
         Assert::IsTrue(uiPos4 == 3); // move to position 3
         Assert::IsTrue(0 == inv.GetObjectInfo(uiPos4).m_link);

         // in topmost container there always are 8 slots
         Assert::IsTrue(8 == inv.GetNumSlots());

         // remove some items from topmost container
         inv.RemoveFromContainer(uiPos1, c_inventorySlotNoItem);
         Assert::IsTrue(inv.GetObjectInfo(uiPos1).m_link == 0);
         inv.Free(uiPos1);
         Assert::IsTrue(inv.GetObjectInfo(uiPos1).m_itemID == c_inventorySlotNoItem);

         inv.RemoveFromContainer(uiPos4, c_inventorySlotNoItem);
         Assert::IsTrue(inv.GetObjectInfo(uiPos4).m_link == 0);
         inv.Free(uiPos4);
         Assert::IsTrue(inv.GetObjectInfo(uiPos4).m_itemID == c_inventorySlotNoItem);

         // still 8 slots in the topmost container
         Assert::IsTrue(8 == inv.GetNumSlots());
      }

      /// Tests inventory functions; test slot list building for container
      TEST_METHOD(TestInventory_SlotListBuildingContainer)
      {
         Underworld::Inventory inv;
         inv.Create();

         Uint16 uiPosContainer = inv.Allocate();
         inv.GetObjectInfo(uiPosContainer).m_itemID = 0x0080; // a_sack
         inv.GetObjectInfo(uiPosContainer).m_isQuantity = false; // m_quantity is a link
         Assert::IsTrue(true == inv.AddToContainer(uiPosContainer, c_inventorySlotNoItem));

         // always 8 slots in topmost container
         Assert::IsTrue(8 == inv.GetNumSlots());

         inv.OpenContainer(uiPosContainer);

         Assert::IsTrue(0 == inv.GetNumSlots()); // no object yet

         Uint16 uiPos1 = inv.Allocate();
         inv.GetObjectInfo(uiPos1).m_itemID = 0x0001;
         Assert::IsTrue(true == inv.AddToContainer(uiPos1, uiPosContainer));
         Assert::IsTrue(1 == inv.GetNumSlots());

         Uint16 uiPos2 = inv.Allocate();
         inv.GetObjectInfo(uiPos2).m_itemID = 0x0002;
         Assert::IsTrue(true == inv.AddToContainer(uiPos2, uiPosContainer));
         Assert::IsTrue(2 == inv.GetNumSlots());

         Uint16 uiPos3 = inv.Allocate();
         inv.GetObjectInfo(uiPos3).m_itemID = 0x0003;
         Assert::IsTrue(true == inv.AddToContainer(uiPos3, uiPosContainer));
         Assert::IsTrue(3 == inv.GetNumSlots());

         Uint16 uiPos4 = inv.Allocate();
         inv.GetObjectInfo(uiPos4).m_itemID = 0x0004;
         Assert::IsTrue(true == inv.AddToContainer(uiPos4, uiPosContainer));
         Assert::IsTrue(4 == inv.GetNumSlots());

         // remove from front
         inv.RemoveFromContainer(uiPos1, uiPosContainer);
         Assert::IsTrue(3 == inv.GetNumSlots());
         inv.Free(uiPos1);

         // remove from last
         inv.RemoveFromContainer(uiPos4, uiPosContainer);
         Assert::IsTrue(2 == inv.GetNumSlots());
         inv.Free(uiPos4);

         // remove from in between
         inv.RemoveFromContainer(uiPos3, uiPosContainer);
         Assert::IsTrue(1 == inv.GetNumSlots());
         inv.Free(uiPos3);

         inv.CloseContainer();

         // still 8 slots in the topmost container
         Assert::IsTrue(8 == inv.GetNumSlots());
      }

      /// Tests inventory functions; allocate all items, causing item list enlargement
      TEST_METHOD(TestInventory_ItemListResize)
      {
         Underworld::Inventory inv;
         inv.Create();

         for (unsigned int i = 0; i < 256; i++)
         {
            Uint16 uiPos = inv.Allocate();
            inv.GetObjectInfo(uiPos).m_itemID = 0x0001;
         }
      }

      /// Tests inventory functions; test dropping object to parent container
      /// - dropping objects to the symbol of the currently open container (the parent)
      /// - dropping objects to an empty place in the container, basically adding
      ///   it to the end of the slot list
      /// - dropping objects to the paperdoll or paperdoll slots
      /// - dropping object to an item in the container; this generates three further
      ///   possibilities:
      ///   - when the object is a container, drop item in this container (if possible)
      ///   - when the objects can be combined, the new object is dropped (there still
      ///     may be a floating object afterwards, e.g. lighting incense with a torch)
      ///   - when the objects can't be combined, the objects just swap
      TEST_METHOD(TestInventory_DropParent)
      {
         Underworld::Inventory inv;
         inv.Create();

         // add container
         Uint16 uiContainerPos1 = inv.Allocate();
         inv.GetObjectInfo(uiContainerPos1).m_itemID = 0x0080; // a_sack
         inv.GetObjectInfo(uiContainerPos1).m_isQuantity = false;
         inv.AddToContainer(uiContainerPos1, c_inventorySlotNoItem);

         // open it
         inv.OpenContainer(uiContainerPos1);
         Assert::IsTrue(uiContainerPos1 == inv.GetContainerPos());
         Assert::IsTrue(c_inventorySlotNoItem == inv.GetParentContainerPos());

         // add floating object
         {
            Uint16 uiFloatObjPos1 = inv.Allocate();
            inv.GetObjectInfo(uiFloatObjPos1).m_itemID = 0x0001;
            Assert::IsTrue(true == inv.FloatObject(uiFloatObjPos1));
         }

         // drop to parent container, the topmost one
         Assert::IsTrue(true == inv.DropFloatingObject(inv.GetParentContainerPos()));
         Assert::IsTrue(0 == inv.GetNumSlots()); // current slot list is untouched

         // check if object was really added to topmost container
         // must be after first container
         Assert::IsTrue(0x0080 == inv.GetObjectInfo(0).m_itemID);
         Assert::IsTrue(0x0001 == inv.GetObjectInfo(1).m_itemID);
         Assert::IsTrue(0 == inv.GetObjectInfo(1).m_link); // must have no link

         // add another container
         Uint16 uiContainerPos2 = inv.Allocate();
         inv.GetObjectInfo(uiContainerPos2).m_itemID = 0x0082; // a_sack
         inv.GetObjectInfo(uiContainerPos2).m_isQuantity = false;
         inv.AddToContainer(uiContainerPos2, uiContainerPos1);

         // open it
         inv.OpenContainer(uiContainerPos2);
         Assert::IsTrue(uiContainerPos2 == inv.GetContainerPos());
         Assert::IsTrue(uiContainerPos1 == inv.GetParentContainerPos());

         // add another floating object
         {
            Uint16 uiFloatObjPos2 = inv.Allocate();
            inv.GetObjectInfo(uiFloatObjPos2).m_itemID = 0x0002;
            Assert::IsTrue(true == inv.FloatObject(uiFloatObjPos2));
         }

         // drop to parent container, the parent one
         Assert::IsTrue(true == inv.DropFloatingObject(inv.GetParentContainerPos()));
         Assert::IsTrue(0 == inv.GetNumSlots()); // current slot list is untouched

         // check if item was dropped to parent container
         // two objects in list; first, the second container, then the dropped object
         {
            const Underworld::ObjectInfo& objInfo = inv.GetObjectInfo(inv.GetParentContainerPos());
            Assert::IsTrue(uiContainerPos2 == objInfo.m_quantity);

            Uint16 uiNonFloatObjectPos2 = inv.GetObjectInfo(objInfo.m_quantity).m_link;
            Assert::IsTrue(0x0002 == inv.GetObjectInfo(uiNonFloatObjectPos2).m_itemID);
            Assert::IsTrue(0 == inv.GetObjectInfo(uiNonFloatObjectPos2).m_link); // must have no link
         }

         // check previous container
         // has two objects; container 1 and dropped item 1
         inv.CloseContainer();
         Assert::IsTrue(2 == inv.GetNumSlots());
         Assert::IsTrue(uiContainerPos2 == inv.GetSlotListPos(0));
         Assert::IsTrue(0x0082 == inv.GetObjectInfo(inv.GetSlotListPos(0)).m_itemID);
         Assert::IsTrue(0x0002 == inv.GetObjectInfo(inv.GetSlotListPos(1)).m_itemID);

         // check topmost container
         inv.CloseContainer();
         Assert::IsTrue(8 == inv.GetNumSlots());
         Assert::IsTrue(uiContainerPos1 == inv.GetSlotListPos(0));
         Assert::IsTrue(0x0080 == inv.GetObjectInfo(inv.GetSlotListPos(0)).m_itemID);
         Assert::IsTrue(0x0001 == inv.GetObjectInfo(inv.GetSlotListPos(1)).m_itemID);
         Assert::IsTrue(Underworld::c_itemIDNone == inv.GetObjectInfo(inv.GetSlotListPos(2)).m_itemID);
      }

      /// test floating object functions
      TEST_METHOD(TestInventory_FloatObjects)
      {
         Underworld::Inventory inv;
         inv.Create();

         // add item
         Uint16 uiPos1 = inv.Allocate();
         inv.GetObjectInfo(uiPos1).m_itemID = 0x0001;

         // make floating
         inv.FloatObject(uiPos1);

         // add some more items to topmost container
         Uint16 uiPos2 = inv.Allocate();
         inv.GetObjectInfo(uiPos2).m_itemID = 0x0002;
         Assert::IsTrue(true == inv.AddToContainer(uiPos2, c_inventorySlotNoItem));

         Uint16 uiPos3 = inv.Allocate();
         inv.GetObjectInfo(uiPos3).m_itemID = 0x0003;
         Assert::IsTrue(true == inv.AddToContainer(uiPos3, c_inventorySlotNoItem));

         Uint16 uiPos4 = inv.Allocate();
         inv.GetObjectInfo(uiPos4).m_itemID = 0x0004;
         Assert::IsTrue(true == inv.AddToContainer(uiPos4, c_inventorySlotNoItem));

         // drop item in topmost container
         Assert::IsTrue(true == inv.DropFloatingObject(c_inventorySlotNoItem));
      }
   };
} // namespace UnitTest
