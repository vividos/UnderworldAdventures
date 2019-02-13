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
/// \file conv.cpp
/// \brief conversations
//
#include "common.hpp"
#include "conv.hpp"
#include "underworld.hpp"
#include "gamelogic.hpp"

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
// * compare
// * random
// x plural
// * contains
// x append
// x copy
// x find
// * length
// x val
// x say
// x respond
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
/// \todo move more function implementations to CodeVM
void Conversation::ImportedFunc(const char* functionName)
{
   std::string funcname(functionName);

   Uint16 argpos = m_stack.GetStackPointer();
   Uint16 argcount = m_stack.At(argpos);
   argpos--;

   if (funcname.compare("compare") == 0)
   {
      UaAssert(argcount == 2);

      // get arguments
      Uint16 arg1 = m_stack.At(argpos--);
      arg1 = m_stack.At(arg1);

      Uint16 arg2 = m_stack.At(argpos);
      arg2 = m_stack.At(arg2);

      // get strings
      std::string str1(m_localStrings[arg1]), str2(m_localStrings[arg2]);

      Base::String::Lowercase(str1);
      Base::String::Lowercase(str2);

      // check if first string contains second
      m_resultRegister = str1 == str2;
   }
   else if (funcname.compare("random") == 0)
   {
      UaAssert(argcount == 1);

      Uint16 arg = m_stack.At(argpos--);
      arg = m_stack.At(arg);

      // this code assumes that rand() can return RAND_MAX

      // rnum is in the range [0..1[
      double rnum = double(rand()) / double(RAND_MAX + 1);
      rnum *= arg; // now in range [0..arg[
      m_resultRegister = Uint16(rnum + 1.0); // now from [1..arg+1[
   }
   else if (funcname.compare("plural") == 0)
   {
      UaTrace("CodeVM: intrinsic plural() not implemented");
      UaAssert(false);
   }
   else if (funcname.compare("contains") == 0)
   {
      UaAssert(argcount == 2);

      // get arguments
      Uint16 arg1 = m_stack.At(argpos--);
      arg1 = m_stack.At(arg1);

      Uint16 arg2 = m_stack.At(argpos);
      arg2 = m_stack.At(arg2);

      // get strings
      std::string str1(m_localStrings[arg1]), str2(m_localStrings[arg2]);

      Base::String::Lowercase(str1);
      Base::String::Lowercase(str2);

      // check if first string contains second
      m_resultRegister = str1.find(str2) != std::string::npos;
   }
   else if (funcname.compare("append") == 0)
   {
      UaTrace("CodeVM: intrinsic append() not implemented");
      UaAssert(false);
   }
   else if (funcname.compare("copy") == 0)
   {
      UaTrace("CodeVM: intrinsic copy() not implemented");
      UaAssert(false);
   }
   else if (funcname.compare("find") == 0)
   {
      UaTrace("CodeVM: intrinsic find() not implemented");
      UaAssert(false);
   }
   else if (funcname.compare("length") == 0)
   {
      UaAssert(argcount == 1);

      // get argument
      Uint16 arg = m_stack.At(argpos--);
      arg = m_stack.At(arg);

      // return string length
      m_resultRegister = static_cast<Uint16>(m_localStrings[arg].size());
   }
   else if (funcname.compare("val") == 0)
   {
      UaTrace("CodeVM: intrinsic val() not implemented");
      UaAssert(false);
   }
   else if (funcname.compare("say") == 0)
   {
      UaTrace("CodeVM: intrinsic say() not implemented");
      UaAssert(false);
   }
   else if (funcname.compare("respond") == 0)
   {
      UaTrace("CodeVM: intrinsic respond() not implemented");
      UaAssert(false);
   }
   else if (funcname.compare("sex") == 0)
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
      val = info_ext.npc_xhome;
   else if (globname.compare("npc_yhome") == 0)
      val = info_ext.npc_yhome;
   else if (globname.compare("npc_attitude") == 0)
      val = info_ext.npc_attitude;
   else if (globname.compare("npc_goal") == 0)
      val = info_ext.npc_goal;
   else if (globname.compare("npc_gtarg") == 0)
      val = info_ext.npc_gtarg;
   else if (globname.compare("npc_hp") == 0)
      val = info_ext.npc_hp;
   else if (globname.compare("npc_hunger") == 0)
      val = info_ext.npc_hunger;
   else if (globname.compare("npc_level") == 0)
      val = info_ext.npc_level;
   else if (globname.compare("npc_talkedto") == 0)
      val = info_ext.npc_talkedto ? 1 : 0;
   else if (globname.compare("npc_whoami") == 0)
      val = info_ext.npc_whoami;
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
      info_ext.npc_xhome = val;
   else if (globname.compare("npc_yhome") == 0)
      info_ext.npc_yhome = val;
   else if (globname.compare("npc_attitude") == 0)
      info_ext.npc_attitude = val;
   else if (globname.compare("npc_goal") == 0)
      info_ext.npc_goal = val;
   else if (globname.compare("npc_gtarg") == 0)
      info_ext.npc_gtarg = val;
   else if (globname.compare("npc_hp") == 0)
      info_ext.npc_hp = val;
   else if (globname.compare("npc_hunger") == 0)
      info_ext.npc_hunger = val;
   else if (globname.compare("npc_level") == 0)
      info_ext.npc_level = val;
   else if (globname.compare("npc_talkedto") == 0)
      info_ext.npc_talkedto = val != 0;
   else if (globname.compare("npc_whoami") == 0)
      info_ext.npc_whoami = val;
   else
   {
      CodeVM::SetGlobal(globalName, val);
   }
}
