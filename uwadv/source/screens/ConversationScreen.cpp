//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2021 Underworld Adventures Team
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
/// \file ConversationScreen.cpp
/// \brief conversation screen implementation
//
#include "pch.hpp"
#include "ConversationScreen.hpp"
#include "Object.hpp"
#include "GameStrings.hpp"
#include "Underworld.hpp"
#include "GameLogic.hpp"
#include "Renderer.hpp"
#include "Audio.hpp"
#include "uwadv/DebugServer.hpp"
#include "ConvLoader.hpp"
#include "ImageManager.hpp"
#include <sstream>

/// time to fade in / out screen
const double ConversationScreen::s_fadeTime = 0.5;

/// time to wait before conversation partner answers
const double ConversationScreen::s_answerWaitTime = 0.4;

ConversationScreen::ConversationScreen(IGame& game,
   Uint16 convObjectPos)
   :Screen(game),
   m_convObjectPos(convObjectPos),
   m_codeVM(game.GetGameLogic()),
   m_convDebugger(game.GetGameLogic(), m_codeVM)
{
}

void ConversationScreen::Init()
{
   Screen::Init();

   UaTrace("conversation screen started\n");
   UaTrace("talking to npc at object pos %u, ", m_convObjectPos);

   // get npc object to talk to
   Underworld::ObjectPtr obj = m_game.GetGameLogic().
      GetCurrentLevel().GetObjectList().GetObject(m_convObjectPos);

   Underworld::NpcObject& npcObj = obj->GetNpcObject();
   Underworld::NpcInfo& npcInfo = npcObj.GetNpcInfo();

   Uint16 convslot = npcInfo.m_npc_whoami;

   if (convslot == 0)
   {
      // generic conversation
      convslot = npcObj.GetObjectInfo().m_itemID - 64 + 0x0100;
   }

   UaTrace("conversation slot 0x%04x\n", convslot);

   // init renderer
   m_game.GetRenderer().SetupForUserInterface();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   m_normalFont.Load(m_game.GetResourceManager(), fontNormal);

   // background image
   {
      IndexedImage& image = m_backgroundImage.GetImage();

      bool isUw2 = m_game.GetSettings().GetGameType() == Base::gameUw2;

      if (!isUw2)
      {
         const char* mainscreenname = "data/main.byt";

         // replace name when using uw_demo
         if (m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
            mainscreenname = "data/dmain.byt";

         m_game.GetImageManager().Load(image, mainscreenname, 0, 0, imageByt);
      }
      else
      {
         m_game.GetImageManager().LoadFromArk(image, "data/byt.ark", 2, 0);
      }

      std::vector<IndexedImage> converseImages;
      m_game.GetImageManager().LoadList(converseImages, "converse");

      image.FillRect(43, 0, 190, 47, 1);

      // name panels
      image.PasteImage(converseImages[0], 43, 0);
      image.PasteImage(converseImages[0], 139, 0);

      // names
      std::string name1 = m_game.GetGameStrings().GetString(7, 16 + convslot);

      Underworld::Player& player = m_game.GetUnderworld().GetPlayer();
      std::string name2 = player.GetName();

      if (convslot == 0)
      {
         // generic conversation
         name1 = m_game.GetGameStrings().GetString(4, npcObj.GetObjectInfo().m_itemID);

         std::string::size_type pos;
         pos = name1.find('_');
         if (pos != std::string::npos) name1.erase(0, pos + 1);

         pos = name1.find('&');
         if (pos != std::string::npos) name1.erase(pos);
      }

      IndexedImage img_name;
      m_normalFont.CreateString(img_name, name1, 101);
      image.PasteImage(img_name, 48, 2, true);

      m_normalFont.CreateString(img_name, name2, 101);
      image.PasteImage(img_name, 144, 2, true);

      // barter areas
      image.PasteImage(converseImages[1], 82, 9);
      image.PasteImage(converseImages[1], 139, 9);

      // portrait frames/images
      image.PasteImage(converseImages[2], 43, 9);
      image.PasteImage(converseImages[2], 195, 9);

      // portrait images

      if (convslot <= 28)
      {
         // portrait is in "charhead.gr"
         std::vector<IndexedImage> imgs_charhead;
         m_game.GetImageManager().LoadList(imgs_charhead, "charhead");

         image.PasteImage(imgs_charhead[convslot - 1], 45, 11);
      }
      else
      {
         // portrait is in "genheads.gr"
         std::vector<IndexedImage> imgs_genheads;
         m_game.GetImageManager().LoadList(imgs_genheads, "genhead");

         unsigned int gen_head = npcObj.GetObjectInfo().m_itemID - 64;
         if (gen_head > 59) gen_head = 0;

         image.PasteImage(imgs_genheads[gen_head], 45, 11);
      }

      std::vector<IndexedImage> imgs_playerheads;
      m_game.GetImageManager().LoadList(imgs_playerheads, "heads");

      unsigned int appearance = player.GetAttribute(Underworld::attrAppearance) +
         (player.GetAttribute(Underworld::attrGender) == 0 ? 0 : 5);

      image.PasteImage(imgs_playerheads[appearance], 197, 11);


      // scroll frames up/down
      image.PasteImage(converseImages[3], 42, 48);
      image.PasteImage(converseImages[4], 42, 126);

      // fill scroll area
      image.FillRect(56, 51, 164, 81, 42);

      m_backgroundImage.Init(m_game, 0, 0);
      m_backgroundImage.Update();
      RegisterWindow(&m_backgroundImage);
   }

   // adjust scroll width for uw_demo
   unsigned int scrollWidth = 289;

   if (m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
      scrollWidth = 218;

   // init text scrolls
   m_conversationScroll.Init(m_game, 58, 51, 161, 81, 42);
   m_conversationScroll.SetColorCode(colorCodeBlack);
   RegisterWindow(&m_conversationScroll);

   m_menuScroll.Init(m_game, 15, 169, scrollWidth, 30, 42);
   m_menuScroll.SetColorCode(colorCodeBlack);
   RegisterWindow(&m_menuScroll);

   // init mouse cursor
   m_mouseCursor.Init(m_game, 0);
   m_mouseCursor.Show(true);
   RegisterWindow(&m_mouseCursor);


   // init conversation code virtual machine
   {
      // load code into vm
      Import::LoadConvCode(m_codeVM, m_game.GetSettings(), m_game.GetResourceManager(),
         "data/cnv.ark", static_cast<Uint16>(convslot));

      // get local strings
      // note: convslot is used to load strings, not the strblock value set in
      // conv header
      std::vector<std::string> localStrings = m_game.GetGameStrings().GetStringBlock(m_codeVM.GetStringBlock());

      size_t level = m_game.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrMapLevel);
      m_codeVM.Init(level, m_convObjectPos, this, localStrings);

      m_convDebugger.Init(m_game.GetDebugger());
   }

   m_state = convScreenStateFadeIn;
   m_fader.Init(true, m_game.GetTickRate(), s_fadeTime);

   m_waitCount = 0;

   // start audio track "maps & legends" for conversations
   m_game.GetAudioManager().StartMusicTrack(Audio::musicUw1_MapsAndLegends, false);
}

void ConversationScreen::Destroy()
{
   m_convDebugger.Done();

   // write back conv. globals
   m_codeVM.Done();

   UaTrace("conversation screen ended\n\n");
}

void ConversationScreen::Draw()
{
   // calculate quad brightness
   Uint8 light = 255;
   if (m_state == convScreenStateFadeIn || m_state == convScreenStateFadeOut)
      light = m_fader.GetFadeValue();

   glColor3ub(light, light, light);

   // render ui elements
   Screen::Draw();

   // render text edit window when available
   if (m_state == convScreenStateTextInput)
      m_textEdit.Draw();
}

bool ConversationScreen::ProcessEvent(SDL_Event& event)
{
   if (Screen::ProcessEvent(event))
      return true;

   // process text input-specific events
   if (m_state == convScreenStateTextInput)
   {
      // let text edit window process event
      if (m_textEdit.ProcessEvent(event))
         return true;

      // user event?
      if (event.type == SDL_USEREVENT &&
         (event.user.code == gameEventTexteditFinished ||
            event.user.code == gameEventTexteditAborted))
      {
         // get string
         Uint16 answer_id = m_codeVM.AllocString(m_textEdit.GetText());
         m_codeVM.SetResultRegister(answer_id);

         // continue processing code
         m_state = convScreenStateRunning;

         m_textEdit.Done();

         // clear menu scroll
         m_menuScroll.ClearScroll();

         m_waitCount = unsigned(s_answerWaitTime * m_game.GetTickRate());
      }
   }

   switch (event.type)
   {
   case SDL_KEYDOWN:
      if (m_state == convScreenStateWaitMenu &&
         event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9)
      {
         unsigned int selection = event.key.keysym.sym - (Uint32)SDLK_1;

         if (selection < m_answerValues.size())
         {
            m_codeVM.SetResultRegister(m_answerValues[selection]);

            // print answer
            std::string answer(m_codeVM.GetLocalString(m_answerStringIds[selection]));
            m_codeVM.ReplacePlaceholder(answer);

            m_conversationScroll.SetColorCode(colorCodeOrange);
            m_conversationScroll.Print(answer.c_str());
            m_conversationScroll.SetColorCode(colorCodeBlack);

            // continue processing
            m_state = convScreenStateRunning;

            // clear menu scroll
            m_menuScroll.ClearScroll();

            m_waitCount = unsigned(s_answerWaitTime * m_game.GetTickRate());
         }
      }
      else
         if (m_state == convScreenStateWaitEnd)
         {
            // start fade out
            m_state = convScreenStateFadeOut;
            m_fader.Init(false, m_game.GetTickRate(), s_fadeTime);

            m_waitCount = 0;

            // fade out music
            m_game.GetAudioManager().FadeoutMusic(static_cast<int>(s_fadeTime * 1000));
         }
      break;

   default:
      break;
   }

   return false;
}

void ConversationScreen::Tick()
{
   // still waiting?
   if (m_waitCount > 0)
   {
      m_waitCount--;
      return; // do nothing
   }

   // execute code until finished, waiting for an action or have [MORE]
   // lines to scroll
   while (m_state == convScreenStateRunning &&
      !m_menuScroll.IsWaitingMore() &&
      !m_conversationScroll.IsWaitingMore() &&
      m_convDebugger.ContinueSteppingCode())
   {
      m_convDebugger.EvaluateDebuggerState();

      if (!m_codeVM.Step())
         m_state = convScreenStateWaitEnd;
   }

   // finished conv. code?
   if (m_state == convScreenStateWaitEnd)
   {
      // clear menu scroll
      m_menuScroll.ClearScroll();
      return;
   }

   // check for fading in/out
   if ((m_state == convScreenStateFadeIn || m_state == convScreenStateFadeOut) && m_fader.Tick())
   {
      if (m_state == convScreenStateFadeIn)
         m_state = convScreenStateRunning;
      else
      {
         // leave screen
         m_game.RemoveScreen();
      }
   }
}

void ConversationScreen::Say(Uint16 index)
{
   std::string str{ m_codeVM.GetLocalString(index) };
   m_codeVM.ReplacePlaceholder(str);

   m_conversationScroll.Print(str.c_str());
}

Uint16 ConversationScreen::BablMenu(const std::vector<Uint16>& answerStringIds)
{
   // get pointer to list
   m_answerValues.clear();
   m_answerStringIds.clear();
   m_menuScroll.ClearScroll();

   unsigned int ask_count = 0;
   for (Uint16 arg : answerStringIds)
   {
      ask_count++;

      // format menu entry string
      std::ostringstream buffer;
      buffer << ask_count << ". " << m_codeVM.GetLocalString(arg);

      std::string menuentry(buffer.str());
      m_codeVM.ReplacePlaceholder(menuentry);

      // print menu entry
      m_menuScroll.Print(menuentry.c_str());

      // remember answer value/string id
      m_answerValues.push_back(ask_count);
      m_answerStringIds.push_back(arg);
   }

   m_state = convScreenStateWaitMenu;

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
Uint16 ConversationScreen::ExternalFunc(const char* the_funcname,
   Conv::ConvStack& stack)
{
   std::string funcname(the_funcname);

   Uint16 argpos = stack.GetStackPointer();
   Uint16 argcount = stack.At(argpos);
   UNUSED(argcount);
   argpos--;

   Uint16 result_register = 0xffff;

   if (funcname.compare("babl_menu") == 0)
   {
#if 0
      // get pointer to list
      argpos = stack.At(argpos);

      m_answerValues.clear();
      m_answerStringIds.clear();
      m_menuScroll.ClearScroll();

      unsigned int ask_count = 0;
      Uint16 arg = stack.At(argpos++);

      while (arg != 0)
      {
         ask_count++;

         // format menu entry string
         std::ostringstream buffer;
         buffer << ask_count << ". " << m_codeVM.GetLocalString(arg);

         std::string menuentry(buffer.str());
         m_codeVM.ReplacePlaceholder(menuentry);

         // print menu entry
         m_menuScroll.print(menuentry.c_str());

         // remember answer value/string id
         m_answerValues.push_back(ask_count);
         m_answerStringIds.push_back(arg);

         // get next argument
         arg = stack.At(argpos++);
      }

      m_state = convScreenStateWaitMenu;
#endif
   }
   else if (funcname.compare("babl_fmenu") == 0)
   {
      // get pointer to the two lists
      Uint16 argpos1 = stack.At(argpos--);
      Uint16 argpos2 = stack.At(argpos);

      m_answerValues.clear();
      m_answerStringIds.clear();
      m_menuScroll.ClearScroll();

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
            buffer << ask_count << ". " << m_codeVM.GetLocalString(arg1);

            std::string menuentry(buffer.str());
            m_codeVM.ReplacePlaceholder(menuentry);

            // print menu entry
            m_menuScroll.Print(menuentry.c_str());

            // remember answer value/string id
            m_answerValues.push_back(arg1);
            m_answerStringIds.push_back(arg1);
         }

         // get next arguments
         arg1 = stack.At(argpos1++);
         arg2 = stack.At(argpos2++);
      }

      m_state = convScreenStateWaitMenu;
   }
   else if (funcname.compare("print") == 0)
   {
      Uint16 arg = stack.At(argpos++);
      arg = stack.At(arg);

      std::string printtext(m_codeVM.GetLocalString(arg));
      m_codeVM.ReplacePlaceholder(printtext);

      m_conversationScroll.Print(printtext.c_str());
   }
   else if (funcname.compare("babl_ask") == 0)
   {
      // start user input mode
      m_menuScroll.ClearScroll();

      // init text edit
      m_textEdit.Init(m_game, m_menuScroll.GetXPos() + 1, m_menuScroll.GetYPos() + 1,
         m_menuScroll.GetWidth(), 42, 1, 46, ">");

      m_state = convScreenStateTextInput;
   }
   else if (funcname.compare("get_quest") == 0)
   {
      Uint16 arg = stack.At(argpos--);
      arg = stack.At(arg);

      result_register = m_game.GetUnderworld().GetPlayer().GetQuestFlags().GetFlag(arg);

      UaTrace("get_quest[%u] = %u\n", arg, result_register);
   }
   else if (funcname.compare("set_quest") == 0)
   {
      Uint16 arg1 = stack.At(argpos--);
      arg1 = stack.At(arg1);

      Uint16 arg2 = stack.At(argpos);
      arg2 = stack.At(arg2);

      m_game.GetUnderworld().GetPlayer().GetQuestFlags().SetFlag(arg2, arg1);

      UaTrace("set_quest[%u] = %u\n", arg2, arg1);
   }
   else
      UaTrace("codevm: unknown intrinsic %s()\n", funcname.c_str());

   return result_register;
}
