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
/*! \file conversation.cpp

   \brief conversation screen implementation

*/

// needed includes
#include "common.hpp"
#include "conversation.hpp"
#include <sstream>


// ua_conversation_screen constants

//! time to fade in / out screen
const double ua_conversation_screen::fade_time = 0.5;

//! time to wait before conversation partner answers
const double ua_conversation_screen::answer_wait_time = 0.4;

//! time to wait before fading out at end of conversation
const double ua_conversation_screen::endconv_wait_time = 1.0;


// ua_conversation_screen methods

ua_conversation_screen::ua_conversation_screen(
   unsigned int conv_level,Uint16 conv_objpos)
:level(conv_level),objpos(conv_objpos)
{
}

void ua_conversation_screen::init(ua_game_core_interface* thecore)
{
   ua_ui_screen_base::init(thecore);

   ua_trace("conversation screen started\n");
   ua_trace("talking to npc at level %u, objpos %u, ",level,objpos);

   // get npc object to talk to
   ua_object& npc_obj =
      core->get_underworld().get_level(level).get_mapobjects().get_object(objpos);

   npcdata = npc_obj.get_object_info().data;
   Uint16 convslot = npcdata[0];

   if (convslot==0)
   {
      // generic conversation
      convslot = npc_obj.get_object_info().item_id-64+0x0100;
   }

   ua_trace("conversation slot 0x%04x\n",convslot);

   // clear screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

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
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   glEnable(GL_TEXTURE_2D);

   font_normal.init(core->get_settings(),ua_font_normal);

   // background image
   {
      ua_player& pl = core->get_underworld().get_player();

      const char *mainscreenname = "data/main.byt";

      // replace name when using uw_demo
      if (core->get_settings().get_gametype() == ua_game_uw_demo)
         mainscreenname = "data/dmain.byt";

      img_back.load_raw(core->get_settings(),mainscreenname);
      img_back.init(&core->get_texmgr(),0,0,320,200);

      ua_image_list img_converse;
      img_converse.load(core->get_settings(),"converse");

      img_back.fill_rect(43,0,190,47,1);

      // name panels
      img_back.paste_image(img_converse.get_image(0),43,0);
      img_back.paste_image(img_converse.get_image(0),139,0);

      // names
      std::string name1 = core->get_strings().get_string(7,16+convslot);
      std::string name2 = pl.get_name();

      if (npcdata[0]==0)
      {
         // generic conversation
         name1 = core->get_strings().get_string(4,npc_obj.get_object_info().item_id);

         std::string::size_type pos;
         pos = name1.find('_');
         if (pos!=std::string::npos) name1.erase(0,pos+1);

         pos = name1.find('&');
         if (pos!=std::string::npos) name1.erase(pos);
      }

      ua_image img_name;
      font_normal.create_string(img_name,name1.c_str(),101);
      img_back.paste_image(img_name,48,2,true);

      font_normal.create_string(img_name,name2.c_str(),101);
      img_back.paste_image(img_name,144,2,true);

      // barter areas
      img_back.paste_image(img_converse.get_image(1),82,9);
      img_back.paste_image(img_converse.get_image(1),139,9);

      // portrait frames/images
      img_back.paste_image(img_converse.get_image(2),43,9);
      img_back.paste_image(img_converse.get_image(2),195,9);

      // portrait images

      if (convslot<=28)
      {
         // portrait is in "charhead.gr"
         ua_image_list img_charhead;
         img_charhead.load(core->get_settings(),"charhead");

         img_back.paste_image(img_charhead.get_image(convslot-1),45,11);
      }
      else
      {
         // portrait is in "genheads.gr"
         ua_image_list img_genheads;
         img_genheads.load(core->get_settings(),"genhead");

         unsigned int gen_head = npc_obj.get_object_info().item_id-64;
         if (gen_head>59) gen_head = 0;

         img_back.paste_image(img_genheads.get_image(gen_head),45,11);
      }

      ua_image_list img_playerheads;
      img_playerheads.load(core->get_settings(),"heads");

      unsigned int appearance = pl.get_attr(ua_attr_appearance) +
         (pl.get_attr(ua_attr_gender)==0 ? 0 : 5);

      img_back.paste_image(img_playerheads.get_image(appearance),197,11);


      // scroll frames up/down
      img_back.paste_image(img_converse.get_image(3),42,48);
      img_back.paste_image(img_converse.get_image(4),42,126);

      // fill scroll area
      img_back.fill_rect(56,51, 164,81,42);

      img_back.convert_upload();
   }

   // adjust scroll width for uw_demo
   unsigned int scrollwidth = 289;

   if (core->get_settings().get_gametype() == ua_game_uw_demo)
      scrollwidth = 218;

   // init text scrolls
   scroll_conv.init(*core,58,51,161,81,13,42);
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

   state = ua_state_fadein;
   fade_ticks = 0;
   wait_count = 0;

   // start audio track "maps & legends" for conversations
   core->get_audio().start_music(11,false);
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

   ua_trace("conversation screen ended\n\n");
}

