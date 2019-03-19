//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file ObjectList.cpp
/// \brief object list
//
#include "pch.hpp"
#include "ObjectList.hpp"
#include "Savegame.hpp"
#include "Tilemap.hpp"

using Underworld::ObjectList;
using Underworld::ObjectPtr;
using Underworld::ObjectType;

void ObjectList::Create()
{
   m_objectList.resize(0x400);
   m_tilemapListStart.resize(c_underworldTilemapSize * c_underworldTilemapSize, g_objectListPosNone);
}

void ObjectList::Destroy()
{
   m_objectList.clear();
   m_tilemapListStart.clear();
}

/// Allocates a new object by searching next free object position in list. The
/// new object itself isn't set; use SetObject to do that.
/// \note may enlarge object list
/// \todo integrate SetObject() call by passing ObjectPtr
Uint16 ObjectList::Allocate()
{
   // start with position 1; 0 is reserved
   Uint16 pos = 1;
   Uint16 size = static_cast<Uint16>(m_objectList.size());
   while (pos < size && m_objectList[pos].get() != NULL)
      pos++;

   // hit end of list?
   if (pos >= m_objectList.size())
   {
      // already at maximum size?
      if (m_objectList.size() == 0x10000)
         throw Base::RuntimeException("Error while enlarging object list; already at maximum size");

      // new pos is start of enlarged list
      pos = static_cast<Uint16>(m_objectList.size());

      // enlarge list by factor 1,25
      unsigned int uiNewSize = m_objectList.size();
      uiNewSize += uiNewSize >> 2;

      // limit to Uint16 range
      if (uiNewSize >= 0x10000)
         uiNewSize = 0x10000;

      m_objectList.resize(uiNewSize);
   }

   return pos;
}

void ObjectList::Free(Uint16 objectPos)
{
   // object must not be part of a tile list
   UaAssert(GetObject(objectPos)->GetPosInfo().m_tileX == c_tileNotAPos);
   UaAssert(GetObject(objectPos)->GetPosInfo().m_tileY == c_tileNotAPos);
   UaAssert(objectPos < m_objectList.size());
   UaAssert(objectPos != g_objectListPosNone);
   UaAssert(m_objectList[objectPos].get() != NULL); // can only free allocated objects

   m_objectList[objectPos].reset();
}

ObjectPtr ObjectList::GetObject(Uint16 objectPos)
{
   UaAssert(objectPos < m_objectList.size());
   UaAssert(objectPos != g_objectListPosNone);

   return m_objectList[objectPos];
}

const ObjectPtr ObjectList::GetObject(Uint16 objectPos) const
{
   UaAssert(objectPos < m_objectList.size());
   UaAssert(objectPos != g_objectListPosNone);

   return m_objectList[objectPos];
}

void ObjectList::SetObject(Uint16 objectPos, const ObjectPtr& object)
{
   UaAssert(objectPos < m_objectList.size());
   UaAssert(objectPos != g_objectListPosNone);

   m_objectList[objectPos] = object;
}

Uint16 ObjectList::GetListStart(Uint8 xpos, Uint8 ypos) const
{
   UaAssert(xpos < c_underworldTilemapSize);
   UaAssert(ypos < c_underworldTilemapSize);

   return m_tilemapListStart[ypos * c_underworldTilemapSize + xpos];
}

/// It is allowed that objectPos may be equal to g_objectListPosNone, to
/// empty the list.
void ObjectList::SetListStart(Uint16 objectPos, Uint8 xpos, Uint8 ypos)
{
   UaAssert(xpos < c_underworldTilemapSize);
   UaAssert(ypos < c_underworldTilemapSize);
   UaAssert(objectPos < m_objectList.size());

   m_tilemapListStart[ypos * c_underworldTilemapSize + xpos] = objectPos;
}

