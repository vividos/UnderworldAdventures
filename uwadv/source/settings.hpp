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
/*! \file settings.hpp

   config file loader class

*/

// include guard
#ifndef __uwadv_settings_hpp_
#define __uwadv_settings_hpp_

// needed includes
#include <string>


// enum

//! game type enum
typedef enum
{
   //! no game available
   ua_game_none=0,

   //! we have the full game
   ua_game_uw1,

   //! only the demo is available
   ua_game_uw_demo,

   //! we have uw2
   ua_game_uw2,

} ua_game_type;


// classes

//! config class
class ua_settings
{
public:
   //! ctor
   ua_settings(){}

   //! loads settings from config file
   void load();

   //! path to uw1
   std::string uw1_path;

   //! game type
   ua_game_type gtype;

protected:
   //! initializes more stuff after loading
   void init();

   //! checks if a file is available
   bool file_isavail(const char *fname);

   //! processes a single option
   void process_option(int option, const char *value);
};

#endif
