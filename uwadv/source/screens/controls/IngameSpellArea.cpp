//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022 Underworld Adventures Team
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
/// \file IngameSpellArea.cpp
/// \brief ingame spell area control
//
#include "pch.hpp"
#include "IngameSpellArea.hpp"
#include "../OriginalIngameScreen.hpp"
#include "GameLogic.hpp"
#include "ImageManager.hpp"

void IngameSpellArea::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   GetImage().Create(51, 18);

   // load images 232..255; A-Z without X and Z
   game.GetImageManager().LoadList(m_spellImages, "spells");

   ImageQuad::Init(game, xpos, ypos);
}

/// Updates the active spell area image.
/// \todo actually get spells from Underworld
void IngameSpellArea::UpdateSpellArea()
{
   IndexedImage& img_area = GetImage();
   img_area.Clear(0);

   unsigned int spell[3] = { 0, 0, 0 };

   for (unsigned int i = 0; i < 3; i++)
   {
      if (spell[i] == 0)
         continue;

      // paste appropriate spell image
      IndexedImage& img_spell = m_spellImages[(spell[i] - 1) % 21];

      unsigned int dest = m_hasBorder ? 1 : 0;
      img_area.PasteRect(img_spell, 0, 0, 16, 18,
         i * 17 + dest, dest, true);
   }

   Update();
}

bool IngameSpellArea::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
   {
      m_parent->GetGameInterface().GetGameLogic().UserAction(userActionClickedActiveSpell);
      return true;
   }

   return false;
}
