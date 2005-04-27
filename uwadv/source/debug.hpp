/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004,2005 Underworld Adventures Team

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


// structs

//! debug server message that gets sent to client
struct ua_debug_server_message
{
   //! ctor
   ua_debug_server_message()
      :msg_type(0), msg_arg1(0), msg_arg2(0), msg_arg3(0.0){}

   //! message type; see enum ua_debug_server_message_type
   unsigned int msg_type;

   //! message argument 1
   unsigned int msg_arg1;
   //! message argument 2
   unsigned int msg_arg2;
   //! message argument 3
   double msg_arg3;

   //! message text
   std::string msg_text;
};


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

   //! returns if the debugger is running
   bool is_debugger_running();

   //! does tick processing
   void tick();

   //! waits for the debugger client to shutdown
   void shutdown();

   //! locks or unlocks debugger access to underworld
   virtual void lock(bool set_lock);

   //! starts code debugger
   void start_code_debugger(ua_debug_code_interface* code_debugger);

   //! ends code debugging
   void end_code_debugger(ua_debug_code_interface* code_debugger);

protected:
   //! debugger thread procedure
   static int thread_proc(void* ptr);

   // ua_debug_server_interface methods

   virtual bool check_interface_version(unsigned int interface_ver);

   virtual unsigned int get_flag(unsigned int flag_id);

   virtual unsigned int get_game_path(char* buffer, unsigned int bufsize);

   virtual void load_game(const char* path);

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

   virtual unsigned int get_tile_info_value(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type);

   virtual unsigned int get_objlist_info(unsigned int level,
      unsigned int pos, unsigned int type);

   virtual void set_objlist_info(unsigned int level,
      unsigned int pos, unsigned int type, unsigned int value);

   virtual bool enum_gamestr_block(unsigned int index,
      unsigned int& blocknum);

   virtual unsigned int get_gamestr_blocksize(unsigned int block);

   virtual unsigned int get_game_string(unsigned int block, unsigned int nr,
      char* buffer, unsigned int maxsize);

   virtual bool get_object_list_imagelist(unsigned int& num_objects,
      unsigned char* buffer, unsigned int size);

   //! adds message to client message queue
   void add_message(ua_debug_server_message& msg);

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

   //! map of all code debugger
   std::map<unsigned int, ua_debug_code_interface*> all_code_debugger;

   //! id of last code debugger
   unsigned int last_code_debugger_id;
};


#endif
//@}
