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
#include "savegame.hpp"


// enums

//! player movement enum
enum ua_player_movement_mode
{
   ua_move_walk_forward = 1,
   ua_move_rotate_left  = 2,
   ua_move_rotate_right = 4,
};


//! player attribute enum
/*! player attributes are values that characterize the player, and
    may be used for combat/spell/etc. calculations */
typedef enum
{
   ua_attr_gender=0,   // 0 means male
   ua_attr_handedness, // 0 means left-handedness
   ua_attr_appearance, // values from 0..4
   ua_attr_profession, // values from 0..7

   ua_attr_maplevel,

   ua_attr_strength,
   ua_attr_dexterity,
   ua_attr_intelligence,
   ua_attr_attack,
   ua_attr_defense,

   ua_attr_life,
   ua_attr_max_life,
   ua_attr_mana,
   ua_attr_max_mana,

   ua_attr_weariness,
   ua_attr_hungriness,
   ua_attr_poisoned,

   ua_attr_talks,
   ua_attr_kills,
   ua_attr_level,
   ua_attr_exp_points,

   ua_attr_max

} ua_player_attributes;

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

   ua_skill_max
} ua_player_skills;


// classes

class ua_player
{
public:
   //! ctor
   ua_player();

   //! initializes player object
   void init();

   // set functions

   //! sets player position
   void set_pos(double x, double y){ xpos=x; ypos=y; }

   //! sets player view angle
   void set_angle(double theangle){ angle=theangle; }

   //! sets and delete movement mode values
   void set_movement_mode(unsigned int set,unsigned int del=0);


   //! sets player attribute value
   void set_attr(ua_player_attributes which, unsigned int value);

   //! sets player skill value
   void set_skill(ua_player_skills which, unsigned int value);

   // get functions

   //! returns x position
   double get_xpos(){ return xpos; }

   //! returns y position
   double get_ypos(){ return ypos; }

   //! returns view angle
   double get_angle(){ return angle; }

   //! returns movement mode
   unsigned int get_movement_mode(){ return move_mode; }


   //! returns player attribute value
   unsigned int get_attr(ua_player_attributes which);

   //! returns player skill value
   unsigned int get_skill(ua_player_skills which);


   // loading/saving/importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

protected:
   //! the name of the player
   std::string name;

   // positional values
   double xpos,ypos,angle;

   //! current movement mode
   unsigned int move_mode;

   //! array with all player attributes
   unsigned int attributes[ua_attr_max];

   //! array with all player skills
   unsigned int skills[ua_skill_max];
};

#endif
