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
/*! \file debug.cpp

   \brief debug server implementation

*/

// needed includes
#include "common.hpp"
#include "debug.hpp"
#include "dbgserver.hpp"
#include "game_interface.hpp"
#include "underworld.hpp"


// impl. classes

#ifdef WIN32
//! win32 debug lib context
class ua_debug_lib_context_win32: public ua_debug_lib_context
{
public:
   ua_debug_lib_context_win32():dll(NULL){}

   virtual void init()
   {
      dll = ::LoadLibrary("uadebug.dll");
   }

   virtual void done()
   {
      ::FreeLibrary(dll);
   }

   virtual bool is_avail()
   {
      return dll != NULL && ::GetProcAddress(dll,"uadebug_start") != NULL;
   }

   virtual void debug_start(ua_debug_server_interface* server)
   {
      typedef void (*uadebug_start_func)(void*);

      // get function pointer
      uadebug_start_func uadebug_start =
         (uadebug_start_func)::GetProcAddress(dll, "uadebug_start");

      // start debugger
      uadebug_start(server);
   }

protected:
   //! DLL module handle
   HMODULE dll;
};
#endif

#ifdef LINUX
//! linux debug lib context
/*! Uses dlopen() and dlsym() to load the shared object libuadebug.so and
    start the debugger
*/
class ua_debug_lib_context_linux: public ua_debug_lib_context
{
public:
   //! ctor
   ua_debug_lib_context_linux(){}
   //! inits debug server
   virtual void init(){}
   //! cleans up debug server
   virtual void done(){}
   //! returns if debugger is available
   virtual bool is_avail(){ return false; }
   //! starts debugger
   virtual void debug_start(ua_debug_server_interface* server){}
};
#endif


// ua_debug_server methods

ua_debug_server::ua_debug_server()
:debug_lib(NULL), thread_debugger(NULL), sem_debugger(NULL), mutex_lock(NULL),
 schedule_prepare(false)
{
   // init mutex, semaphore and thread handle
   mutex_lock = SDL_CreateMutex();
   sem_debugger = SDL_CreateSemaphore(0);

#ifdef WIN32
   debug_lib = new ua_debug_lib_context_win32;
#else
   debug_lib = new ua_debug_lib_context;
#endif

   ua_assert(debug_lib != NULL);
}

ua_debug_server::~ua_debug_server()
{
   SDL_DestroyMutex(mutex_lock);

   SDL_DestroySemaphore(sem_debugger);

   if (thread_debugger != NULL)
      SDL_KillThread(thread_debugger);

   debug_lib->done();
   delete debug_lib;
   debug_lib = NULL;
}

void ua_debug_server::init()
{
   debug_lib->init();

   ua_trace("debug server inited; debugger is %savailable\n",
      debug_lib->is_avail() ? "" : "not ");
}

bool ua_debug_server::start_debugger(ua_basic_game_interface* the_game)
{
   game = the_game;

   // check if debugger already runs
   if (debug_lib->is_avail() && SDL_SemValue(sem_debugger)==0)
   {
      ua_trace("starting uadebug thread\n");

      // start new thread
      thread_debugger = SDL_CreateThread(thread_proc,this);
   }
   else
   {
      ua_trace(!debug_lib->is_avail() ?
         "debugger not available\n" : "debugger already running\n");
   }

   return false;
}

void ua_debug_server::tick()
{
   if (SDL_SemValue(sem_debugger)!=0)
   {
      lock(true);

      if (schedule_prepare)
      {
         // change level per underworld object
         schedule_prepare = false;
         ua_underworld& underw = game->get_underworld();
         underw.change_level(underw.get_player().get_attr(ua_attr_maplevel));
      }

      lock(false);
   }
}

void ua_debug_server::shutdown()
{
   if (debug_lib->is_avail() && thread_debugger != NULL)
   {
      ua_trace("shutting down debugger...\n");

      // send quit message
      lock(true);
      ua_debug_server_message msg;
      msg.msg_type = ua_msg_shutdown;

      add_message(msg);
      lock(false);

      // wait for thread
      SDL_WaitThread(thread_debugger,NULL);
      thread_debugger = NULL;
   }
}

void ua_debug_server::lock(bool set_lock)
{
   if (set_lock)
      SDL_LockMutex(mutex_lock);
   else
      SDL_UnlockMutex(mutex_lock);
}

