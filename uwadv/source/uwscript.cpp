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
/*! \file uwscript.cpp

   \brief underworld Lua script bindings

*/

// needed includes
#include "common.hpp"
#include "uwscript.hpp"
#include "underworld.hpp"


// ua_underworld_script_bindings methods

void ua_underworld_script_bindings::init(ua_underworld *uw)
{
   // init lua state
   L = lua_open(256);

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

   // load all lua scripts
   ua_files_manager &fmgr = uw->get_game_core()->get_filesmgr();

   // read init text file
   std::string initlist_str;
   {
      SDL_RWops *uwinit = fmgr.get_uadata_file("uw1/scripts/uwinit.txt");

      if (uwinit==NULL)
         throw ua_exception("could not find underworld Lua scripts init file");

      // read in all of the file
      std::vector<char> initlist;

      SDL_RWseek(uwinit,0,SEEK_END);
      int size = SDL_RWtell(uwinit);
      SDL_RWseek(uwinit,0,SEEK_SET);

      if (size<=0)
         throw ua_exception("could not read underworld Lua scripts init file");

      initlist.resize(size+1,0);
      SDL_RWread(uwinit,&initlist[0],size,1);
      initlist[size]=0;

      initlist_str.assign(reinterpret_cast<char *>(&initlist[0]));
   }

   std::string::size_type pos=0;

   // remove '\r' chars
   do
   {
      pos = initlist_str.find_first_of('\r');
      if (pos != std::string::npos)
         initlist_str.erase(pos,1);

   } while (pos != std::string::npos);

   // load all scripts mentioned in init text file
   do
   {
      pos = initlist_str.find_first_of('\n');

      if (pos != std::string::npos)
      {
         // load single script
         std::string basename;
         basename.assign(initlist_str.c_str(),pos);

         if (basename.size()>0)
         {
            int ret = fmgr.load_lua_script(L,basename.c_str());
            if (ret!=0)
               throw ua_exception("could not execute underworld Lua script");
         }

         // remove from list
         initlist_str.erase(0,pos+1);
      }

   } while (pos != std::string::npos);

   register_functions();

   // call init function lua_init_script(self)
   lua_getglobal(L,"lua_init_script");
   lua_pushuserdata(L,uw);
   checked_lua_call(1,0);
}

void ua_underworld_script_bindings::register_functions()
{
//   lua_register(L,"func_name",func_name);

   // ua_player access
   lua_register(L,"player_get_attr",player_get_attr);
   lua_register(L,"player_set_attr",player_set_attr);
   lua_register(L,"player_get_skill",player_get_skill);
   lua_register(L,"player_set_skill",player_set_skill);
   lua_register(L,"player_get_pos",player_get_pos);
   lua_register(L,"player_set_pos",player_set_pos);
   lua_register(L,"player_get_angle",player_get_angle);
   lua_register(L,"player_set_angle",player_set_angle);
}

void ua_underworld_script_bindings::checked_lua_call(int params, int retvals)
{
   int ret = lua_call(L,params,retvals);
   if (ret!=0)
      ua_trace("Lua function call error code: %u\n",ret);
//      throw ua_exception("");
}

double ua_underworld_script_bindings::get_lua_constant(const char *name)
{
   lua_getglobal(L,name);

   double val = 0.0;
   if (lua_isnumber(L,-1)) val = lua_tonumber(L,-1);

   lua_pop(L,1);

   return val;
}

void ua_underworld_script_bindings::done()
{
   // call init function lua_init_script(self)
   lua_getglobal(L,"lua_done_script");
   lua_pushuserdata(L,this);
   checked_lua_call(1,0);

   lua_close(L);
}


// Lua functions callable from C

ua_inv_item_category ua_underworld_script_bindings::lua_inventory_categorize_item(Uint16 item_id)
{
   // call Lua function
   lua_getglobal(L,"lua_inventory_categorize_item");
   lua_pushnumber(L,static_cast<double>(item_id));

   checked_lua_call(1,1);

   // get return value
   ua_inv_item_category cat = ua_inv_cat_normal;

   if (lua_isnumber(L,-1))
   {
      cat = static_cast<ua_inv_item_category>(
         static_cast<unsigned int>(lua_tonumber(L,-1)));
   }

   lua_pop(L,1);
   return cat;
}

