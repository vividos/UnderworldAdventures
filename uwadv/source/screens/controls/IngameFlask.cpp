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
/// \file IngameFlask.cpp
/// \brief ingame flask control
//
#include "pch.hpp"
#include "IngameFlask.hpp"
#include "../OriginalIngameScreen.hpp"
#include "GameLogic.hpp"
#include "ImageManager.hpp"

void IngameFlask::Init(IBasicGame& game, unsigned int xpos,
   unsigned int ypos)
{
   ImageWindow::Init(xpos, ypos, 24, 33);

   // load flask images
   {
      std::vector<IndexedImage> temp_flasks;
      game.GetImageManager().LoadList(temp_flasks, "flasks");

      unsigned int maximg = m_isVitalityFlask ? 28 : 14;
      m_flaskImages.resize(maximg);

      // paste together all flask fill heights
      for (unsigned int i = 0; i < maximg; i += 14)
      {
         IndexedImage base_img = temp_flasks[75];

         static unsigned int flask_pos[13] =
         { 26, 24, 22, 20, 18, 16, 15, 14, 13, 11, 9, 7, 5 };

         unsigned int offset = m_isVitalityFlask ? (i == 0 ? 0 : 50) : 25;

         // image 0 is the empty flask
         m_flaskImages[i] = base_img;

         // generate all images
         for (unsigned int j = 0; j < 13; j++)
         {
            base_img.PasteImage(temp_flasks[offset + j], 0, flask_pos[j]);
            m_flaskImages[i + j + 1] = base_img;
         }
      }
   }

   m_lastFlaskImageIndex = 14 * 2;
   m_isPoisoned = false;
}

void IngameFlask::Draw()
{
   Underworld::Player& player = m_parent.GetGameInterface().GetUnderworld().
      GetPlayer();
   m_isPoisoned = player.GetAttribute(Underworld::attrPoisoned) != 0;

   unsigned int curval = player.GetAttribute(
      m_isVitalityFlask ? Underworld::attrVitality : Underworld::attrMana);
   unsigned int maxval = player.GetAttribute(
      m_isVitalityFlask ? Underworld::attrMaxVitality : Underworld::attrMaxMana);
   unsigned int curimg = maxval == 0 ? 0 : unsigned((curval * 13.0) / maxval);

   // check if flask image has to be update
   unsigned int new_image = m_isVitalityFlask && m_isPoisoned ? curimg + 14 : curimg;

   if (m_lastFlaskImageIndex != new_image)
   {
      m_lastFlaskImageIndex = new_image;
      UpdateFlask();
   }

   ImageWindow::Draw();
}

void IngameFlask::UpdateFlask()
{
   GetImage().PasteImage(m_flaskImages[m_lastFlaskImageIndex], 0, 0);
   UpdateImage();
}

bool IngameFlask::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
   {
      m_parent.GetGameInterface().GetGameLogic().UserAction(
         m_isVitalityFlask ? userActionClickedVitalityFlash : userActionClickedManaFlask);

      return true;
   }

   return false;
}
