/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

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
/*! \file inventory.hpp

   \brief inventory

*/

// include guard
#ifndef uwadv_underw_inventory_hpp_
#define uwadv_underw_inventory_hpp_

// needed includes
#include <vector>
#include "object.hpp"

namespace Base
{
   class Savegame;
}

namespace Underworld
{

//! inventory position that indicates "no item"
const Uint16 c_uiInventorySlotNoItem = 0xffff;


//! inventory slot
enum EInventoryPaperdollSlot
{
   slotTopmostFirstItem = 0, // topmost inventory; first item list pos

   slotLeftHand = 8,
   slotRightHand,
   slotLeftShoulder,
   slotRightShoulder,

   slotLeftFinger,
   slotRightFinger,

   slotPaperdollLegs,
   slotPaperdollChest,
   slotPaperdollHands,
   slotPaperdollFeet,
   slotPaperdollHead,

   slotMax,                // from here on indices in object list are free

   slotPlayerObjectsStart = slotLeftHand, // first slot with objects held by player
   slotPaperdollStart = slotPaperdollLegs // first slot with paperdoll objects
};

// classes

//! Inventory
/*! The inventory is organized as a list with object infos held in an ObjectInfo
    struct. The first 8 entries in this list are the items in the topmost
    inventory view (where you usually place bags and other containers). The
    next entries are for the paperdoll object on the paperdoll. After these
    the list for objects in containers start; this area is used dynamically to
    store object infos.

    When entering containers, a stack with containers previously opened is
    managed to let the user return to the container above.

    The inventory objects of the current container is given as a "slot list"
    that contains all objects that are currently visible. The slot list can
    have various lengths. When in the top inventory, there are 8 fixed slots.
    When in a container that e.g. only supports n slots, the list can only get
    as long as n slots.

    The inventory also manages a single object that can float above the other
    icons, e.g. for reordering inventory or for dropping into the underworld.
*/
class Inventory
{
public:
   //! ctor
   Inventory();

   //! create inventory
   void Create();

   //! destroys inventory contents
   void Destroy();

   // object handling

   //! allocates a new object slot
   Uint16 Allocate();

   //! frees object slot
   void Free(Uint16 uiPos);

   //! retrieves an object from the list
   ObjectInfo& GetObject(Uint16 uiIndex)
   {
      UaAssert(uiIndex < m_vecObjectList.size());
      return m_vecObjectList[uiIndex];
   }

   //! retrieves an object from the list
   const ObjectInfo& GetObject(Uint16 uiIndex) const
   {
      UaAssert(uiIndex < m_vecObjectList.size());
      return m_vecObjectList[uiIndex];
   }

   // slot list handling

   //! returns number of slots in list
   unsigned int GetNumSlots() const { return m_vecSlotList.size(); }

   //! returns object position of object stored in one of the slots
   Uint16 GetSlotListPos(unsigned int uiIndex) const;

   // container handling

   //! returns true if the given item id is a container item
   bool IsContainer(Uint16 uiItemID) const;

   //! returns current container object position, or c_uiInventorySlotNoItem when topmost
   Uint16 GetContainerPos() const;

   //! returns container object position of parent, or c_uiInventorySlotNoItem when topmost
   Uint16 GetParentContainerPos() const;

   //! opens a container and sets it as current container
   void OpenContainer(Uint16 uiPos);

   //! closes current container and enables the previous one
   void CloseContainer();

   //! adds object to container
   bool AddToContainer(Uint16& uiPos, Uint16 uiContainerPos);

   //! removes object from container
   void RemoveFromContainer(Uint16 uiPos, Uint16 uiContainerPos);

   // floating item support

   //! returns position of currently floating item, or c_uiInventorySlotNoItem when none
   Uint16 GetFloatingObjectPos() const { return m_uiFloatingObjectPos; }

   //! makes an object in the inventory "floating"
   bool FloatObject(Uint16 uiPos);

   //! drops floating object into container given by container position
   bool DropFloatingObject(Uint16 uiContainerPos, Uint16 uiObjectIndex = c_uiInventorySlotNoItem);

   // loading / saving

   //! loads inventory from savegame
   void Load(Base::Savegame& sg);

   //! saves inventory to savegame
   void Save(Base::Savegame& sg) const;

private:
   //! builds new slot list from given container link
   void BuildSlotList(Uint16 uiLink);

   //! drops floating object on given one
   bool DropOnObject(Uint16 uiContainerPos, Uint16 uiPos);

private:
   //! object list
   std::vector<ObjectInfo> m_vecObjectList;

   //! list with all objects in current slot list
   std::vector<Uint16> m_vecSlotList;

   //! stack with currently opened containers (stored as object list position)
   std::vector<Uint16> m_vecContainerStack;

   //! floating object position
   Uint16 m_uiFloatingObjectPos;
};

#if 0
// classes

//! inventory
class Inventory
{
public:
   //! calculates weight of inventory in stones
   double get_inventory_weight();

   //! calculates weight of items in given container in stones
   double get_container_weight(Uint16 cont_pos);

   // floating object functionality

   //! returns currently floating item, or ua_slot_no_item when none floats
   Uint16 get_floating_item();

   //! makes an item "floating", by slot index
   bool float_item_slot(Uint16 slot_index);

   //! makes an item in the itemlist "floating"
   bool float_item(Uint16 index);

   //! drops a floating item into the given slot
   bool drop_floating_item_slot(Uint16 slot_index);

   //! drops a floating item onto the given itemlist index
   bool drop_floating_item(Uint16 index);

   //! drops a floating item into the parent's list
   bool drop_floating_item_parent();

   //! inserts new item and makes it floating; returns itemlist pos
   Uint16 insert_floating_item(ua_object_info& objinfo);

protected:
   //! removes an object from current inventory by unlinking it
   void unlink_object(Uint16 item);

   //! appends item to link1'ed container list
   void append_item(Uint16 cont, Uint16 item);
};

#endif

} // namespace Underworld

#endif
