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
/*! \file texture.hpp

   a

*/

// include guard
#ifndef __uwadv_texture_hpp_
#define __uwadv_texture_hpp_

// needed includes
#include "settings.hpp"
#include <vector>


// typedefs
typedef Uint8 ua_onepalette[256][4];


// classes

//! palette class
class ua_palettes
{
public:
   //! ctor
   ua_palettes(){}

   //! loads main palettes and all auxiliary palette tables
   void load(ua_settings &settings);

   //! returns an auxiliary palette table; type is Uint8 [256][4]
   ua_onepalette &get_palette(Uint8 pal){ return allpals[pal]; }

   //! returns an auxiliary palette table; type is Uint8 [16]
   Uint8 *get_auxpal(Uint8 idx){ return allauxpals[idx]; }

protected:
   //! all main palettes
   ua_onepalette allpals[8];

   //! all auxiliary palettes
   Uint8 allauxpals[32][16];
};


//! texture manager
class ua_texture_manager
{
public:
   //! ctor
   ua_texture_manager();
   //! dtor
   ~ua_texture_manager();

   //! initializes texture manager
   void init(ua_settings &settings);

   //! resets usage of textures in OpenGL
   void reset();

   //! prepares a texture for usage in OpenGL
   void prepare(bool wall, int idx);

   //! use a texture in OpenGL
   void use(bool wall, int idx);

private:
   //! loads all palettes
   void load_palettes(const char *allpalname);

   //! loads textures from file
   void load_textures(bool wall, const char *texfname);

   //! deletes memory for textures
   void delete_textures(bool wall);

protected:
   //! all palettes
   ua_palettes pals;

   //! OpenGL wall texture numbers
   std::vector<GLuint> wall_texnums;

   //! OpenGL floor texture numbers
   std::vector<GLuint> floor_texnums;

   //! last bound texture name
   GLuint last_texname;

   // wall texture stuff

   //! all wall textures
   Uint8 **allwalltex;

   //! number of wall textures
   int numwalltex;

   //! resolution of wall textures
   Uint8 wallxyres;

   // floor texture stuff

   //! all floor textures
   Uint8 **allfloortex;

   //! number of floor textures
   int numfloortex;

   //! resolution of floor textures
   Uint8 floorxyres;
};

#endif
