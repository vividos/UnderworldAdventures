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
//! \ingroup screens

//@{

// include guard
#ifndef uwadv_save_game_hpp_
#define uwadv_save_game_hpp_

// needed includes
#include "screen.hpp"
#include "savegame.hpp"
#include "imgquad.hpp"
#include "fading.hpp"
#include "font.hpp"
#include "textscroll.hpp"
#include "mousecursor.hpp"


// forward references
class ua_save_game_screen;


// enums

//! button id's
enum ua_save_game_button_id
{
   ua_button_save=0,  //!< save button
   ua_button_load,    //!< load button
   ua_button_refresh, //!< refresh button
   ua_button_exit     //!< exit button
};


// classes

//! savegame screen button class
class ua_save_game_button: public ua_image_quad
{
public:
   //! ctor
   ua_save_game_button(){}

   //! initializes button
   void init(ua_save_game_screen* screen, ua_game_interface& game,
      unsigned int xpos, unsigned int ypos, const char* text,
      ua_save_game_button_id id);

   // virtual methods from ua_window

   virtual bool process_event(SDL_Event& event);

   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! updates button image according to state
   void update_button(bool state_pressed);

protected:
   // constants

   //! standard button width
   static const unsigned int button_width;

   //! pointer to savegame screen
   ua_save_game_screen* screen;

   //! button id
   ua_save_game_button_id id;

   //! indicates if one of the mouse buttons is down
   bool leftbuttondown, rightbuttondown;

   //! button images
   std::vector<ua_image> img_buttons;
};


//! savegames list class
class ua_save_game_savegames_list: public ua_image_quad
{
public:
   //! ctor
   ua_save_game_savegames_list(){}

   //! initializes savegames list
   void init(ua_save_game_screen* screen, ua_game_interface& game,
      unsigned int xpos, unsigned int ypos, bool show_new);

   //! updates list
   void update_list();

   //! returns selected savegame index
   int get_selected_savegame(){ return selected_savegame; }

   // virtual methods from ua_window

   virtual void mouse_event(bool button_clicked, bool left_button,
      bool button_down, unsigned int mousex, unsigned int mousey);

protected:
   //! pointer to savegames manager
   ua_savegames_manager* savegames_manager;

   //! pointer to savegame screen 
   ua_save_game_screen* screen;

   //! font for list text
   ua_font font_normal;

   //! first list item to show
   unsigned int list_base;

   //! index of selected savegame (or -1 when none)
   int selected_savegame;

   //! indicates if the "new savegame" entry is shown
   bool show_new;
};


//! save game screen class
class ua_save_game_screen: public ua_screen
{
public:
   //! ctor
   ua_save_game_screen(bool from_menu);
   //! dtor
   virtual ~ua_save_game_screen(){}

   //! called from ua_save_game_button when a button is pressed
   void press_button(ua_save_game_button_id id);

   // virtual functions from ua_screen
   virtual void init();
   virtual void destroy();
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
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


   // buttons

   //! save button
   ua_save_game_button button_save;
   ua_save_game_button button_load;
   ua_save_game_button button_refresh;
   ua_save_game_button button_exit;

   //! button id pressed
   ua_save_game_button_id pressed_button;

   //! savegames list
   ua_save_game_savegames_list savegames_list;

   //! indicates if screen is called from start menu
   bool from_menu;

   //! left background image
   ua_image_quad img_back1;

   //! right background image
   ua_image_quad img_back2;
/*
   //! original image contents for right background image
   ua_image img_back2_orig;
*/
   //! indicates if preview image is shown
   bool show_preview;

   //! savegame preview image
   ua_texture tex_preview;
/*
   //! image list with buttons / heads
   ua_image_list img_buttons;
*/

   //! mouse cursor
   ua_mousecursor mousecursor;

   //! fading helper
   ua_fading_helper fader;

   //! fade in/out state
   unsigned int fade_state;

/*
   //! indicates if mouse button is pressed
   bool button_pressed;

   //! indicates if we're editing the savegame description
   bool edit_desc;

   //! text scroll to edit savegame description
   ua_textscroll desc_scroll;

   //! savegame description text
   std::string desc;
*/
};


#endif
//@}
