/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures team

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
/*! \file savegame.cpp

   \brief savegame reading/writing implementation

   savegame naming scheme is "uasaveXXXXX.uas"

*/

// needed includes
#include "common.hpp"
#include "savegame.hpp"
#include "player.hpp"
#include "fread_endian.hpp"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>


// constants

//! current savegame version
const Uint32 ua_savegame::current_version = 0;


// ua_savegame_info methods
ua_savegame_info::ua_savegame_info()
:type(0),title("no savegame title"),gender(0),appearance(0),profession(0),
 maplevel(0),strength(0),dexterity(0),intelligence(0),vitality(0),
 image_xres(0), image_yres(0)
{
}

void ua_savegame_info::fill_infos(const ua_player& player)
{
   name = player.get_name();

   gender = player.get_attr(ua_attr_gender);
   appearance = player.get_attr(ua_attr_appearance);
   profession = player.get_attr(ua_attr_profession);
   maplevel = player.get_attr(ua_attr_maplevel);

   strength = player.get_attr(ua_attr_strength);
   dexterity = player.get_attr(ua_attr_dexterity);
   intelligence = player.get_attr(ua_attr_intelligence);
   vitality = player.get_attr(ua_attr_life);
}


// ua_savegame methods

Uint32 ua_savegame::get_version()
{
   return save_version;
}

#ifdef HAVE_ZLIB_SAVEGAME

Uint8 ua_savegame::read8()
{
   return gzgetc(sg);
}

Uint16 ua_savegame::read16()
{
   Uint16 data;
   gzread(sg,&data,2);
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert16(data);
#endif
   return data;
}

Uint32 ua_savegame::read32()
{
   Uint32 data;
   gzread(sg,&data,4);
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   data = ua_endian_convert32(data);
#endif
   return data;
}

void ua_savegame::write8(Uint8 value)
{
   gzputc(sg,value);
}

void ua_savegame::write16(Uint16 value)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   value = ua_endian_convert16(value);
#endif
   gzwrite(sg,&value,2);
}

void ua_savegame::write32(Uint32 value)
{
#if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
   value = ua_endian_convert32(value);
#endif
   gzwrite(sg,&value,4);
}

#else // HAVE_ZLIB_SAVEGAME

Uint8 ua_savegame::read8()
{
   return fgetc(sg);
}

Uint16 ua_savegame::read16()
{
   return fread16(sg);
}

Uint32 ua_savegame::read32()
{
   return fread32(sg);
}

void ua_savegame::write8(Uint8 value)
{
   fputc(value,sg);
}

void ua_savegame::write16(Uint16 value)
{
   fwrite16(sg,value);
}

void ua_savegame::write32(Uint32 value)
{
   fwrite32(sg,value);
}

#endif // HAVE_ZLIB_SAVEGAME

void ua_savegame::read_string(std::string& str)
{
   str.erase();
   Uint16 len = read16();
   for(unsigned int i=0; i<len; i++)
      str.append(1,static_cast<char>(read8()));
}

void ua_savegame::write_string(const char* str)
{
   Uint16 len = strlen(str);
   write16(len);
   for(unsigned int i=0; i<len; i++)
      write8(static_cast<Uint8>(str[i]));
}

void ua_savegame::begin_section(const char* section_name)
{
   if (saving)
   {
      write_string(section_name);
   }
   else
   {
      std::string read_name;
      read_string(read_name);

      // check if section names are the same
      if (0 != read_name.compare(section_name))
         throw ua_exception("savegame loading: section name mismatch");
   }
}

void ua_savegame::end_section()
{
}

void ua_savegame::close()
{
#ifdef HAVE_ZLIB_SAVEGAME
   gzclose(sg);
#else
   fclose(sg);
#endif
}

void ua_savegame::open(const char* filename, bool issaving)
{
   saving = issaving;

#ifdef HAVE_ZLIB_SAVEGAME
   sg = gzopen(filename, saving ? "wb" : "rb");
   if (saving)
      gzsetparams(sg,9,Z_DEFAULT_STRATEGY);
#else
   sg = fopen(filename, saving ? "wb" : "rb");
#endif

   // read or write header
   begin_section("header");
   if (saving)
   {
      // write header
      write32(current_version);
/*
#ifdef HAVE_ZLIB_SAVEGAME
      write8(1); // compression: zlib
#else
      write8(0); // compression: none
#endif
*/
      write_info();
   }
   else
   {
      // read header
      save_version = read32();
      //Uint8 compression = read8();

      read_info();
   }
   end_section();
}

void ua_savegame::write_info()
{
   write8(info.type);
   write_string(info.title.c_str());
   write_string(info.gamefolder.c_str());

   write_string(info.name.c_str());

   write8(info.gender);
   write8(info.appearance);
   write8(info.profession);
   write8(info.maplevel);

   write8(info.strength);
   write8(info.dexterity);
   write8(info.intelligence);
   write8(info.vitality);

   // write image
   write16(info.image_xres);
   write16(info.image_yres);

   unsigned int max = info.image_xres * info.image_yres;
   for(unsigned int i=0; i<max; i++)
      write32(info.image_rgba[i]);
}

