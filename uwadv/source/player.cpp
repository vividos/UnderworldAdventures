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

void ua_player::init()
{
   xpos = ypos = 32.0;
   angle = 0.0;
   gender_male = false;
   left_handedness = true;
   appearance = 0;

   memset(stats,0,sizeof(stats));
   memset(skills,0,sizeof(skills));
}

unsigned int ua_player::get_stat(ua_player_stats which)
{
   if (unsigned(which)>=ua_stat_max)
      return 0;
   return stats[unsigned(which)];
}

unsigned int ua_player::get_skill(ua_player_skills which)
{
   if (unsigned(which)>=ua_skill_max)
      return 0;
   return skills[unsigned(which)];
}
