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
/*! \file cutscene_view.hpp

   \brief cutscene view screen

*/

// include guard
#ifndef uwadv_cutscene_view_hpp_
#define uwadv_cutscene_view_hpp_

// needed includes
#include "screen.hpp"
#include "cutscene.hpp"
#include "font.hpp"
#include "script/luascript.hpp"


// forward references
struct lua_State;


// classes

//! cutscene view screen
class ua_cutscene_view_screen: public ua_screen
{
public:
   //! ctor
   ua_cutscene_view_screen(unsigned int cuts=0):cutscene(cuts){}

   // virtual functions from ua_ui_screen_base
   virtual void init();
   virtual void destroy();
   virtual void draw();
   virtual bool process_event(SDL_Event& event);
   virtual void tick();

protected:
   //! creates text image from string
   void create_text_image(const char* str);

   //! performs cutscene action; called by cuts_do_action
   void do_action();

   // registered lua C functions

   //! performs an action given by the script
   static int cuts_do_action(lua_State* L);

protected:
   // constants

   //! frames per second for cutscene animation
   static const double anim_fps;

   //! time needed to fade in/out text
   static const double fade_time;

   //! name of lua userdata variable containing the "this" pointer
   static const char* lua_thisptr_name;


   //! lua scripting interface
   ua_lua_scripting lua;

   //! cutscene number to show
   unsigned int cutscene;

   //! number of ticks
   unsigned int tickcount;

   //! indicates if cutscene script ended
   bool ended;

   // subtitle text stuff

   //! indicates if the subtitle text is currently shown
   bool showtext;

   //! string block to use
   unsigned int strblock;

   //! text color
   Uint8 textcolor;

   //! subtitle font
   ua_font font_big;

   //! current text as image
   ua_image_quad img_text;

   //! text fadein/fadeout state
   unsigned int text_fade_state;

   //! count for text fadein/fadeout
   unsigned int text_fadecount;

   // cutscene animation stuff

   //! indicates if animation should be shown
   bool showanim;

   //! indicates if animation moves
   bool loopanim;

   //! cutscene animation sequence
   ua_cutscene cuts_anim;

   //! current animation frame
   unsigned int curframe;

   //! frame to stop at; if negative, don't stop
   int stopframe;

   //! animation tick count
   double animcount;

   //! anim fadein/fadeout state
   unsigned int anim_fade_state;

   //! count for anim fadein/fadeout
   unsigned int anim_fadecount;

   //! can playback sound
   bool canplaysound;

   //! can show text
   bool canshowtext;
};

#endif
