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
/// \file object.cpp
/// \brief object
//
#include "underw.hpp"
#include "object.hpp"
#include "savegame.hpp"

using Underworld::ObjectInfo;
using Underworld::ObjectPositionInfo;
using Underworld::NpcInfo;
using Underworld::Object;
using Underworld::NpcObject;

void ObjectInfo::Load(Base::Savegame& sg)
{
   m_itemID = sg.Read16();

   if (m_itemID == c_itemIDNone)
      return; // nothing more to read

   m_link = sg.Read16();
   m_quality = sg.Read16();
   m_owner = sg.Read16();
   m_quantity = sg.Read16();

   m_flags = sg.Read16();

   m_isEnchanted = sg.Read8() != 0;
   m_isHidden = sg.Read8() != 0;
   m_isQuantity = sg.Read8() != 0;
}

void ObjectInfo::Save(Base::Savegame& sg) const
{
   sg.Write16(m_itemID);

   if (m_itemID == c_itemIDNone)
      return; // nothing more to write

   sg.Write16(m_link);
   sg.Write16(m_quality);
   sg.Write16(m_owner);
   sg.Write16(m_quantity);

   sg.Write16(m_flags);

   sg.Write8(m_isEnchanted ? 1 : 0);
   sg.Write8(m_isHidden ? 1 : 0);
   sg.Write8(m_isQuantity ? 1 : 0);
}

void ObjectPositionInfo::Load(Base::Savegame& sg)
{
   m_xpos = sg.Read8() & 7;
   m_ypos = sg.Read8() & 7;
   m_zpos = sg.Read8();
   m_heading = sg.Read8() & 7;

   m_tileX = sg.Read8();
   m_tileY = sg.Read8();
}

void ObjectPositionInfo::Save(Base::Savegame& sg) const
{
   sg.Write8(m_xpos & 7);
   sg.Write8(m_ypos & 7);
   sg.Write8(m_zpos);
   sg.Write8(m_heading & 7);

   sg.Write8(m_tileX);
   sg.Write8(m_tileY);
}

void NpcInfo::Load(Base::Savegame& sg)
{
   npc_hp = sg.Read8();

   npc_goal = sg.Read8();
   npc_gtarg = sg.Read8();
   npc_level = sg.Read8();
   npc_talkedto = sg.Read8() != 0;
   npc_attitude = sg.Read8();

   npc_xhome = sg.Read8();
   npc_yhome = sg.Read8();

   npc_hunger = sg.Read8();
   npc_whoami = sg.Read8();

   m_animationState = sg.Read8();
   m_animationFrame = sg.Read8();
}

void NpcInfo::Save(Base::Savegame& sg) const
{
   sg.Write8(npc_hp);

   sg.Write8(npc_goal);
   sg.Write8(npc_gtarg);
   sg.Write8(npc_level);
   sg.Write8(npc_talkedto ? 1 : 0);
   sg.Write8(npc_attitude);

   sg.Write8(npc_xhome);
   sg.Write8(npc_yhome);

   sg.Write8(npc_hunger);
   sg.Write8(npc_whoami);

   sg.Write8(m_animationState);
   sg.Write8(m_animationFrame);
}

void Object::Load(Base::Savegame& sg)
{
   m_objInfo.Load(sg);
   m_posInfo.Load(sg);
}

void Object::Save(Base::Savegame& sg) const
{
   m_objInfo.Save(sg);
   if (m_objInfo.m_itemID != c_itemIDNone)
      m_posInfo.Save(sg);
}

void NpcObject::Load(Base::Savegame& sg)
{
   Object::Load(sg);
   if (GetObjectInfo().m_itemID != c_itemIDNone)
      m_npcInfo.Load(sg);
}

void NpcObject::Save(Base::Savegame& sg) const
{
   Object::Save(sg);
   m_npcInfo.Save(sg);
}
