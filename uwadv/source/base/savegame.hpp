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
/*! \file savegame.hpp

   \brief savegame class and savegames manager

   The ua_savegame class represents a savegame on disk and can be in one of
   two states, either loading and saving. It has a ua_savegame_info struct
   that contains infos about the savegame. The ua_savegames_manager manages
   savegames on disk and creating new ones. It also has support for quicksave
   savegames.

   Using the classes is simple. Just init() and let the savegames manager
   rescan() to find the savegame files (*.uas). The list of available
   savegames can be queried with get_savegames_count() and get_savegame_info()
   and a savegame to load can be retrieved with get_savegame_load() or
   get_savegame_from_file(). New savegames can be created with
   get_savegame_save_new_slot() or get_savegame_save_overwrite().

   A savegame contains several sections that are started with begin_section()
   and ended with end_section(). Values can be read and write via the readN()
   and writeN() functions. When done, close() should be called.

   A savegame carries a version number that lets the user decide which fields
   or parts of a savegame have to be loaded. This way older savegames can be
   supported.

   The savegame naming scheme is "uasaveXXXXX.uas".

*/
//! \ingroup base

//@{

// include guard
#ifndef uwadv_savegame_hpp_
#define uwadv_savegame_hpp_

// we have zlib compressed savegames
#define HAVE_ZLIB_SAVEGAME

// needed includes
#include "settings.hpp"

#ifdef HAVE_ZLIB_SAVEGAME
#include <zlib.h>
#endif


// structs

//! savegame info struct
struct ua_savegame_info
{
   //! ctor
   ua_savegame_info();

   //! game type; 0 = uw1, 1 = uw2
   unsigned int type;

   //! savegame title
   std::string title;

   //! name of game prefix
   std::string game_prefix;

   // player infos

   //! player name
   std::string name;

   //! player gender (0=male, 1=female)
   unsigned int gender;

   //! player appearance (0..4)
   unsigned int appearance;

   //! player profession (0..7)
   unsigned int profession;

   //! current map level
   unsigned int maplevel;

   //! basic player stats
   unsigned int strength,dexterity,intelligence,vitality;

   // savegame preview screenshot

   //! screenshot bytes in RGBA format
   std::vector<Uint32> image_rgba;

   //! screenshot resolution
   unsigned int image_xres, image_yres;
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

   //! writes out savegame info
   void write_info();

   //! reads in savegame info
   void read_info();

protected:

   //! input file
#ifdef HAVE_ZLIB_SAVEGAME
   gzFile sg;
#else
   FILE* sg;
#endif

   //! true when currently saving
   bool saving;

   //! savegame version
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
   void init(ua_settings& settings);

   //! rescans for existing savegames
   void rescan();

   //! returns number of available savegames
   unsigned int get_savegames_count();

   //! returns title of savegame
   void get_savegame_info(unsigned int index, ua_savegame_info& info);

   //! returns name of savegame file
   std::string get_savegame_filename(unsigned int index);

   //! returns savegame object for loading
   ua_savegame get_savegame_load(unsigned int index, bool save_image=false);

   //! returns savegame object for loading from specified filename
   ua_savegame get_savegame_from_file(const char* filename);

   //! creates a savegame in a new slot; uses savegame info for savegame
   ua_savegame get_savegame_save_new_slot(ua_savegame_info& info);

   //! overwrites an existing savegame; uses savegame info
   ua_savegame get_savegame_save_overwrite(unsigned int index,
      ua_savegame_info& info);

   //! returns true when a quicksave savegame is available
   bool quicksave_avail();

   //! sets screenshot for next save operation
   void set_save_screenshot(std::vector<Uint32>& image_rgba,
      unsigned int xres, unsigned int yres);

protected:
   //! savegame folder name
   std::string savegame_folder;

   //! game prefix of current file
   std::string game_prefix;

   //! list of all current savegames
   std::vector<std::string> savegames;

   //! savegame image in rgba format
   std::vector<Uint32> image_savegame;

   //! savegame image resolution
   unsigned int image_xres,image_yres;
};


#endif
//@}
