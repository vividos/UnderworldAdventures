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
/*! \file cutscene.hpp

   cutscene handling class

*/

// include guard
#ifndef __uwadv_cutscene_hpp_
#define __uwadv_cutscene_hpp_

// needed includes
#include <vector>
#include "settings.hpp"
#include "texture.hpp"


// classes

//! cutscene animation class
class ua_cutscene
{
public:
   //! ctor
   ua_cutscene(){ curframe=(unsigned int)-1; }

   //! loads a cutscene by main and sub number
   void load(ua_settings &settings, unsigned int main, unsigned int sub);

   //! loads a cutscene by relative filename
   void load(ua_settings &settings, const char *filename);

   //! loads a cutscene by absolute filename
   void load(const char *filename);

   //! returns width of animation
   unsigned int get_width(){ return width; }
   //! returns height of animation
   unsigned int get_height(){ return height; }

   //! returns maximum number of frames
   unsigned int get_maxframes(){ return records; }

   //! extracts a new frame into a texture
   void get_frame(ua_texture &tex, unsigned int framenum);

protected:
   //! decodes one frame
   void decode_frame(unsigned int framenum);

protected:
   //! width of animation bitmap
   unsigned int width;
   //! height of animation bitmap
   unsigned int height;

   //! number of large pages in file
   unsigned int largepages;
   //! number of records in file
   unsigned int records;

   //! number of current frame
   unsigned int curframe;

   //! animation palette
   Uint8 palette[256][4];

   //! large page descriptor struct type
   struct lp_descriptor
   {
      Uint16 base;
      Uint16 records;
      Uint16 bytes;
   };

   //! large page descriptor array values
   std::vector<lp_descriptor> lpdarray;

   //! all large pages
   std::vector<Uint8> lpages;

   //! output buffer
   std::vector<Uint8> outbuffer;
};

#endif
