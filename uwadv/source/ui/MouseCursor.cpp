//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2023 Underworld Adventures Team
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
/// \file MouseCursor.cpp
/// \brief mouse cursor
//
#include "pch.hpp"
#include "MouseCursor.hpp"
#include "ImageManager.hpp"

void MouseCursor::Init(IBasicGame& game, unsigned int initialType,
   unsigned int paletteIndex)
{
   ImageQuad::Init(game, 0, 0);

   game.GetImageManager().LoadList(m_cursorImages, "cursors",
      0, 0, paletteIndex);

   m_isVisible = false;
   SetType(initialType);
}

void MouseCursor::Show(bool show)
{
   m_isVisible = show;
}

void MouseCursor::SetType(unsigned int type)
{
   if (type < m_cursorImages.size())
      SetCustom(m_cursorImages[type]);

   m_offsetX = m_offsetY = 0;
   if (type == 12) // the map editing quill
   {
      m_offsetX = 21;
      m_offsetY = -8;
   }
}

void MouseCursor::SetCustom(IndexedImage& cursorImage)
{
   m_windowWidth = cursorImage.GetXRes();
   m_windowHeight = cursorImage.GetYRes();

   IndexedImage& image = GetImage();
   image.Create(m_windowWidth + 1, m_windowHeight + 1);
   image.Clear();
   image.PasteRect(cursorImage, 0, 0, m_windowWidth, m_windowHeight, 0, 0);

   image.SetPalette(cursorImage.GetPalette());

   Update();
}

void MouseCursor::Draw()
{
   if (!m_isVisible)
      return;

   bool isBlendEnabled = glIsEnabled(GL_BLEND) == GL_TRUE;

   // mouse cursor must be drawn with blending enabled
   if (!isBlendEnabled)
      glEnable(GL_BLEND);

   ImageQuad::Draw();

   if (!isBlendEnabled)
      glDisable(GL_BLEND);
}

bool MouseCursor::ProcessEvent(SDL_Event& event)
{
   if (event.type == SDL_MOUSEMOTION)
   {
      CalcMousePosition(event, m_windowXPos, m_windowYPos);

      m_windowXPos -= m_windowWidth / 2.0;
      m_windowYPos -= m_windowHeight / 2.0;

      m_windowXPos += m_offsetX;
      m_windowYPos += m_offsetY;
   }

   return false;
}
