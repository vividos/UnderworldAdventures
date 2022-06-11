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
/// \file IngameDragon.cpp
/// \brief ingame dragon control
//
#include "pch.hpp"
#include "IngameDragon.hpp"
#include "../OriginalIngameScreen.hpp"
#include "GameLogic.hpp"
#include "ImageManager.hpp"

IngameDragon::IngameDragon(OriginalIngameScreen& screen, bool leftDragon)
   :OriginalIngameControl(screen),
   m_leftDragon(leftDragon)
{
}

void IngameDragon::Init(IBasicGame& game, unsigned int xpos,
   unsigned int ypos)
{
   // load images
   game.GetImageManager().LoadList(m_dragonImages, "dragons",
      m_leftDragon ? 0 : 18, m_leftDragon ? 18 : 36);

   // prepare image
   IndexedImage& img = GetImage();

   img.Create(37, 104);
   img.SetPalette(game.GetImageManager().GetPalette(0));
   img.Clear(0);


   ImageQuad::Init(game, xpos, ypos);


   // dragon part that's never moving
   img.PasteImage(m_dragonImages[0], m_leftDragon ? 0 : 24, 69);

   UpdateDragon();
}

void IngameDragon::UpdateDragon()
{
   IndexedImage& img = GetImage();

   unsigned int head_frame = 1;
   img.PasteImage(m_dragonImages[head_frame], 0, 80);

   unsigned int tail_frame = 14;
   img.PasteImage(m_dragonImages[tail_frame], m_leftDragon ? 4 : 20, 0);

   Update();
}

bool IngameDragon::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
   {
      m_parent.GetGameInterface().GetGameLogic().UserAction(userActionClickedDragons);
      return true;
   }

   return false;
}
