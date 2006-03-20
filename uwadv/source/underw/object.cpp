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
/*! \file object.cpp

   \brief object

*/

// needed includes
#include "underworld.hpp"
#include "object.hpp"
#include "savegame.hpp"

using Underworld::ObjectInfo;
using Underworld::ObjectPositionInfo;
using Underworld::NpcInfo;
using Underworld::Object;
using Underworld::NpcObject;

// ObjectInfo methods

void ObjectInfo::Load(Base::Savegame& sg)
{
   // read basic object info
   m_uiItemID = sg.Read16();

   if (m_uiItemID == g_uiItemIDNone)
      return; // nothing more to read

   m_uiLink = sg.Read16();
   m_uiQuality = sg.Read16();
   m_uiOwner = sg.Read16();
   m_uiQuantity = sg.Read16();

   m_uiFlags = sg.Read16();

   m_bEnchanted = sg.Read8() != 0;
   m_bHidden = sg.Read8() != 0;
   m_bIsQuantity = sg.Read8() != 0;
}

void ObjectInfo::Save(Base::Savegame& sg) const
{
   // write basic object info
   sg.Write16(m_uiItemID);

   if (m_uiItemID == g_uiItemIDNone)
      return; // nothing more to write

   sg.Write16(m_uiLink);
   sg.Write16(m_uiQuality);
   sg.Write16(m_uiOwner);
   sg.Write16(m_uiQuantity);

   sg.Write16(m_uiFlags);

   sg.Write8(m_bEnchanted ? 1 : 0);
   sg.Write8(m_bHidden ? 1 : 0);
   sg.Write8(m_bIsQuantity ? 1 : 0);
}

// ObjectPositionInfo methods

void ObjectPositionInfo::Load(Base::Savegame& sg)
{
   // read extended object info
   m_xpos = sg.Read8() & 7;
   m_ypos = sg.Read8() & 7;
   m_zpos = sg.Read8();
   m_uiHeading = sg.Read8() & 7;

   m_uiTileX = sg.Read8();
   m_uiTileY = sg.Read8();
}

void ObjectPositionInfo::Save(Base::Savegame& sg) const
{
   sg.Write8(m_xpos & 7);
   sg.Write8(m_ypos & 7);
   sg.Write8(m_zpos);
   sg.Write8(m_uiHeading & 7);

   sg.Write8(m_uiTileX);
   sg.Write8(m_uiTileY);
}

// NpcInfo methods

void NpcInfo::Load(Base::Savegame& sg)
{
   m_uiNpc_hp = sg.Read8();

   m_uiNpc_goal = sg.Read8();
   m_uiNpc_gtarg = sg.Read8();
   m_uiNpc_level = sg.Read8();
   m_bNpc_talkedto = sg.Read8() != 0;
   m_uiNpc_attitude = sg.Read8();

   m_uiNpc_xhome = sg.Read8();
   m_uiNpc_yhome = sg.Read8();

   m_uiNpc_hunger = sg.Read8();
   m_uiNpc_whoami = sg.Read8();

   m_uiAnimationState = sg.Read8();
   m_uiAnimationFrame = sg.Read8();
}

void NpcInfo::Save(Base::Savegame& sg) const
{
   sg.Write8(m_uiNpc_hp);

   sg.Write8(m_uiNpc_goal);
   sg.Write8(m_uiNpc_gtarg);
   sg.Write8(m_uiNpc_level);
   sg.Write8(m_bNpc_talkedto ? 1 : 0);
   sg.Write8(m_uiNpc_attitude);

   sg.Write8(m_uiNpc_xhome);
   sg.Write8(m_uiNpc_yhome);

   sg.Write8(m_uiNpc_hunger);
   sg.Write8(m_uiNpc_whoami);

   sg.Write8(m_uiAnimationState);
   sg.Write8(m_uiAnimationFrame);
}

// Object methods

void Object::Load(Base::Savegame& sg)
{
   m_objInfo.Load(sg);
   m_posInfo.Load(sg);
}

void Object::Save(Base::Savegame& sg) const
{
   m_objInfo.Save(sg);
   if (m_objInfo.m_uiItemID != g_uiItemIDNone)
      m_posInfo.Save(sg);
}

void NpcObject::Load(Base::Savegame& sg)
{
   Object::Load(sg);
   if (m_objInfo.m_uiItemID != g_uiItemIDNone)
      m_npcInfo.Load(sg);
}

void NpcObject::Save(Base::Savegame& sg) const
{
   Object::Save(sg);
   m_npcInfo.Save(sg);
}
