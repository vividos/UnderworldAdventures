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
/*! \file panel.hpp

   \brief panel for inventory/stats/rune bag

*/
//! \ingroup screens

//@{

// include guard
#ifndef uwadv_panel_hpp_
#define uwadv_panel_hpp_

// needed includes
#include "panel.hpp"
#include "imgquad.hpp"
#include "font.hpp"


// enums

//! panel types
enum ua_panel_type
{
   ua_panel_inventory=0, //!< inventory panel
   ua_panel_stats,       //!< stats panel
   ua_panel_runebag,     //!< runebag panel
};


// classes

//! panel parent interface
class ua_panel_parent_interface
{
public:
   ua_panel_parent_interface(){}
   virtual ~ua_panel_parent_interface(){}

   virtual ua_game_interface& get_game_interface()=0;
};


//! panel
/*!
    \todo when rotating adjust y coords of windows, too, like in the original
*/
class ua_panel: public ua_image_quad
{
public:
   //! ctor
   ua_panel();

   //! initializes panel
   virtual void init(ua_panel_parent_interface* panel_parent, unsigned int xpos,
      unsigned int ypos);

   // virtual functions from ua_window
   virtual void destroy();
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);
   virtual void tick();

protected:
   //! updates panel image
   void update_panel();

   //! update chain images only
   void update_chains();

   //! updates stats panel
   void update_stats();

   //! updates runebag panel
   void update_runebag();

protected:
   //! interface to panel owner
   ua_panel_parent_interface* panel_parent;

   //! panel type
   ua_panel_type panel_type;

   //! indicates if female armor images are used
   bool armor_female;

   //! tickrate in ticks/s
   double tickrate;

   //! start of scrollable stats area
   unsigned int stats_scrollstart;


   // images / image lists

   //! panel background images
   std::vector<ua_image> img_panels;

   //! chains images
   std::vector<ua_image> img_chains;

   //! inventory background for bags
   ua_image img_inv_bagpanel;

   //! up/down buttons for inventory
   std::vector<ua_image> img_inv_updown;

   //! armor pieces for paperdoll
   std::vector<ua_image> img_armor;

   //! paperdoll body images
   std::vector<ua_image> img_bodies;

   //! inventory objects
   std::vector<ua_image> img_objects;


   //! chains image on top
   ua_image_quad img_chains_top;

   //! chains image on bottom
   ua_image_quad img_chains_bottom;

   //! font for stats list
   ua_font font_stats;


   //! indicates if panel is currently rotating
   bool rotate_panel;

   //! rotation angle
   double rotate_angle;

   //! new panel that is shown when rotation completes
   ua_panel_type rotate_panel_type;

   //! indicates if old-style "jerky" rotation should be used
   bool rotation_oldstyle;


/*
protected:
   //! updates panel texture
   void update_panel_texture();

   void update_cursor_image();

   //! clicked on inventory
   void inventory_click(
      bool pressed, bool left_button, unsigned int area);

   void inventory_dragged_item();

protected:
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

   //! normal font for inventory weight
   ua_font font_normal;
*/
};


#endif
//@}
