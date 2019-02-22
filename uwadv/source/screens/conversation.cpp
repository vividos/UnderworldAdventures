//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file conversation.cpp
/// \brief conversation screen implementation
//
#include "common.hpp"
#include "conversation.hpp"
#include "object.hpp"
#include "gamestrings.hpp"
#include "underworld.hpp"
#include "gamelogic.hpp"
#include "renderer.hpp"
#include "audio.hpp"
#include "debug.hpp"
#include "convloader.hpp"
#include <sstream>

/// time to fade in / out screen
const double ua_conversation_screen::fade_time = 0.5;

/// time to wait before conversation partner answers
const double ua_conversation_screen::answer_wait_time = 0.4;

ua_conversation_screen::ua_conversation_screen(IGame& game,
   Uint16 conv_objpos)
   :ua_screen(game), objpos(conv_objpos),
   code_vm(game.GetGameLogic())
{
}

void ua_conversation_screen::init()
{
   ua_screen::init();

   UaTrace("conversation screen started\n");
   UaTrace("talking to npc at objpos %u, ", objpos);

   // get npc object to talk to
   Underworld::ObjectPtr obj = game.GetGameLogic().
      GetCurrentLevel().GetObjectList().GetObject(objpos);

   Underworld::NpcObject& npcObj = obj->GetNpcObject();
   Underworld::NpcInfo& npcInfo = npcObj.GetNpcInfo();

   Uint16 convslot = npcInfo.npc_whoami;

   if (convslot == 0)
   {
      // generic conversation
      convslot = npcObj.GetObjectInfo().m_itemID - 64 + 0x0100;
   }

   UaTrace("conversation slot 0x%04x\n", convslot);

   // init renderer
   game.get_renderer().setup_camera2d();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   font_normal.load(game.get_settings(), ua_font_normal);

   // background image
   {
      Underworld::Player& pl = game.GetUnderworld().GetPlayer();

      const char* mainscreenname = "data/main.byt";

      // replace name when using uw_demo
      if (game.get_settings().GetBool(Base::settingUw1IsUwdemo))
         mainscreenname = "data/dmain.byt";

      IndexedImage& img = img_back.get_image();

      game.get_image_manager().load(img, mainscreenname, 0, 0, ua_img_byt);

      std::vector<IndexedImage> imgs_converse;
      game.get_image_manager().load_list(imgs_converse, "converse");

      img.fill_rect(43, 0, 190, 47, 1);

      // name panels
      img.paste_image(imgs_converse[0], 43, 0);
      img.paste_image(imgs_converse[0], 139, 0);

      // names
      std::string name1 = game.GetGameStrings().GetString(7, 16 + convslot);
      std::string name2 = pl.GetName();

      if (convslot == 0)
      {
         // generic conversation
         name1 = game.GetGameStrings().GetString(4, npcObj.GetObjectInfo().m_itemID);

         std::string::size_type pos;
         pos = name1.find('_');
         if (pos != std::string::npos) name1.erase(0, pos + 1);

         pos = name1.find('&');
         if (pos != std::string::npos) name1.erase(pos);
      }

      IndexedImage img_name;
      font_normal.create_string(img_name, name1.c_str(), 101);
      img.paste_image(img_name, 48, 2, true);

      font_normal.create_string(img_name, name2.c_str(), 101);
      img.paste_image(img_name, 144, 2, true);

      // barter areas
      img.paste_image(imgs_converse[1], 82, 9);
      img.paste_image(imgs_converse[1], 139, 9);

      // portrait frames/images
      img.paste_image(imgs_converse[2], 43, 9);
      img.paste_image(imgs_converse[2], 195, 9);

      // portrait images

      if (convslot <= 28)
      {
         // portrait is in "charhead.gr"
         std::vector<IndexedImage> imgs_charhead;
         game.get_image_manager().load_list(imgs_charhead, "charhead");

         img.paste_image(imgs_charhead[convslot - 1], 45, 11);
      }
      else
      {
         // portrait is in "genheads.gr"
         std::vector<IndexedImage> imgs_genheads;
         game.get_image_manager().load_list(imgs_genheads, "genhead");

         unsigned int gen_head = npcObj.GetObjectInfo().m_itemID - 64;
         if (gen_head > 59) gen_head = 0;

         img.paste_image(imgs_genheads[gen_head], 45, 11);
      }

      std::vector<IndexedImage> imgs_playerheads;
      game.get_image_manager().load_list(imgs_playerheads, "heads");

      unsigned int appearance = pl.GetAttribute(Underworld::attrAppearance) +
         (pl.GetAttribute(Underworld::attrGender) == 0 ? 0 : 5);

      img.paste_image(imgs_playerheads[appearance], 197, 11);


      // scroll frames up/down
      img.paste_image(imgs_converse[3], 42, 48);
      img.paste_image(imgs_converse[4], 42, 126);

      // fill scroll area
      img.fill_rect(56, 51, 164, 81, 42);

      img_back.init(game, 0, 0);
      img_back.update();
      register_window(&img_back);
   }

   // adjust scroll width for uw_demo
   unsigned int scrollwidth = 289;

   if (game.get_settings().GetBool(Base::settingUw1IsUwdemo))
      scrollwidth = 218;

   // init text scrolls
   scroll_conv.init(game, 58, 51, 161, 81, 42);
   scroll_conv.set_color_code(ua_cc_black);
   register_window(&scroll_conv);

   scroll_menu.init(game, 15, 169, scrollwidth, 30, 42);
   scroll_menu.set_color_code(ua_cc_black);
   register_window(&scroll_menu);

   // init mouse cursor
   mousecursor.init(game, 0);
   mousecursor.show(true);
   register_window(&mousecursor);


   // init conversation code virtual machine
   {
      std::string cnv_name(game.get_settings().GetString(Base::settingUnderworldPath));
      cnv_name.append("data/cnv.ark");

      // load code into vm
      Import::LoadConvCode(code_vm, cnv_name.c_str(), static_cast<Uint16>(convslot));

      // get local strings
      // note: convslot is used to load strings, not the strblock value set in
      // conv header
      std::vector<std::string> localstrings = game.GetGameStrings().GetStringBlock(code_vm.GetStringBlock());

      // TODO code_vm.Init(this,game.GetUnderworld().get_conv_globals());

      // notify debugger of start of code debugger
      // \todo only works with ua_conversation_debug class
      //game.get_debugger().start_code_debugger(&code_vm);

      unsigned int level = game.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrMapLevel);
      code_vm.Init(level, objpos, this, localstrings);
   }

   state = ua_state_fadein;
   fader.init(true, game.get_tickrate(), fade_time);

   wait_count = 0;

   // start audio track "maps & legends" for conversations
   game.get_audio_manager().StartMusicTrack(Audio::musicUw1_MapsAndLegends, false);
}

