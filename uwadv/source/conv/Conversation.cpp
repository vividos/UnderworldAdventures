//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2005,2019 Underworld Adventures Team
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
/// \file Conversation.cpp
/// \brief conversations
//
#include "common.hpp"
#include "Conversation.hpp"
#include "Underworld.hpp"
#include "GameLogic.hpp"

using Conv::Conversation;

void Conversation::Init(unsigned int conversationLevel,
   Uint16 conversationObjectPos,
   Conv::ICodeCallback* codeCallback,
   std::vector<std::string>& localStrings)
{
   m_conversationLevel = conversationLevel;
   m_conversationObjectPos = conversationObjectPos;

   m_localStrings.insert(m_localStrings.begin(), localStrings.begin(), localStrings.end());

   CodeVM::Init(codeCallback, m_gameLogic.GetUnderworld().GetPlayer().GetConvGlobals());
}

void Conversation::Done()
{
   // write back conv. globals
   CodeVM::Done(m_gameLogic.GetUnderworld().GetPlayer().GetConvGlobals());
}

// *=implemented, x=assert
//   babl_menu
//   babl_fmenu
//   print
//   babl_ask
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
//   get_quest
//   set_quest
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
void Conversation::ImportedFunc(const char* functionName)
{
   std::string funcname(functionName);

   Uint16 argpos = m_stack.GetStackPointer();
   Uint16 argcount = m_stack.At(argpos);
   argpos--;

   if (funcname.compare("sex") == 0)
   {
      UaAssert(argcount == 2);

      Uint16 arg1 = m_stack.At(argpos--);
      arg1 = m_stack.At(arg1);

      Uint16 arg2 = m_stack.At(argpos);
      arg2 = m_stack.At(arg2);

      // check player gender
      if (m_gameLogic.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrGender) == 0)
         arg1 = arg2;

      m_resultRegister = arg1;
   }
   else
   {
      CodeVM::ImportedFunc(functionName);
   }
}

Uint16 Conversation::GetGlobal(const char* globalName)
{
   std::string globname(globalName);
   Uint16 val = 0;

   Underworld::Player& player = m_gameLogic.GetUnderworld().GetPlayer();

   // get npc object to talk to
   Underworld::ObjectPtr npc_obj = m_gameLogic.GetUnderworld().GetLevelList().
      GetLevel(m_conversationLevel).GetObjectList().GetObject(m_conversationObjectPos);

   UaAssert(npc_obj->IsNpcObject());

   Underworld::NpcInfo& info_ext = npc_obj->GetNpcObject().GetNpcInfo();

   if (globname.compare("play_name") == 0)
      val = AllocString(player.GetName().c_str());
   else if (globname.compare("npc_xhome") == 0)
      val = info_ext.m_npc_xhome;
   else if (globname.compare("npc_yhome") == 0)
      val = info_ext.m_npc_yhome;
   else if (globname.compare("npc_attitude") == 0)
      val = info_ext.m_npc_attitude;
   else if (globname.compare("npc_goal") == 0)
      val = info_ext.m_npc_goal;
   else if (globname.compare("npc_gtarg") == 0)
      val = info_ext.m_npc_gtarg;
   else if (globname.compare("npc_hp") == 0)
      val = info_ext.m_npc_hp;
   else if (globname.compare("npc_hunger") == 0)
      val = info_ext.m_npc_hunger;
   else if (globname.compare("npc_level") == 0)
      val = info_ext.m_npc_level;
   else if (globname.compare("npc_talkedto") == 0)
      val = info_ext.m_npc_talkedto ? 1 : 0;
   else if (globname.compare("npc_whoami") == 0)
      val = info_ext.m_npc_whoami;
   else
   {
      return CodeVM::GetGlobal(globalName);
   }

   UaTrace("CodeVM: GetGlobal %s returned %04x\n", globname.c_str(), val);

   return val;
}

/// globals that still need implementation:
/// play_hunger
/// play_health
/// play_arms
/// play_power
/// play_hp
/// play_mana
/// play_level
/// new_player_exp
/// play_poison
/// play_drawn
/// play_sex
/// npc_health
/// npc_arms
/// npc_power
/// npc_name
/// dungeon_level
/// riddlecounter
/// game_time
/// game_days
/// game_mins
void Conversation::SetGlobal(const char* globalName, Uint16 val)
{
   UaTrace("CodeVM: SetGlobal: %s = %04x\n", globalName, val);

   std::string globname(globalName);

   Underworld::Player& player = m_gameLogic.GetUnderworld().GetPlayer();

   // get npc object to talk to
   Underworld::ObjectPtr npc_obj = m_gameLogic.GetUnderworld().GetLevelList().
      GetLevel(m_conversationLevel).GetObjectList().GetObject(m_conversationObjectPos);

   UaAssert(npc_obj->IsNpcObject());

   Underworld::NpcInfo& info_ext = npc_obj->GetNpcObject().GetNpcInfo();

   if (globname.compare("play_name") == 0)
   {
      UaAssert(val < m_localStrings.size());
      player.SetName(m_localStrings[val]);
   }
   else if (globname.compare("npc_xhome") == 0)
      info_ext.m_npc_xhome = val;
   else if (globname.compare("npc_yhome") == 0)
      info_ext.m_npc_yhome = val;
   else if (globname.compare("npc_attitude") == 0)
      info_ext.m_npc_attitude = val;
   else if (globname.compare("npc_goal") == 0)
      info_ext.m_npc_goal = val;
   else if (globname.compare("npc_gtarg") == 0)
      info_ext.m_npc_gtarg = val;
   else if (globname.compare("npc_hp") == 0)
      info_ext.m_npc_hp = val;
   else if (globname.compare("npc_hunger") == 0)
      info_ext.m_npc_hunger = val;
   else if (globname.compare("npc_level") == 0)
      info_ext.m_npc_level = val;
   else if (globname.compare("npc_talkedto") == 0)
      info_ext.m_npc_talkedto = val != 0;
   else if (globname.compare("npc_whoami") == 0)
      info_ext.m_npc_whoami = val;
   else
   {
      CodeVM::SetGlobal(globalName, val);
   }
}
