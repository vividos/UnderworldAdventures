/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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


// enums

/*! textscroll color codes for call to ua_textscroll::set_color_code() */
enum ua_textscroll_colorcode
{
   ua_cc_black='0',  //!< black (46) (normal text and conversation)
   ua_cc_orange,     //!< orange (38) (conversation answer)
   ua_cc_white,      //!< white (11) (not used in uw1)
   ua_cc_blue,       //!< blue (48) (not used in uw1)
   ua_cc_red,        //!< red (180) (restore game failed)
   ua_cc_yellow,     //!< yellow (21) (not used in uw1)
   ua_cc_green,      //!< green (212) (save file description)
   ua_cc_violet,     //!< violet (251) (not used in uw1)
   ua_cc_gray,       //!< gray (24) (not used in uw1)
   ua_cc_light_blue  //!< light-blue (192) (not used in uw1)
};


// classes

//! text scroll class
/*! Text scroll window that can be used to show text messages. The scroll
    stops when the scroll is full and shows a "[MORE]" text, waiting for a key
    press. The text scroll understands some color codes that controls the
    text color. The color codes consist of a \ (backslash) and a following
    number, e.g. "\0" or "\3". These numbers are available:
    0: black
    1: orange
    2: white
    3: blue
    4: red
    5: yellow
    6: green
    7: violet
    8: gray
    9: light-blue
    The enumeration ua_textscroll_colorcode can be used in the call to
    set_color_code().

    \todo factor out input mode into own ua_input_ctrl or ua_text_edit_ctrl
    \todo add up/down buttons from buttons.gr, images 0/ or 27/28, to go
    through scroll history
    \todo add left and right borders
    \todo limit scroll history to some max. number of lines, e.g. 256
    \todo implement more codes: \p paragraph, \m show "more" immediately
*/
class ua_textscroll: public ua_image_quad
{
public:
   //! ctor
   ua_textscroll(){}

   //! initializes text scroll
   void init(ua_game_interface& game, unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height, Uint8 bg_color);

   //! sets new foreground text color code
   void set_color_code(char color);

   //! prints a string to the text scroll
   bool print(const char* text);

   //! clears scroll contents
   void clear_scroll();

   // virtual methods from ua_window
   virtual bool process_event(SDL_Event& event);

protected:
   //! updates scroll texture
   void update_scroll();

   //! calculates image width of a string; ignores color codes
   unsigned int calc_colored_length(const char* text);

   //! creates image from string; processes color codes
   void create_colored_string(ua_image& img, const char* text);

protected:
   //! background color
   Uint8 bg_color;

   //! current color code (from '0' to '9')
   char color_code;

   //! font to render text
   ua_font font_normal;

   //! maximal number of lines to show
   unsigned int maxlines;

   //! number of first visible line
   unsigned int first_visible_line;

   //! indicates if user has to press a key to see the rest of the scroll text
   bool more_mode;

   //! line where the [MORE] is shown; only valid in 'more mode'
   unsigned int more_line;

   //! all text lines printed so far
   std::vector<std::string> textlines;

   //! scroll start base coordinates
   unsigned int scroll_basex, scroll_basey;
};


// inline methods

inline void ua_textscroll::set_color_code(char color)
{
   color_code = color;
}


#endif
//@}
