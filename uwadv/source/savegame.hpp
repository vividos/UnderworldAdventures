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
/*! \file savegame.hpp

   \brief savegame class and savegames manager

*/

// include guard
#ifndef __uwadv_savegame_hpp_
#define __uwadv_savegame_hpp_

// needed includes
#include "settings.hpp"


// structs

//! savegame info struct
struct ua_savegame_info
{
   //! ctor
   ua_savegame_info():title("no savegame title"), type(0)
   {
   }

   //! savegame title
   std::string title;

   //! game type; 0 = uw1, 1 = uw2
   unsigned int type;

//   ua_image preview;
};


// classes

//! savegame class
class ua_savegame
{
   friend class ua_savegames_manager;
public:
   // savegame loading functions

   //! returns version of savegame to load
   Uint32 get_version();

   //! reads a 8-bit value
   Uint8 read8();

   //! reads a 16-bit value
   Uint16 read16();

   //! reads a 32-bit value
   Uint32 read32();

   //! reads string from savegame
   void read_string(std::string& str);

   // savegame saving functions

   //! writes a 8-bit value
   void write8(Uint8 value);

   //! writes a 16-bit value
   void write16(Uint16 value);

   //! writes a 32-bit value
   void write32(Uint32 value);

   //! writes string to savegame
   void write_string(const char* str);

   // common functions

   //! starts new section to read/write
   void begin_section(const char* section_name);

   //! ends current section
   void end_section();

   //! finally closes savegame file
   void close();

   //! returns savegame info
   ua_savegame_info& get_savegame_info(){ return info; }

   //! current version
   static const Uint32 current_version;

protected:
   //! ctor
   ua_savegame():save_version(current_version){}

   //! opens savegame
   void open(const char* filename, bool saving);

protected:
   FILE *sg;
   bool saving;
   Uint32 save_version;

   //! savegame info
   ua_savegame_info info;
};


//! savegames manager
class ua_savegames_manager
{
public:
   //! ctor
   ua_savegames_manager();

   //! inits savegame manager
   void init(ua_settings &settings);

   //! rescans for existing savegames
   void rescan();

   //! returns number of available savegames
   unsigned int get_savegames_count();

   //! returns title of savegame
   void get_savegame_info(unsigned int index, ua_savegame_info &info);

   //! returns name of savegame file
   std::string get_savegame_filename(unsigned int index);

   //! returns savegame object for loading
   ua_savegame get_savegame_load(unsigned int index);

   //! creates a savegame in a new slot
   ua_savegame get_savegame_save_new_slot();

   //! overwrites an existing savegame
   ua_savegame get_savegame_save_overwrite(unsigned int index);

   //! returns true when a quicksave savegame is available
   bool quicksave_avail();

   //! returns the quicksave savegame
   ua_savegame get_quicksave(bool saving);

protected:
   //! savegame folder name
   std::string savegame_folder;

   //! list of all current savegames
   std::vector<std::string> savegames;
};

#endif
