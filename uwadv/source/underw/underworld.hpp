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
/*! \file underworld.hpp

   \brief underworld class

*/
/*! \defgroup underworld Underworld Objects Documentation

   underworld objects documentation yet to come ...

*/
//@{

// include guard
#ifndef uwadv_underworld_hpp_
#define uwadv_underworld_hpp_

// needed includes
#include <vector>
#include <set>
#include "files.hpp"
#include "level.hpp"
#include "player.hpp"
#include "inventory.hpp"
#include "conv/codevm.hpp"
#include "level.hpp"
#include "physics.hpp"
#include "gamestrings.hpp"
#include "savegame.hpp"
#include "properties.hpp"
#include "mapnotes.hpp"


// forward declaration
class ua_game_core_interface;
class ua_scripting;


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

   //! starts drawing the weapon
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
   ua_notify_update_gem,
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
   virtual void uw_start_conversation(unsigned int list_pos)=0;
};


//! underworld class
class ua_underworld
{
public:
   //! ctor
   ua_underworld();

   //! initializes underworld
   void init(ua_settings& settings, ua_files_manager& filesmgr);

   //! clean up the underworld
   void done();

   //! evaluates whole underworld for a given time point
   void eval_underworld(double time);

   //! user performed an action
   void user_action(ua_underworld_user_action action, unsigned int param=0);

   //! returns if enhanced features are turned on
   bool have_enhanced_features(){ return enhanced_features; }

   //! stops or resumes game
   void pause_eval(bool stop=true){ stopped = stop; }

   //! registers a callback
   void register_callback(ua_underworld_callback* cb=NULL){ callback = cb; }

   //! returns callback interface
   ua_underworld_callback* get_callback(){ return callback; };

   //! sets scripting interface
   void set_scripting(ua_scripting* script){ scripting = script; }

   // access to underworld components

   //! returns player
   ua_player& get_player(){ return player; }
   const ua_player& get_player() const { return player; }

   //! returns player's inventory
   ua_inventory &get_inventory(){ return inventory; }

   //! returns physics model
   ua_physics_model &get_physics(){ return physics; }

   //! returns conversation globals
   ua_conv_globals &get_conv_globals(){ return conv_globals; }

   //! returns a levelmap
   ua_level &get_level(unsigned int level){ return levels[level]; }

   //! returns current level
   ua_level &get_current_level();
   const ua_level &get_current_level() const;

   //! returns game strings object
   ua_gamestrings& get_strings(){ return gstr; }

   //! changes current level
   void change_level(unsigned int level);

   //! returns number of levels available
   unsigned int get_num_levels(){ return levels.size(); }

   //! returns quest flag vector
   std::vector<Uint16>& get_questflags(){ return questflags; }

   //! returns object property object
   ua_object_properties& get_obj_properties(){ return properties; }

   //! returns map notes object
   ua_map_notes& get_map_notes(){ return mapnotes; }

   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame& sg);

   //! saves to a savegame
   void save_game(ua_savegame& sg);

   //! imports a saved game
   void import_savegame(ua_settings& settings,const char* folder,bool initial);

protected:
   //! checks if player is near move triggers
   void check_move_trigger();

protected:
   //! indicates if enhanced features are enabled
   bool enhanced_features;

   //! is true when game is stopped
   bool stopped;

   //! the player object
   ua_player player;

   //! player's inventory
   ua_inventory inventory;

   //! conversation globals of the current underworld
   ua_conv_globals conv_globals;

   //! physics model to use in underworld
   ua_physics_model physics;

   //! all underworld levels
   std::vector<ua_level> levels;

   //! list of all quest flags
   std::vector<Uint16> questflags;

   //! object property class
   ua_object_properties properties;

   //! map notes
   ua_map_notes mapnotes;

   //! hey, it's all the game strings!
   ua_gamestrings gstr;

   //! list with active triggers
   std::set<Uint16> trigger_active;

   //! pointer to underworld callback class
   ua_underworld_callback* callback;

   //! pointer to scripting class
   ua_scripting* scripting;
};


#endif
//@}
