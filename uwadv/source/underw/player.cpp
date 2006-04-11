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
/*! \file player.cpp

   \brief player

*/

// needed includes
#include "underw.hpp"
#include "player.hpp"
#include "savegame.hpp"

using Underworld::Player;

// Player methods

Player::Player()
:m_strName("GRONKEY"),
 m_xpos(32.0), m_ypos(32.0),
 m_dHeight(0.0),
 m_dRotateAngle(0.0),
 m_dPanAngle(0.0)
{
   memset(m_aAttributes, 0, SDL_TABLESIZE(m_aAttributes));
   memset(m_aSkills, 0, SDL_TABLESIZE(m_aSkills));
}

void Player::Load(Base::Savegame& sg)
{
   sg.BeginSection("player");

   // read in name
   sg.ReadString(m_strName);

   // read position
   m_xpos = sg.Read32() / 256.0;
   m_ypos = sg.Read32() / 256.0;
   m_dHeight = sg.Read32() / 256.0;
   m_dRotateAngle = sg.Read32() / 256.0;
   m_dPanAngle = sg.Read32() / 256.0;

   // read attributes and skills
   unsigned int ui=0;
   for(ui=0; ui<SDL_TABLESIZE(m_aAttributes); ui++)
      m_aAttributes[ui] = sg.Read16();

   for(ui=0; ui<SDL_TABLESIZE(m_aSkills); ui++)
      m_aSkills[ui] = sg.Read16();

   sg.EndSection();

   m_inventory.Load(sg);
   m_runebag.Load(sg);
   m_convGlobals.Load(sg);
   m_questFlags.Load(sg);
}

void Player::Save(Base::Savegame& sg) const
{
   sg.BeginSection("player");

   // write name
   sg.WriteString(m_strName.c_str());

   // write position
   sg.Write32(static_cast<Uint32>(m_xpos*256.0));
   sg.Write32(static_cast<Uint32>(m_ypos*256.0));
   sg.Write32(static_cast<Uint32>(m_dHeight*256.0));
   sg.Write32(static_cast<Uint32>(m_dRotateAngle*256.0));
   sg.Write32(static_cast<Uint32>(m_dPanAngle*256.0));

   // write attributes and skills
   unsigned int ui=0;
   for(ui=0; ui<SDL_TABLESIZE(m_aAttributes); ui++)
      sg.Write16(m_aAttributes[ui]);

   for(ui=0; ui<SDL_TABLESIZE(m_aSkills); ui++)
      sg.Write16(m_aSkills[ui]);

   sg.EndSection();

   m_inventory.Save(sg);
   m_runebag.Save(sg);
   m_convGlobals.Save(sg);
   m_questFlags.Save(sg);
}

/*! Fills the player-related fields of the savegame info struct. */
void Player::FillSavegamePlayerInfos(Base::SavegameInfo& info)
{
   info.strPlayerName = m_strName;

   info.uiGender = GetAttribute(attrGender);
   info.uiAppearance = GetAttribute(attrAppearance);
   info.uiProfession = GetAttribute(attrProfession);
   info.uiMapLevel = GetAttribute(attrMapLevel);

   info.uiStrength = GetAttribute(attrStrength);
   info.uiDexterity = GetAttribute(attrDexterity);
   info.uiIntelligence = GetAttribute(attrIntelligence);
   info.uiVitality = GetAttribute(attrVitality);
}
