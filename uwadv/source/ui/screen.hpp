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
/*! \file screen.hpp

   \brief user interface screens base classes




   base class for user interface screens, e.g. main game screen, conversation screen,
   map screen etc.

   note that a derived class should call at least the init() and done()
   methods from their overridden methods

   also note that ua_ui_screen_base has a mousecursor member, but the screen
   must initialize it by itself to be usable.

*/

// include guard
#ifndef uwadv_screen_hpp_
#define uwadv_screen_hpp_

// needed includes
#include "window.hpp"


// classes

//! screen base class
class ua_screen: public ua_window
{
public:
   //! ctor
   ua_screen();
   //! dtor
   virtual ~ua_screen();

   //! inits the screen
   virtual void init(/*double ratio_x, double ratio_y*/);

   //! destroys window
   virtual void destroy();

   //! draws window contents
   virtual void draw();

   //! processes SDL events; returns true when event shouldn't processed further
   virtual bool process_event(SDL_Event& event);

   //! called every game tick
   virtual void tick();

   //! registers a window as subwindow
   void register_window(ua_window* window);

protected:
   //! list of all subwindows controlled by the screen
   std::vector<ua_window*> subwindows;
};








//#include "core.hpp"
//#include "mousecursor.hpp"


// forward references
//class ua_game_core_interface;


// structs
/*
//! screen area struct
struct ua_screen_area_data
{
   unsigned int area_id;
   unsigned int xmin, xmax, ymin, ymax;
};

const unsigned int ua_area_none = 0;
*/

// classes
/*
//! screen/control base class
class ua_screen_ctrl_base
{
public:
   //! ctor
   ua_screen_ctrl_base(){}
   //! dtor
   virtual ~ua_screen_ctrl_base(){}

   // virtual methods

   //! initializes screen/control
   virtual void init(ua_game_core_interface* core);

   //! handles event
   virtual void handle_event(SDL_Event& event);

   //! called for a given mouse action; click is false for mouse moves
   virtual void mouse_action(bool click, bool left_button, bool pressed);

   //! called when screen/control gets deactivated
   virtual void suspend(){}

   //! called when screen/control gets active again
   virtual void resume(){}

   //! clean up
   virtual void done(){}

   //! renders the screen
   virtual void render(){}

   //! called on every game tick
   virtual void tick(){}

   // static methods

   //! returns area the given mouse coordinates is over
   static unsigned int get_area(const ua_screen_area_data* table,
      unsigned int xpos,unsigned int ypos);

protected:
   //! game core interface pointer
   ua_game_core_interface *core;

   //! mouse cursor coordinates in 320x200 window range
   unsigned int cursorx,cursory;

   //! mouse button states
   bool leftbuttondown_old,rightbuttondown_old;
};


//! screen base class
class ua_ui_screen_base: public ua_screen_ctrl_base
{
public:
   //! ctor
   ua_ui_screen_base(){}
   //! dtor
   virtual ~ua_ui_screen_base(){}


   //! mouse cursor of screen
   ua_mousecursor& get_mousecursor(){ return mousecursor; }

protected:
   //! mouse cursor for screen
   ua_mousecursor mousecursor;
};
*/

#endif
