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
/*! \file underworld.hpp

   \brief underworld class

*/

// include guard
#ifndef __uwadv_underworld_hpp_
#define __uwadv_underworld_hpp_

// needed includes
#include <vector>
#include "files.hpp"
#include "level.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "conv/codevm.hpp"
#include "level.hpp"
#include "physics.hpp"
#include "uwscript.hpp"
#include "gamestrings.hpp"
#include "savegame.hpp"


// forward declaration
class ua_game_core_interface;


// classes

//! underworld class
class ua_underworld
{
public:
   //! ctor
   ua_underworld(){}

   //! initializes underworld
   void init(ua_settings& settings, ua_files_manager& filesmgr);

   //! clean up the underworld
   void done();

   //! evaluates whole underworld for a given time point
   void eval_underworld(double time);

   //! returns if enhanced features are turned on
   bool have_enhanced_features(){ return enhanced_features; }

   // access to underworld components

   //! returns player
   ua_player &get_player(){ return player; }

   //! returns player's inventory
   ua_inventory &get_inventory(){ return inventory; }

   //! returns physics model
   ua_physics_model &get_physics(){ return physics; }

   //! returns conversation globals
   ua_conv_globals &get_conv_globals(){ return conv_globals; }

   //! returns a levelmap
   ua_level &get_level(unsigned int level){ return levels[level]; }

   //! returns current level
   ua_level &get_current_level();

   //! returns script bindings object
   ua_underworld_script_bindings &get_scripts(){ return script; }

   //! returns game strings object
   ua_gamestrings& get_strings(){ return gstr; }

   //! changes current level
   void change_level(unsigned int level);

   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   //! imports a saved game
   void import_savegame(ua_settings &settings,const char *folder,bool initial);

protected:
   //! indicates if enhanced features are enabled
   bool enhanced_features;

   //! the player object
   ua_player player;

   //! player's inventory
   ua_inventory inventory;

   //! conversation globals of the current underworld
   ua_conv_globals conv_globals;

   //! physics model to use in underworld
   ua_physics_model physics;

   //! all underworld levels
   std::vector<ua_level> levels;

   //! underworld script bindings;
   ua_underworld_script_bindings script;

   //! hey, it's all the game strings!
   ua_gamestrings gstr;
};

#endif
