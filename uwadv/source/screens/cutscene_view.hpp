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
/*! \file cutscene_view.hpp

   \brief cutscene view screen

*/

// include guard
#ifndef __uwadv_cutscene_view_hpp_
#define __uwadv_cutscene_view_hpp_

// needed includes
#include "screen.hpp"
#include "cutscene.hpp"
#include "font.hpp"


// classes

//! cutscene view screen
class ua_cutscene_view_screen: public ua_ui_screen_base
{
public:
   //! ctor
   ua_cutscene_view_screen(unsigned int cuts=0):cutscene(cuts){}

   // virtual functions from ua_ui_screen_base
   virtual void init();
   virtual void done();
   virtual void handle_event(SDL_Event &event);
   virtual void render();
   virtual void tick();

protected:
   //! creates text image from string
   void create_text_image(const char *str);

   //! performs cutscene action; called by cuts_do_action
   void do_action();

   // registered lua C functions

   //! performs an action given by the script
   static int cuts_do_action(lua_State *L);

protected:
   //! lua script state
   lua_State *L;

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
   ua_image img_text;

   //! texture for cutscene text
   ua_texture tex_text;

   //! text fadein/fadeout state
   unsigned int text_fade_state;

   //! count for fadein/fadeout
   unsigned int fadecount;

   // cutscene animation stuff

   //! indicates if animation should be shown
   bool showanim;

   //! indicates if animation moves
   bool loopanim;

   //! cutscene animation sequence
   ua_cutscene cuts;

   //! texture for animation
   ua_texture tex_anim;

   //! current animation frame
   unsigned int curframe;

   //! animation tick count
   double animcount;
};

#endif
