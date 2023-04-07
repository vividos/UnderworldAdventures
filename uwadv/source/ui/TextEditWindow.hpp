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
/// \file TextEditWindow.hpp
/// \brief text edit window
//
#pragma once

#include "ImageQuad.hpp"
#include "Font.hpp"
#include <functional>

/// \brief Text edit window class
/// \note sends gameEventTexteditFinished user event to screen when the user
/// finished editing, either by pressing return or escape
class TextEditWindow : public ImageQuad
{
public:
   /// ctor
   TextEditWindow() {}
   /// dtor
   virtual ~TextEditWindow() noexcept;

   /// initializes text scroll
   void Init(IBasicGame& game, unsigned int xpos, unsigned int ypos,
      unsigned int width, Uint8 backgroundColor, Uint8 prefixColor, Uint8 textColor,
      const char* prefixText, const char* startText = "", bool border = false,
      FontId fontId = fontNormal);

   /// cleans up text scroll
   void Done();

   /// returns currently typed in text
   const std::string& GetText() const { return m_text; }

   /// sets a handler function that is called when editing has finished or was aborted
   void SetEditingFinished(
      std::function<void(bool wasAborted)> editingFinishedHandler
      = std::function<void(bool wasAborted)>())
   {
      m_editingFinishedHandler = editingFinishedHandler;
   }

   /// sets a handler function that is called when the cursor position has changed
   void SetCursorPosChangedHandler(
      std::function<void(unsigned int cursorPosX)> cursorPosChangedHandler
      = std::function<void(unsigned int cursorPosX)>())
   {
      m_cursorPosChangedHandler = cursorPosChangedHandler;
   }

   // virtual methods from Window
   virtual bool ProcessEvent(SDL_Event& event) override;

private:
   /// updates text edit window
   void UpdateText();

   /// sends text edit user event
   void SendTextEditEvent(Sint32 userEventCode);

private:
   /// background color
   Uint8 m_backgroundColor = 0;

   /// color of prefix text
   Uint8 m_prefixColor = 1;

   /// text color
   Uint8 m_textColor = 1;

   /// border color, if a border is used
   Uint8 m_borderColor = 1;

   /// cursor position; index into text string
   size_t m_cursorPos = 0;

   /// indicates if the edit window has a border
   bool m_border = false;

   /// indicates if only uppercase letters should be used
   bool m_uppercase = false;

   /// prefix text
   std::string m_prefix;

   /// input text
   std::string m_text;

   /// text font
   Font m_font;

   /// handler to call when cursor position has changed
   std::function<void(unsigned int cursorPosX)> m_cursorPosChangedHandler;

   /// handler to call when text editing was finished or aborted
   std::function<void(bool wasAborted)> m_editingFinishedHandler;
};