void ua_conversation_screen::destroy()
{
   // \todo notify debugger of end of code debugger
   //game.get_debugger().end_code_debugger(&code_vm);

   // write back conv. globals
   code_vm.Done();

   UaTrace("conversation screen ended\n\n");
}

void ua_conversation_screen::draw()
{
   // calculate quad brightness
   Uint8 light = 255;
   if (state == ua_state_fadein || state == ua_state_fadeout)
      light = fader.get_fade_value();

   glColor3ub(light, light, light);

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
         Uint16 answer_id = code_vm.AllocString(textedit.get_text());
         code_vm.SetResultRegister(answer_id);

         // continue processing code
         state = ua_state_running;

         textedit.done();

         // clear menu scroll
         scroll_menu.clear_scroll();

         wait_count = unsigned(answer_wait_time * game.get_tickrate());
      }
   }

   switch (event.type)
   {
   case SDL_KEYDOWN:
      if (state == ua_state_wait_menu &&
         event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)
      {
         unsigned int selection = event.key.keysym.sym - (Uint32)SDLK_1;

         if (selection < answer_values.size())
         {
            code_vm.SetResultRegister(answer_values[selection]);

            // print answer
            std::string answer(code_vm.GetLocalString(answer_string_ids[selection]));
            code_vm.ReplacePlaceholder(answer);

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
            fader.init(false, game.get_tickrate(), fade_time);

            wait_count = 0;

            // fade out music
            game.get_audio_manager().FadeoutMusic(fade_time);
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
   if (wait_count > 0)
   {
      wait_count--;
      return; // do nothing
   }

   // execute code until finished, waiting for an action or have [MORE]
   // lines to scroll
   while (state == ua_state_running &&
      !scroll_menu.is_waiting_more() && !scroll_conv.is_waiting_more())
   {
      if (!code_vm.Step())
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

void ua_conversation_screen::Say(Uint16 index)
{
   std::string str(code_vm.GetLocalString(index));
   code_vm.ReplacePlaceholder(str);

   scroll_conv.print(str.c_str());
}

Uint16 ua_conversation_screen::BablMenu(const std::vector<Uint16>& answerStringIds)
{
   // get pointer to list
   answer_values.clear();
   answer_string_ids.clear();
   scroll_menu.clear_scroll();

   unsigned int ask_count = 0;
   for (Uint16 arg : answerStringIds)
   {
      ask_count++;

      // format menu entry string
      std::ostringstream buffer;
      buffer << ask_count << ". " << code_vm.GetLocalString(arg);

      std::string menuentry(buffer.str());
      code_vm.ReplacePlaceholder(menuentry);

      // print menu entry
      scroll_menu.print(menuentry.c_str());

      // remember answer value/string id
      answer_values.push_back(ask_count);
      answer_string_ids.push_back(arg);
   }

   state = ua_state_wait_menu;

   return 0xffff;
}

// *=implemented, x=assert
// * babl_menu
// * babl_fmenu
// * print
// * babl_ask
//   compare
//   random
//   plural
//   contains
//   append
//   copy
//   find
//   length
//   val
//   say
//   respond
// * get_quest
// * set_quest
// * sex
//   show_inv
//   give_to_npc
//   give_ptr_npc
//   take_from_npc
//   take_id_from_npc
//   identify_inv
//   do_offer
//   do_demand
//   do_inv_create
//   do_inv_delete
//   check_inv_quality
//   set_inv_quality
//   count_inv
//   setup_to_barter
//   end_barter
//   do_judgement
//   do_decline
//   pause
//   set_likes_dislikes
//   gronk_door
//   set_race_attitude
//   place_object
//   take_from_npc_inv
//   add_to_npc_inv
//   remove_talker
//   set_attitude
//   x_skills
//   x_traps
//   x_obj_pos
//   x_obj_stuff
//   find_inv
//   find_barter
//   find_barter_total
Uint16 ua_conversation_screen::ExternalFunc(const char* the_funcname,
   Conv::ConvStack& stack)
{
   std::string funcname(the_funcname);

   Uint16 argpos = stack.GetStackPointer();
   Uint16 argcount = stack.At(argpos); argcount;
   argpos--;

   Uint16 result_register = 0xffff;

   if (funcname.compare("babl_menu") == 0)
   {
#if 0
      // get pointer to list
      argpos = stack.At(argpos);

      answer_values.clear();
      answer_string_ids.clear();
      scroll_menu.clear_scroll();

      unsigned int ask_count = 0;
      Uint16 arg = stack.At(argpos++);

      while (arg != 0)
      {
         ask_count++;

         // format menu entry string
         std::ostringstream buffer;
         buffer << ask_count << ". " << code_vm.GetLocalString(arg);

         std::string menuentry(buffer.str());
         code_vm.ReplacePlaceholder(menuentry);

         // print menu entry
         scroll_menu.print(menuentry.c_str());

         // remember answer value/string id
         answer_values.push_back(ask_count);
         answer_string_ids.push_back(arg);

         // get next argument
         arg = stack.At(argpos++);
      }

      state = ua_state_wait_menu;
#endif
   }
   else if (funcname.compare("babl_fmenu") == 0)
   {
      // get pointer to the two lists
      Uint16 argpos1 = stack.At(argpos--);
      Uint16 argpos2 = stack.At(argpos);

      answer_values.clear();
      answer_string_ids.clear();
      scroll_menu.clear_scroll();

      unsigned int ask_count = 0;
      Uint16 arg1 = stack.At(argpos1++);
      Uint16 arg2 = stack.At(argpos2++);

      while (arg1 != 0)
      {
         if (arg2 != 0)
         {
            ask_count++;

            // format menu entry string
            std::ostringstream buffer;
            buffer << ask_count << ". " << code_vm.GetLocalString(arg1);

            std::string menuentry(buffer.str());
            code_vm.ReplacePlaceholder(menuentry);

            // print menu entry
            scroll_menu.print(menuentry.c_str());

            // remember answer value/string id
            answer_values.push_back(arg1);
            answer_string_ids.push_back(arg1);
         }

         // get next arguments
         arg1 = stack.At(argpos1++);
         arg2 = stack.At(argpos2++);
      }

      state = ua_state_wait_menu;
   }
   else if (funcname.compare("print") == 0)
   {
      Uint16 arg = stack.At(argpos++);
      arg = stack.At(arg);

      std::string printtext(code_vm.GetLocalString(arg));
      code_vm.ReplacePlaceholder(printtext);

      scroll_conv.print(printtext.c_str());
   }
   else if (funcname.compare("babl_ask") == 0)
   {
      // start user input mode
      scroll_menu.clear_scroll();

      // init text edit
      textedit.init(game, scroll_menu.get_xpos() + 1, scroll_menu.get_ypos() + 1,
         scroll_menu.get_width(), 42, 1, 46, ">");

      state = ua_state_text_input;
   }
   else if (funcname.compare("get_quest") == 0)
   {
      Uint16 arg = stack.At(argpos--);
      arg = stack.At(arg);

      result_register = game.GetUnderworld().GetPlayer().GetQuestFlags().GetFlag(arg);

      UaTrace("get_quest[%u] = %u\n", arg, result_register);
   }
   else if (funcname.compare("set_quest") == 0)
   {
      Uint16 arg1 = stack.At(argpos--);
      arg1 = stack.At(arg1);

      Uint16 arg2 = stack.At(argpos);
      arg2 = stack.At(arg2);

      game.GetUnderworld().GetPlayer().GetQuestFlags().SetFlag(arg2, arg1);

      UaTrace("set_quest[%u] = %u\n", arg2, arg1);
   }
   else
      UaTrace("code_vm: unknown intrinsic %s()\n", funcname.c_str());

   return result_register;
}
