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
/*! \file ingame_orig.hpp

   \brief original, old style game screen user interface

*/

// include guard
#ifndef __uwadv_ingame_orig_hpp_
#define __uwadv_ingame_orig_hpp_

// needed includes
#include "screen.hpp"
#include "image.hpp"
#include "font.hpp"
#include "keymap.hpp"


// enums

//! game modes
typedef enum
{
   ua_mode_default=0, // nothing selected
   ua_mode_options,
   ua_mode_talk,
   ua_mode_get,
   ua_mode_look,
   ua_mode_fight,
   ua_mode_use,

} ua_ingame_orig_game_mode;

//! screen area values
typedef enum
{
   ua_area_screen3d=0,
   ua_area_textscroll,
   ua_area_gargoyle,
   ua_area_powergem,

   ua_area_menu_button0,
   ua_area_menu_button1,
   ua_area_menu_button2,
   ua_area_menu_button3,
   ua_area_menu_button4,
   ua_area_menu_button5,

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

   ua_area_none

} ua_ingame_orig_area;


// classes

//! user interface abstract base class
class ua_ingame_orig_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_ingame_orig_screen(){}
   virtual ~ua_ingame_orig_screen(){}

   // virtual functions
   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! sets up OpenGL stuff, flags, etc.
   void setup_opengl();

   //! renders 2d user interface
   void render_ui();

   //! handles keyboard action
   void handle_key_action(Uint8 type, SDL_keysym &keysym);

   //! returns area the given mouse coordinates is over
   ua_ingame_orig_area get_area(unsigned int xpos,unsigned int ypos);

   //! called for a given mouse action; click is false for mouse moves
   void mouse_action(bool click, bool left_button, bool pressed);

protected:

   //! field of view angle
   double fov;

   //! current view angle
   double viewangle;

   //! current mouse cursor
   unsigned int cursor_image;
   //! mouse cursor coordinates
   unsigned int cursorx,cursory;

   //! indicates if cursor is an object icon
   bool cursor_is_object;

   //! object the cursor currently is
   Uint16 cursor_object;

   //! start of inventory slots the user sees
   unsigned int slot_start;

   //! indicates if dragging should be checked
   bool check_dragging;

   //! inventory item that is dragged
   Uint16 drag_item;

   //! area the item is dragged from
   ua_ingame_orig_area drag_area;


   // misc. stuff

   //! ticks since start of screen
   unsigned long tickcount;

   //! key mappings
   ua_keymap keymap;

   //! look up/down states
   bool look_down,look_up;

   //! selected game mode
   ua_ingame_orig_game_mode gamemode;


   // mouse button states
   bool leftbuttondown,rightbuttondown;


   // images, textures and fonts

   //! 2d ui image background
   ua_image img_back;

   //! temp image to assemble 2d ui image
   ua_image img_temp;
   //! main 2d ui texture
   ua_texture tex;

   //! normal font
   ua_font font_normal;

   //! left "command" buttons
   ua_image_list img_cmd_btns;

   //! compass graphics
   ua_image_list img_compass;

   //! player appearance body graphics
   ua_image_list img_bodies;

   //! cursor images
   ua_image_list img_cursors;

   //! 2d object images
   ua_image_list img_objects;

   //! paperdoll armour images
   ua_image_list img_armor;

   //! alternative inventory panel for inside containers
   ua_image img_inv_bagpanel;

   //! up/down button for scrollable inventory
   ua_image_list img_inv_updown;
};

#endif
