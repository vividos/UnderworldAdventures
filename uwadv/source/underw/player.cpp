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
/*! \file player.cpp

   \brief player object

*/

// needed includes
#include "common.hpp"
#include "player.hpp"
#include "underworld.hpp"


// constants

//! max speed a player can walk, in tiles / second
const double ua_player_max_walk_speed = 2.4;

//! max speed a player can slide left or right
const double ua_player_max_slide_speed = 1.0;

//! max speed a player can rotate, in degrees / second
const double ua_player_max_rotate_speed = 90;

//! max speed a player can rotate the view, in degrees / second
const double ua_player_max_view_rotate_speed = 60;

//! x size of the player ellipsoid
const double ua_ellipsoid_x = 0.2;
//! y size of the player ellipsoid
const double ua_ellipsoid_y = 0.2;
//! z size of the player ellipsoid
const double ua_ellipsoid_z = 11.9;


// ua_player methods

ua_player::ua_player()
{
   name.assign("GRONKEY");
   ellipsoid = ua_vector3d(ua_ellipsoid_x, ua_ellipsoid_y, ua_ellipsoid_z);
   fall_time = 0.0;
   fall_height_start = 0.0;
}

void ua_player::init(ua_underworld& underw)
{
   xpos = ypos = 32.0;
   height = 0.0;
   rotangle = panangle = 0.0;
   move_mode = 0;

   memset(attributes,0,sizeof(attributes));
   memset(skills,0,sizeof(skills));

   move_factors[ua_move_walk] = 0.0;
   move_factors[ua_move_rotate] = 0.0;
   move_factors[ua_move_lookup] = 0.0;
   move_factors[ua_move_jump] = 0.0;
   move_factors[ua_move_slide] = 0.0;
   move_factors[ua_move_float] = 0.0;

   max_panangle = underw.have_enhanced_features() ? 45.0 : 75.0;
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

void ua_player::set_skill(ua_player_skills which, unsigned int value)
{
   if (which<ua_skill_max)
      skills[static_cast<unsigned int>(which)]=value;
}

void ua_player::set_name(std::string name)
{
    this->name = name;
}

unsigned int ua_player::get_attr(ua_player_attributes which) const
{
   if (which>=ua_attr_max) return 0;
   return attributes[static_cast<unsigned int>(which)];
}

unsigned int ua_player::get_skill(ua_player_skills which) const
{
   if (which>=ua_skill_max) return 0;
   return skills[static_cast<unsigned int>(which)];
}

void ua_player::load_game(ua_savegame& sg)
{
   sg.begin_section("player");

   unsigned int n=0;

   // read in name
   sg.read_string(name);

   // read position
   xpos = sg.read32()/256.0;
   ypos = sg.read32()/256.0;
   rotangle = sg.read32()/256.0;
   panangle = sg.read32()/256.0;
   height = sg.read32()/256.0;

   // read attributes and skills
   for(n=0; n<SDL_TABLESIZE(attributes); n++)
      attributes[n] = sg.read16();

   for(n=0; n<SDL_TABLESIZE(skills); n++)
      skills[n] = sg.read16();

   // load runes
   {
      // load runebag
      std::bitset<24>& runebag = runes.get_runebag();
      runebag.reset();

      Uint32 bagbits = sg.read32();
      for(unsigned int i=0; i<24; i++)
         if ((bagbits & (1L << i)) != 0)
            runebag.set(i);

      // load runeshelf
      runes.reset_runeshelf();
      unsigned int count = sg.read8();
      for(Uint8 n=0; n<count; n++)
         runes.add_runeshelf_rune(sg.read8());
   }

   sg.end_section();
}

void ua_player::save_game(ua_savegame& sg)
{
   sg.begin_section("player");

   unsigned int n=0;

   // write name
   sg.write_string(name.c_str());

   // write position
   sg.write32(Uint32(xpos*256.0));
   sg.write32(Uint32(ypos*256.0));
   sg.write32(Uint32(rotangle*256.0));
   sg.write32(Uint32(panangle*256.0));
   sg.write32(Uint32(height*256.0));

   // write attributes and skills
   for(n=0; n<SDL_TABLESIZE(attributes); n++)
      sg.write16(attributes[n]);

   for(n=0; n<SDL_TABLESIZE(skills); n++)
      sg.write16(skills[n]);

   // save runes
   {
      // store runebag
      // note: we could use std::bitset::to_ulong(), but it probably isn't
      // portable enough
      std::bitset<24>& runebag = runes.get_runebag();
      Uint32 bagbits = 0;
      for(unsigned int i=0; i<24; i++)
         if (runebag.test(i))
            bagbits |= 1L << i;
      sg.write32(bagbits);

      // store runeshelf
      Uint8 count = runes.get_runeshelf_count() % 3;
      sg.write8(count);
      for(unsigned int n=0; n<count; n++)
         sg.write8(runes.get_runeshelf_rune(n));
   }

   sg.end_section();
}

void ua_player::fill_savegame_infos(ua_savegame_info& info)
{
   info.name = name;

   info.gender = get_attr(ua_attr_gender);
   info.appearance = get_attr(ua_attr_appearance);
   info.profession = get_attr(ua_attr_profession);
   info.maplevel = get_attr(ua_attr_maplevel);

   info.strength = get_attr(ua_attr_strength);
   info.dexterity = get_attr(ua_attr_dexterity);
   info.intelligence = get_attr(ua_attr_intelligence);
   info.vitality = get_attr(ua_attr_vitality);
}

void ua_player::rotate_move(double time_elapsed)
{
   unsigned int mode = get_movement_mode();

   // player rotation
   if (mode & ua_move_rotate)
   {
      double angle = ua_player_max_rotate_speed * time_elapsed *
         get_movement_factor(ua_move_rotate);
      angle += get_angle_rot();

      // keep angle in range [0; 360]
      while(angle>360.0) angle -= 360.0;
      while(angle<0.0) angle += 360.0;

      set_angle_rot(angle);
   }

   // view up/down
   if (mode & ua_move_lookup)
   {
      double viewangle = get_angle_pan();
      viewangle -= ua_player_max_view_rotate_speed * time_elapsed *
         get_movement_factor(ua_move_lookup);

      // restrict up-down view angle
      if (viewangle < -max_panangle) viewangle = -max_panangle;
      if (viewangle > max_panangle) viewangle = max_panangle;

      set_angle_pan(viewangle);
   }
}

void ua_player::set_new_elapsed_time(double time_elapsed)
{
   fall_time += time_elapsed;
}

ua_vector3d ua_player::get_pos()
{
   return ua_vector3d(xpos,ypos,height+ellipsoid.z);
}

void ua_player::set_pos(ua_vector3d& pos)
{
   xpos = pos.x;
   ypos = pos.y;
   height = pos.z-ellipsoid.z;
}

bool my_movement;

ua_vector3d ua_player::get_dir()
{
   double time_elapsed = 0.05;

   ua_vector3d dir;

   unsigned int mode = get_movement_mode();

   if ((mode & ua_move_walk) || (mode & ua_move_slide))
   {
      double speed = ua_player_max_walk_speed*time_elapsed;
      double angle = get_angle_rot();

my_movement = true;

      // adjust angle for sliding
      if (mode & ua_move_slide)
      {
         angle -= 90.0;
         speed *= get_movement_factor(ua_move_slide);
         speed *= ua_player_max_slide_speed/ua_player_max_walk_speed;
      }
      else
         speed *= get_movement_factor(ua_move_walk);

      // construct direction vector
      dir.x = 1.0;
      dir.rotate_z(angle);
      dir *= speed;
   }
   return dir;
}

void ua_player::reset_gravity()
{
   fall_time = 0.0;
   fall_height_start = height;
}

ua_vector3d ua_player::get_gravity_force()
{
   double gravity = 15.0*fall_time*fall_time;

   // limit gravity for falling
   if (gravity > 10.0)
      gravity = 10.0;

   return ua_vector3d(0.0, 0.0, -gravity);
}

/*! \todo hurt player when falling from too high */
void ua_player::hit_floor()
{
   double fall_height = fall_height_start - height;
}
