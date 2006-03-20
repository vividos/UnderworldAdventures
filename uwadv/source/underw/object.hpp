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
/*! \file object.hpp

   \brief object

*/

// include guard
#ifndef uwadv_underw_object_hpp_
#define uwadv_underw_object_hpp_

// needed includes
#include "smart_ptr.hpp"

namespace Base
{
   class Savegame;
}

namespace Underworld
{

//! item id for unused object
const Uint16 g_uiItemIDNone = 0xffff;

// enums

//! object type
enum EObjectType
{
   objectNone,    //!< not an object
   objectNormal=1,//!< normal object of type Object
   objectNpc=2,   //!< object of type NpcObject
};

// structs

//! general object info
struct ObjectInfo
{
   ObjectInfo()
      :m_uiItemID(g_uiItemIDNone),
      m_uiLink(0),
      m_uiQuality(0),
      m_uiOwner(0),
      m_uiQuantity(0),
      m_uiFlags(0),
      m_bEnchanted(false),
      m_bHidden(false),
      m_bIsQuantity(true)
   {
   }

   Uint16 m_uiItemID;   //!<< object item id
   Uint16 m_uiLink;     //!< object chain link
   Uint16 m_uiQuality;  //!< quality
   Uint16 m_uiOwner;    //!< owner / special field
   Uint16 m_uiQuantity; //!< quantity / special / special link
   Uint16 m_uiFlags;    //!< object flags

   // flags
   bool m_bEnchanted;   //!< true when object is enchanted
   bool m_bHidden;      //!< indicates if object is hidden or visible
   bool m_bIsQuantity;  //!< true when "quantity" field is a quantity/special property

   void Load(Base::Savegame& sg);
   void Save(Base::Savegame& sg) const;
};

//! object position info
struct ObjectPositionInfo
{
   ObjectPositionInfo()
      :m_xpos(0), m_ypos(0), m_zpos(0),
      m_uiHeading(0), m_uiTileX(0xff), m_uiTileY(0xff)
   {
   }

   Uint8 m_xpos;  //!< fractional x position in tile (0..7)
   Uint8 m_ypos;  //!< fractional y position in tile (0..7)
   Uint8 m_zpos;  //!< z position (0..127)
   Uint8 m_uiHeading;   //!< direction (0..7)
   Uint8 m_uiTileX;     //!< tilemap x coordinate
   Uint8 m_uiTileY;     //!< tilemap y coordinate

   void Load(Base::Savegame& sg);
   void Save(Base::Savegame& sg) const;
};

//! npc info
/*! Most members of this struct directly correspond to the variables available
    in conversations.
*/
struct NpcInfo
{
   //! \todo use enums where values are known and fixed
   NpcInfo()
      :m_uiNpc_hp(0),
      m_uiNpc_goal(0),
      m_uiNpc_gtarg(0),
      m_uiNpc_level(0),
      m_bNpc_talkedto(false),
      m_uiNpc_attitude(0),
      m_uiNpc_xhome(0),
      m_uiNpc_yhome(0),
      m_uiNpc_hunger(0),
      m_uiNpc_whoami(0),
      m_uiAnimationState(0),
      m_uiAnimationFrame(0)
   {
   }

   Uint8 m_uiNpc_hp;       //!< hit points

   Uint8 m_uiNpc_goal;     //!< goal of the npc
   Uint8 m_uiNpc_gtarg;    //!< goal target
   Uint8 m_uiNpc_level;    //!< experience level
   bool  m_bNpc_talkedto;  //!< is true when player talked to npc
   Uint8 m_uiNpc_attitude; //!< attitude

   Uint8 m_uiNpc_xhome;    //!< home tile x coordinate
   Uint8 m_uiNpc_yhome;    //!< home tile y coordinate

   Uint8 m_uiNpc_hunger;   //!< hungriness of the npc
   Uint8 m_uiNpc_whoami;   //!< conversation slot of npc, or 0 for generic

   Uint8 m_uiAnimationState;  //!< animation state
   Uint8 m_uiAnimationFrame;  //!< current animation frame

   void Load(Base::Savegame& sg);
   void Save(Base::Savegame& sg) const;
};

// classes

//! object
class Object
{
public:
   //! ctor
   Object():m_objectType(objectNormal){}

   //! returns object type
   EObjectType GetObjectType() const { return m_objectType; }

   ObjectInfo& GetObjectInfo(){ return m_objInfo; }
   const ObjectInfo& GetObjectInfo() const { return m_objInfo; }

   ObjectPositionInfo& GetPosInfo(){ return m_posInfo; }
   const ObjectPositionInfo& GetPosInfo() const { return m_posInfo; }

   // loading / saving

   //! loads object from savegame
   virtual void Load(Base::Savegame& sg);

   //! saves object to savegame
   virtual void Save(Base::Savegame& sg) const;

protected:
   //! object type
   EObjectType m_objectType;

   //! object info
   ObjectInfo m_objInfo;

   //! object position info
   ObjectPositionInfo m_posInfo;
};

//! Object that additionally has NPC infos
/*! Allowed range for NPC objects is item IDs 0x0040-0x007f. */
class NpcObject: public Object
{
public:
   //! ctor
   NpcObject(){ m_objectType = objectNormal; }

   //! returns npc info
   NpcInfo& GetNpcInfo(){ return m_npcInfo; }
   //! returns npc info
   const NpcInfo& GetNpcInfo() const { return m_npcInfo; }

   // loading / saving

   //! loads object from savegame
   virtual void Load(Base::Savegame& sg);

   //! saves object to savegame
   virtual void Save(Base::Savegame& sg) const;

private:
   //! npc info
   NpcInfo m_npcInfo;
};


//! smart pointer to Object
typedef Base::SmartPtr<Object> ObjectPtr;


//! returns if given object is a NPC object
inline bool IsNpcObject(ObjectPtr& ptr)
{
   return ptr->GetObjectType() == objectNormal;
}

//! Casts object ptr to npc object
/*! Check pointer with IsNpcObject() before attempting to cast.
*/
inline NpcObject& CastToNpcObject(ObjectPtr& ptr)
{
   UaAssert(IsNpcObject(ptr) == true);
   return *reinterpret_cast<NpcObject*>(ptr.get());
}

} // namespace Underworld

#endif
