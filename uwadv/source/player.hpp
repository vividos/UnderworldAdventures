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
#include "uamath.hpp"
#include "physicsobject.hpp"

// enums

//! player movement enum
enum ua_player_movement_mode
{
   ua_move_walk = 1,    // walks forward (or backwards, when factor is negative)
   ua_move_rotate  = 2, // rotates player left (or right)
   ua_move_lookup = 4,  // moves player look angle up (or down)
   ua_move_jump = 8,    // jumps forward (or factor 0.0 for standing jump)
   ua_move_slide = 16,  // slides right (or left)
   ua_move_float = 32,  // floats player up (or down)
};


//! player attribute enum
/*! player attributes are values that characterize the player, and
    may be used for combat/spell/etc. calculations */
typedef enum
{
   // note: scripts depend on numeral values, they must be in-sync with uwinterface.lua
   ua_attr_gender=0,     // 0 means male
   ua_attr_handedness=1, // 0 means left-handedness
   ua_attr_appearance=2, // values from 0..4
   ua_attr_profession=3, // values from 0..7

   ua_attr_maplevel=4,

   ua_attr_strength=5,
   ua_attr_dexterity=6,
   ua_attr_intelligence=7,

   ua_attr_life=8,
   ua_attr_max_life=9,
   ua_attr_mana=10,
   ua_attr_max_mana=11,

   ua_attr_weariness=12,
   ua_attr_hungriness=13,
   ua_attr_poisoned=14,
   ua_attr_mentalstate=15, // drunk, tripping, etc. 0 means normal
   ua_attr_nightvision=16,

   ua_attr_talks=17,
   ua_attr_kills=18,
   ua_attr_level=19,
   ua_attr_exp_points=20,

   ua_attr_difficulty=21, // 0=easy, 1=normal

   ua_attr_max

} ua_player_attributes;

//! player skills enum
typedef enum
{
   // note: scripts depend on numeral values, they must be in-sync with uwinterface.lua
   ua_skill_attack=0,
   ua_skill_defense=1,
   ua_skill_unarmed=2,
   ua_skill_sword=3,
   ua_skill_axe=4,
   ua_skill_mace=5,
   ua_skill_missile=6,
   ua_skill_mana=7,
   ua_skill_lore=8,
   ua_skill_casting=9,
   ua_skill_traps=10,
   ua_skill_search=11,
   ua_skill_track=12,
   ua_skill_sneak=13,
   ua_skill_repair=14,
   ua_skill_charm=15,
   ua_skill_picklock=16,
   ua_skill_acrobat=17,
   ua_skill_appraise=18,
   ua_skill_swimming=19,

   ua_skill_max
} ua_player_skills;


// classes

//! player class
class ua_player : public ua_physics_object
{
public:
   //! ctor
   ua_player();

   //! initializes player object
   void init();


   // set functions

   //! sets player position
   void set_pos(double x, double y);

   //! sets player view rotation angle
   void set_angle_rot(double theangle);

   //! sets player view panning angle
   void set_angle_pan(double theangle);

   //! sets player height
   void set_height(double theheight);


   //! sets and delete movement mode values
   void set_movement_mode(unsigned int set,unsigned int del=0);

   //! sets movement factor for a given movement type; range is [-1.0; 1.0]
   void set_movement_factor(ua_player_movement_mode mode, double factor);


   //! sets player attribute value
   void set_attr(ua_player_attributes which, unsigned int value);

   //! sets player skill value
   void set_skill(ua_player_skills which, unsigned int value);

   //! sets player name
   void set_name(std::string name);


   // get functions

   //! returns x position
   double get_xpos();

   //! returns y position
   double get_ypos();

   //! returns view rotation angle
   double get_angle_rot();

   //! returns view panning angle
   double get_angle_pan();

   //! returns player height
   double get_height();

   //! returns player name
   std::string& get_name();


   //! returns movement mode
   unsigned int get_movement_mode();

   //! returns movement factor for given movement mode
   double get_movement_factor(ua_player_movement_mode mode);


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
   double xpos, ypos, rotangle, panangle, height;

   //! current movement mode
   unsigned int move_mode;

   //! array with all player attributes
   unsigned int attributes[ua_attr_max];

   //! array with all player skills
   unsigned int skills[ua_skill_max];

   //! movement factors map
   std::map<ua_player_movement_mode,double> move_factors;
};


// inline methods

inline void ua_player::set_pos(double x, double y)
{
   xpos=x; ypos=y;
}

inline void ua_player::set_angle_rot(double theangle)
{
   rotangle=theangle;
}

inline void ua_player::set_angle_pan(double theangle)
{
   panangle=theangle;
}

inline void ua_player::set_height(double theheight)
{
   height=theheight;
}

inline void ua_player::set_movement_factor(ua_player_movement_mode mode, double factor)
{
   move_factors[mode] = factor;
}

inline double ua_player::get_movement_factor(ua_player_movement_mode mode)
{
   return move_factors[mode];
}

inline double ua_player::get_xpos()
{
   return xpos;
}

inline double ua_player::get_ypos()
{
   return ypos;
}

inline double ua_player::get_angle_rot()
{
   return rotangle;
}

inline double ua_player::get_angle_pan()
{
   return panangle;
}

inline double ua_player::get_height()
{
   return height;
}

inline std::string& ua_player::get_name()
{
   return name;
}

inline unsigned int ua_player::get_movement_mode()
{
   return move_mode;
}


#endif
