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


// ua_debug_server methods

ua_debug_server::ua_debug_server()
:debug_impl(NULL), thread_debugger(NULL), sem_debugger(NULL), mutex_lock(NULL)
{
   // init mutex, semaphore and thread handle
   mutex_lock = SDL_CreateMutex();
   sem_debugger = SDL_CreateSemaphore(0);
}

ua_debug_server::~ua_debug_server()
{
   SDL_DestroyMutex(mutex_lock);

   SDL_DestroySemaphore(sem_debugger);

   if (thread_debugger!=NULL)
      SDL_KillThread(thread_debugger);

   if (debug_impl != NULL)
      debug_impl->done();

   delete debug_impl;
   debug_impl = NULL;
}

void ua_debug_server::init()
{
#ifdef WIN32
   // create win32 debug server implementation
   extern ua_debug_server_impl* ua_create_debug_server_impl();
   debug_impl = ua_create_debug_server_impl();
#endif
   if (debug_impl != NULL)
      debug_impl->init();

   ua_trace("debug server inited; debugger is %savailable\n",
      debug_impl != NULL && debug_impl->is_avail() ? "" : "not ");
}

bool ua_debug_server::start_debugger(ua_game_interface* game)
{
   // check if debugger already runs
   if (debug_impl != NULL && debug_impl->is_avail() && SDL_SemValue(sem_debugger)==0)
   {
      ua_trace("starting uadebug thread\n");

      // start new thread
      thread_debugger = SDL_CreateThread(thread_proc,game);
   }
   else
   {
      ua_trace(debug_impl == NULL || !debug_impl->is_avail() ?
         "debugger not available\n" : "debugger already running\n");
   }

   return false;
}

void ua_debug_server::tick()
{
   if (debug_impl && debug_impl->is_avail() && SDL_SemValue(sem_debugger)!=0)
      debug_impl->tick();
}

/*! \todo implement */
void ua_debug_server::shutdown()
{
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
   ua_game_interface* game = reinterpret_cast<ua_game_interface*>(ptr);
   ua_debug_server* This = &game->get_debugger();

   // increase semaphore count
   SDL_SemPost(This->sem_debugger);

   if (This->debug_impl != NULL && This->debug_impl->is_avail())
      This->debug_impl->start_debugger(game);

   ua_trace("uadebug thread ended\n");

   // decrease semaphore count
   SDL_SemWait(This->sem_debugger);

   return 0;
}
