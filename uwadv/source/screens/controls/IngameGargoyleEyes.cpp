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
/// \file IngameGargoyleEyes.cpp
/// \brief ingame gargoyle eyes control
//
#include "pch.hpp"
#include "IngameGargoyleEyes.hpp"
#include "../OriginalIngameScreen.hpp"
#include "GameLogic.hpp"
#include "ImageManager.hpp"

void IngameGargoyleEyes::Init(IGameInstance& game, unsigned int xpos,
   unsigned int ypos)
{
   ImageWindow::Init(xpos, ypos, 20, 3);

   game.GetImageManager().LoadList(m_eyesImages, "eyes");
}

/// \todo update eyes from data in Underworld
void IngameGargoyleEyes::UpdateEyes()
{
   unsigned int new_image = 0;

   GetImage().PasteImage(m_eyesImages[new_image], 0, 0);
   UpdateImage();
}

bool IngameGargoyleEyes::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
   {
      m_gameInstance.GetGameLogic().UserAction(userActionClickedGargoyle);
      return true;
   }

   return false;
}
