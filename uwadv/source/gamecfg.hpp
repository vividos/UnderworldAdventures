/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file gamecfg.hpp

   \brief game config loader

*/
//! \ingroup game

//@{

// include guard
#ifndef uwadv_gamecfg_hpp_
#define uwadv_gamecfg_hpp_

// needed includes
#include "cfgfile.hpp"
#include <string>


// forward references
class ua_game_interface;


// classes

//! game.cfg loader
/*! Loads game configuration from game.cfg file found in uadata subfolders
    prefixed with the current game prefix (%game-prefix%/game.cfg).

    To start loading, call ua_cfgfile::load().

    \todo use member game_name somewhere
*/
class ua_gamecfg_loader: public ua_cfgfile
{
public:
   //! ctor
   ua_gamecfg_loader(ua_game_interface& game_int):game(game_int){}

protected:
   //! called to load a specific value
   virtual void load_value(const std::string& name, const std::string& value);

protected:
   //! game interface
   ua_game_interface& game;

   //! game name
   std::string game_name;
};


#endif
//@}
