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
/*! \file savegame.cpp

   \brief savegame reading/writing implementation

   savegame naming scheme is "uasaveXXXXX.uas"

*/

// needed includes
#include "common.hpp"
#include "savegame.hpp"
#include "fread_endian.hpp"
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>


// constants

//! current version
const Uint32 ua_savegame::current_version = 0;


// ua_savegame methods

Uint32 ua_savegame::get_version()
{
   return save_version;
}

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

void ua_savegame::read_string(std::string& str)
{
   str.erase();
   Uint16 len = read16();
   for(unsigned int i=0; i<len; i++)
      str.append(1,static_cast<char>(read8()));
}

void ua_savegame::write8(Uint8 value)
{
   fputc(value,sg);
}

void ua_savegame::write16(Uint16 value)
{
   fwrite16(sg,value);
}

void read_string(std::string& str)
{
}

void ua_savegame::write32(Uint32 value)
{
   fwrite32(sg,value);
}

void ua_savegame::write_string(const char* str)
{
   Uint16 len = strlen(str);
   write16(len);
   for(unsigned int i=0; i<len; i++) write8(static_cast<Uint8>(str[i]));
}

void ua_savegame::begin_section(const char* section_name)
{
   if (saving)
      fputs(section_name,sg);
   else
   {
      unsigned int len = strlen(section_name);

      std::vector<char> buffer;
      buffer.resize(len+1);

      fgets(&buffer[0],len+1,sg);
      buffer[len]=0;

      // check if section names are the same
      if (strcmp(&buffer[0],section_name)!=0)
         throw ua_exception("savegame loading: section name mismatch");
   }
}

void ua_savegame::end_section()
{
}

void ua_savegame::close()
{
   fclose(sg);
}

void ua_savegame::open(const char* filename, bool issaving)
{
   saving = issaving;
   sg = fopen(filename, saving ? "wb" : "rb");

   // read or write header
   begin_section("header");
   if (saving)
   {
      // write header
      write32(current_version);
      write8(0); // compression: none
      write8(info.type); // game type: uw1

      write_string(info.title.c_str());
   }
   else
   {
      // read header
      save_version = read32();
      Uint8 compression = read8();
      info.type = read8();

      read_string(info.title);
   }
   end_section();
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
   info.title = "my savegame";

   // TODO: retrieve image
}

std::string ua_savegames_manager::get_savegame_filename(unsigned int index)
{
   return savegames[index];
}

ua_savegame ua_savegames_manager::get_savegame_load(unsigned int index)
{
   std::string save_name(savegames[index]);

   // open savegame for loading
   ua_savegame sg;
   sg.open(save_name.c_str(),false);

   return sg;
}

ua_savegame ua_savegames_manager::get_savegame_save_new_slot()
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

   // open savegame for saving
   ua_savegame sg;
   sg.open(save_name.c_str(),true);

   return sg;
}

ua_savegame ua_savegames_manager::get_savegame_save_overwrite(unsigned int index)
{
   std::string save_name(savegames[index]);

   // overwrites savegame with existing name
   ua_savegame sg;
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

ua_savegame ua_savegames_manager::get_quicksave(bool saving)
{
   ua_savegame sg;

   std::string quicksave_name(savegame_folder);
   quicksave_name.append("quicksave.uas");

   // open quicksave savegame in given mode
   sg.open(quicksave_name.c_str(),saving);

   return sg;
}
