/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file uwaccess.cpp

   \brief underworld access API implementation

*/

// needed includes
#include "common.hpp"
#include "debug.hpp"
#include "uwaccess.hpp"


// global api methods

void ua_access_player_value(ua_debug_interface* inter,
   bool set, unsigned int index, double& value)
{
   ua_player& pl = inter->get_underworld()->get_player();
   switch(index)
   {
   case 0: // xpos
      if (set) pl.set_pos(value,pl.get_ypos());
      else value = pl.get_xpos();
      break;

   case 1: // ypos
      if (set) pl.set_pos(pl.get_xpos(),value);
      else value = pl.get_ypos();
      break;

   case 2: // height
      if (set) pl.set_height(value);
      else value = pl.get_height();
      break;

   case 3: // angle
      if (set) pl.set_angle_rot(value);
      else value = pl.get_angle_rot();
      break;
   }
}

void ua_access_player_attribute(ua_debug_interface* inter,
   bool set, unsigned int index, unsigned int& value)
{
   ua_player& pl = inter->get_underworld()->get_player();
   if (set) pl.set_attr(ua_player_attributes(index),value);
   else value = pl.get_attr(ua_player_attributes(index));
}

void ua_access_player_skill(ua_debug_interface* inter,
   bool set, unsigned int index, unsigned int& value)
{
   ua_player& pl = inter->get_underworld()->get_player();
   if (set) pl.set_skill(ua_player_skills(index),value);
   else value = pl.get_skill(ua_player_skills(index));
}

// struct methods

void ua_uw_access_api::init()
{
   player_value = ua_access_player_value;
   player_attribute = ua_access_player_attribute;
   player_skill = ua_access_player_skill;
}
