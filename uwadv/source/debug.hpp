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
#include <deque>
#include "dbgserver.hpp"


// forward references
class ua_basic_game_interface;
class ua_debug_server_interface;


// classes

class ua_debug_lib_context
{
public:
   virtual ~ua_debug_lib_context(){}
   //! inits debug server
   virtual void init(){}
   //! cleans up debug server
   virtual void done(){}
   //! returns if debugger is available
   virtual bool is_avail(){ return false; }
   //! starts debugger
   virtual void debug_start(ua_debug_server_interface* server){}
};


//! server side class for underworld debugger
class ua_debug_server: public ua_debug_server_interface
{
public:
   //! ctor
   ua_debug_server();
   //! dtor
   virtual ~ua_debug_server();

   //! inits debug server
   void init();

   //! starts debugger client; returns if debugger was already running
   bool start_debugger(ua_basic_game_interface* game);

   //! does tick processing
   void tick();

   //! waits for the debugger client to shutdown
   void shutdown();

   //! locks or unlocks debugger access to underworld
   virtual void lock(bool set_lock);

protected:
   //! debugger thread procedure
   static int thread_proc(void* ptr);

   // ua_debug_server_interface methods

   virtual bool check_interface_version(unsigned int interface_ver);

   virtual unsigned int get_message_num();

   virtual bool get_message(unsigned int& msg_type,
      unsigned int& msg_arg1, unsigned int& msg_arg2, double& msg_arg3,
      unsigned int& msg_text_size);

   virtual bool get_message_text(char* buffer, unsigned int bufsize);

   virtual bool pop_message();

   virtual double get_player_pos_info(unsigned int idx);

   virtual void set_player_pos_info(unsigned int idx, double val);

   virtual unsigned int get_player_attr(unsigned int idx);

   virtual void set_player_attr(unsigned int idx, unsigned int val);

   virtual unsigned int get_num_levels();

   virtual double get_tile_height(unsigned int level, double xpos,
      double ypos);

   virtual void add_message(ua_debug_server_message& msg);

protected:
   //! debug library context
   ua_debug_lib_context* debug_lib;

   //! debug thread
   SDL_Thread* thread_debugger;

   //! semaphore that indicates if debugger still runs
   SDL_sem* sem_debugger;

   //! mutex to lock/unlock underworld object
   SDL_mutex* mutex_lock;

   //! pointer to game interface
   ua_basic_game_interface* game;

   //! is true when new level textures should be prepared at next tick()
   bool schedule_prepare;

   //! message queue
   std::deque<ua_debug_server_message> message_queue;
};


#endif
//@}