/// Adds object to tile's list of objects. Adds the object to the end of the list.
void ObjectList::AddObjectToTileList(Uint16 objectPos, Uint8 xpos, Uint8 ypos)
{
   UaAssert(objectPos != g_objectListPosNone);
   UaAssert(objectPos < m_objectList.size());
   UaAssert(m_objectList[objectPos].get() != NULL);
   UaAssert(m_objectList[objectPos]->GetObjectInfo().m_link == g_objectListPosNone);

   // search end of tile
   Uint16 link = GetListStart(xpos, ypos);
   if (link == g_objectListPosNone)
   {
      // no object in tile yet
      SetListStart(objectPos, xpos, ypos);
   }
   else
   {
      // follow link to the end
      Uint16 lastLink;
      do
      {
         lastLink = link;
         link = GetObject(link)->GetObjectInfo().m_link;
      } while (link != g_objectListPosNone);

      // set as new end object
      GetObject(lastLink)->GetObjectInfo().m_link = objectPos;
      GetObject(objectPos)->GetObjectInfo().m_link = g_objectListPosNone;
   }

   m_objectList[objectPos]->GetPosInfo().m_tileX = xpos;
   m_objectList[objectPos]->GetPosInfo().m_tileY = ypos;
}

void ObjectList::RemoveObjectFromTileList(Uint16 objectPos, Uint8 xpos, Uint8 ypos)
{
   UaAssert(objectPos != g_objectListPosNone);
   UaAssert(objectPos < m_objectList.size());
   UaAssert(m_objectList[objectPos].get() != NULL);

   // search item in tile list
   Uint16 link = GetListStart(xpos, ypos);
   UaAssert(link != g_objectListPosNone);

   m_objectList[objectPos]->GetPosInfo().m_tileX = c_tileNotAPos;
   m_objectList[objectPos]->GetPosInfo().m_tileY = c_tileNotAPos;

   // first item?
   if (link == objectPos)
   {
      // set next item as list start
      SetListStart(GetObject(objectPos)->GetObjectInfo().m_link, xpos, ypos);
      GetObject(objectPos)->GetObjectInfo().m_link = g_objectListPosNone;
      return;
   }

   // follow link until item is found
   Uint16 lastLink;
   do
   {
      lastLink = link;
      link = GetObject(link)->GetObjectInfo().m_link;

      if (link == objectPos)
      {
         // found; set last object's link to the next one
         GetObject(lastLink)->GetObjectInfo().m_link = GetObject(link)->GetObjectInfo().m_link;
         GetObject(objectPos)->GetObjectInfo().m_link = g_objectListPosNone;
         return;
      }

   } while (link != g_objectListPosNone);

   UaAssert(false); // when reached here, item didn't belong in this list
}

void ObjectList::Load(Base::Savegame& sg)
{
   sg.BeginSection("objectlist");

   Destroy();
   Create();

   // read in list start positions
   unsigned int tileIndex = 0;
   for (tileIndex = 0; tileIndex < c_underworldTilemapSize * c_underworldTilemapSize; tileIndex++)
      m_tilemapListStart[tileIndex] = sg.Read16();

   // read in object list
   Uint16 size = sg.Read16();

   for (tileIndex = 0; tileIndex < size; tileIndex++)
   {
      // read in object type
      ObjectType objectType =
         static_cast<ObjectType>(sg.Read8());

      // generate proper object ptr, depending on object type
      ObjectPtr& object = m_objectList[tileIndex];
      if (objectType == ::Underworld::objectNone)
         object.reset();
      else
         if (objectType == ::Underworld::objectNormal)
            object = ObjectPtr(new Object);
         else
            if (objectType == ::Underworld::objectNpc)
               object = ObjectPtr(new NpcObject);

      // load contents
      if (object.get() != NULL)
         object->Load(sg);
   }

   sg.EndSection();
}

void ObjectList::Save(Base::Savegame& sg) const
{
   sg.BeginSection("objectlist");

   // write list with start positions
   unsigned int tileIndex = 0;
   for (tileIndex = 0; tileIndex < c_underworldTilemapSize * c_underworldTilemapSize; tileIndex++)
      sg.Write16(m_tilemapListStart[tileIndex]);

   // write out object list
   Uint16 size = static_cast<Uint16>(m_objectList.size());
   sg.Write16(size);

   for (tileIndex = 0; tileIndex < size; tileIndex++)
   {
      // write out object type
      const ObjectPtr& object = m_objectList[tileIndex];
      if (object.get() == NULL)
         sg.Write8(static_cast<Uint8>(::Underworld::objectNone));
      else
      {
         sg.Write8(static_cast<Uint8>(object->GetObjectType()));

         // write out object infos, too
         object->Save(sg);
      }
   }

   sg.EndSection();
}

/// Compacts object list by rearranging objects and adjusting links and
/// special links; note that no object list positions must be kept, since
/// they will be invalidated after calling this function.
/// \todo implement
void ObjectList::Compact()
{
}
