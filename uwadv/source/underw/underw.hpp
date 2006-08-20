/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file underw.hpp

   \brief underw common include file

*/

// include guard
#ifndef uwadv_underw_underw_hpp_
#define uwadv_underw_underw_hpp_

// needed includes
#include "base.hpp"

namespace Base
{
   class Savegame;
}


#if 0

// enums

/*! actions that the user performs */
enum ua_underworld_user_action
{
   //! dummy action
   ua_action_nothing=0,

   //! user looks at object
   ua_action_look_object,

   //! user looks at object in inventory
   ua_action_look_object_inventory,

   //! user looks at a wall in 3d world
   ua_action_look_wall,

   //! user uses object
   ua_action_use_object,

   //! user uses object in inventory
   ua_action_use_object_inventory,

   //! user looks at object in inventory
   ua_action_use_wall,

   //! user gets object from 3d view
   ua_action_get_object,

   //! user talks to object
   ua_action_talk_object,

   //! clicked on active spell; param is the spell that is clicked on
   ua_action_clicked_spells,

   //! user clicked on runeshelf to cast a spell
   ua_action_clicked_runeshelf,

   //! user clicked on compass
   ua_action_clicked_compass,

   //! user clicked on vitality flask
   ua_action_clicked_vitality_flask,

   //! user clicked on mana flask
   ua_action_clicked_mana_flask,

   //! user clicked on gargoyle
   ua_action_clicked_gargoyle,

   //! user clicked on dragons
   ua_action_clicked_dragons,

   //! user tracks creatures
   ua_action_track_creatures,

   //! user tries to sleep, using bedroll or bed
   ua_action_sleep,

   /*! enters combat mode; the function returns false when it's not possible
       to start combat, e.g. when in water. */
   ua_action_combat_enter,

   //! leaves combat mode again
   ua_action_combat_leave,

   //! starts drawing back the weapon (start of power gem heatup)
   ua_action_combat_draw,

   /*! user released a weapon; the parameter describes the "part" of the
       screen the mouse was when releasing the weapon; 0=upper third, 1=middle
       of screen, 2=lower part of screen */
   ua_action_combat_release,

   //! selected a target, previously requested with ua_notify_select_target
   ua_action_target_selected,
};

/*! all notification values */
enum ua_underworld_notification
{
   ua_notify_nothing=0,
   ua_notify_update_powergem,
   ua_notify_update_gargoyle_eyes,
   ua_notify_update_flasks,
   ua_notify_update_show_ingame_animation,
   ua_notify_update_spell_area,
   ua_notify_animate_dragons,
   ua_notify_level_change,
   ua_notify_select_target,
   ua_notify_player_hit,
   ua_notify_player_drowning,
   ua_notify_player_dead,
};


// classes

//! callback interface
class ua_underworld_callback
{
public:
   //! notifies callback class
   virtual void uw_notify(ua_underworld_notification notify,
      unsigned int param=0)=0;

   //! called to print text to textscroll
   virtual void uw_print(const char* text)=0;

   //! starts conversation with object in current level, on list position
   virtual void uw_start_conversation(Uint16 list_pos)=0;

   //! adds object triangles to list
   virtual void uw_get_object_triangles(unsigned int x, unsigned int y,
      const ua_object& obj,
      std::vector<ua_triangle3d_textured>& alltriangles)=0;
};


//! underworld class
class ua_underworld: public ua_physics_model_callback
{
public:

   //! evaluates whole underworld for a given time point
   void eval_underworld(double time);

   //! user performed an action
   void user_action(ua_underworld_user_action action, unsigned int param=0);

   //! returns if enhanced features are turned on
   bool have_enhanced_features(){ return enhanced_features; }

   //! stops or resumes game
   void pause_eval(bool stop=true){ stopped = stop; }

   //! returns the attack power, ranging from 0 to 100
   unsigned int get_attack_power(){ return attack_power; }

   //! registers a callback
   void register_callback(ua_underworld_callback* cb=NULL){ callback = cb; }

   //! returns callback interface
   ua_underworld_callback* get_callback(){ return callback; };

   //! sets scripting interface
   void set_scripting(ua_scripting* script){ scripting = script; }

   //! sets scripting interface
   ua_scripting* get_scripting(){ return scripting; }


   //! changes current level
   void change_level(unsigned int level);


protected:
   // ua_physics_model_callback virtual methods
   virtual void get_surrounding_triangles(unsigned int xpos,
      unsigned int ypos, std::vector<ua_triangle3d_textured>& alltriangles);

protected:
   //! checks if player is near move triggers
   void check_move_trigger();

protected:
   //! indicates if enhanced features are enabled
   bool enhanced_features;

   //! is true when game is stopped
   bool stopped;

   //! indicates if player is attacking
   bool attacking;

   //! attack power, ranging from 0 to 100
   unsigned int attack_power;

   //! last timestamp that evaluation occured
   double last_evaltime;

   //! physics model to use in underworld
   ua_physics_model physics;


   //! list with active triggers
   std::set<Uint16> trigger_active;

   //! pointer to underworld callback class
   ua_underworld_callback* callback;

   //! pointer to scripting class
   ua_scripting* scripting;
};
#endif // 0

#endif
