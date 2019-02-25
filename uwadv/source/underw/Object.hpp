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
/// \file Object.hpp
/// \brief object
//
#pragma once

namespace Base
{
   class Savegame;
}

namespace Underworld
{
   class NpcObject;

   /// item id for unused object
   const Uint16 c_itemIDNone = 0xffff;

   /// tile coordinate for "not a position"
   const Uint8 c_tileNotAPos = 0xff;

   /// object type
   enum ObjectType
   {
      objectNone,       ///< not an object
      objectNormal = 1, ///< normal object of type Object
      objectNpc = 2,    ///< object of type NpcObject
   };

   /// general object info
   struct ObjectInfo
   {
      /// ctor
      ObjectInfo()
         :m_itemID(c_itemIDNone),
         m_link(0), // equal to g_objectListPosNone
         m_quality(0),
         m_owner(0),
         m_quantity(0),
         m_flags(0),
         m_isEnchanted(false),
         m_isHidden(false),
         m_isQuantity(true)
      {
      }

      Uint16 m_itemID;     ///< object item id
      Uint16 m_link;       ///< object chain link
      Uint16 m_quality;    ///< quality
      Uint16 m_owner;      ///< owner / special field
      Uint16 m_quantity;   ///< quantity / special / special link
      Uint16 m_flags;      ///< object flags

      // flags
      bool m_isEnchanted;  ///< true when object is enchanted
      bool m_isHidden;     ///< indicates if object is hidden or visible
      bool m_isQuantity;   ///< true when "quantity" field is a quantity/special property

      // loading / saving

      /// loads object info from savegame
      void Load(Base::Savegame& sg);
      /// saves object info to savegame
      void Save(Base::Savegame& sg) const;
   };

   /// object position info
   struct ObjectPositionInfo
   {
      /// ctor
      ObjectPositionInfo()
         :m_xpos(0),
         m_ypos(0),
         m_zpos(0),
         m_heading(0),
         m_tileX(c_tileNotAPos),
         m_tileY(c_tileNotAPos)
      {
      }

      Uint8 m_xpos;     ///< fractional x position in tile (0..7)
      Uint8 m_ypos;     ///< fractional y position in tile (0..7)
      Uint8 m_zpos;     ///< z position (0..127)
      Uint8 m_heading;  ///< direction (0..7)
      Uint8 m_tileX;    ///< tilemap x coordinate
      Uint8 m_tileY;    ///< tilemap y coordinate

      // loading / saving

      /// loads object position info from savegame
      void Load(Base::Savegame& sg);
      /// saves object position info to savegame
      void Save(Base::Savegame& sg) const;
   };

   /// \brief NPC info
   /// \details Most members of this struct directly correspond to the variables available
   /// in conversations.
   struct NpcInfo
   {
      /// \todo use enums where values are known and fixed
      NpcInfo()
         :m_npc_hp(0),
         m_npc_goal(0),
         m_npc_gtarg(0),
         m_npc_level(0),
         m_npc_talkedto(false),
         m_npc_attitude(0),
         m_npc_xhome(0),
         m_npc_yhome(0),
         m_npc_hunger(0),
         m_npc_whoami(0),
         m_animationState(0),
         m_animationFrame(0)
      {
      }

      Uint8 m_npc_hp;        ///< hit points

      Uint8 m_npc_goal;      ///< goal of the npc
      Uint8 m_npc_gtarg;     ///< goal target
      Uint8 m_npc_level;     ///< experience level
      bool  m_npc_talkedto;  ///< is true when player talked to npc
      Uint8 m_npc_attitude;  ///< attitude

      Uint8 m_npc_xhome;     ///< home tile x coordinate
      Uint8 m_npc_yhome;     ///< home tile y coordinate

      Uint8 m_npc_hunger;    ///< hungriness of the npc
      Uint8 m_npc_whoami;    ///< conversation slot of npc, or 0 for generic

      Uint8 m_animationState;  ///< animation state
      Uint8 m_animationFrame;  ///< current animation frame

      // loading / saving

      /// loads npc info from savegame
      void Load(Base::Savegame& sg);
      /// saves npc info to savegame
      void Save(Base::Savegame& sg) const;
   };

   /// object
   class Object
   {
   public:
      /// ctor
      Object()
         :m_objectType(objectNormal)
      {
      }

      /// returns object type
      ObjectType GetObjectType() const { return m_objectType; }

      /// returns object info
      ObjectInfo& GetObjectInfo() { return m_objInfo; }
      /// returns object info; const version
      const ObjectInfo& GetObjectInfo() const { return m_objInfo; }

      /// returns object position info
      ObjectPositionInfo& GetPosInfo() { return m_posInfo; }
      /// returns object position info; const version
      const ObjectPositionInfo& GetPosInfo() const { return m_posInfo; }

      /// returns if this object is an NPC object
      bool IsNpcObject() const { return GetObjectType() == objectNpc; }

      /// returns NPC object when this object is an NPC; check with IsNpcObject() before calling!
      NpcObject& GetNpcObject();

      /// returns NPC object when this object is an NPC; const version
      const NpcObject& Object::GetNpcObject() const;

      // loading / saving

      /// loads object from savegame
      virtual void Load(Base::Savegame& sg);

      /// saves object to savegame
      virtual void Save(Base::Savegame& sg) const;

   protected:
      /// ctor for derived objects
      Object(ObjectType objectType)
         :m_objectType(objectType)
      {
      }

   private:
      /// object type
      ObjectType m_objectType;

      /// object info
      ObjectInfo m_objInfo;

      /// object position info
      ObjectPositionInfo m_posInfo;
   };

   /// \brief Object that additionally has NPC infos
   /// Allowed range for NPC objects is item IDs 0x0040-0x007f.
   class NpcObject : public Object
   {
   public:
      /// ctor
      NpcObject()
         :Object(objectNpc)
      {
      }

      /// returns npc info
      NpcInfo& GetNpcInfo() { return m_npcInfo; }
      /// returns npc info; const version
      const NpcInfo& GetNpcInfo() const { return m_npcInfo; }

      // loading / saving

      /// loads object from savegame
      virtual void Load(Base::Savegame& sg);

      /// saves object to savegame
      virtual void Save(Base::Savegame& sg) const;

   private:
      /// NPC info
      NpcInfo m_npcInfo;
   };


   /// smart pointer to Object
   typedef std::shared_ptr<Object> ObjectPtr;

} // namespace Underworld
