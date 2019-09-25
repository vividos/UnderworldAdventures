//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2019 Underworld Adventures Team
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
/// \file TextEditWindow.cpp
/// \brief text edit window
//
#include "pch.hpp"
#include "TextEditWindow.hpp"
#include "ImageManager.hpp"

TextEditWindow::~TextEditWindow()
{
}

void TextEditWindow::Init(IGame& game, unsigned int xpos,
   unsigned int ypos, unsigned int width, Uint8 backgroundColor,
   Uint8 prefixColor, Uint8 textColor,
   const char* prefixText, const char* startText, bool border)
{
   // init variables
   m_backgroundColor = backgroundColor;
   m_prefixColor = prefixColor;
   m_textColor = textColor;
   m_prefix = prefixText;
   m_text = startText;
   m_border = border;
   m_cursorPos = m_text.size();

   m_font.Load(game.GetResourceManager(), fontNormal);

   // adjust widths when having a border
   if (border)
   {
      xpos--; ypos--;
      width += 2;
   }

   // init image
   IndexedImage& image = GetImage();
   image.Create(width, m_font.GetCharHeight() + 1 + (border ? 2 : 0));
   image.SetPalette(game.GetImageManager().GetPalette(0));

   ImageQuad::Init(game, xpos, ypos);

   UpdateText();
}

void TextEditWindow::Done()
{
}

void TextEditWindow::UpdateText()
{
   IndexedImage& image = GetImage();
   image.Clear(m_backgroundColor);

   if (m_border)
   {
      /// \todo border
      image.FillRect(0, 0, image.GetXRes(), 1, 1);
      image.FillRect(0, image.GetYRes() - 1, image.GetXRes(), 1, 1);
   }

   // prefix text
   IndexedImage tempImage;
   if (m_prefix.size() > 0)
   {
      m_font.CreateString(tempImage, m_prefix.c_str(), m_prefixColor);

      image.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(),
         m_border ? 2 : 1, m_border ? 2 : 1, true);
   }

   unsigned int prefix_xpos = tempImage.GetXRes();

   // main text
   if (m_text.size() > 0)
   {
      m_font.CreateString(tempImage, m_text.c_str(), m_textColor);

      image.PasteImage(tempImage, prefix_xpos + (m_border ? 2 : 1), m_border ? 2 : 1, true);
   }

   // cursor image pos
   std::string posText(m_text.c_str(), m_cursorPos);
   unsigned int cursorPosX = m_font.CalcLength(posText.c_str());

   // draw cursor
   image.FillRect(
      prefix_xpos + cursorPosX + (m_border ? 1 : 0),
      m_border ? 1 : 0,
      1,
      m_font.GetCharHeight(),
      1);

   Update();
}

bool TextEditWindow::ProcessEvent(SDL_Event& event)
{
   bool handled = false;

   // check event type
   switch (event.type)
   {
   case SDL_KEYDOWN:
      // check typeable keys
   {
      // TODO use SDL_TEXTINPUT event instead
      char ch = event.key.keysym.sym;

      if (ch >= SDLK_SPACE && ch <= SDLK_z)
      {
         // add to text and update
         m_text.insert(m_cursorPos, 1, ch);
         m_cursorPos++;

         handled = true;
      }

      SDL_Keycode key = event.key.keysym.sym;

      switch (key)
      {
      case SDLK_LEFT:
         if (m_cursorPos > 0)
         {
            m_cursorPos--;
            handled = true;
         }
         break;

      case SDLK_RIGHT:
         if (m_cursorPos < m_text.size())
         {
            m_cursorPos++;
            handled = true;
         }
         break;

      case SDLK_HOME:
         m_cursorPos = 0;
         handled = true;
         break;

      case SDLK_END:
         m_cursorPos = m_text.size();
         handled = true;
         break;

      case SDLK_BACKSPACE:
         if (m_cursorPos > 0)
         {
            m_cursorPos--;
            m_text.erase(m_cursorPos, 1);
            handled = true;
         }
         break;

      case SDLK_DELETE:
         if (m_text.size() > 0 && m_cursorPos < m_text.size())
         {
            m_text.erase(m_cursorPos, 1);
            handled = true;
         }
         break;

      case SDLK_RETURN:
      case SDLK_ESCAPE:
      {
         // send event
         SDL_Event user_event;
         user_event.type = SDL_USEREVENT;
         user_event.user.code = key == SDLK_RETURN ?
            gameEventTexteditFinished : gameEventTexteditAborted;
         user_event.user.data1 = NULL;
         user_event.user.data2 = NULL;
         SDL_PushEvent(&user_event);

         handled = true;
      }
      break;
      }

      if (handled)
         UpdateText();
   }
   }

   return handled || ImageQuad::ProcessEvent(event);
}
