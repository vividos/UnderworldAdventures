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
#include <sstream>


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
   scroll_conv.init(*core,58,52,161,79,13,42);
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
      ua_conv_code_vm::init(core->get_underworld().get_conv_globals(),
         core->get_strings().get_block(strblock));
   }

   state = ua_state_running;
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
   if (scroll_menu.handle_event(event) || scroll_conv.handle_event(event))
      return;

   switch(event.type)
   {
   case SDL_KEYDOWN:
      if (state == ua_state_wait_menu &&
          event.key.keysym.sym > SDLK_0 && event.key.keysym.sym <= SDLK_9)
      {
         unsigned int selection = event.key.keysym.sym - (Uint32)SDLK_1;

         if (selection<answer_values.size())
         {
            result_register = answer_values[selection];

            // print answer
            std::string answer(localstrings[answer_string_ids[selection]]);
            replace_placeholder(answer);

            scroll_conv.set_color(1);
            scroll_conv.print(answer.c_str());
            scroll_conv.set_color(46);

            // continue processing
            state = ua_state_running;
         }
      }
      break;

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
   if (!finished && !scroll_menu.have_more_lines() && !scroll_conv.have_more_lines())
   {
      while(state == ua_state_running && !finished)
         ua_conv_code_vm::step();
   }

   if (finished)
      core->pop_screen();
}

void ua_conversation_screen::imported_func(const std::string& funcname)
{
   Uint16 argpos = stack.get_stackp();
   Uint16 argcount = stack.at(argpos);
   argpos--;

   if (funcname.compare("babl_menu")==0)
   {
      argpos = stack.at(argpos);

      unsigned int ask_count = 0;
      Uint16 arg = stack.at(argpos++);
      answer_values.clear();
      answer_string_ids.clear();

      scroll_menu.clear_scroll();

      while (arg!=0)
      {
         ask_count++;

         // format menu entry string
         std::ostringstream buffer;
         buffer << ask_count << ". " << localstrings[arg] << std::ends;

         std::string menuentry(buffer.str());
         replace_placeholder(menuentry);

         // print menu entry
         scroll_menu.print(menuentry.c_str());

         // remember answer value/string id
         answer_values.push_back(ask_count);
         answer_string_ids.push_back(arg);

         // get next argument
         arg = stack.at(argpos++);
      }

      state = ua_state_wait_menu;

   } else

   if (funcname.compare("sex")==0)
   {
      Uint16 arg1 = stack.at(argpos--);
      arg1 = stack.at(arg1);

      Uint16 arg2 = stack.at(argpos);
      arg2 = stack.at(arg2);

      // check player gender
      if (core->get_underworld().get_player().get_attr(ua_attr_gender)!=0)
         arg1 = arg2;

      result_register = arg1;
   }
   else
      ua_trace("unknown imported function: %s\n",funcname.c_str());
}

void ua_conversation_screen::say_op(Uint16 str_id)
{
   std::string str(localstrings[str_id]);

   replace_placeholder(str);

   scroll_conv.print(str.c_str());
      // scrolled text view
//      process_code = false;
}

void ua_conversation_screen::store_value(Uint16 at, Uint16 val)
{
   std::map<Uint16,ua_conv_imported_item>::iterator iter =
      imported_globals.find(at);

   if (iter!=imported_globals.end())
      ua_trace("storing: %s = %04x\n",iter->second.name.c_str(),val);
}

void ua_conversation_screen::fetch_value(Uint16 at)
{
   std::map<Uint16,ua_conv_imported_item>::iterator iter =
      imported_globals.find(at);

   if (iter!=imported_globals.end())
      ua_trace("fetching: %s = %04x\n",iter->second.name.c_str(),stack.at(at));
}

Uint16 ua_conversation_screen::get_global(const std::string& globname)
{
   return 0;
}

void ua_conversation_screen::set_global(const std::string& globname, Uint16 val)
{
}
