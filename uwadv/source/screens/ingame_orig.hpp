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
/*! \file ingame_orig.hpp

   \brief original, old style game screen user interface

*/
//! \ingroup screens

//@{

// include guard
#ifndef uwadv_ingame_orig_hpp_
#define uwadv_ingame_orig_hpp_

// needed includes
#include "screen.hpp"
#include "keymap.hpp"
#include "mousecursor.hpp"
#include "fading.hpp"
#include "imgquad.hpp"
#include "textscroll.hpp"
#include "ingame_ctrls.hpp"
#include "panel.hpp"
#include "underworld.hpp"


// enums

//! game modes
enum ua_ingame_game_mode
{
   ua_mode_default=0, //!< nothing selected
   ua_mode_options,   //!< in the options menu
   ua_mode_talk,      //!< talk mode
   ua_mode_get,       //!< get mode
   ua_mode_look,      //!< look mode
   ua_mode_fight,     //!< fight mode
   ua_mode_use        //!< use mode
};

//! actions to perform by screen
enum ua_ingame_orig_action
{
   ua_action_none=0,    //!< no action to perform
   ua_action_exit,      //!< exits the game
   ua_action_load_game, //!< shows savegame loading screen
   ua_action_save_game, //!< shows savegame loading screen
   ua_action_quickload, //!< performs quickloading
   ua_action_quicksave, //!< performs quicksaving
   ua_action_conversation, //!< starts conversation
   ua_action_cutscene,  //!< shows cutscene
};

//! player move state
enum ua_ingame_move_state
{
   ua_move_turn_left=0,
   ua_move_turn_right,
   ua_move_walk_forward,
   ua_move_run_forward,
   ua_move_walk_backwards
};


// classes

//! original ingame screen
class ua_ingame_orig_screen: public ua_screen, public ua_underworld_callback,
   public ua_panel_parent_interface
{
public:
   //! ctor
   ua_ingame_orig_screen(ua_game_interface& game);
   //! dtor
   virtual ~ua_ingame_orig_screen(){}

   // virtual methods from ua_screen
   virtual void init();
   virtual void destroy();
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void key_event(bool key_down, ua_key_value key);
   virtual void tick();

   //! schedules action and starts fadeout if specified
   void schedule_action(ua_ingame_orig_action action, bool fadeout_before);

   //! actually performs scheduled action
   void do_action(ua_ingame_orig_action action);

   //! returns game mode
   ua_ingame_game_mode get_gamemode(){ return gamemode; }

   //! sets game mode
   void set_gamemode(ua_ingame_game_mode my_gamemode){ gamemode = my_gamemode; }

   //! returns move state
   bool get_move_state(ua_ingame_move_state state);

   // virtual methods from ua_panel_parent

   //! sets cursor image
   virtual void set_cursor(int index, bool priority=false);

   //! returns game interface; for controls
   virtual ua_game_interface& get_game_interface(){ return game; }

protected:
   //! suspends game resources while showing another screen
   void suspend();

   //! resumes gameplay and restores resources
   void resume();

   // virtual methods from ua_underworld_callback
   //! notifies callback class
   virtual void uw_notify(ua_underworld_notification notify,
      unsigned int param=0);

   //! called to print text to textscroll
   virtual void uw_print(const char* text);

   //! starts conversation with object in current level, on list position
   virtual void uw_start_conversation(unsigned int list_pos);

   //! takes a screenshot, for savegame preview or to save it to disk
   void do_screenshot(bool for_savegame=false, unsigned int xres=0,
      unsigned int yres=0);

protected:
   // constants

   //! time to fade in/out
   static const double fade_time;


   // screen related

   //! keymap
   ua_keymap keymap;


   //! background image
   ua_image_quad img_background;

   //! mouse cursor
   ua_mousecursor mousecursor;

   //! indicates if there is a movement key pressed
   bool move_turn_left, move_turn_right,
      move_walk_forward, move_run_forward, move_walk_backwards;

   //! indicates if a priority cursor image is in effect
   bool priority_cursor;

   //! all inventory objects
   std::vector<ua_image> img_objects;

   //! fading helper
   ua_fading_helper fading;

   //! current fading state; 0: fadein; 2: fadeout
   unsigned int fade_state;

   //! action to perform after fadeout
   ua_ingame_orig_action fadeout_action;

   //! optional parameter for fadeout action
   unsigned int fadeout_param;


   // controls

   //! panel for inventory, stats or runebag
   ua_panel panel;

   //! compass window
   ua_ingame_compass compass;

   //! message text scroll
   ua_textscroll textscroll;

   //! runeshelf
   ua_ingame_runeshelf runeshelf;

   //! spell area
   ua_ingame_spell_area spellarea;

   //! vitality flask
   ua_ingame_flask vitality_flask;

   //! mana flask
   ua_ingame_flask mana_flask;

   //! gargoyle eyes
   ua_ingame_gargoyle_eyes gargoyle_eyes;

   //! dragon on the left side
   ua_ingame_dragon dragon_left;

   //! dragon on the right side
   ua_ingame_dragon dragon_right;

   //! command buttons
   ua_ingame_command_buttons command_buttons;

   //! 3d view area (invisible)
   ua_ingame_3dview view3d;

   //! powergem
   ua_ingame_powergem powergem;

   //! move arrows
   ua_ingame_move_arrows move_arrows;


   // game related

   //! selected game mode
   ua_ingame_game_mode gamemode;
   
   //! tickcount used for time bookkeeping
   unsigned int tickcount;

   // classes that need direct access to this class
   friend class ua_ingame_command_buttons;
};


#endif
//@}
