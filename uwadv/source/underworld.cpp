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
/*! \file underworld.cpp

   \brief underworld object

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"
#include "uamath.hpp"


// ua_underworld methods

void ua_underworld::init(ua_game_core_interface *thecore)
{
   core=thecore;

   levels.clear();

   physics.init(this);

   inventory.init(this);

   player.init();

   // init the scripts as last step
   script.init(this);
}

void ua_underworld::done()
{
   script.done();
}

void ua_underworld::eval_underworld(double time)
{
   // evaluate physics
   physics.eval_player_movement(time);

   // call Lua tick script
   script.lua_game_tick(time);
}

double ua_underworld::get_player_height()
{
   // TODO: ask physics model about player height
   //return get_current_level().get_floor_height(player.get_xpos(),player.get_ypos());
   
   // Height is now stored in player object after collision detection.
   return player.get_height();
}

ua_level &ua_underworld::get_current_level()
{
   unsigned int curlevel = player.get_attr(ua_attr_maplevel);
   return levels[curlevel];
}

void ua_underworld::change_level(unsigned int level)
{
   if (level>levels.size())
      throw ua_exception("game wanted to change to unknown level");

   // set new level
   player.set_attr(ua_attr_maplevel,level);

   get_current_level().prepare_textures(core->get_texmgr());
}

//! loads a savegame
void ua_underworld::load_game(ua_savegame &sg)
{
   // load all levels
   {
      levels.clear();

      sg.begin_section("tilemaps");

      Uint32 max = sg.read32();

      for(Uint32 i=0; i<max; i++)
      {
         ua_level newlevel;

         // load tilemap / objects list / annotations
         newlevel.load_game(sg);

         levels.push_back(newlevel);
      }

      sg.end_section();
   }

   // load map annotations

   // load objects list

   // load inventory
   inventory.load_game(sg);

   // save player infos
   player.load_game(sg);

   // conv. globals
   conv_globals.load_game(sg);

   // Lua script values
   script.load_game(sg);

   sg.close();

   // reload level
   change_level(player.get_attr(ua_attr_maplevel));
}

//! saves to a savegame
void ua_underworld::save_game(ua_savegame &sg)
{
   // save all levels
   {
      sg.begin_section("tilemaps");

      unsigned int max = levels.size();
      sg.write32(max);

      for(unsigned int i=0; i<max; i++)
      {
         // save tilemap / objects list / annotations
         levels[i].save_game(sg);
      }

      sg.end_section();
   }

   // save inventory
   inventory.save_game(sg);

   // save player infos
   player.save_game(sg);

   // conv. globals
   conv_globals.save_game(sg);

   // Lua script values
   script.save_game(sg);

   sg.close();
}
