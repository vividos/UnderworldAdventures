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
/*! \file conversation.cpp

   \brief conversation screen implementation

*/

// needed includes
#include "common.hpp"
#include "conversation.hpp"
#include "objects.hpp"
#include "gamestrings.hpp"
#include "underworld.hpp"
#include "renderer.hpp"
#include "audio.hpp"
#include <sstream>


// ua_conversation_screen constants

//! time to fade in / out screen
const double ua_conversation_screen::fade_time = 0.5;

//! time to wait before conversation partner answers
const double ua_conversation_screen::answer_wait_time = 0.4;


// ua_conversation_screen methods

ua_conversation_screen::ua_conversation_screen(ua_game_interface& game,
   Uint16 conv_objpos)
:ua_screen(game), objpos(conv_objpos)
{
}

void ua_conversation_screen::init()
{
   ua_screen::init();

   ua_trace("conversation screen started\n");
   ua_trace("talking to npc at objpos %u, ",objpos);

   // get npc object to talk to
   ua_object& npc_obj = game.get_underworld().get_current_level().
      get_mapobjects().get_object(objpos);

   Uint16 convslot = npc_obj.get_ext_object_info().npc_whoami;

   if (convslot==0)
   {
      // generic conversation
      convslot = npc_obj.get_object_info().item_id-64+0x0100;
   }

   ua_trace("conversation slot 0x%04x\n",convslot);

   // init renderer
   game.get_renderer().setup_camera2d();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   font_normal.load(game.get_settings(),ua_font_normal);

   // background image
   {
      ua_player& pl = game.get_underworld().get_player();

      const char* mainscreenname = "data/main.byt";

      // replace name when using uw_demo
      if (game.get_settings().get_bool(ua_setting_uw1_is_uw_demo))
         mainscreenname = "data/dmain.byt";

      ua_image& img = img_back.get_image();

      game.get_image_manager().load(img, mainscreenname,0,0,ua_img_byt);

      std::vector<ua_image> imgs_converse;
      game.get_image_manager().load_list(imgs_converse, "converse");

      img.fill_rect(43,0,190,47,1);

      // name panels
      img.paste_image(imgs_converse[0],43,0);
      img.paste_image(imgs_converse[0],139,0);

      // names
      std::string name1 = game.get_gamestrings().get_string(7,16+convslot);
      std::string name2 = pl.get_name();

      if (convslot==0)
      {
         // generic conversation
         name1 = game.get_gamestrings().get_string(4,npc_obj.get_object_info().item_id);

         std::string::size_type pos;
         pos = name1.find('_');
         if (pos!=std::string::npos) name1.erase(0,pos+1);

         pos = name1.find('&');
         if (pos!=std::string::npos) name1.erase(pos);
      }

      ua_image img_name;
      font_normal.create_string(img_name,name1.c_str(),101);
      img.paste_image(img_name,48,2,true);

      font_normal.create_string(img_name,name2.c_str(),101);
      img.paste_image(img_name,144,2,true);

      // barter areas
      img.paste_image(imgs_converse[1],82,9);
      img.paste_image(imgs_converse[1],139,9);

      // portrait frames/images
      img.paste_image(imgs_converse[2],43,9);
      img.paste_image(imgs_converse[2],195,9);

      // portrait images

      if (convslot<=28)
      {
         // portrait is in "charhead.gr"
         std::vector<ua_image> imgs_charhead;
         game.get_image_manager().load_list(imgs_charhead, "charhead");

         img.paste_image(imgs_charhead[convslot-1],45,11);
      }
      else
      {
         // portrait is in "genheads.gr"
         std::vector<ua_image> imgs_genheads;
         game.get_image_manager().load_list(imgs_genheads, "genhead");

         unsigned int gen_head = npc_obj.get_object_info().item_id-64;
         if (gen_head>59) gen_head = 0;

         img.paste_image(imgs_genheads[gen_head],45,11);
      }

      std::vector<ua_image> imgs_playerheads;
      game.get_image_manager().load_list(imgs_playerheads, "heads");

      unsigned int appearance = pl.get_attr(ua_attr_appearance) +
         (pl.get_attr(ua_attr_gender)==0 ? 0 : 5);

      img.paste_image(imgs_playerheads[appearance],197,11);


      // scroll frames up/down
      img.paste_image(imgs_converse[3],42,48);
      img.paste_image(imgs_converse[4],42,126);

      // fill scroll area
      img.fill_rect(56,51, 164,81,42);

      img_back.init(game,0,0);
      img_back.update();
      register_window(&img_back);
   }

   // adjust scroll width for uw_demo
   unsigned int scrollwidth = 289;

   if (game.get_settings().get_bool(ua_setting_uw1_is_uw_demo))
      scrollwidth = 218;

   // init text scrolls
   scroll_conv.init(game,58,51,161,81,42);
   scroll_conv.set_color_code(ua_cc_black);
   register_window(&scroll_conv);

   scroll_menu.init(game,15,169, scrollwidth,30, 42);
   scroll_menu.set_color_code(ua_cc_black);
   register_window(&scroll_menu);

   // init mouse cursor
   mousecursor.init(game,0);
   mousecursor.show(true);
   register_window(&mousecursor);


   // init conversation code virtual machine
   {
      std::string cnv_name(game.get_settings().get_string(ua_setting_uw_path));
      cnv_name.append("data/cnv.ark");

      // load code into vm
      ua_uw_import import;
      import.load_conv_code(code_vm, cnv_name.c_str(), static_cast<Uint16>(convslot));

      // get local strings
      // note: convslot is used to load strings, not the strblock value set in
      // conv header
      game.get_gamestrings().get_stringblock(
         code_vm.get_strblock(),localstrings);

      code_vm.init(this,game.get_underworld().get_conv_globals());
   }

   state = ua_state_fadein;
   fader.init(true,game.get_tickrate(),fade_time);
   
   wait_count = 0;

   // start audio track "maps & legends" for conversations
   game.get_audio_manager().start_music(ua_music_uw1_maps_legends,false);
}

