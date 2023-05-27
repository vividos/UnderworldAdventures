//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2019,2023 Underworld Adventures Team
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

TextEditWindow::~TextEditWindow() noexcept
{
}

void TextEditWindow::Init(IGameInstance& game, unsigned int xpos,
   unsigned int ypos, unsigned int width, Uint8 backgroundColor,
   Uint8 prefixColor, Uint8 textColor,
   const char* prefixText, const char* startText, bool border,
   FontId fontId, unsigned int paletteIndex)
{
   // init variables
   m_backgroundColor = backgroundColor;
   m_prefixColor = prefixColor;
   m_textColor = textColor;
   m_prefix = prefixText;
   m_text = startText;
   m_border = border;
   m_uppercase = fontId == fontSmall;
   m_cursorPos = m_text.size();

   m_font.Load(game.GetResourceManager(), fontId);

   // adjust widths when having a border
   if (border)
   {
      xpos--; ypos--;
      width += 2;
   }

   // init image
   IndexedImage& image = GetImage();
   image.Create(width, m_font.GetCharHeight() + 1 + (border ? 2 : 0));
   image.SetPalette(game.GetImageManager().GetPalette(paletteIndex));

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
      // draw border
      unsigned int xend = image.GetXRes()/* - 1*/;
      unsigned int yend = image.GetYRes()/* - 1*/;
      image.FillRect(0, 0, xend, yend, m_borderColor);
      image.FillRect(1, 1, xend - 2, yend - 2, m_backgroundColor);
   }

   unsigned int padding = m_border ? 2 : 1;

   // prefix text
   IndexedImage tempImage;
   if (!m_prefix.empty())
   {
      m_font.CreateString(tempImage, m_prefix, m_prefixColor);

      image.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(),
         padding, padding, true);
   }

   unsigned int prefix_xpos = tempImage.GetXRes();

   // main text
   if (!m_text.empty())
   {
      m_font.CreateString(tempImage, m_text, m_textColor);

      image.PasteImage(tempImage, prefix_xpos + padding, padding, true);
   }

   // cursor image pos
   std::string posText{ m_text.c_str(), m_cursorPos };
   unsigned int cursorPosX = m_font.CalcLength(posText);

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

   unsigned int previousCursorPos = m_cursorPos;

   // check event type
   switch (event.type)
   {
   case SDL_TEXTINPUT:
   {
      // only consider the first character in english
      char ch = event.text.text[0];

      if (m_uppercase)
         ch = toupper(ch);

      // check typeable keys
      if (ch >= SDLK_SPACE && ch <= SDLK_z &&
         m_font.IsCharAvailable(ch))
      {
         // add to text and update
         m_text.insert(m_cursorPos, 1, ch);
         m_cursorPos++;

         handled = true;
      }
      break;
   }

   case SDL_KEYDOWN:
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
         if (!m_text.empty() && m_cursorPos < m_text.size())
         {
            m_text.erase(m_cursorPos, 1);
            handled = true;
         }
         break;

      case SDLK_RETURN:
      case SDLK_ESCAPE:
      {
         SendTextEditEvent(key == SDLK_RETURN
            ? gameEventTexteditFinished
            : gameEventTexteditAborted);
         handled = true;
      }
      break;
      }
   }

   if (handled)
      UpdateText();

   if (previousCursorPos != m_cursorPos &&
      m_cursorPosChangedHandler != nullptr)
   {
      std::string posText{ m_text.c_str(), m_cursorPos };
      unsigned int cursorPosX = m_font.CalcLength(posText);

      m_cursorPosChangedHandler(cursorPosX);
   }

   return handled || ImageQuad::ProcessEvent(event);
}

void TextEditWindow::SendTextEditEvent(Sint32 userEventCode)
{
   SDL_Event userEvent{};
   userEvent.type = SDL_USEREVENT;
   userEvent.user.code = userEventCode;
   userEvent.user.data1 = nullptr;
   userEvent.user.data2 = nullptr;
   SDL_PushEvent(&userEvent);

   if (m_editingFinishedHandler)
   {
      m_editingFinishedHandler(userEventCode == gameEventTexteditAborted);
   }
}