void ua_conversation_screen::handle_event(SDL_Event& event)
{
   if (scroll_menu.handle_event(event) || scroll_conv.handle_event(event))
   {
      // in user input mode?
      if (state == ua_state_wait_input)
      {
         // check if string was given
         std::string answer;
         if (scroll_menu.is_input_done(answer))
         {
            result_register = alloc_string(answer);

            // continue processing
            state = ua_state_running;

            // clear menu scroll
            scroll_menu.clear_scroll();

            wait_count = unsigned(answer_wait_time * core->get_tickrate());
         }
      }

      return;
   }

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

            scroll_conv.set_color(38);
            scroll_conv.print(answer.c_str());
            scroll_conv.set_color(46);

            // continue processing
            state = ua_state_running;

            // clear menu scroll
            scroll_menu.clear_scroll();

            wait_count = unsigned(answer_wait_time * core->get_tickrate());
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
   // calculate quad brightness
   Uint8 light = 255;
   if (state == ua_state_fadein || state == ua_state_fadeout)
   {
      light = Uint8(255*(double(fade_ticks) / (core->get_tickrate()*fade_time)));

      if (state == ua_state_fadeout)
         light = 255-light;
   }
   glColor3ub(light,light,light);

   // render images
   img_back.render();
   scroll_conv.render();
   scroll_menu.render();

   // render mouse cursor
   glEnable(GL_BLEND);
   mousecursor.draw();
   glDisable(GL_BLEND);
}

void ua_conversation_screen::tick()
{
   // still waiting?
   if (wait_count>0)
   {
      wait_count--;
      return; // do nothing
   }

   // execute code until finished, waiting for an action or have [MORE]
   // lines to scroll
   while(state == ua_state_running && !finished &&
         !scroll_menu.have_more_lines() && !scroll_conv.have_more_lines())
   {
      try
      {
         ua_conv_code_vm::step();
      }
      catch(ua_conv_vm_exception e)
      {
         ua_trace("caught ua_conv_vm_exception: ip=%04x, type: ",instrp);
         switch(e)
         {
         case ua_ex_error_loading: ua_trace("error while loading"); break;
         case ua_ex_div_by_zero: ua_trace("division by zero"); break;
         case ua_ex_code_access: ua_trace("code access"); break;
         case ua_ex_globals_access: ua_trace("globals access"); break;
         case ua_ex_stack_access: ua_trace("stack access"); break;
         case ua_ex_unk_opcode: ua_trace("unknown opcode"); break;
         case ua_ex_imported_na: ua_trace("imported function not available"); break;
         default: ua_trace("unknown exception number %u",e); break;
         }
         finished = true;
      }
      catch(...)
      {
         ua_trace("caught unknown exception from ua_conv_code_vm::step()\n");
         finished = true;
      }
   }

   if (finished && state == ua_state_running)
   {
      // clear menu scroll
      scroll_menu.clear_scroll();

      // wait a bit, then fade out
      state = ua_state_fadeout;
      fade_ticks = 0;

      wait_count = unsigned(endconv_wait_time * core->get_tickrate());

      // fade out music
      core->get_audio().fadeout_music(fade_time);
      return;
   }

   // check for fading in/out
   if ((state == ua_state_fadein || state == ua_state_fadeout) &&
      ++fade_ticks >= (core->get_tickrate()*fade_time))
   {
      if (state == ua_state_fadein)
         state = ua_state_running;
      else
      {
         // clear screen
         glClearColor(0,0,0,0);
         glClear(GL_COLOR_BUFFER_BIT);
         SDL_GL_SwapBuffers();

         // leave screen
         core->pop_screen();
      }
   }
}

