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
/*! \file ingame_ctrls.hpp

   \brief ingame screen controls

*/
//! \ingroup screens

//@{

// include guard
#ifndef uwadv_ingame_ctrls_hpp_
#define uwadv_ingame_ctrls_hpp_

// needed includes
#include "imgquad.hpp"
#include "keymap.hpp"


// forward references
class ua_ingame_orig_screen;


// classes

//! ingame screen control base class
class ua_ingame_orig_ctrl: public ua_image_quad
{
public:
   //! sets parent window
   void set_parent(ua_ingame_orig_screen* orig){ parent = orig; }

protected:
   //! parent screen
   ua_ingame_orig_screen* parent;
};


//! compass control
class ua_ingame_compass: public ua_ingame_orig_ctrl
{
public:
   //! initializes compass
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! draws compass
   virtual void draw();

   // virtual methods from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! current compass image
   unsigned int compass_curimg;

   //! compass images
   std::vector<ua_image> img_compass;
};


//! runeshelf control
class ua_ingame_runeshelf: public ua_ingame_orig_ctrl
{
public:
   //! initializes runeshelf
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! updates runeshelf
   void update_runeshelf();

   // virtual methods from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! all runestones
   std::vector<ua_image> img_runestones;
};


//! active spell area control
class ua_ingame_spell_area: public ua_ingame_orig_ctrl
{
public:
   //! dtor
   virtual ~ua_ingame_spell_area(){}

   //! initializes active spells area
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! updates spell area
   void update_spell_area();

   // virtual methods from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! all runestones
   std::vector<ua_image> img_spells;
};


//! vitality / mana flask
/*! \todo implement flask fluid bubbling (remaining images in flasks.gr)
*/
class ua_ingame_flask: public ua_ingame_orig_ctrl
{
public:
   //! ctor
   ua_ingame_flask(bool is_vitality_flask):vitality_flask(is_vitality_flask){}

   //! initializes flask
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! draws compass
   virtual void draw();

   //! updates flask image
   void update_flask();

   // virtual methods from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! indicates if showing a vitality or mana flask
   bool vitality_flask;

   //! indicates if player is poisoned (only when vitality flask)
   bool is_poisoned;

   //! indicates last flask image
   unsigned int last_image;

   //! flask images
   std::vector<ua_image> img_flask;
};


//! gargoyle eyes
class ua_ingame_gargoyle_eyes: public ua_ingame_orig_ctrl
{
public:
   //! initializes flask
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! updates eyes image
   void update_eyes();

   // virtual methods from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! eyes images
   std::vector<ua_image> img_eyes;
};


//! dragons
/*! images from dragons.gr:
    image 0     / 18: lower part, left/right
    image 1     / 19: idle looking head
    image 2-5   / 20-23: scrolling dragon feet
    image 6-9   / 24-27: scrolling dragon head
    image 10-13 / 28-31: dragon looking away
    image 14-17 / 32-35: dragon tail movement
*/

//! left/right dragon
/*! \todo implement animation */
class ua_ingame_dragon: public ua_ingame_orig_ctrl
{
public:
   //! ctor
   ua_ingame_dragon(bool dragon_left);

   //! initializes dragon
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! updates the dragon's image
   void update_dragon();

   // virtual methods from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! specifies if the left or right dragon has to be drawn
   bool dragon_left;

   //! compass images
   std::vector<ua_image> img_dragon;
};


//! nonvisible 3d view window area
class ua_ingame_3dview: public ua_ingame_orig_ctrl
{
public:
   //! initializes 3d view window
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! returns if 3d view is in mouse move mode
   bool get_mouse_move(){ return mouse_move; }

   // virtual methods from ua_window
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! is true when the mouse cursor is in 3d view
   bool in_view3d;

   //! indicates that mouse movement is currently on
   bool mouse_move;
};


//! power gem
class ua_ingame_powergem: public ua_ingame_orig_ctrl
{
public:
   //! initializes powergem
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   //! sets attack mode, e.g. for keyboard combat
   void set_attack_mode(bool attack){ attack_mode = attack; }

   //! updates gem image
   void update_gem();

   // virtual methods from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);
   virtual void tick();

protected:
   //! indicates if in attack mode, i.e. with drawn weapon
   bool attack_mode;

   //! frame used when having maximum power
   unsigned int maxpower_frame;

   //! powergame images
   std::vector<ua_image> img_powergem;
};


//! move arrows (below the compass)
class ua_ingame_move_arrows: public ua_ingame_orig_ctrl
{
public:
   //! initializes move arrows window
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   // virtual methods from ua_window
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! last (virtually) pressed key
   ua_key_value selected_key;
};


//! menu currently shown by ua_ingame_command_buttons
enum ua_ingame_command_menu
{
   ua_cmd_menu_actions=0,  //!< game actions menu (special)
   ua_cmd_menu_options=1,  //!< options menu
   ua_cmd_menu_quit=2,     //!< quit menu
   ua_cmd_menu_music=3,    //!< music menu
   ua_cmd_menu_sound=4,    //!< sound menu
   ua_cmd_menu_details=5,  //!< details menu
   ua_cmd_menu_save=6,     //!< save menu; not used in uwadv
   ua_cmd_menu_restore=7,  //!< restore menu; not used in uwadv
   ua_cmd_menu_max
};

//! command buttons
/*! images from optbtns.gr: <pre>
    image 0: game actions, not selected, at 4/10, size 35/108
    image 1: options menu, at 4/10, size 35/108
    image 2: save game menu, at 4/10
    image 3: quit game menu, at 4/10
    image 4: music menu, at 3/10
    image 5: detail level menu, at 3/9
    image 6/7: save game button, not selected/selected
    image 8/9: restore game
    image 10/11: music
    image 12/13: sound
    image 14/15: detail
    image 16/17: return to game
    image 18/19: quit game
    image 20/21: on
    image 22/23: off
    image 24/25: cancel
    image 26/27: done
    image 28/29: done
    image 30/31 .. 36/37: I, II, III, IV
    image 38/39 .. 44/45: low, medium, high, very high
    image 46: "restore game:"
    image 47/48: music is on/off.
    image 49/50: sound is on/off.
    image 51: "turn music:"
    image 52: "turn sound:"
    image 53..56: "detail level is low/medium/high/very high"
    image 57/58: yes
    image 59/60: no

    images from lfti.gr:
    image 0/1: options, not selected/selected
    image 2/3: talk
    image 4/5: get
    image 6/7: look
    image 8/9: fight
    image 10/11: use
</pre>
*/
class ua_ingame_command_buttons: public ua_ingame_orig_ctrl
{
public:
   //! initializes command buttons
   virtual void init(ua_game_interface& game, unsigned int xpos,
      unsigned int ypos);

   // virtual functions from ua_window
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

   //! selects (but doesn't press) specified button
   void select_button(int button=-1);

   //! select previous button (if any)
   void select_previous_button(bool jump_to_start=false);

   //! select next button (if any)
   void select_next_button(bool jump_to_end=false);

   //! performs button action
   void do_button_action();

protected:
   //! updates menu image
   void update_menu();

protected:
   //! button images
   std::vector<ua_image> img_buttons;

   //! action buttons
   std::vector<ua_image> img_actions;

   //! current menu
   ua_ingame_command_menu menu;

   //! options
   /*! the options are as following
       0: music
       1: sound
       2: detail
   */
   unsigned int options[3];

   //! currently selected button
   signed int button_selected;

   //! indicates if user is toggling off an action type
   bool toggle_off;

   //! action button stored when in main menu
   int saved_action_button;
};


#endif
//@}