void ua_conversation_screen::destroy()
{
   // write back conv. globals
   code_vm.done(game.get_underworld().get_conv_globals());

   ua_trace("conversation screen ended\n\n");
}

void ua_conversation_screen::draw()
{
   // calculate quad brightness
   Uint8 light = 255;
   if (state == ua_state_fadein || state == ua_state_fadeout)
      light = fader.get_fade_value();

   glColor3ub(light,light,light);

   // render ui elements
   ua_screen::draw();

   // render text edit window when available
   if (state == ua_state_text_input)
      textedit.draw();
}

bool ua_conversation_screen::process_event(SDL_Event& event)
{
   if (ua_screen::process_event(event))
      return true;

   // process text input-specific events
   if (state == ua_state_text_input)
   {
      // let text edit window process event
      if (textedit.process_event(event))
         return true;

      // user event?
      if (event.type == SDL_USEREVENT &&
          (event.user.code == ua_event_textedit_finished ||
           event.user.code == ua_event_textedit_aborted))
      {
         // get string
         Uint16 answer_id = alloc_string(textedit.get_text());
         code_vm.set_result_register(answer_id);

         // continue processing code
         state = ua_state_running;

         textedit.done();

         // clear menu scroll
         scroll_menu.clear_scroll();

         wait_count = unsigned(answer_wait_time * game.get_tickrate());
      }
   }

   switch(event.type)
   {
   case SDL_KEYDOWN:
      if (state == ua_state_wait_menu &&
          event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)
      {
         unsigned int selection = event.key.keysym.sym - (Uint32)SDLK_1;

         if (selection<answer_values.size())
         {
            code_vm.set_result_register(answer_values[selection]);

            // print answer
            std::string answer(localstrings[answer_string_ids[selection]]);
            code_vm.replace_placeholder(answer);

            scroll_conv.set_color_code(ua_cc_orange);
            scroll_conv.print(answer.c_str());
            scroll_conv.set_color_code(ua_cc_black);

            // continue processing
            state = ua_state_running;

            // clear menu scroll
            scroll_menu.clear_scroll();

            wait_count = unsigned(answer_wait_time * game.get_tickrate());
         }
      }
      else
      if (state == ua_state_wait_end)
      {
         // start fade out
         state = ua_state_fadeout;
         fader.init(false,game.get_tickrate(),fade_time);

         wait_count = 0;

         // fade out music
         game.get_audio_manager().fadeout_music(fade_time);
      }
      break;

   default:
      break;
   }

   return false;
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
   while(state == ua_state_running &&
      !scroll_menu.is_waiting_more() && !scroll_conv.is_waiting_more())
   {
      if (!code_vm.step())
         state = ua_state_wait_end;
   }

   // finished conv. code?
   if (state == ua_state_wait_end)
   {
      // clear menu scroll
      scroll_menu.clear_scroll();
      return;
   }

   // check for fading in/out
   if ((state == ua_state_fadein || state == ua_state_fadeout) && fader.tick())
   {
      if (state == ua_state_fadein)
         state = ua_state_running;
      else
      {
         // leave screen
         game.remove_screen();
      }
   }
}

