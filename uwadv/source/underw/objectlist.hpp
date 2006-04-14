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
/*! \file objectlist.hpp

   \brief object list

*/

// include guard
#ifndef uwadv_underw_objectlist_hpp_
#define uwadv_underw_objectlist_hpp_

// needed includes
#include <vector>
#include "object.hpp"

namespace Base
{
   class Savegame;
}

namespace Underworld
{

//! object position for "no position"
const Uint16 g_uiObjectListPosNone = 0x0000;


//! object list
class ObjectList
{
public:
   //! ctor
   ObjectList(){}

   //! create object list
   void Create();

   //! destroys object list contents
   void Destroy();

   //! allocates a new object list slot
   Uint16 Allocate();

   //! frees object list slot
   void Free(Uint16 uiObjectPos);

   //! returns object from object list position
   ObjectPtr GetObject(Uint16 uiObjectPos);

   //! sets new object for given object position
   void SetObject(Uint16 uiObjectPos, const ObjectPtr& pObj);

   //! returns object list start for tile on given coordinates
   Uint16 GetListStart(Uint8 xpos, Uint8 ypos) const;

   //! adds object to list for given tile
   void AddObjectToTileList(Uint16 uiObjectPos, Uint8 xpos, Uint8 ypos);

   //! removes object from list for given tile
   void RemoveObjectFromTileList(Uint16 uiObjectPos, Uint8 xpos, Uint8 ypos);

   // loading / saving

   //! loads object list from savegame
   void Load(Base::Savegame& sg);

   //! saves object list to savegame
   void Save(Base::Savegame& sg) const;

   //! compacts object list
   void Compact();

private:
   //! sets object list start for tile on given coordinates
   void SetListStart(Uint16 uiObjectPos, Uint8 xpos, Uint8 ypos);

private:
   //! master object list
   std::vector<ObjectPtr> m_objectList;

   //! object list start positions for all tiles in tilemap
   std::vector<Uint16> m_tilemapListStart;
};

} // namespace Underworld

#endif