ua_obj_combine_result ua_underworld_script_bindings::lua_obj_combine(
   Uint16 item_id1, Uint16 item_id2, Uint16 &result_id)
{
   // call Lua function
   lua_getglobal(L,"lua_obj_combine");
   lua_pushnumber(L,static_cast<double>(item_id1));
   lua_pushnumber(L,static_cast<double>(item_id2));

   checked_lua_call(2,2);

   // retrieve return values
   ua_obj_combine_result res = ua_obj_cmb_failed;

   if (lua_isnumber(L,-2))
   {
      res = static_cast<ua_obj_combine_result>(
         static_cast<unsigned int>(lua_tonumber(L,-2)));
   }

   result_id = 0xffff;
   if (lua_isnumber(L,-1))
   {
      result_id = static_cast<Uint16>(lua_tonumber(L,-1));
   }
   else
      res = ua_obj_cmb_failed;

   lua_pop(L,2);

   return res;
}

/*! params is the number of parameters the current registered function
    expects. it is needed to determine where the "self" userdata value is
    stored. */
ua_underworld& ua_underworld_script_bindings::lua_get_underworld(lua_State* L,int params)
{
   ua_underworld* self =
      reinterpret_cast<ua_underworld*>(lua_touserdata(L,-params));

   if (self->get_scripts().L != L)
      throw ua_exception("wrong 'self' parameter in ua_underworld Lua script");

   return *self;
}


// registered C functions callable from Lua

int ua_underworld_script_bindings::player_get_attr(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,2).get_player();

   ua_player_attributes attr_type =
      static_cast<ua_player_attributes>(static_cast<int>(lua_tonumber(L,-1)));

   unsigned int attr_value = pl.get_attr(attr_type);
   lua_pushnumber(L,static_cast<double>(attr_value));

   return 1; // one return parameter
}

int ua_underworld_script_bindings::player_set_attr(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,3).get_player();

   ua_player_attributes attr_type =
      static_cast<ua_player_attributes>(static_cast<int>(lua_tonumber(L,-2)));
   unsigned int attr_value = static_cast<unsigned int>(lua_tonumber(L,-1));

   pl.set_attr(attr_type,attr_value);

   return 0;
}

int ua_underworld_script_bindings::player_get_skill(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,2).get_player();

   ua_player_skills skill_type =
      static_cast<ua_player_skills>(static_cast<int>(lua_tonumber(L,-1)));

   unsigned int skill_value = pl.get_skill(skill_type);
   lua_pushnumber(L,static_cast<double>(skill_value));

   return 1;
}

int ua_underworld_script_bindings::player_set_skill(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,3).get_player();

   ua_player_skills skill_type =
      static_cast<ua_player_skills>(static_cast<int>(lua_tonumber(L,-2)));
   unsigned int skill_value = static_cast<unsigned int>(lua_tonumber(L,-1));

   pl.set_skill(skill_type,skill_value);

   return 0;
}

int ua_underworld_script_bindings::player_get_pos(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,1).get_player();

   lua_pushnumber(L,pl.get_xpos());
   lua_pushnumber(L,pl.get_ypos());

   return 2;
}

int ua_underworld_script_bindings::player_set_pos(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,3).get_player();

   double xpos = lua_tonumber(L,-2);
   double ypos = lua_tonumber(L,-1);

   pl.set_pos(xpos,ypos);

   return 0;
}

int ua_underworld_script_bindings::player_get_angle(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,1).get_player();

   lua_pushnumber(L,pl.get_angle());

   return 1;
}

int ua_underworld_script_bindings::player_set_angle(lua_State *L)
{
   ua_player &pl = lua_get_underworld(L,2).get_player();

   double angle = lua_tonumber(L,-1);
   pl.set_angle(angle);

   return 0;
}
