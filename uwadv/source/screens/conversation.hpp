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
/*! \file conversation.hpp

   \brief conversation screen

*/
//! \ingroup screens

//@{

// include guard
#ifndef uwadv_conversation_hpp_
#define uwadv_conversation_hpp_

// needed includes
#include "screen.hpp"
#include "imgquad.hpp"
#include "textscroll.hpp"
#include "mousecursor.hpp"
#include "codevm.hpp"
#include "textedit.hpp"
#include "fading.hpp"


// enums

//! conv screen state values
enum ua_conv_screen_state
{
   ua_state_running=0,  //!< vm code can run
   ua_state_wait_menu,  //!< waiting for menu selection
   ua_state_wait_end,   //!< waiting for a key to end conversation
   ua_state_text_input, //!< user currently can enter input text
   ua_state_fadein,     //!< fading in screen
   ua_state_fadeout,    //!< fading out screen
};


// classes

//! conversation screen class
class ua_conversation_screen: public ua_screen,
   public ua_conv_code_callback
{
public:
   //! ctor
   ua_conversation_screen(ua_game_interface& game, Uint16 conv_objpos);
   //! dtor
   virtual ~ua_conversation_screen(){}

   // virtual functions from ua_screen
   virtual void init();
   virtual void destroy();
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void tick();

   // virtual functions from ua_conv_code_callback
   virtual void say(Uint16 index);
   virtual const char* get_local_string(Uint16 index);
   virtual Uint16 external_func(const char* funcname, ua_conv_stack& stack);
   virtual Uint16 get_global(const char* globname);
   virtual void set_global(const char* globname, Uint16 val);

protected:
   //! allocates new local string
   Uint16 alloc_string(const char* the_str);

protected:
   // constants

   //! time to fade in / out screen
   static const double fade_time;

   //! time to wait before conversation partner answers
   static const double answer_wait_time;


   //! conversation code virtual machine
   ua_conv_code_vm code_vm;

   //! all current local strings
   std::vector<std::string> localstrings;


   // ui elements

   //! background image
   ua_image_quad img_back;

   //! conversation scroll
   ua_textscroll scroll_conv;

   //! menu text scroll
   ua_textscroll scroll_menu;

   //! text edit window for input
   ua_textedit_window textedit;

   //! mouse cursor
   ua_mousecursor mousecursor;

   //! fading helper
   ua_fading_helper fader;

   //! font for panel names
   ua_font font_normal;

   //! counter to wait some ticks
   unsigned int wait_count;


   // conversation stuff

   //! position in master object list of npc object
   Uint16 objpos;

   //! screen state
   ua_conv_screen_state state;

   //! possible values for answer to push on stack after selection
   std::vector<Uint16> answer_values;

   //! answer string id's
   std::vector<Uint16> answer_string_ids;
};


#endif
//@}
