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

   a

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"
#include <cmath>


// ua_underworld methods

void ua_underworld::init(ua_game_core_interface *thecore)
{
   core=thecore;

   level.load(core->get_settings(),0);
   level.prepare_textures(core->get_texmgr());

   player.init(32.f,2.f);
}

void ua_underworld::load_game()
{
}

void ua_underworld::new_game()
{
}

void ua_underworld::save_game()
{
}

void ua_underworld::done()
{
}

void ua_underworld::walk_player(float angle)
{
   // speed: 1.2 tiles per second
   float speed = 1.8f / core->get_tickrate();

   angle+=90.f;

   player.move_player(
      player.get_xpos()+speed*cos(ua_deg2rad(angle)),
      player.get_ypos()+speed*sin(ua_deg2rad(angle)) );
}

float ua_underworld::get_player_height()
{
   return level.get_floor_height(player.get_xpos(),player.get_ypos());
}

void ua_underworld::render()
{
   level.render(core->get_texmgr());
}

ua_level &ua_underworld::get_level(unsigned int level)
{
   return this->level;
}