int ua_debug_server::thread_proc(void* ptr)
{
   ua_debug_server* This = reinterpret_cast<ua_debug_server*>(ptr);

   // increase semaphore count
   SDL_SemPost(This->sem_debugger);

   if (This->debug_lib->is_avail())
      This->debug_lib->debug_start(This);

   ua_trace("uadebug thread ended\n");

   // decrease semaphore count
   SDL_SemWait(This->sem_debugger);

   return 0;
}

bool ua_debug_server::check_interface_version(unsigned int interface_ver)
{
   return interface_ver == ua_debug_server_interface_version;
}

unsigned int ua_debug_server::get_flag(unsigned int flag_id)
{
   unsigned int flag = 0;
   switch(flag_id)
   {
   case 0:
#ifdef COMPILE_UASTUDIO
      flag = 1; // in uastudio mode
#else
      flag = 0;
#endif
      break;

   default:
      ua_assert(false);
      break;
   }
   return flag;
}

const char* ua_debug_server::get_game_path()
{
   return "";//game->get_settings().get_string(ua_setting_game_prefix);
}

void ua_debug_server::load_game(const char* path)
{
   game->done_game();

   std::string strpath(path);
   game->get_settings().set_value(ua_setting_game_prefix, strpath);

   game->init_game();
}

unsigned int ua_debug_server::get_message_num()
{
   return message_queue.size();
}

bool ua_debug_server::get_message(unsigned int& msg_type,
   unsigned int& msg_arg1, unsigned int& msg_arg2, double& msg_arg3,
   unsigned int& msg_text_size)
{
   if (message_queue.size()>0)
   {
      ua_debug_server_message& msg = message_queue.front();
      msg_type = msg.msg_type;
      msg_arg1 = msg.msg_arg1;
      msg_arg2 = msg.msg_arg2;
      msg_arg3 = msg.msg_arg3;
      msg_text_size = msg.msg_text.size()+1;
   }
   return message_queue.size()>0;
}

bool ua_debug_server::get_message_text(char* buffer, unsigned int bufsize)
{
   if (message_queue.size()>0)
   {
      ua_debug_server_message& msg = message_queue.front();
      strncpy(buffer,msg.msg_text.c_str(), bufsize);
   }
   return message_queue.size()>0;
}

bool ua_debug_server::pop_message()
{
   if (message_queue.size()>0)
   {
      message_queue.pop_front();
   }
   return message_queue.size()>0;
}

double ua_debug_server::get_player_pos_info(unsigned int idx)
{
   double val = 0.0;
   if (game == NULL) return val;

   ua_player& pl = game->get_underworld().get_player();
   switch(idx)
   {
   case 0: val = pl.get_xpos(); break;
   case 1: val = pl.get_ypos(); break;
   case 2: val = pl.get_height(); break;
   case 3: val = pl.get_angle_rot(); break;
   default:
      break;
   }
   return val;
}

void ua_debug_server::set_player_pos_info(unsigned int idx, double val)
{
   if (game == NULL) return;

   ua_player& pl = game->get_underworld().get_player();
   switch(idx)
   {
   case 0: pl.set_pos(val, pl.get_ypos()); break;
   case 1: pl.set_pos(pl.get_xpos(), val); break;
   case 2: pl.set_height(val); break;
   case 3: pl.set_angle_rot(val); break;
   default:
      break;
   }
}

unsigned int ua_debug_server::get_player_attr(unsigned int idx)
{
   if (game == NULL) return 0;
   return game->get_underworld().get_player().get_attr((ua_player_attributes)idx);
}

void ua_debug_server::set_player_attr(unsigned int idx, unsigned int val)
{
   if (game == NULL) return;

   ua_player& pl = game->get_underworld().get_player();

   unsigned int oldlevel = pl.get_attr(ua_attr_maplevel);

   pl.set_attr((ua_player_attributes)idx,val);

   // changing level? then schedule new level preparation
   if (ua_attr_maplevel == (ua_player_attributes)idx && oldlevel != val)
   {
      // note: cannot prepare textures in this thread, since it doesn't own
      //       the OpenGL rendering context.
      schedule_prepare = true;
   }
}

unsigned int ua_debug_server::get_num_levels()
{
   if (game == NULL) return 0;
   return game->get_underworld().get_levelmaps_list().get_num_levels();
}

double ua_debug_server::get_tile_height(unsigned int level, double xpos,
   double ypos)
{
   if (game == NULL) return 0.0;
   return game->get_underworld().get_levelmaps_list().get_level(level).get_floor_height(xpos,ypos);
}

void ua_debug_server::add_message(ua_debug_server_message& msg)
{
   message_queue.push_back(msg);
}
