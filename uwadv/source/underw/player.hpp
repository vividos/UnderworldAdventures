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
/*! \file player.hpp

   \brief player character representation

*/
//! \ingroup underworld

//@{

// include guard
#ifndef uwadv_player_hpp_
#define uwadv_player_hpp_

// needed includes
#include "savegame.hpp"
#include "uamath.hpp"
#include "physicsbody.hpp"
#include "runes.hpp"


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
    may be used for combat/spell/etc. calculations
    \todo add more needed skills
*/
typedef enum
{
   // note: scripts depend on numerical values, they must be in-sync with
   // those in uwinterface.lua
   ua_attr_gender=0,     //!< 0 means male
   ua_attr_handedness=1, //!< 0 means left-handedness
   ua_attr_appearance=2, //!< values from 0..4
   ua_attr_profession=3, //!< values from 0..7, fighter, mage, bard, ...

   ua_attr_maplevel=4,   //!< map level the player currently is

   ua_attr_strength=5,
   ua_attr_dexterity=6,
   ua_attr_intelligence=7,

   ua_attr_vitality=8,
   ua_attr_max_vitality=9,
   ua_attr_mana=10,
   ua_attr_max_mana=11,

   ua_attr_weariness=12,
   ua_attr_hungriness=13,
   ua_attr_poisoned=14,    //!< 1 when poisoned
   ua_attr_mentalstate=15, //!< drunk, tripping, etc. 0 means normal
   ua_attr_nightvision=16,

   ua_attr_talks=17,       //!< number of conversations
   ua_attr_kills=18,       //!< number of successful kills
   ua_attr_exp_level=19,   //!< experience level
   ua_attr_exp_points=20,  //!< number of experience points

   ua_attr_difficulty=21,  //!< 0=easy, 1=normal

   // todo: game time, etc.

   ua_attr_max

} ua_player_attributes;


//! player skills enum
typedef enum
{
   // note: scripts depend on numerical values, they must be in-sync with uwinterface.lua
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
class ua_player: public ua_physics_body
{
public:
   //! ctor
   ua_player();

   //! initializes player object
   void init(class ua_underworld& underw);


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
   double get_xpos() const;

   //! returns y position
   double get_ypos() const;

   //! returns view rotation angle
   double get_angle_rot() const;

   //! returns view panning angle
   double get_angle_pan() const;

   //! returns player height
   double get_height() const;

   //! returns player name
   const std::string& get_name() const;


   //! returns movement mode
   unsigned int get_movement_mode() const;

   //! returns movement factor for given movement mode
   double get_movement_factor(ua_player_movement_mode mode);

   //! returns runes object
   ua_runes& get_runes();


   //! returns player attribute value
   unsigned int get_attr(ua_player_attributes which) const;

   //! returns player skill value
   unsigned int get_skill(ua_player_skills which) const;


   //! does rotation moves
   void rotate_move(double time_elapsed);


   // loading/saving

   //! loads a savegame
   void load_game(ua_savegame& sg);

   //! saves to a savegame
   void save_game(ua_savegame& sg);

   //! fills details into savegame info struct
   void fill_savegame_infos(ua_savegame_info& info);

   // virtual methods from ua_physics_object
   virtual void set_new_elapsed_time(double time_elapsed);
   virtual ua_vector3d get_pos();
   virtual void set_pos(ua_vector3d& pos);
   virtual ua_vector3d get_dir();

   virtual void reset_gravity();
   virtual ua_vector3d get_gravity_force();
   virtual void hit_floor();

protected:
   //! the name of the player
   std::string name;

   //! positional values
   double xpos, ypos, rotangle, panangle, height;

   //! maximum pan angle
   double max_panangle;

   //! current movement mode
   unsigned int move_mode;

   //! array with all player attributes
   unsigned int attributes[ua_attr_max];

   //! array with all player skills
   unsigned int skills[ua_skill_max];

   //! movement factors map
   std::map<ua_player_movement_mode,double> move_factors;

   //! runes object
   ua_runes runes;

   //! current fall time
   double fall_time;

   //! player height at fall start
   double fall_height_start;
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

inline double ua_player::get_xpos() const
{
   return xpos;
}

inline double ua_player::get_ypos() const
{
   return ypos;
}

inline double ua_player::get_angle_rot() const
{
   return rotangle;
}

inline double ua_player::get_angle_pan() const
{
   return panangle;
}

inline double ua_player::get_height() const
{
   return height;
}

inline const std::string& ua_player::get_name() const
{
   return name;
}

inline unsigned int ua_player::get_movement_mode() const
{
   return move_mode;
}

inline ua_runes& ua_player::get_runes()
{
   return runes;
}


#endif
//@}
