/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
#include "font.hpp"
#include "fread_endian.hpp"


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

void ua_font::init(ua_settings &settings, ua_font_id fontid)
{
   if (fontid >= SDL_TABLESIZE(ua_font_names))
      throw ua_exception("ua_font::init: invalid font id");

   // do font name
   std::string fontname;
   fontname.assign(settings.get_string(ua_setting_uw_path));
   fontname.append("data/");
   fontname.append(ua_font_names[fontid]);

   // and load it
   init(fontname.c_str());
}

void ua_font::init(const char *fontname)
{
   // open font file
   FILE *fd = fopen(fontname,"rb");
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
   charsize = fread16(fd);
   spacewidth = fread16(fd);
   charheight = fread16(fd);
   rowwidth = fread16(fd);
   maxwidth = fread16(fd);

   // fix for overly large font chars like 'm' or 'w'
   if (strstr(fontname,"fontbig")!=0)
      maxwidth += 4;

   // calculate number of chars
   nchars = (flen-12) / (charsize+1);

   // allocate memory for new data
   fontdata.resize(nchars*charheight*maxwidth);
   charlengths.resize(nchars);

   for(unsigned int n=0; n<nchars; n++)
   {
      unsigned int bits = 0;
      Uint8 curbits=0;

      // read in whole character
      for (unsigned int h=0; h<charheight; h++)
      {
         for (unsigned int w=0; w<maxwidth; w++)
         {
            if (bits==0)
            {
               curbits=fgetc(fd);
               bits=8;
            }

            if ((curbits&0x80)==0)
            {
               // background pixel
               fontdata[n*charheight*maxwidth + h*maxwidth + w] = 0;
            }
            else
            {
               fontdata[n*charheight*maxwidth + h*maxwidth + w] = 1;
            }

            curbits <<= 1;
            bits--;
         }

         // reset bit reservoir every line
         bits=0;
      }

      // read char length in pixels
      charlengths[n] = fgetc(fd);
   }

   fclose(fd);
}
