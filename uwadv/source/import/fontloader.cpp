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
/*! \file fontloader.cpp

   \brief font loading implementation

   to simplify interface, fonts are loaded via an ua_font_id. more infos about
   font file layout can be found in the file docs/ua-formats.txt

*/

// needed includes
#include "common.hpp"
#include "import.hpp"
#include "font.hpp"
#include "io_endian.hpp"


// constants

//! uw1 font names
const char *ua_font_names[6] =
{
   "font4x5p.sys",
   "font5x6i.sys",
   "font5x6p.sys",
   "fontbig.sys",
   "fontbutn.sys",
   "fontchar.sys"
};


// ua_font methods

void ua_font::load(const char* uw_path, ua_font_id fontid)
{
   if (fontid >= (int)SDL_TABLESIZE(ua_font_names))
      throw ua_exception("ua_font::init: invalid font id");

   // do font name
   std::string fontname(uw_path);
   fontname.append("data/");
   fontname.append(ua_font_names[fontid]);

   // and load it
#ifndef UA_NO_IMPORTS
   ua_uw_import uw_import;

   uw_import.load_font(fontname.c_str(),*this);
#endif
}

void ua_uw_import::load_font(const char* fontname, ua_font& font)
{
   // open font file
   FILE* fd = fopen(fontname,"rb");
   if (fd==NULL)
   {
      std::string text("could not open font file: ");
      text.append(fontname);
      throw ua_exception(text.c_str());
   }

   // determine file length and number of chars
   fseek(fd,0,SEEK_END);
   unsigned long flen = ftell(fd);
   fseek(fd,0,SEEK_SET);

   // read in header
   Uint16 dummy;
   dummy = fread16(fd);
   font.charsize = fread16(fd);
   font.spacewidth = fread16(fd);
   font.charheight = fread16(fd);
   font.rowwidth = fread16(fd);
   font.maxwidth = fread16(fd);

   // fix for overly large font chars like 'm' or 'w'
   font.maxwidth |= 7;

   // calculate number of chars
   font.nchars = (flen-12) / (font.charsize+1);

   // allocate memory for new data
   font.fontdata.clear();
   font.charlengths.clear();

   font.fontdata.resize(font.nchars*font.charheight*font.maxwidth);
   font.charlengths.resize(font.nchars);

   for(unsigned int n=0; n<font.nchars; n++)
   {
      unsigned int bits = 0;
      Uint8 curbits=0;

      // read in whole character
      for (unsigned int h=0; h<font.charheight; h++)
      {
         for (unsigned int w=0; w<font.maxwidth; w++)
         {
            if (bits==0)
            {
               curbits=fgetc(fd);
               bits=8;
            }

            if ((curbits&0x80)==0)
            {
               // background pixel
               font.fontdata[n*font.charheight*font.maxwidth + h*font.maxwidth + w] = 0;
            }
            else
            {
               font.fontdata[n*font.charheight*font.maxwidth + h*font.maxwidth + w] = 1;
            }

            curbits <<= 1;
            bits--;
         }

         // reset bit reservoir every line
         bits=0;
      }

      // read char length in pixels
      font.charlengths[n] = fgetc(fd);
   }

   fclose(fd);
}
