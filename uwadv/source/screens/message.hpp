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
/*! \file message.hpp

   \brief screen message processing

*/

// include guard
#ifndef uwadv_message_hpp_
#define uwadv_message_hpp_

// needed includes
#include <map>


// structs

struct ua_msg_area_coord
{
   //! ctor
//   ua_msg_area_coord(unsigned int xmin, unsigned int xmax, unsigned int ymin, unsigned int ymax);

   unsigned int xmin, xmax;
   unsigned int ymin, ymax;

   inline bool is_in_area(unsigned int xpos, unsigned int ypos) const;

   inline bool operator<(const ua_msg_area_coord& coord) const;
};


// template classes

template <typename T>
class ua_message_processor
{
public:
   //! base class type
   typedef T base_class_type;

   //! area handler member pointer type
   typedef void (T::*ua_msg_area_handler_func_type)(bool is_btn_click,
      bool left_btn, bool btn_pressed);

   //! keymap handler member pointer type
   typedef void (T::*ua_msg_keymap_handler_func_type)(ua_key_value keyval,
      SDL_Event& event);

   //! area map type
   typedef std::map<ua_msg_area_coord,ua_msg_area_handler_func_type> ua_msg_area_map;

public:
   //! ctor
   ua_message_processor(){}

   //! inits message processor
   void message_init(unsigned int screen_width, unsigned int screen_height);

   //! registers handler function for mouse actions in given area
   void register_area_handler(
      ua_msg_area_coord& coords,
      ua_msg_area_handler_func_type handler);

   //! registers handler function for given keymap object
   void register_keymap_handler(
      ua_keymap* keymap,
      ua_msg_keymap_handler_func_type handler);

   //! handles SDL events as messages to process
   void message_handle_event(SDL_Event& event);

protected:
   // virtual functions

   //! area handler call function
   virtual void call_area_handler(bool is_btn_click, bool left_btn,
      bool btn_pressed, ua_msg_area_handler_func_type handler)=0;

   //! keymap handler call function
   virtual void call_keymap_handler(ua_key_value keyval, SDL_Event& event,
      ua_msg_keymap_handler_func_type handler)=0;

protected:
   //! indicates if left button is pressed down
   bool leftbuttondown;

   //! indicates if right button is pressed down
   bool rightbuttondown;

private:
   //! map with area and area handler
   ua_msg_area_map area_map;

   //! keymap handler member pointer
   ua_msg_keymap_handler_func_type keymap_handler;

   //! screen sizes
   unsigned int scrwidth, scrheight;
};


// call handler implementation macro

#define UA_MSG_CALL_HANDLER_IMPL() \
   virtual void call_area_handler(bool is_btn_click, bool left_btn, \
      bool btn_pressed, ua_msg_area_handler_func_type handler) \
         { (this->*handler)(is_btn_click,left_btn,btn_pressed); } \
   virtual void call_keymap_handler(ua_key_value keyval, SDL_Event& event, \
      ua_msg_keymap_handler_func_type handler) \
         { (this->*handler)(keyval,event); }


// inline methods

inline bool ua_msg_area_coord::is_in_area(unsigned int xpos,
   unsigned int ypos) const
{
   return (xpos >= xmin && xpos <= xmax &&
           ypos >= ymin && ypos <= ymax);
}

inline bool ua_msg_area_coord::operator<(const ua_msg_area_coord& coord) const
{
   return coord.xmin < xmin && coord.ymin < ymin;
}


#endif
