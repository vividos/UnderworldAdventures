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
#include "underworld.hpp"
/*
#include "image.hpp"
#include "font.hpp"
#include "renderer.hpp"
#include "uwscript.hpp"
#include "debug.hpp"
#include "panel.hpp"
*/

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

/*
//! screen area values
enum ua_screen_area_id
{
   ua_area_screen3d=1,
   ua_area_textscroll,
   ua_area_gargoyle,
   ua_area_powergem,

   ua_area_compass,
   ua_area_move_left,
   ua_area_move_foreward,
   ua_area_move_right,

   ua_area_spell0,
   ua_area_spell1,
   ua_area_spell2,
   ua_area_bookshelf,
   ua_area_flask_vitality,
   ua_area_flask_mana,
   ua_area_stats_chain,

   // inventory stuff
   ua_area_inv_slot0,
   ua_area_inv_slot1,
   ua_area_inv_slot2,
   ua_area_inv_slot3,
   ua_area_inv_slot4,
   ua_area_inv_slot5,
   ua_area_inv_slot6,
   ua_area_inv_slot7,

   ua_area_inv_container,
   ua_area_inv_scroll_up,
   ua_area_inv_scroll_down,

   ua_area_equip_left_hand,
   ua_area_equip_left_shoulder,
   ua_area_equip_left_ring,

   ua_area_equip_right_hand,
   ua_area_equip_right_shoulder,
   ua_area_equip_right_ring,

   ua_area_paperdoll_head,
   ua_area_paperdoll_chest,
   ua_area_paperdoll_hand,
   ua_area_paperdoll_legs,
   ua_area_paperdoll_feet,
};
*/

// classes

//! original ingame screen
class ua_ingame_orig_screen: public ua_screen, public ua_underworld_callback
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

   //! returns underworld object; for controls
   ua_underworld& get_underworld(){ return game.get_underworld(); }

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


/*
   virtual void ui_show_cutscene(unsigned int cutscene);

   //! sets cursor image
   void set_cursor_image(bool is_object, Uint16 image, bool prio=false);
*/
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

   //! fading helper
   ua_fading_helper fading;

   //! current fading state; 0: fadein; 2: fadeout
   unsigned int fade_state;

   //! action to perform after fadeout
   ua_ingame_orig_action fadeout_action;

   //! optional parameter for fadeout action
   unsigned int fadeout_param;


   // controls

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


   // game related

   //! selected game mode
   ua_ingame_game_mode gamemode;
   
   //! tickcount used for time bookkeeping
   unsigned int tickcount;


/*
   //! finds out selection on specific mouse cursor position
   GLuint get_selection(unsigned int xpos, unsigned int ypos);

   //! takes screenshot from current image
   void do_screenshot(bool with_menu, unsigned int xres=0, unsigned int yres=0);

protected:

   //! mouse cursor image
   unsigned int cursor_image;

   //! true when cursor is a priority cursor
   bool prio_cursor;


   // misc. stuff

   // fading in/out

   //! level of npc object to converse with
   unsigned int conv_level;

   //! objpos of npc object to converse with
   unsigned int conv_objpos;


   // images, textures and fonts

   //! all inventory objects
   ua_image_list img_objects;


   //! inventory/stats/runebag panel
   ua_panel panel;


   //! screenshot image in rgba format
   std::vector<Uint32> screenshot_rgba;

   //! screenshot image resolution
   unsigned int screenshot_xres,screenshot_yres;
*/

   // classes that need direct access to this class

   friend class ua_ingame_command_buttons;
};


#endif
//@}
