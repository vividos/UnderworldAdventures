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
/*! \file conversation.hpp

   \brief conversation screen

*/

// include guard
#ifndef __uwadv_conversation_hpp_
#define __uwadv_conversation_hpp_

// needed includes
#include "screen.hpp"
#include "imgquad.hpp"
#include "textscroll.hpp"
#include "mousecursor.hpp"
#include "conv/codevm.hpp"


// classes

//! conversation screen class
class ua_conversation_screen:
   public ua_ui_screen_base,
   public ua_conv_code_vm
{
public:
   //! ctor
   ua_conversation_screen(unsigned int conv):convslot(conv){}
   //! dtor
   virtual ~ua_conversation_screen(){}

   // virtual functions from ua_ui_screen_base

   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

   // virtual functions from ua_conv_code_vm

   virtual void imported_func(Uint16 number);
   virtual void say_op(Uint16 str_id);

protected:
   // ui elements

   //! background image
   ua_image_quad img_back;

   //! conversation scroll
   ua_textscroll scroll_conv;

   //! menu text scroll
   ua_textscroll scroll_menu;

   //! mouse cursor
   ua_mousecursor mousecursor;

   // conversation stuff

   //! conversation slot to use
   unsigned int convslot;

   //! indicates if code can be processed
   bool process_code;
};

#endif
