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
/*! \file debug.hpp

   \brief underworld debugger interface

*/
/*! \defgroup debug Underworld Adventures Debugger

   uadebug documentation yet to come ...

*/
//@{

// include guard
#ifndef uwadv_debug_hpp_
#define uwadv_debug_hpp_

// needed includes
#include <SDL_thread.h>


// forward references
class ua_basic_game_interface;
class ua_debug_server_impl;


// classes

//! server side class for underworld debugger
class ua_debug_server
{
public:
   //! ctor
   ua_debug_server();
   //! dtor
   ~ua_debug_server();

   //! inits debug server
   void init();

   //! starts debugger client; returns if debugger was already running
   bool start_debugger(ua_basic_game_interface* game);

   //! does tick processing
   void tick();

   //! waits for the debugger client to shutdown
   void shutdown();

   //! locks or unlocks debugger access to underworld
   void lock(bool set_lock);

protected:
   //! debugger thread procedure
   static int thread_proc(void* ptr);

protected:
   //! debug server implementation
   ua_debug_server_impl* debug_impl;

   //! debug thread
   SDL_Thread* thread_debugger;

   //! semaphore that indicates if debugger still runs
   SDL_sem* sem_debugger;

   //! mutex to lock/unlock underworld object
   SDL_mutex* mutex_lock;
};


#endif
//@}
