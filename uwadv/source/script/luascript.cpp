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
/*! \file luascript.cpp

   \brief Lua scripting implementation

*/

// needed includes
#include "common.hpp"
#include "luascript.hpp"
#include "game_interface.hpp"
#include "files.hpp"


// ua_lua_scripting methods

void ua_lua_scripting::init(ua_game_interface* the_game)
{
   game = the_game;

   // init lua state
   L = lua_open(256);

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

//   register_functions();
}

bool ua_lua_scripting::load_script(const char* basename)
{
   std::string scriptname(basename);
   scriptname.append(".lua");

   bool compiled = false;

   // load lua script
   SDL_RWops* script =
      game->get_files_manager().get_uadata_file(scriptname.c_str());

   // not found? try binary one
   if (script==NULL)
   {
      scriptname.assign(basename);
      scriptname.append(".lob");
      compiled = true;

      script = game->get_files_manager().get_uadata_file(scriptname.c_str());
   }

   // still not found? exception
   if (script==NULL)
   {
      std::string text("could not load Lua script from uadata: ");
      text.append(scriptname.c_str());
      throw ua_exception(text.c_str());
   }

   int ret = load_script(script, basename);

   ua_trace("loaded Lua %sscript \"%s\"\n",
      compiled ? "compiled " : "", scriptname.c_str());

   SDL_RWclose(script);

   return ret == 0;
}

void ua_lua_scripting::done()
{
   lua_close(L);
}

void ua_lua_scripting::init_new_game()
{
}

void ua_lua_scripting::eval_critter(unsigned int pos)
{
}

void ua_lua_scripting::do_trigger(unsigned int pos)
{
}

void ua_lua_scripting::cast_spell()
{
}

void ua_lua_scripting::on_changing_level()
{
}

void ua_lua_scripting::object_look(unsigned int pos)
{
}

void ua_lua_scripting::object_use(unsigned int pos)
{
}

void ua_lua_scripting::inventory_combine(unsigned int pos,unsigned int pos2)
{
}

void ua_lua_scripting::inventory_look(unsigned int pos)
{
}

int ua_lua_scripting::load_script(SDL_RWops* rwops, const char* chunkname)
{
   // load script into buffer
   std::vector<char> buffer;
   unsigned int len=0;
   {
      SDL_RWseek(rwops,0,SEEK_END);
      len = SDL_RWtell(rwops);
      SDL_RWseek(rwops,0,SEEK_SET);

      buffer.resize(len+1,0);

      SDL_RWread(rwops,&buffer[0],len,1);
      buffer[len]=0;
   }

   // execute script
   int ret = lua_dobuffer(L,&buffer[0],len,chunkname);

   if (ret!=0)
      ua_trace("Lua script loading ended with error code %u\n",ret);

   return ret;
}


ua_scripting* ua_scripting::create_scripting(ua_scripting_language)
{
   return new ua_lua_scripting;
}
