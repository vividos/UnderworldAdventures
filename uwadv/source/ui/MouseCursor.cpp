//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
#include "GameInterface.hpp"
#include "Renderer.hpp"
#include "ImageManager.hpp"

void MouseCursor::Init(IBasicGame& game, unsigned int initialType)
{
   Window::Create(0, 0, 0, 0);

   game.GetImageManager().LoadList(m_cursorImages, "cursors");

   m_mouseTexture.Init(1);

   m_isVisible = false;
   SetType(initialType);

   m_smoothUI = game.GetSettings().GetBool(Base::settingUISmooth);
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

   IndexedImage tempImage;
   tempImage.Create(m_windowWidth + 1, m_windowHeight + 1);
   tempImage.PasteRect(cursorImage, 0, 0, m_windowWidth, m_windowHeight, 0, 0);

   tempImage.SetPalette(cursorImage.GetPalette());

   m_mouseTexture.Convert(tempImage);
   m_mouseTexture.Upload();
}

void MouseCursor::Destroy()
{
   m_mouseTexture.Done();
}

void MouseCursor::Draw()
{
   if (!m_isVisible)
      return;

   bool blend_enabled = glIsEnabled(GL_BLEND) == GL_TRUE;

   // mouse cursor must be drawn with blending enabled
   if (!blend_enabled)
      glEnable(GL_BLEND);

   m_mouseTexture.Use();
   double u = m_mouseTexture.GetTexU(), v = m_mouseTexture.GetTexV();
   u -= m_mouseTexture.GetTexU() / m_mouseTexture.GetXRes();
   v -= m_mouseTexture.GetTexV() / m_mouseTexture.GetYRes();

   // set wrap parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_smoothUI ? GL_LINEAR : GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_smoothUI ? GL_LINEAR : GL_NEAREST);

   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v); glVertex2i(m_windowXPos, 200 - m_windowYPos);
   glTexCoord2d(u, v); glVertex2i(m_windowXPos + m_windowWidth, 200 - m_windowYPos);
   glTexCoord2d(u, 0.0); glVertex2i(m_windowXPos + m_windowWidth, 200 - m_windowYPos + m_windowHeight);
   glTexCoord2d(0.0, 0.0); glVertex2i(m_windowXPos, 200 - m_windowYPos + m_windowHeight);
   glEnd();

   if (!blend_enabled)
      glDisable(GL_BLEND);
}

bool MouseCursor::ProcessEvent(SDL_Event& event)
{
   if (event.type == SDL_MOUSEMOTION)
   {
      CalcMousePosition(event, m_windowXPos, m_windowYPos);

      m_windowXPos -= m_windowWidth / 2.0;
      m_windowYPos += m_windowHeight / 2.0;

      m_windowXPos += m_offsetX;
      m_windowYPos += m_offsetY;
   }

   return false;
}
