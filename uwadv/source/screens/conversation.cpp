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
/*! \file conversation.cpp

   \brief conversation screen implementation

*/

// needed includes
#include "common.hpp"
#include "conversation.hpp"


// ua_conversation_screen methods

void ua_conversation_screen::init()
{
   // init OpenGL
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   gluOrtho2D(0, 320, 0, 200);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glDisable(GL_DEPTH_TEST);
   glDisable(GL_BLEND);

   // background image
   {
      const char *mainscreenname = "data/main.byt";

      // replace name when using uw_demo
      if (core->get_settings().get_gametype() == ua_game_uw_demo)
         mainscreenname = "data/dmain.byt";

      img_back.load_raw(core->get_settings(),mainscreenname);
      img_back.init(&core->get_texmgr(),0,0,320,200);

      img_back.convert_upload();
   }

   // adjust scroll width for uw_demo
   unsigned int scrollwidth = 289;

   if (core->get_settings().get_gametype() == ua_game_uw_demo)
      scrollwidth = 218;

   // init text scrolls
   scroll_conv.init(*core,58,52,161,79,10,42);
   scroll_conv.set_color(46);

   scroll_menu.init(*core,15,169, scrollwidth,30, 5, 42);
   scroll_menu.set_color(46);

   // init mouse cursor
   mousecursor.init(core,0);
   mousecursor.show(true);


   // init conv. code virtual machine
   {
      std::string cnv_name(core->get_settings().get_string(ua_setting_uw_path));
      cnv_name.append("data/cnv.ark");

      ua_conv_code_vm::load_code(cnv_name.c_str(),static_cast<Uint16>(convslot));
      ua_conv_code_vm::init(core->get_underworld().get_conv_globals());

      process_code = true;
   }
}

void ua_conversation_screen::done()
{
   // write back conv. globals
   ua_conv_code_vm::done(core->get_underworld().get_conv_globals());

   // clean up all ui elements
   img_back.done();
   scroll_conv.done();
   scroll_menu.done();
   mousecursor.done();
}

void ua_conversation_screen::handle_event(SDL_Event &event)
{
   if (scroll_menu.handle_event(event))
      return;

   switch(event.type)
   {
   case SDL_MOUSEMOTION: // mouse has moved
      {
         // calculate cursor position
         int x,y;
         SDL_GetMouseState(&x,&y);

         unsigned int cursorx = unsigned(double(x)/core->get_screen_width()*320.0);
         unsigned int cursory = unsigned(double(y)/core->get_screen_height()*200.0);

         mousecursor.updatepos(cursorx,cursory);
      }
      break;
   default: break;
   }
}

void ua_conversation_screen::render()
{
   glColor3ub(255,255,255);

   img_back.render();
   scroll_conv.render();
   scroll_menu.render();

   mousecursor.draw();
}

void ua_conversation_screen::tick()
{
//   while(process_code)
//      ua_conv_code_vm::step();

//   core->pop_screen();
}

void ua_conversation_screen::imported_func(Uint16 number)
{
}

void ua_conversation_screen::say_op(Uint16 str_id)
{
}
