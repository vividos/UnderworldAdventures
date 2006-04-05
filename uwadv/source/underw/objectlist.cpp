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
/*! \file objectlist.cpp

   \brief object list

*/

// needed includes
#include "underworld.hpp"
#include "objectlist.hpp"

using Underworld::ObjectList;
using Underworld::ObjectPtr;
using ::Underworld::EObjectType;

// ObjectList methods

void ObjectList::Create()
{
   m_objectList.resize(0x400);
   m_tilemapListStart.resize(64*64, g_uiObjectListPosNone);
}

void ObjectList::Destroy()
{
   m_objectList.clear();
   m_tilemapListStart.clear();
}

/*! Allocates a new object by searching next free object position in list. The
    new object itself isn't set; use SetObject to do that.
    \note may enlarge object list
    \todo integrate SetObject() call by passing ObjectPtr
*/
Uint16 ObjectList::Allocate()
{
   // start with position 1; 0 is reserved
   Uint16 uiPos = 1;
   while (uiPos < m_objectList.size() && m_objectList[uiPos].get() != NULL)
      uiPos++;

   // hit end of list?
   if (uiPos >= m_objectList.size())
   {
      // new pos is start of enlarged list
      uiPos = static_cast<Uint16>(m_objectList.size());

      // enlarge list by factor 2
      m_objectList.resize(m_objectList.size()*2);
   }

   return uiPos;
}

/*! \todo check m_uiTileX and m_uiTileY */
void ObjectList::Free(Uint16 uiObjectPos)
{
   // object must not be part of a tile list
   //UaAssert(GetObject(uiObjectPos)->GetPosInfo().m_uiTileX == 0xff);
   //UaAssert(GetObject(uiObjectPos)->GetPosInfo().m_uiTileY == 0xff);
   UaAssert(uiObjectPos < m_objectList.size());
   UaAssert(uiObjectPos != g_uiObjectListPosNone);

   m_objectList[uiObjectPos].reset();
}

ObjectPtr ObjectList::GetObject(Uint16 uiObjectPos)
{
   UaAssert(uiObjectPos < m_objectList.size());
   UaAssert(uiObjectPos != g_uiObjectListPosNone);

   return m_objectList[uiObjectPos];
}

void ObjectList::SetObject(Uint16 uiObjectPos, const ObjectPtr& pObj)
{
   UaAssert(uiObjectPos < m_objectList.size());
   UaAssert(uiObjectPos != g_uiObjectListPosNone);

   m_objectList[uiObjectPos] = pObj;
}

Uint16 ObjectList::GetListStart(unsigned int xpos, unsigned int ypos) const
{
   UaAssert(xpos < 64);
   UaAssert(ypos < 64);

   return m_tilemapListStart[ypos*64+xpos];
}

/*! It is allowed that uiObjectPos may be equal to g_uiObjectListPosNone, to
    empty the list.
*/
void ObjectList::SetListStart(Uint16 uiObjectPos, unsigned int xpos, unsigned int ypos)
{
   UaAssert(xpos < 64);
   UaAssert(ypos < 64);
   UaAssert(uiObjectPos < m_objectList.size());

   m_tilemapListStart[ypos*64+xpos] = uiObjectPos;
}

/*! Adds object to tile's list of objects. Adds the object to the end of the
    list.
    \todo also reset m_uiTileX and m_uiTileY fields of object
*/
void ObjectList::AddObjectToTileList(Uint16 uiObjectPos, unsigned int xpos, unsigned int ypos)
{
   UaAssert(uiObjectPos != g_uiObjectListPosNone);
   UaAssert(uiObjectPos < m_objectList.size());
   UaAssert(m_objectList[uiObjectPos].get() != NULL);
   UaAssert(m_objectList[uiObjectPos]->GetObjectInfo().m_uiLink == g_uiObjectListPosNone);

   // search end of tile
   Uint16 uiLink = GetListStart(xpos, ypos);
   if (uiLink == g_uiObjectListPosNone)
   {
      // no object in tile yet
      SetListStart(uiObjectPos, xpos, ypos);
   }
   else
   {
      // follow link to the end
      Uint16 uiLastLink;
      do
      {
         uiLastLink = uiLink;
         uiLink = GetObject(uiLink)->GetObjectInfo().m_uiLink;
      } while(uiLink != g_uiObjectListPosNone);

      // set as new end object
      GetObject(uiLastLink)->GetObjectInfo().m_uiLink = uiObjectPos;
      GetObject(uiObjectPos)->GetObjectInfo().m_uiLink = g_uiObjectListPosNone;
   }
}

