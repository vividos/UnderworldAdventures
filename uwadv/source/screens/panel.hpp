/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file panel.hpp

   \brief inventory/stats/rune bag panel

*/

// include guard
#ifndef uwadv_panel_hpp_
#define uwadv_panel_hpp_

// needed includes
#include "core.hpp"
#include "font.hpp"
#include "screen.hpp"


// forward references
//enum ua_screen_area_id;
class ua_ingame_orig_screen;


// classes

//! panel
class ua_panel: public ua_screen_ctrl_base
{
public:
   ua_panel();

   void init_panel(ua_game_core_interface* core, ua_ingame_orig_screen* ingame_orig);
   virtual void suspend();
   virtual void resume();
   virtual void done();
   virtual bool mouse_action(bool click, bool left_button, bool pressed);
   virtual void render();
   virtual void tick();

protected:
   //! updates panel texture
   void update_panel_texture();

   //! clicked on inventory
   void inventory_click(
      bool pressed, bool left_button, unsigned int area);

   void inventory_dragged_item();

protected:
   //! ingame screen
   ua_ingame_orig_screen* ingame_orig;


   // inventory / item dragging

   //! start of inventory slots the user sees
   unsigned int slot_start;

   //! indicates if dragging should be checked
   bool check_dragging;

   //! area the item is dragged from
   unsigned int drag_area;

   //! item index that is currently dragged
   Uint16 drag_item;


   // panel graphics

   //! panels
   ua_image_list img_panels;

   //! panel texture
   ua_texture tex_panel;

   //! current panel type; 0 = inventory, 1 = runebag, 2 = stats
   unsigned int panel_type;

   //! alternative inventory panel when inside container
   ua_image img_inv_bagpanel;

   //! up/down button for scrollable inventory
   ua_image_list img_inv_updown;

   //! paperdoll armor images
   ua_image_list img_armor;

   //! indicates if armor is female
   bool armor_female;

   //! player appearance body graphics
   ua_image_list img_bodies;

   //! all inventory objects
   ua_image_list img_objects;

   //! normal font for inventory weight
   ua_font font_normal;
};

#endif
