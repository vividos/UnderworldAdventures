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
}

void ua_underworld_script_bindings::register_functions()
{
//   lua_register(L,"func_name",func_name);
}

void ua_underworld_script_bindings::checked_lua_call(int params, int retvals)
{
   int ret = lua_call(L,params,retvals);
   if (ret!=0)
      ua_trace("Lua function call error code: %u\n",ret);
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
      cat = static_cast<ua_inv_item_category>(
         static_cast<unsigned int>(lua_tonumber(L,-1)));

   lua_pop(L,1);
   return cat;
}

ua_obj_combine_result ua_underworld_script_bindings::lua_obj_combine(
   Uint16 item_id1, Uint16 item_id2, Uint16 &result_id)
{
   result_id = 0x012b;
   return ua_obj_cmb_failed;
}


// registered C functions callable from Lua
