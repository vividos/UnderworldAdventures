/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2003 Underworld Adventures Team

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
/*! \file import.hpp

   \brief game data import

   import

*/

// include guard
#ifndef uwadv_import_hpp_
#define uwadv_import_hpp_

// needed includes
#include "image.hpp"


// forward references
class ua_settings;
class ua_font;
class ua_image;
class ua_lp_descriptor;
class ua_object_properties;
class ua_player;
class ua_level;
class ua_object_list;
class ua_object;


//! imports common to uw1 and uw2
class ua_uw_import
{
public:
   //! loads 8 main palettes
   void load_palettes(const char* allpalname,
      ua_palette256_ptr allpalettes[8]);

   //! loads all 32 auxiliary palettes with 16 indices each
   void load_aux_palettes(const char* auxpalname,
      Uint8 allauxpals[32][16]);

   //! loads a *.gr image
   void load_image_gr(ua_image& img, const char* imgname, unsigned int imgnum,
      Uint8 auxpalettes[32][16]);

   //! loads a list of image from a *.gr file
   void load_image_gr_list(std::vector<ua_image>& imglist,
      const char* imgname, unsigned int img_from, unsigned int img_to,
      Uint8 auxpalettes[32][16]);

   //! loads a *.byt image
   void load_image_byt(const char* imgname, Uint8* pixels);

   //! loads texture images
   void load_textures(std::vector<ua_image>& tex_images,
      unsigned int startidx, const char* texname,
      ua_palette256_ptr palette);

   //! loads a font
   void load_font(const char* fontname, ua_font& font);

   //! loads cutscene data
   void load_cutscene(const char* filename, ua_image& image,
      std::vector<ua_lp_descriptor>& lpdarray,
      std::vector<Uint8>& lpages, unsigned int& records);

   //! extracts cutscene data from source data
   void extract_cutscene_data(Uint8* src, Uint8* dst, unsigned int maxpix);


   //! loads properties
   void load_properties(ua_object_properties& prop, const char* path);

   //! loads player info
   void load_player(ua_player& player, const char* path);

   //! loads map objects
   void load_mapobjects(ua_object_list& objlist, SDL_RWops* rwops, Uint16 texmap[64]);

   //! loads uw1, uw_demo or uw2 level maps
   virtual void load_levelmaps(std::vector<ua_level> &levels, ua_settings &settings,
      const char* folder);

protected:

   //! loads tilemap infos
   void load_tilemap(ua_level& level, SDL_RWops* rwops, Uint16 textures[64], bool uw2_mode);

   //! loads texture info
   void load_texinfo(ua_level& level, SDL_RWops* rwops, Uint16 textures[64], bool uw2_mode);

   //! adds object to master object list and follows link1 and link2 objs
   void addobj_follow(std::vector<ua_object>& master_obj_list,
      Uint32 objprop[0x400*2], Uint8 npcinfo[0x100*19],
      Uint16 objpos, Uint16 texmap[64], Uint8 tilex, Uint8 tiley);

private:
   //! loads *.gr image into pixels array
   void load_image_gr_impl(ua_image& image, FILE* fd, Uint8 auxpalidx[32][16],
      bool special_panels);
};

//! uw1 specific imports
class ua_uw1_import: public ua_uw_import
{
public:
   //! loads uw1 or uw_demo level maps
   virtual void load_levelmaps(std::vector<ua_level> &levels, ua_settings &settings,
      const char* folder);
};

//! uw2 specific imports
class ua_uw2_import: public ua_uw_import
{
public:
   //! loads uw2 levelmaps
   virtual void load_levelmaps(std::vector<ua_level> &levels, ua_settings &settings,
      const char* folder);

protected:
   //! creates SDL_RWops struct from compressed .ark file blocks (uw2 only)
   SDL_RWops* get_rwops_uw2dec(FILE* fd,unsigned int blocknum, unsigned int destsize);

   //! callback function to free uw2dec RWops memory
   static int uw2dec_close(SDL_RWops* rwops);
};


// inline functions

//! retrieves "count" bits from "value", starting at bit "start"
inline Uint32 ua_get_bits(Uint32 value, unsigned int start, unsigned int count)
{
   return (value>>start) & ((1<<count)-1);
}


#endif
