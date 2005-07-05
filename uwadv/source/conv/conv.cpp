/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2005 Underworld Adventures Team

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
/*! \file conv.cpp

   \brief conversations

*/

// needed includes
#include "common.hpp"
#include "conv.hpp"
#include "underworld.hpp"


// ua_basic_conversation methods

void ua_basic_conversation::init(unsigned int the_conv_level,
   Uint16 the_conv_objpos, ua_basic_game_interface& the_game,
   ua_conv_code_callback* code_callback,
   std::vector<std::string>& the_localstrings)
{
   conv_level = the_conv_level;
   conv_objpos = the_conv_objpos,

   game = &the_game;

   localstrings.insert(localstrings.begin(), the_localstrings.begin(), the_localstrings.end());

   ua_conv_code_vm::init(code_callback, game->get_underworld().get_conv_globals());
}

void ua_basic_conversation::done(ua_basic_game_interface& the_game)
{
   // write back conv. globals
   ua_conv_code_vm::done(the_game.get_underworld().get_conv_globals());
}

std::string ua_basic_conversation::get_local_string(Uint16 str_nr)
{
   ua_assert(str_nr < localstrings.size());
   return localstrings[str_nr];
}

Uint16 ua_basic_conversation::alloc_string(const char* the_str)
{
   std::string str(the_str);
   Uint16 pos = localstrings.size();
   localstrings.push_back(str);
   return pos;
}

/*!
   *=implemented, x=assert
     babl_menu
     babl_fmenu
     print
     babl_ask
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
void ua_basic_conversation::imported_func(const char* the_funcname)
{
   std::string funcname(the_funcname);

   Uint16 argpos = stack.get_stackp();
   Uint16 argcount = stack.at(argpos);
   argpos--;

   if (funcname.compare("compare")==0)
   {
      ua_assert(argcount == 2);

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
      ua_assert(argcount == 1);

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
      ua_assert(argcount == 2);

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
      ua_assert(argcount == 1);

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
   if (funcname.compare("sex")==0)
   {
      ua_assert(argcount == 2);

      Uint16 arg1 = stack.at(argpos--);
      arg1 = stack.at(arg1);

      Uint16 arg2 = stack.at(argpos);
      arg2 = stack.at(arg2);

      // check player gender
      if (game->get_underworld().get_player().get_attr(ua_attr_gender)==0)
         arg1 = arg2;

      result_register = arg1;
   }
   else
   {
      ua_conv_code_vm::imported_func(the_funcname);
   }
}

Uint16 ua_basic_conversation::get_global(const char* the_globname)
{
   std::string globname(the_globname);
   Uint16 val = 0;

   ua_player& pl = game->get_underworld().get_player();

   // get npc object to talk to
   ua_object& npc_obj = game->get_underworld().get_levelmaps_list().
      get_level(conv_level).get_mapobjects().get_object(conv_objpos);

   ua_object_info_ext& info_ext = npc_obj.get_ext_object_info();

   if (globname.compare("play_name")==0)
      val = alloc_string(pl.get_name().c_str());
   else
   if (globname.compare("npc_xhome")==0)
      val = info_ext.npc_xhome;
   else
   if (globname.compare("npc_yhome")==0)
      val = info_ext.npc_yhome;
   else
   if (globname.compare("npc_attitude")==0)
      val = info_ext.npc_attitude;
   else
   if (globname.compare("npc_goal")==0)
      val = info_ext.npc_goal;
   else
   if (globname.compare("npc_gtarg")==0)
      val = info_ext.npc_gtarg;
   else
   if (globname.compare("npc_hp")==0)
      val = info_ext.npc_hp;
   else
   if (globname.compare("npc_hunger")==0)
      val = info_ext.npc_hunger;
   else
   if (globname.compare("npc_level")==0)
      val = info_ext.npc_level;
   else
   if (globname.compare("npc_talkedto")==0)
      val = info_ext.npc_talkedto ? 1 : 0;
   else
   if (globname.compare("npc_whoami")==0)
      val = info_ext.npc_whoami;
   else
   {
      return ua_conv_code_vm::get_global(the_globname);
   }

   ua_trace("code_vm: get_global %s returned %04x\n", globname.c_str(), val);

   return val;
}

/*! globals that still need implementation:
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

void ua_basic_conversation::set_global(const char* the_globname, Uint16 val)
{
   ua_trace("code_vm: set_global: %s = %04x\n", the_globname, val);

   std::string globname(the_globname);

   ua_player& pl = game->get_underworld().get_player();

   // get npc object to talk to
   ua_object& npc_obj = game->get_underworld().get_levelmaps_list().
      get_level(conv_level).get_mapobjects().get_object(conv_objpos);

   ua_object_info_ext& info_ext = npc_obj.get_ext_object_info();

   if (globname.compare("play_name")==0)
   {
      ua_assert(val < localstrings.size());
      pl.ua_player::set_name(localstrings[val]);
   }
   else
   if (globname.compare("npc_xhome")==0)
      info_ext.npc_xhome = val;
   else
   if (globname.compare("npc_yhome")==0)
      info_ext.npc_yhome = val;
   else
   if (globname.compare("npc_attitude")==0)
      info_ext.npc_attitude = val;
   else
   if (globname.compare("npc_goal")==0)
      info_ext.npc_goal = val;
   else
   if (globname.compare("npc_gtarg")==0)
      info_ext.npc_gtarg = val;
   else
   if (globname.compare("npc_hp")==0)
      info_ext.npc_hp = val;
   else
   if (globname.compare("npc_hunger")==0)
      info_ext.npc_hunger = val;
   else
   if (globname.compare("npc_level")==0)
      info_ext.npc_level = val;
   else
   if (globname.compare("npc_talkedto")==0)
      info_ext.npc_talkedto = val != 0;
   else
   if (globname.compare("npc_whoami")==0)
      info_ext.npc_whoami = val;
   else
   {
      ua_conv_code_vm::set_global(the_globname, val);
   }
}
