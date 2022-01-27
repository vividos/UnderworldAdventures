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
/// \file TextEditWindow.hpp
/// \brief text edit window
//
#pragma once

#include "ImageQuad.hpp"
#include "Font.hpp"

/// \brief Text edit window class
/// \note sends gameEventTexteditFinished user event to screen when the user
/// finished editing, either by pressing return or escape
class TextEditWindow : public ImageQuad
{
public:
   /// ctor
   TextEditWindow() {}
   virtual ~TextEditWindow();

   /// initializes text scroll
   void Init(IGame& game, unsigned int xpos, unsigned int ypos,
      unsigned int width, Uint8 backgroundColor, Uint8 prefixColor, Uint8 textColor,
      const char* prefixText, const char* startText = "", bool border = false);

   /// cleans up text scroll
   void Done();

   /// returns currently typed in text
   const char* GetText() { return m_text.c_str(); }

   /// updates text edit window
   void UpdateText();

   // virtual methods from Window
   virtual bool ProcessEvent(SDL_Event& event) override;

protected:
   /// background color
   Uint8 m_backgroundColor;

   /// color of prefix text
   Uint8 m_prefixColor;

   /// text color
   Uint8 m_textColor;

   /// cursor position; index into text string
   size_t m_cursorPos;

   /// indicates if the edit window has a border
   bool m_border;

   /// prefix text
   std::string m_prefix;

   /// input text
   std::string m_text;

   /// text font
   Font m_font;
};
