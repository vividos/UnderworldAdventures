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

   underworld class

*/

// include guard
#ifndef __uwadv_underworld_hpp_
#define __uwadv_underworld_hpp_

// needed includes
#include <vector>
#include "core.hpp"
#include "level.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "conv/codevm.hpp"
#include "level.hpp"
#include "quadtree.hpp"
#include "physics.hpp"


// forward declaration
class ua_game_core_interface;


// classes

//! underworld class
class ua_underworld
{
public:
   ua_underworld(){}

   //! initializes underworld
   void init(ua_game_core_interface *core);

   //! loads complete game state from savegame
   void load_game(/*...*/);

   //! loads new game state
   void new_game(/*...*/);

   //! saves game state
   void save_game(/*...*/);

   //! clean up the underworld
   void done();

   //! moves player according to the direction angle
   void walk_player(double angle);

   //! transfers player to other location
//   void move_player(float x, float y, int level=-1);

   float get_player_height();

   //! renders the current game map and all objects
   void render(ua_frustum &fr);

   // access to underworld components

   ua_player &get_player(){ return player; }

   ua_inventory &get_inventory(){ return inventory; }

   ua_conv_globals &get_conv_globals(){ return conv_globals; }

   ua_level &get_level(unsigned int level);

protected:
   //! interface to core game class
   ua_game_core_interface *core;

   //! the player object
   ua_player player;

   //! player's inventory
   ua_inventory inventory;

   //! conversation globals of the current underworld
   ua_conv_globals conv_globals;

   ua_level level;

   //! physics model to use in underworld
   ua_physics_model physics;

   //! current level
//   unsigned int curlevel;

   //! all underworld levels
//   std::vector<ua_level*> levels;
};

#endif
