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
/*! \file player.cpp

   \brief player object

*/

// needed includes
#include "common.hpp"
#include "player.hpp"


// ua_player methods

ua_player::ua_player()
{
}

void ua_player::init()
{
   xpos = ypos = 32.0;
   angle = 0.0;
   move_mode = 0;

   memset(attributes,0,sizeof(attributes));
   memset(stats,0,sizeof(stats));
   memset(skills,0,sizeof(skills));
}

void ua_player::set_movement_mode(unsigned int set,unsigned int del)
{
   move_mode = (move_mode | set) & (~del);
}

void ua_player::set_attr(ua_player_attributes which, unsigned int value)
{
   if (which<ua_attr_max)
      attributes[static_cast<unsigned int>(which)]=value;
}

void ua_player::set_stat(ua_player_stats which, unsigned int value)
{
   if (which<ua_stat_max)
      attributes[static_cast<unsigned int>(which)]=value;
}

void ua_player::set_skill(ua_player_skills which, unsigned int value)
{
   if (which<ua_skill_max)
      attributes[static_cast<unsigned int>(which)]=value;
}

unsigned int ua_player::get_attr(ua_player_attributes which)
{
   if (which>=ua_attr_max) return 0;
   return attributes[static_cast<unsigned int>(which)];
}

unsigned int ua_player::get_stat(ua_player_stats which)
{
   if (which>=ua_stat_max) return 0;
   return stats[static_cast<unsigned int>(which)];
}

unsigned int ua_player::get_skill(ua_player_skills which)
{
   if (which>=ua_skill_max) return 0;
   return skills[static_cast<unsigned int>(which)];
}
