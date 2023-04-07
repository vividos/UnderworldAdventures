//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2005,2019,2021 Underworld Adventures Team
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
#include "pch.hpp"
#include "Conversation.hpp"
#include "Underworld.hpp"
#include "GameLogic.hpp"

using Conv::Conversation;

void Conversation::Init(size_t conversationLevel,
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

// The following functions are implemented:
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
   std::string function{ functionName };

   Uint16 argpos = m_stack.GetStackPointer();
   Uint16 argcount = m_stack.At(argpos);
   argpos--;

   if (function == "sex")
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
   std::string globname{ globalName };
   Uint16 val = 0;

   const Underworld::Player& player = m_gameLogic.GetUnderworld().GetPlayer();

   // get npc object to talk to
   const Underworld::ObjectPtr npcObject = m_gameLogic.GetUnderworld().GetLevelList().
      GetLevel(m_conversationLevel).GetObjectList().GetObject(m_conversationObjectPos);

   UaAssert(npcObject->IsNpcObject());

   const Underworld::NpcInfo& npcInfo = npcObject->GetNpcObject().GetNpcInfo();

   if (globname == "play_name") val = AllocString(player.GetName());
   else if (globname == "npc_xhome") val = npcInfo.m_npc_xhome;
   else if (globname == "npc_yhome") val = npcInfo.m_npc_yhome;
   else if (globname == "npc_attitude") val = npcInfo.m_npc_attitude;
   else if (globname == "npc_goal") val = npcInfo.m_npc_goal;
   else if (globname == "npc_gtarg") val = npcInfo.m_npc_gtarg;
   else if (globname == "npc_hp") val = npcInfo.m_npc_hp;
   else if (globname == "npc_hunger") val = npcInfo.m_npc_hunger;
   else if (globname == "npc_level") val = npcInfo.m_npc_level;
   else if (globname == "npc_talkedto") val = npcInfo.m_npc_talkedto ? 1 : 0;
   else if (globname == "npc_whoami") val = npcInfo.m_npc_whoami;
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
   Underworld::ObjectPtr npcObject = m_gameLogic.GetUnderworld().GetLevelList().
      GetLevel(m_conversationLevel).GetObjectList().GetObject(m_conversationObjectPos);

   UaAssert(npcObject->IsNpcObject());

   Underworld::NpcInfo& npcInfo = npcObject->GetNpcObject().GetNpcInfo();

   if (globname == "play_name")
   {
      UaAssert(val < m_localStrings.size());
      player.SetName(m_localStrings[val]);
   }
   else if (globname == "npc_xhome") npcInfo.m_npc_xhome = val;
   else if (globname == "npc_yhome") npcInfo.m_npc_yhome = val;
   else if (globname == "npc_attitude") npcInfo.m_npc_attitude = val;
   else if (globname == "npc_goal") npcInfo.m_npc_goal = val;
   else if (globname == "npc_gtarg") npcInfo.m_npc_gtarg = val;
   else if (globname == "npc_hp") npcInfo.m_npc_hp = val;
   else if (globname == "npc_hunger") npcInfo.m_npc_hunger = val;
   else if (globname == "npc_level") npcInfo.m_npc_level = val;
   else if (globname == "npc_talkedto") npcInfo.m_npc_talkedto = val != 0;
   else if (globname == "npc_whoami") npcInfo.m_npc_whoami = val;
   else
   {
      CodeVM::SetGlobal(globalName, val);
   }
}
