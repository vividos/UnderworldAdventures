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
/*! \file cutscene_view.cpp

   \brief cutscene view implementation

*/

// needed includes
#include "common.hpp"
#include "cutscene_view.hpp"


// ua_cutscene_view_screen methods

void ua_cutscene_view_screen::init()
{
   ended = false;
   tickcount=0;

   // get a new lua state
   L = lua_open(128);

   // register C functions
   lua_register(L,"cuts_play_sound",cuts_play_sound);
   lua_register(L,"cuts_finished",cuts_finished);

   // load lua script
   std::string scriptname(core->get_settings().uadata_path);
   scriptname.append("uw1/scripts/cutscene.lua");

   int ret = lua_dofile(L,scriptname.c_str());
   if (ret!=0){ ended = true; return; }

   // call "cuts_init(this)"
   lua_getglobal(L,"cuts_init");
   lua_pushuserdata(L,this);
   lua_call(L,1,0);
}

void ua_cutscene_view_screen::done()
{
   lua_close(L);
}

void ua_cutscene_view_screen::handle_event(SDL_Event &event)
{
}

void ua_cutscene_view_screen::render()
{
   glClear(GL_COLOR_BUFFER_BIT);
}

void ua_cutscene_view_screen::tick()
{
   if (ended)
   {
      // we're finished
      core->pop_screen();
      return;
   }

   double ticktime = double(tickcount) / core->get_tickrate();

   // call lua "cuts_tick(ticktime)" function
   lua_getglobal(L,"cuts_tick");
   lua_pushnumber(L,ticktime);
   lua_call(L,1,0);

   ++tickcount;
}


// static ua_cutscene_view_screen methods

int ua_cutscene_view_screen::cuts_play_sound(lua_State *L)
{
   // check for "self" parameter being userdata
   int n=lua_gettop(L);
   if (lua_isuserdata(L,n-1))
   {
      // get pointer to object
      ua_cutscene_view_screen *self =
         reinterpret_cast<ua_cutscene_view_screen*>(lua_touserdata(L,n-1));

      // play sound
      const char *str = lua_tostring(L,n);
      self->core->get_audio().play_sound(atoi(str));
   }
   return 0;
}

int ua_cutscene_view_screen::cuts_finished(lua_State *L)
{
   // check for "self" parameter being userdata
   int n=lua_gettop(L);
   if (lua_isuserdata(L,n))
   {
      // get pointer to object
      ua_cutscene_view_screen *self =
         reinterpret_cast<ua_cutscene_view_screen*>(lua_touserdata(L,n));

      // signal cutscene ending
      self->ended = true;
   }
   return 0;
}
