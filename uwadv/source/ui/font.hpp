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
/*! \file font.hpp

   \brief font class

*/

// include guard
#ifndef uwadv_font_hpp_
#define uwadv_font_hpp_

// needed includes
#include <vector>
#include "settings.hpp"
#include "image.hpp"


// typedefs
typedef enum
{
   ua_font_small=0,
   ua_font_italic=1,  // for character stats screen
   ua_font_normal=2,  // for scroll messages
   ua_font_big=3,     // for cutscenes
   ua_font_buttons=4, // for buttons
   ua_font_chargen=5  // for character generation
} ua_font_id;


// classes

//! font class
class ua_font
{
public:
   //! ctor
   ua_font(){}
   //! dtor
   //~ua_font();

   //! initializes and loads a font
   void init(ua_settings& settings, ua_font_id fontid);

   //! initializes and loads a font from file
   void init(const char* fontname);

   //! returns height of chars in pixels
   unsigned int get_charheight(){ return charheight; }

   //! calculates and returns length of string in pixel
   unsigned int calc_length(const char* str);

   //! creates image from string, using the font, using a foreground palette index
   void create_string(ua_image& image, const char* str, Uint8 fg_idx);

protected:
   //! font data
   std::vector<Uint8> fontdata;

   //! length info for every char in font
   std::vector<Uint8> charlengths;

   //! size of a character in bytes
   Uint16 charsize;

   //! width of a space char, in pixels
   Uint16 spacewidth;

   //! font height in pixels
   Uint16 charheight;

   //! width of a character row in bytes
   Uint16 rowwidth;

   //! maximum width of a char in pixels
   Uint16 maxwidth;

   //! number of characters in font
   unsigned int nchars;
};

#endif