Uint16 ua_conversation_screen::alloc_string(const char* the_str)
{
   std::string str(the_str);
   Uint16 pos = localstrings.size();
   localstrings.push_back(str);
   return pos;
}

void ua_conversation_screen::say(Uint16 index)
{
   ua_assert(index<localstrings.size());

   std::string str(localstrings[index]);
   code_vm.replace_placeholder(str);

   scroll_conv.print(str.c_str());
}

const char* ua_conversation_screen::get_local_string(Uint16 index)
{
   ua_assert(index<localstrings.size());
   return localstrings[index].c_str();
}

/* *=implemented, x=assert
   * babl_menu
   * babl_fmenu
   * print
   * babl_ask
   * compare
   * random
   x plural
   * contains
   x append
   x copy
   x find
   * length
   x val
   x say
   x respond
   * get_quest
   * set_quest
   * sex
     show_inv
     give_to_npc
     give_ptr_npc
     take_from_npc
     take_id_from_npc
     identify_inv
     do_offer
     do_demand
     do_inv_create
     do_inv_delete
     check_inv_quality
     set_inv_quality
     count_inv
     setup_to_barter
     end_barter
     do_judgement
     do_decline
     pause
     set_likes_dislikes
     gronk_door
     set_race_attitude
     place_object
     take_from_npc_inv
     add_to_npc_inv
     remove_talker
     set_attitude
     x_skills
     x_traps
     x_obj_pos
     x_obj_stuff
     find_inv
     find_barter
     find_barter_total

*/
Uint16 ua_conversation_screen::external_func(const char* the_funcname,
   ua_conv_stack& stack)
{
   std::string funcname(the_funcname);

   Uint16 argpos = stack.get_stackp();
   Uint16 argcount = stack.at(argpos);
   argpos--;

   Uint16 result_register = 0xffff;

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
         code_vm.replace_placeholder(menuentry);

         // print menu entry
         scroll_menu.print(menuentry.c_str());

         // remember answer value/string id
         answer_values.push_back(ask_count);
         answer_string_ids.push_back(arg);

         // get next argument
         arg = stack.at(argpos++);
      }

      state = ua_state_wait_menu;
   }
   else

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
            code_vm.replace_placeholder(menuentry);

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
   }
   else

   if (funcname.compare("print")==0)
   {
      Uint16 arg = stack.at(argpos++);
      arg = stack.at(arg);

      std::string printtext(localstrings[arg]);
      code_vm.replace_placeholder(printtext);

      scroll_conv.print(printtext.c_str());
   }
   else

   if (funcname.compare("babl_ask")==0)
   {
      // start user input mode
      scroll_menu.clear_scroll();

      // init text edit
      textedit.init(game, scroll_menu.get_xpos()+1, scroll_menu.get_ypos()+1,
         scroll_menu.get_width(), 42, 1, 46,">");

      state = ua_state_text_input;
   }
   else

   if (funcname.compare("compare")==0)
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
      result_register = str1 == str2;
   }
   else

   if (funcname.compare("random")==0)
   {
      Uint16 arg = stack.at(argpos--);
      arg = stack.at(arg);

      // this code assumes that rand() can return RAND_MAX

      // rnum is in the range [0..1[
      double rnum = double(rand())/double(RAND_MAX+1);
      rnum *= arg; // now in range [0..arg[
      result_register = Uint16(rnum + 1.0); // now from [1..arg+1[
   }
   else

   if (funcname.compare("plural")==0)
   {
      ua_trace("conv_vm: intrinsic plural() not implemented");
      ua_assert(false);
   }
   else

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
   }
   else

   if (funcname.compare("append")==0)
   {
      ua_trace("conv_vm: intrinsic append() not implemented");
      ua_assert(false);
   }
   else

   if (funcname.compare("copy")==0)
   {
      ua_trace("conv_vm: intrinsic copy() not implemented");
      ua_assert(false);
   }
   else

   if (funcname.compare("find")==0)
   {
      ua_trace("conv_vm: intrinsic find() not implemented");
      ua_assert(false);
   }
   else

   if (funcname.compare("length")==0)
   {
      // get argument
      Uint16 arg = stack.at(argpos--);
      arg = stack.at(arg);

      // return string length
      result_register = localstrings[arg].size();
   }
   else

   if (funcname.compare("val")==0)
   {
      ua_trace("conv_vm: intrinsic val() not implemented");
      ua_assert(false);
   }
   else

   if (funcname.compare("say")==0)
   {
      ua_trace("conv_vm: intrinsic say() not implemented");
      ua_assert(false);
   }
   else

   if (funcname.compare("respond")==0)
   {
      ua_trace("conv_vm: intrinsic respond() not implemented");
      ua_assert(false);
   }
   else

   if (funcname.compare("get_quest")==0)
   {
      Uint16 arg = stack.at(argpos--);
      arg = stack.at(arg);

      result_register = game.get_underworld().get_questflags()[arg];

      ua_trace("get_quest[%u] = %u\n",arg,result_register);
   }
   else

   if (funcname.compare("set_quest")==0)
   {
      Uint16 arg1 = stack.at(argpos--);
      arg1 = stack.at(arg1);

      Uint16 arg2 = stack.at(argpos);
      arg2 = stack.at(arg2);

      game.get_underworld().get_questflags()[arg2] = arg1;

      ua_trace("set_quest[%u] = %u\n",arg2,arg1);
   }
   else

   if (funcname.compare("sex")==0)
   {
      Uint16 arg1 = stack.at(argpos--);
      arg1 = stack.at(arg1);

      Uint16 arg2 = stack.at(argpos);
      arg2 = stack.at(arg2);

      // check player gender
      if (game.get_underworld().get_player().get_attr(ua_attr_gender)==0)
         arg1 = arg2;

      result_register = arg1;
   }
   else
      ua_trace("code_vm: unknown intrinsic %s()\n",funcname.c_str());

   return result_register;
}

