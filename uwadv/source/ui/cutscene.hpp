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
/*! \file cutscene.hpp

   \brief cutscene handling class

*/
//! \ingroup userinterface

//@{

// include guard
#ifndef uwadv_cutscene_hpp_
#define uwadv_cutscene_hpp_

// needed includes
#include <vector>
#include "settings.hpp"
#include "imgquad.hpp"
#include "texture.hpp"


// classes

//! cutscene animation class
class ua_cutscene: public ua_image_quad
{
public:
   //! ctor
   ua_cutscene(){ curframe=(unsigned int)-1; }

   //! loads a cutscene by main and sub number
   void load(ua_settings& settings, unsigned int main, unsigned int sub);

   //! loads a cutscene by relative filename
   void load(ua_settings& settings, const char *filename);

   //! loads a cutscene by absolute filename
   void load(const char *filename);

   //! initializes cutscene quad after loading
   void init(ua_texture_manager* texmgr, unsigned int xpos=0, unsigned int ypos=0);

   //! returns maximum number of frames
   unsigned int get_maxframes(){ return records; }

   //! extracts a new frame into the current image
   void get_frame(unsigned int framenum);

   // TODO: remove that method when it's no longer needed
   //! returns animation palette
   ua_onepalette& get_anim_palette(){ return quadpalette; }

protected:
   //! decodes one frame
   void decode_frame(unsigned int framenum);

protected:
   //! number of large pages in file
   unsigned int largepages;

   //! number of records in file
   unsigned int records;

   //! number of current frame
   unsigned int curframe;

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
};


#endif
//@}
