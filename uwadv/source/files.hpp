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
/*! \file files.hpp

   \brief resource files management

*/

// include guard
#ifndef __uwadv_files_hpp_
#define __uwadv_files_hpp_

// needed includes
#include "settings.hpp"
#include "SDL_rwops.h"
#include <string>
#include <vector>
#include "savegame.hpp"


// classes

//! resource files manager
class ua_files_manager
{
public:
   //! ctor
   ua_files_manager();

   //! initializes files manager; loads all config files; also sets gametype in ua_settings
   void init(ua_settings &settings);

   //! returns "uadata" file
   SDL_RWops *get_uadata_file(const char *relpath);

   //! loads a lua script
   int load_lua_script(lua_State *L, const char *basename);

   //! returns true when savegames are available
   bool savegames_avail();

   //! returns a quicksave savegame
   ua_savegame get_quicksave(bool saving);

   //! returns a list of all configuration files to load
   std::vector<std::string> &get_cfgfiles_list() { return cfgfiles_list; }

protected:
   //! initialize list of config files
   void init_cfgfiles_list();

protected:
   //! path to "uadata" dir
   std::string uadata_path;

   //! list of all config files to load
   std::vector<std::string> cfgfiles_list;

   //! savegame folder name
   std::string savegame_folder;

};

#endif
