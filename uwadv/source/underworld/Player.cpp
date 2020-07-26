//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file Player.cpp
/// \brief player
//
#include "pch.hpp"
#include "Player.hpp"
#include "Savegame.hpp"

using Underworld::Player;

Player::Player()
   :m_name("GRONKEY"),
   m_xpos(32.0), m_ypos(32.0),
   m_height(0.0),
   m_rotateAngle(0.0),
   m_panAngle(0.0)
{
   memset(m_attributes, 0, SDL_TABLESIZE(m_attributes));
   memset(m_skills, 0, SDL_TABLESIZE(m_skills));
}

void Player::Load(Base::Savegame& sg)
{
   sg.BeginSection("player");

   sg.ReadString(m_name);

   m_xpos = sg.Read32() / 256.0;
   m_ypos = sg.Read32() / 256.0;
   m_height = sg.Read32() / 256.0;
   m_rotateAngle = sg.Read32() / 256.0;
   m_panAngle = sg.Read32() / 256.0;

   unsigned int index = 0;
   for (index = 0; index < SDL_TABLESIZE(m_attributes); index++)
      m_attributes[index] = sg.Read16();

   for (index = 0; index < SDL_TABLESIZE(m_skills); index++)
      m_skills[index] = sg.Read16();

   sg.EndSection();

   m_inventory.Load(sg);
   m_runebag.Load(sg);
   m_runeshelf.Load(sg);
   m_convGlobals.Load(sg);
   m_questFlags.Load(sg);
}

void Player::Save(Base::Savegame& sg) const
{
   sg.BeginSection("player");

   sg.WriteString(m_name.c_str());

   sg.Write32(static_cast<Uint32>(m_xpos * 256.0));
   sg.Write32(static_cast<Uint32>(m_ypos * 256.0));
   sg.Write32(static_cast<Uint32>(m_height * 256.0));
   sg.Write32(static_cast<Uint32>(m_rotateAngle * 256.0));
   sg.Write32(static_cast<Uint32>(m_panAngle * 256.0));

   unsigned int index = 0;
   for (index = 0; index < SDL_TABLESIZE(m_attributes); index++)
      sg.Write16(m_attributes[index]);

   for (index = 0; index < SDL_TABLESIZE(m_skills); index++)
      sg.Write16(m_skills[index]);

   sg.EndSection();

   m_inventory.Save(sg);
   m_runebag.Save(sg);
   m_runeshelf.Save(sg);
   m_convGlobals.Save(sg);
   m_questFlags.Save(sg);
}

/// Fills the player-related fields of the savegame info struct.
void Player::FillSavegamePlayerInfos(Base::SavegameInfo& info)
{
   info.m_playerName = m_name;

   info.m_gender = GetAttribute(attrGender);
   info.m_appearance = GetAttribute(attrAppearance);
   info.m_profession = GetAttribute(attrProfession);
   info.m_mapLevel = GetAttribute(attrMapLevel);

   info.m_strength = GetAttribute(attrStrength);
   info.m_dexterity = GetAttribute(attrDexterity);
   info.m_intelligence = GetAttribute(attrIntelligence);
   info.m_vitality = GetAttribute(attrVitality);
}
