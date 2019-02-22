//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file textedit.hpp
/// \brief text edit window
//
#pragma once

#include "imgquad.hpp"
#include "font.hpp"

/// \brief Text edit window class
/// \note sends ua_event_textedit_finished user event to screen when the user
/// finished editing, either by pressing return or escape
class ua_textedit_window : public ua_image_quad
{
public:
   /// ctor
   ua_textedit_window() {}
   virtual ~ua_textedit_window();

   /// initializes text scroll
   void init(IGame& game, unsigned int xpos, unsigned int ypos,
      unsigned int width, Uint8 bg_color, Uint8 prefix_color, Uint8 text_color,
      const char* prefix_text, const char* start_text = "", bool border = false);

   /// cleans up text scroll
   void done();

   /// returns currently typed in text
   const char* get_text() { return text.c_str(); }

   /// updates text edit window
   void update_text();

   // virtual methods from ua_window
   virtual bool process_event(SDL_Event& event) override;

protected:
   /// background color
   Uint8 bg_color;

   /// color of prefix text
   Uint8 prefix_color;

   /// text color
   Uint8 text_color;

   /// cursor position; index into text string
   unsigned int cursor_pos;

   /// indicates if the edit window has a border
   bool border;

   /// prefix text
   std::string prefix;

   /// input text
   std::string text;

   /// text font
   ua_font font;
};
