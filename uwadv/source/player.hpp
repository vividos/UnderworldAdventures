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
/*! \file player.hpp

   \brief player character representation

*/

// include guard
#ifndef __uwadv_player_hpp_
#define __uwadv_player_hpp_

// needed includes


// enums

//! player stats enum
typedef enum
{
   ua_stat_strength=0,
   ua_stat_dexterity,
   ua_stat_intelligence,
   ua_stat_attack,
   ua_stat_defense,

   ua_stat_life,
   ua_stat_max_life,
   ua_stat_mana,
   ua_stat_max_mana,

   ua_stat_max=ua_stat_max_mana

} ua_player_stats;

//! player skills enum
typedef enum
{
   ua_skill_unarmed=0,
   ua_skill_sword,
   ua_skill_axe,
   ua_skill_mace,
   ua_skill_missile,
   ua_skill_lore,
   ua_skill_casting,
   ua_skill_traps,
   ua_skill_search,
   ua_skill_track,
   ua_skill_sneak,
   ua_skill_repair,
   ua_skill_charm,
   ua_skill_picklock,
   ua_skill_acrobat,
   ua_skill_appraise,
   ua_skill_swimming,
   ua_skill_max = ua_skill_swimming
} ua_player_skills;


// classes

class ua_player
{
public:
   //! ctor
   ua_player(){}

   //! initializes player object
   void init();

   // set functions

   //! sets player position
   void set_pos(double x, double y){ xpos=x; ypos=y; }

   //! sets player view angle
   void set_angle(double theangle){ angle=theangle; }

   //! sets gender of player
   void set_gender(bool is_male){ gender_male=is_male; }

   //! sets appearance (ranges from 0 to 4)
   void set_appearance(unsigned int app){ appearance=app; }


   // get functions

   //! returns x position
   double get_xpos(){ return xpos; }

   //! returns y position
   double get_ypos(){ return ypos; }

   //! returns view angle
   double get_angle(){ return angle; }

   //! returns player stat value
   unsigned int get_stat(ua_player_stats which);

   //! returns player skill value
   unsigned int get_skill(ua_player_skills which);

   //! returns true if the player's gender is male
   bool is_gender_male(){ return gender_male; }

   //! returns true if the player is left handed
   bool is_left_handed(){ return left_handedness; }

   //! returns the appearance number
   unsigned int get_appearance(){ return appearance; }

protected:
   //! the name of the player
   std::string name;

   // positional
   double xpos,ypos,angle;

   //! true when the player is male
   bool gender_male;

   //! true when the player is left handed
   bool left_handedness;

   //! player appearance graphic (0..4)
   unsigned int appearance;

   //! array with all player stats
   unsigned int stats[ua_stat_max];

   //! array with all player skills
   unsigned int skills[ua_skill_max];
};

#endif