Uint16 ua_conversation_screen::get_global(const char* the_globname)
{
   std::string globname(the_globname);
   Uint16 val = 0;

   ua_player& pl = game.get_underworld().get_player();

   // get npc object to talk to
   ua_object& npc_obj = game.get_underworld().get_current_level().
      get_mapobjects().get_object(objpos);

   if (globname.compare("play_name")==0)
      val = alloc_string(pl.get_name().c_str());
   else
   if (globname.compare("npc_xhome")==0)
      val = npc_obj.get_ext_object_info().npc_xhome;
   else
   if (globname.compare("npc_yhome")==0)
      val = npc_obj.get_ext_object_info().npc_yhome;
   else
   if (globname.compare("npc_attitude")==0)
      val = npc_obj.get_ext_object_info().npc_attitude;
   else
   if (globname.compare("npc_goal")==0)
      val = npc_obj.get_ext_object_info().npc_goal;
   else
   if (globname.compare("npc_gtarg")==0)
      val = npc_obj.get_ext_object_info().npc_gtarg;
   else
   if (globname.compare("npc_hp")==0)
      val = npc_obj.get_ext_object_info().npc_hp;
   else
   if (globname.compare("npc_hunger")==0)
      val = npc_obj.get_ext_object_info().npc_hunger;
   else
   if (globname.compare("npc_level")==0)
      val = npc_obj.get_ext_object_info().npc_level;
   else
   if (globname.compare("npc_talkedto")==0)
      val = npc_obj.get_ext_object_info().npc_talkedto;
   else
   if (globname.compare("npc_whoami")==0)
      val = npc_obj.get_ext_object_info().npc_whoami;
   else
      ua_trace("code_vm: get global: unknown global %s\n",globname.c_str());

   return val;
}

void ua_conversation_screen::set_global(const char* globname, Uint16 val)
{
}
/*
play_hunger
play_health
play_arms
play_power
play_hp
play_mana
play_level
new_player_exp
play_poison
play_drawn
play_sex
npc_health
npc_arms
npc_power
npc_name
dungeon_level
riddlecounter
game_time
game_days
game_mins
*/