void ua_conversation_screen::imported_func(const std::string& funcname)
{
   Uint16 argpos = stack.get_stackp();
   Uint16 argcount = stack.at(argpos);
   argpos--;

   if (funcname.compare("babl_menu")==0)
   {
      // get pointer to list
      argpos = stack.at(argpos);

      answer_values.clear();
      answer_string_ids.clear();
      scroll_menu.clear_scroll();

      unsigned int ask_count = 0;
      Uint16 arg = stack.at(argpos++);

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

   if (funcname.compare("babl_fmenu")==0)
   {
      // get pointer to the two lists
      Uint16 argpos1 = stack.at(argpos--);
      Uint16 argpos2 = stack.at(argpos);

      answer_values.clear();
      answer_string_ids.clear();
      scroll_menu.clear_scroll();

      unsigned int ask_count = 0;
      Uint16 arg1 = stack.at(argpos1++);
      Uint16 arg2 = stack.at(argpos2++);

      while (arg1!=0)
      {
         if (arg2!=0)
         {
            ask_count++;

            // format menu entry string
            std::ostringstream buffer;
            buffer << ask_count << ". " << localstrings[arg1] << std::ends;

            std::string menuentry(buffer.str());
            replace_placeholder(menuentry);

            // print menu entry
            scroll_menu.print(menuentry.c_str());

            // remember answer value/string id
            answer_values.push_back(arg1);
            answer_string_ids.push_back(arg1);
         }

         // get next arguments
         arg1 = stack.at(argpos1++);
         arg2 = stack.at(argpos2++);
      }

      state = ua_state_wait_menu;

   } else

   if (funcname.compare("print")==0)
   {
      Uint16 arg = stack.at(argpos++);
      arg = stack.at(arg);

      std::string printtext(localstrings[arg]);
      replace_placeholder(printtext);

      scroll_conv.print(printtext.c_str());

   } else

   if (funcname.compare("babl_ask")==0)
   {
      // start user input mode
      scroll_menu.clear_scroll();
      scroll_menu.print(">");
      scroll_menu.enter_input_mode();

      state = ua_state_wait_input;

   } else

   if (funcname.compare("compare")==0)
   {
   } else

   if (funcname.compare("random")==0)
   {
   } else

   if (funcname.compare("contains")==0)
   {
      // get arguments
      Uint16 arg1 = stack.at(argpos--);
      arg1 = stack.at(arg1);

      Uint16 arg2 = stack.at(argpos);
      arg2 = stack.at(arg2);

      // get strings
      std::string str1(localstrings[arg1]), str2(localstrings[arg2]);

      ua_str_lowercase(str1);
      ua_str_lowercase(str2);

      // check if first string contains second
      result_register = str1.find(str2) != std::string::npos;

   } else

   if (funcname.compare("length")==0)
   {
      // get argument
      Uint16 arg = stack.at(argpos--);
      arg = stack.at(arg);

      // return string length
      result_register = localstrings[arg].size();

   } else

   if (funcname.compare("get_quest")==0)
   {
      Uint16 arg = stack.at(argpos--);
      arg = stack.at(arg);

      result_register = core->get_underworld().get_questflags()[arg];

      ua_trace("get_quest[%u] = %u\n",arg,result_register);

   } else

   if (funcname.compare("set_quest")==0)
   {
      Uint16 arg1 = stack.at(argpos--);
      arg1 = stack.at(arg1);

      Uint16 arg2 = stack.at(argpos);
      arg2 = stack.at(arg2);

      core->get_underworld().get_questflags()[arg2] = arg1;

      ua_trace("set_quest[%u] = %u\n",arg2,arg1);
   
   } else

   if (funcname.compare("sex")==0)
   {
      Uint16 arg1 = stack.at(argpos--);
      arg1 = stack.at(arg1);

      Uint16 arg2 = stack.at(argpos);
      arg2 = stack.at(arg2);

      // check player gender
      if (core->get_underworld().get_player().get_attr(ua_attr_gender)==0)
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
   Uint16 val = 0;

   if (globname.compare("play_name")==0)
   {
      val = alloc_string(core->get_underworld().get_player().get_name());
   }
//   else
//      ua_trace("get global: unhandled global %s\n",globname.c_str());

   return val;
}

void ua_conversation_screen::set_global(const std::string& globname, Uint16 val)
{
}