void ua_savegame::read_info()
{
   // savegame infos
   info.type = read8();
   read_string(info.title);
   read_string(info.gamefolder);

   // player infos
   read_string(info.name);

   info.gender = read8();
   info.appearance = read8();
   info.profession = read8();
   info.maplevel = read8();

   info.strength = read8();
   info.dexterity = read8();
   info.intelligence = read8();
   info.vitality = read8();

   // read image
   info.image_xres = read16();
   info.image_yres = read16();

   unsigned int max = info.image_xres * info.image_yres;

   info.image_rgba.clear();
   info.image_rgba.resize(max);

   for(unsigned int i=0; i<max; i++)
      info.image_rgba[i] = read32();
}


// ua_savegames_manager methods

ua_savegames_manager::ua_savegames_manager()
{
}

void ua_savegames_manager::init(ua_settings &settings)
{
   savegame_folder = settings.get_string(ua_setting_savegame_folder);

   rescan();
}

void ua_savegames_manager::rescan()
{
   savegames.clear();

   // add quicksave savegame name
   if (quicksave_avail())
   {
      std::string quicksave_name(savegame_folder);
      quicksave_name.append("quicksave.uas");
      savegames.push_back(quicksave_name);
   }

   std::string pathname(savegame_folder);
   pathname.append("uasave*.uas");

   ua_find_files(pathname.c_str(),savegames);

   std::sort(savegames.begin(),savegames.end());
}

unsigned int ua_savegames_manager::get_savegames_count()
{
   return savegames.size();
}

void ua_savegames_manager::get_savegame_info(unsigned int index,
   ua_savegame_info &info)
{
   // open savegame and retrieve info
   ua_savegame sg = get_savegame_load(index);
   info = sg.get_savegame_info();
   sg.close();
}

std::string ua_savegames_manager::get_savegame_filename(unsigned int index)
{
   return savegames[index];
}

ua_savegame ua_savegames_manager::get_savegame_load(unsigned int index,
   bool save_image)
{
   std::string save_name(savegames[index]);

   // open savegame for loading
   ua_savegame sg;
   sg.open(save_name.c_str(),false);

   // set loaded image as new savegame image
   if (save_image)
   {
      ua_savegame_info& info = sg.get_savegame_info();
      set_save_screenshot(info.image_rgba, info.image_xres, info.image_yres);
   }

   return sg;
}

ua_savegame ua_savegames_manager::get_savegame_from_file(const char* filename)
{
   // open savegame for loading
   ua_savegame sg;
   sg.open(filename,false);

   return sg;
}

ua_savegame ua_savegames_manager::get_savegame_save_new_slot(
   ua_savegame_info& info)
{
   std::string save_name;

   // search for free savegame slot
   unsigned int index = 0;
   do
   {
      std::ostringstream buffer;

      // create savegame name
      buffer << savegame_folder.c_str() << "uasave"
         << std::setfill('0')
         << std::setw(5) << index
         << ".uas" << std::ends;

      save_name.assign(buffer.str().c_str());

      index++;

   } while( ua_file_exists(save_name.c_str()) );

   // add savegame preview image to savegame info
   info.image_rgba.clear();
   info.image_rgba = image_savegame;
   info.image_xres = image_xres;
   info.image_yres = image_yres;

   // open savegame for saving
   ua_savegame sg;
   sg.get_savegame_info() = info; // set savegame info
   sg.open(save_name.c_str(),true);

   return sg;
}

ua_savegame ua_savegames_manager::get_savegame_save_overwrite(
   unsigned int index, ua_savegame_info& info)
{
   std::string save_name(savegames[index]);

   // add savegame preview image to savegame info
   info.image_rgba.clear();
   info.image_rgba = image_savegame;
   info.image_xres = image_xres;
   info.image_yres = image_yres;

   // overwrites savegame with existing name
   ua_savegame sg;
   sg.get_savegame_info() = info; // set savegame info

   sg.open(save_name.c_str(),true);

   return sg;
}

bool ua_savegames_manager::quicksave_avail()
{
   std::string quicksave_name(savegame_folder);
   quicksave_name.append("quicksave.uas");

   // check if quicksave savegame file is available
   return ua_file_exists(quicksave_name.c_str());
}

ua_savegame ua_savegames_manager::get_quicksave(bool saving,
   const ua_player& player)
{
   ua_savegame sg;

   std::string quicksave_name(savegame_folder);
   quicksave_name.append("quicksave.uas");

   if (saving)
   {
      ua_savegame_info info;
      info.title = "Quicksave Savegame";

      info.fill_infos(player);

      // add savegame preview image to savegame info
      info.image_rgba.clear();
      info.image_rgba = image_savegame;
      info.image_xres = image_xres;
      info.image_yres = image_yres;

      sg.get_savegame_info() = info;
   }

   // open quicksave savegame in given mode
   sg.open(quicksave_name.c_str(),saving);

   return sg;
}

void ua_savegames_manager::set_save_screenshot(
   std::vector<Uint32>& image_rgba, unsigned int xres, unsigned int yres)
{
   image_savegame = image_rgba;
   image_xres = xres;
   image_yres = yres;
}
