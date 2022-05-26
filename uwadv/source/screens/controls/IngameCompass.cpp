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
/// \file IngameCompass.cpp
/// \brief ingame compass control
//
#include "pch.hpp"
#include "IngameCompass.hpp"
#include "../OriginalIngameScreen.hpp"
#include "GameLogic.hpp"
#include "ImageManager.hpp"

void IngameCompass::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   // init image
   GetImage().Create(52, 26);

   ImageQuad::Init(game, xpos, ypos);

   // load compass images
   {
      std::vector<IndexedImage> temp_compass;
      game.GetImageManager().LoadList(temp_compass, "compass");

      Palette256Ptr pal0 = game.GetImageManager().GetPalette(0);

      m_compassImages.resize(16);

      static unsigned int compass_tip_coords[16 * 2] =
      {
         24, 0, 16, 2,  8, 3,  4, 6,
          0,10,  0,14,  4,16, 12,19,
         24,21, 32,19, 44,16, 48,14,
         48,10, 44, 6, 40, 3, 32, 2,
      };

      // create compass images and add needle tips and right/bottom borders
      for (unsigned int n = 0; n < 16; n++)
      {
         IndexedImage& img = m_compassImages[n];
         img.Create(52, 26);
         img.SetPalette(pal0);

         img.PasteImage(temp_compass[n & 3], 0, 0);

         // compass needle
         img.PasteImage(temp_compass[n + 4],
            compass_tip_coords[n * 2], compass_tip_coords[n * 2 + 1]);
      }
   }

   m_currentCompassImageIndex = 16;
}

void IngameCompass::Draw()
{
   // check if we have to reupload the image

   // calculate current angle and images
   Underworld::Player& player = m_parent.GetGameInterface().GetUnderworld().
      GetPlayer();

   double angle = fmod(-player.GetRotateAngle() + 90.0 + 360.0, 360.0);
   unsigned int compassimg = unsigned((angle + 11.25) / 22.5) & 15;

   // prepare texture
   if (m_currentCompassImageIndex != compassimg)
   {
      // reupload compass texture
      m_currentCompassImageIndex = compassimg;

      unsigned int dest = m_hasBorder ? 1 : 0;
      GetImage().PasteImage(m_compassImages[compassimg], dest, dest);

      Update();
   }

   ImageQuad::Draw();
}

bool IngameCompass::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
   {
      m_parent.GetGameInterface().GetGameLogic().UserAction(userActionClickedCompass);
      return true;
   }

   return false;
}
