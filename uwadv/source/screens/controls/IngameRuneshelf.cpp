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
/// \file IngameRuneshelf.cpp
/// \brief ingame runeshelf control
//
#include "pch.hpp"
#include "IngameRuneshelf.hpp"
#include "../OriginalIngameScreen.hpp"
#include "Underworld.hpp"
#include "GameLogic.hpp"
#include "ImageManager.hpp"

void IngameRuneshelf::Init(IBasicGame& game, unsigned int xpos,
   unsigned int ypos)
{
   ImageWindow::Init(xpos, ypos, 46, 16);

   // load images 232..255; A-Z without X and Z
   game.GetImageManager().LoadList(m_runestoneImages, "objects", 232, 256);
}

/// Updates the runeshelf image from runeshelf content.
void IngameRuneshelf::UpdateRuneshelf()
{
   IndexedImage& img_shelf = GetImage();
   img_shelf.Clear(0);

   Underworld::Runeshelf& runeshelf = m_gameInstance.GetUnderworld().
      GetPlayer().GetRuneshelf();

   size_t max = static_cast<unsigned int>(runeshelf.GetNumRunes() % 3);
   for (unsigned int i = 0; i < max; i++)
   {
      Underworld::RuneType rune = static_cast<Underworld::RuneType>(runeshelf.GetRune(i) % 24);

      // paste appropriate rune image
      IndexedImage& img_rune = m_runestoneImages[rune];

      img_shelf.PasteRect(img_rune, 0, 0, 14, 14,
         i * 15, 0, true);
   }

   UpdateImage();
}

bool IngameRuneshelf::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
   {
      m_gameInstance.GetGameLogic().UserAction(userActionClickedRuneshelf);
      return true;
   }

   return false;
}
