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
/*! \file underworld.cpp

   \brief underworld object

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"
#include "uamath.hpp"
#include "import.hpp"
#include "script.hpp"


// ua_underworld methods

ua_underworld::ua_underworld()
:callback(NULL), scripting(NULL)
{
}

void ua_underworld::init(ua_settings& settings, ua_files_manager& filesmgr)
{
   enhanced_features = settings.get_bool(ua_setting_uwadv_features);

   stopped = false;
   attacking = false;
   attack_power = 0;

   // init underworld members
   levels.clear();

   physics.init(this);

   inventory.init(this);

   player.init();

   questflags.resize(0x0040,0);

   properties.import(settings);

   // load game strings
   ua_trace("loading game strings ... ");
   gstr.load(settings);
   ua_trace("done\n");
}

void ua_underworld::done()
{
   if (scripting != NULL)
      scripting->done();
}

void ua_underworld::eval_underworld(double time)
{
   if (stopped)
      return;

   if (attacking)
   {
      attack_power += 5;
      if (attack_power > 100)
         attack_power = 100;

      if (callback != NULL)
         callback->uw_notify(ua_notify_update_powergem);
   }

   // evaluate physics
   physics.eval_physics(time);

   check_move_trigger();
}

void ua_underworld::user_action(ua_underworld_user_action action,
   unsigned int param)
{
   if (scripting == NULL) return;

   ua_trace("user action: action=%u param=%u\n", action,param);

   switch(action)
   {
   case ua_action_combat_draw:
      attacking = true;
      attack_power = 0;
      // additionally tell scripting with type of attack
      scripting->user_action(action, param);
      break;

   case ua_action_combat_release:
      // do the actual attack
      scripting->user_action(action, attack_power);
      ua_trace("attacking with power=%u\n", attack_power);

      // switch off attacking
      attacking = false;
      attack_power = 0;

      if (callback != NULL)
         callback->uw_notify(ua_notify_update_powergem);
      break;

   default:
      // pass on to scripting
      scripting->user_action(action, param);
      break;
   }
}

ua_level &ua_underworld::get_current_level()
{
   unsigned int curlevel = player.get_attr(ua_attr_maplevel);
   return levels[curlevel];
}

const ua_level &ua_underworld::get_current_level() const
{
   unsigned int curlevel = player.get_attr(ua_attr_maplevel);
   return levels[curlevel];
}

void ua_underworld::change_level(unsigned int level)
{
   if (level>=levels.size())
      throw ua_exception("game wanted to change to unknown level");

   // set new level
   player.set_attr(ua_attr_maplevel,level);

//TODO   script.lua_change_level(level);

   // clear activated move triggers
   trigger_active.clear();
}

void ua_underworld::load_game(ua_savegame &sg)
{
   // load all levels
   {
      levels.clear();

      sg.begin_section("tilemaps");

      Uint32 max = sg.read32();

      for(Uint32 i=0; i<max; i++)
      {
         ua_level newlevel;

         // load tilemap / objects list / annotations
         newlevel.load_game(sg);

         levels.push_back(newlevel);
      }

      sg.end_section();
   }

   // load map annotations

   // load objects list

   // load inventory
   inventory.load_game(sg);

   // load player infos
   player.load_game(sg);

   // load quest flags
   {
      sg.begin_section("questflags");

      unsigned int max = sg.read16();
      questflags.clear();
      questflags.resize(max,0);

      for(unsigned int i=0; i<max; i++)
         questflags[i] = sg.read16();

      sg.end_section();
   }

   // load map notes
   mapnotes.load_game(sg);

   // load conv. globals
   conv_globals.load_game(sg);

   // load Lua script values
//TODO   script.load_game(sg);

   sg.close();

   // reload level
   change_level(player.get_attr(ua_attr_maplevel));
}

void ua_underworld::save_game(ua_savegame &sg)
{
   // save all levels
   {
      sg.begin_section("tilemaps");

      unsigned int max = levels.size();
      sg.write32(max);

      for(unsigned int i=0; i<max; i++)
      {
         // save tilemap / objects list / annotations
         levels[i].save_game(sg);
      }

      sg.end_section();
   }

   // save inventory
   inventory.save_game(sg);

   // save player infos
   player.save_game(sg);

   // save quest flags
   {
      sg.begin_section("questflags");

      unsigned int max = questflags.size();
      sg.write16(max);

      for(unsigned int i=0; i<max; i++)
         sg.write16(questflags[i]);

      sg.end_section();
   }

   // save map notes
   mapnotes.save_game(sg);

   // save conv. globals
   conv_globals.save_game(sg);

   // save Lua script values
//TODO   script.save_game(sg);

   sg.close();
}

void ua_underworld::import_savegame(ua_settings& settings,const char* folder,bool initial)
{
   ua_uw_import import;

   // load level maps
   import.load_levelmaps(levels,settings,folder);

   // load conv globals
   {
      std::string bgname(settings.get_string(ua_setting_uw_path));
      bgname.append(folder);
      bgname.append(initial ? "babglobs.dat" : "bglobals.dat");
      conv_globals.import(bgname.c_str(),initial);
   }

   // load player infos
   if (!initial)
      import.load_player(player,folder);

   // init a new game
   if (scripting != NULL)
      scripting->init_new_game();

   // reload level
   change_level(player.get_attr(ua_attr_maplevel));
}

void ua_underworld::check_move_trigger()
{
   ua_vector3d pl_pos(player.get_xpos(),player.get_ypos(),player.get_height());

   unsigned int tilex, tiley;
   tilex = static_cast<unsigned int>(pl_pos.x);
   tiley = static_cast<unsigned int>(pl_pos.y);

   // check all surrounding tiles
   for(int i=-1; i<2; i++)
   for(int j=-1; j<2; j++)
   {
      int x = static_cast<int>(tilex) + i;
      int y = static_cast<int>(tiley) + j;
      if (x<0 || y<0 || x>64-1 || y>64-1) continue;

      ua_object_list& objlist = get_current_level().get_mapobjects();

      // check tile objects for move trigger
      Uint16 pos =
         objlist.get_tile_list_start(
            static_cast<unsigned int>(x),static_cast<unsigned int>(y));

      while (pos != 0)
      {
         // check if move trigger
         ua_object& obj = objlist.get_object(pos);
         if (obj.get_object_info().item_id == 0x01a0)
         {
            // found move trigger; check if it's in range
            ua_object_info_ext& extinfo = obj.get_ext_object_info();
            ua_vector3d trig_pos(
               static_cast<double>(x)+(extinfo.xpos+0.5)/8.0,
               static_cast<double>(y)+(extinfo.ypos+0.5)/8.0,
               extinfo.zpos);

            trig_pos -= pl_pos;

            if (trig_pos.length()<0.5)
            {
               // trigger in range

               // check if trigger already active
               if (trigger_active.find(pos) == trigger_active.end())
               {
                  // not active yet
                  trigger_active.insert(pos);

                  ua_trace("move trigger: activate trigger at %04x\n",pos);

                  unsigned int curlevel = player.get_attr(ua_attr_maplevel);
//TODO                  script.lua_trigger_set_off(curlevel,pos);
               }
               else
               {
                  // trigger is active; do nothing
               }
            }
            else
            {
               // not in range; check if we can deactivate it
               if (trigger_active.find(pos) != trigger_active.end())
               {
                  ua_trace("move trigger: deactivate trigger at %04x\n",pos);
                  trigger_active.erase(pos);
               }
            }
         }

         // next object in chain
         pos = obj.get_object_info().link;
      }
   }
}
