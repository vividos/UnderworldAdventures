/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file textscroll.hpp

   \brief text scroll

   the text scroll class is based on the generic image quad rendering class
   ua_image_quad

*/
//! \ingroup userinterface

//@{

// include guard
#ifndef uwadv_textscroll_hpp_
#define uwadv_textscroll_hpp_

// needed includes
#include <vector>
#include "imgquad.hpp"
#include "font.hpp"


// classes

//! text scroll class
/*! Text scroll window that can be used to show text messages. The scroll
    stops when the scroll is full and shows a "[MORE]" text, waiting for a key
    press. It also has a built-in input mode, e.g. for conversation answers
    and such.

    \todo factor out input mode into own ua_input_ctrl or ua_text_edit_ctrl
    \todo add up/down buttons from buttons.gr, images 0/ or 27/28, to go
    through scroll history
    \todo add left and right borders
    \todo limit scroll history to some max. number of lines, e.g. 256
*/
class ua_textscroll: public ua_image_quad
{
public:
   //! ctor
   ua_textscroll(){}

   //! initializes text scroll
   void init(ua_game_interface& game, unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int lines, Uint8 bg_color);

   //! sets new color
   inline void set_color(Uint8 color);

   //! prints a string to the text scroll; returns true when text had to scroll
   bool print(const char* text);

   //! clears scroll contents
   void clear_scroll();

   //! handles events needed for the text scroll; returns true when handled
   bool handle_event(SDL_Event& event);

   //! starts input mode on the current line
   void enter_input_mode(const char* text="");

   //! returns true when text was entered; "text" is updated then
   bool is_input_done(std::string& text);

   //! returns true when more lines to show are available
   inline bool have_more_lines();

protected:
   //! updates scroll texture
   void update_scroll();

   //! scroll more lines
   void show_more_lines();

protected:
   //! stack with currently shown lines
   std::vector<std::string> linestack;

   //! color values for all shown lines
   std::vector<Uint8> linecolors;

   //! stack with lines to show after "[MORE]"
   std::vector<std::string> morestack;

   //! history of lines printed
   std::vector<std::string> linehistory;

   //! font to render lines
   ua_font font_normal;

   //! maximal number of lines to show
   unsigned int maxlines;

   //! background color
   Uint8 bg_color;

   //! current text color
   Uint8 text_color;

   //! indicates if text scroll is in input mode
   bool input_mode;

   //! text the user typed in so far
   std::string input_text;

   //! line with cursor to input text
   unsigned int input_line;
};


// inline methods

inline void ua_textscroll::set_color(Uint8 color)
{
   text_color = color;
}

inline bool ua_textscroll::have_more_lines()
{
   return morestack.size()>0;
}


#endif
//@}
