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
/*! \file save_game.hpp

   \brief save game screen

*/

// include guard
#ifndef uwadv_save_game_hpp_
#define uwadv_save_game_hpp_

// needed includes
#include "screen.hpp"
#include "savegame.hpp"
#include "imgquad.hpp"
#include "font.hpp"
#include "textscroll.hpp"
#include "mousecursor.hpp"


// classes

//! save game screen class
class ua_save_game_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_save_game_screen(bool from_menu);

   //! dtor
   virtual ~ua_save_game_screen(){}

   // virtual functions from ua_ui_screen_base

   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! updates list of savegames
   void update_list();

   //! updates button look/states
   void update_buttons();

   //! presses button
   void press_button();

   //! updates savegame info with selected savegame
   void update_info();

protected:
   // constants

   //! time to fade in/out
   static const double fade_time;


   //! savegames manager
   ua_savegames_manager* sgmgr;

   //! left background image
   ua_image_quad img_back1;

   //! right background image
   ua_image_quad img_back2;

   //! indicates if infos on left pane are shown
   bool show_infos;

   //! savegame preview image
   ua_texture tex_preview;

   //! image list with buttons / heads
   ua_image_list img_buttons;

   //! indicates if screen is called from start menu
   bool from_menu;

   //! button font
   ua_font font_btns;

   //! font for list entries
   ua_font font_normal;

   //! index of first game in list
   unsigned int list_base_game;

   //! index of selected savegame (or -1 when none)
   int selected_savegame;

   //! highlighted button (or -1 when none)
   int button_highlight;

   //! mouse cursor
   ua_mousecursor mousecursor;

   //! indicates if mouse button is pressed
   bool button_pressed;

   //! fade in/out state
   unsigned int fade_state;

   //! fade ticks
   unsigned int fade_ticks;

   //! indicates if we're editing the savegame description
   bool edit_desc;

   //! text scroll to edit savegame description
   ua_textscroll desc_scroll;

   //! savegame description text
   std::string desc;
};

#endif