/*! \todo also reset m_uiTileX and m_uiTileY fields of object */
void ObjectList::RemoveObjectFromTileList(Uint16 uiObjectPos, unsigned int xpos, unsigned int ypos)
{
   UaAssert(uiObjectPos != g_uiObjectListPosNone);
   UaAssert(uiObjectPos < m_objectList.size());
   UaAssert(m_objectList[uiObjectPos].get() != NULL);

   // search item in tile list
   Uint16 uiLink = GetListStart(xpos, ypos);
   UaAssert(uiLink != g_uiObjectListPosNone);

   // first item?
   if (uiLink == uiObjectPos)
   {
      // set next item as list start
      SetListStart(GetObject(uiObjectPos)->GetObjectInfo().m_uiLink, xpos, ypos);
      GetObject(uiObjectPos)->GetObjectInfo().m_uiLink = g_uiObjectListPosNone;
      return;
   }

   // follow link until item is found
   Uint16 uiLastLink;
   do
   {
      uiLastLink = uiLink;
      uiLink = GetObject(uiLink)->GetObjectInfo().m_uiLink;

      if (uiLink == uiObjectPos)
      {
         // found; set last object's link to the next one
         GetObject(uiLastLink)->GetObjectInfo().m_uiLink = GetObject(uiLink)->GetObjectInfo().m_uiLink;
         GetObject(uiObjectPos)->GetObjectInfo().m_uiLink = g_uiObjectListPosNone;
         return;
      }

   } while(uiLink != g_uiObjectListPosNone);

   UaAssert(false); // when reached here, item didn't belong in this list
}

void ObjectList::Load(Base::Savegame& sg)
{
   sg.BeginSection("objectlist");

   Destroy();
   Create();

   // read in list start positions
   unsigned int ui=0;
   for(ui=0; ui<64*64; ui++)
      m_tilemapListStart[ui] = sg.Read16();

   // read in object list
   Uint16 uiSize = sg.Read16();

   for(ui=0; ui<uiSize; ui++)
   {
      // read in object type
      EObjectType objectType =
         static_cast<EObjectType>(sg.Read8());

      // generate proper object ptr, depending on object type
      ObjectPtr& pObj = m_objectList[ui];
      if (objectType == ::Underworld::objectNone)
         pObj.reset();
      else
      if (objectType == ::Underworld::objectNormal)
         pObj = ObjectPtr(new Object);
      else
      if (objectType == ::Underworld::objectNpc)
         pObj = ObjectPtr(new NpcObject);

      // load contents
      if (pObj.get() != NULL)
         pObj->Load(sg);
   }

   sg.EndSection();
}

void ObjectList::Save(Base::Savegame& sg) const
{
   sg.BeginSection("objectlist");

   // write list with start positions
   unsigned int ui=0;
   for(ui=0; ui<64*64; ui++)
      sg.Write16(m_tilemapListStart[ui]);

   // write out object list
   Uint16 uiSize = static_cast<Uint16>(m_objectList.size());
   sg.Write16(uiSize);

   for(ui=0; ui<uiSize; ui++)
   {
      // write out object type
      const ObjectPtr& pObj = m_objectList[ui];
      if (pObj.get() == NULL)
         sg.Write8(static_cast<Uint8>(::Underworld::objectNone));
      else
      {
         sg.Write8(static_cast<Uint8>(pObj->GetObjectType()));

         // write out object infos, too
         pObj->Save(sg);
      }
   }

   sg.EndSection();
}

/*! Compacts object list by rearranging objects and adjusting links and
    special links; note that no object list positions must be kept, since
    they will be invalidated after calling this function.
    \todo implement
*/
void ObjectList::CompactObjectList()
{
}
