/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004 Underworld Adventures Team

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

   \brief win32 debug interface implementation

*/

// needed includes
#include "common.hpp"
#include "debug.hpp"
#include "dbgserver.hpp"
#include "game_interface.hpp"
#include "underworld.hpp"
#include "renderer.hpp"
#include <deque>


#ifdef WIN32

//! debug server implementation for win32
class ua_debug_server_impl_win32: public ua_debug_server_impl
{
public:
   ua_debug_server_impl_win32()
      :dll(NULL), game(NULL), schedule_prepare(false)
   {
   }

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
   virtual void start_debugger(ua_game_interface* the_game)
   {
      game = the_game;

      typedef void (*uadebug_start_func)(void*);

      // get function pointer
      uadebug_start_func uadebug_start =
         (uadebug_start_func)::GetProcAddress(dll, "uadebug_start");

      // start debugger
      uadebug_start(this);

      // debugger returned; reset game object
      game = NULL;
   }

   virtual void tick()
   {
      if (game == NULL)
         return;

      lock(true);

      if (schedule_prepare)
      {
         schedule_prepare = false;

         game->get_renderer().prepare_level(
            game->get_underworld().get_current_level());
      }


      lock(false);
   }

   virtual void lock(bool set_lock)
   {
      if (game != NULL)
         game->get_debugger().lock(set_lock);
   }

   virtual unsigned int get_message_num()
   {
      return message_queue.size();
   }

   virtual bool get_message(unsigned int& msg_type,
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

   virtual bool get_message_text(char* buffer, unsigned int bufsize)
   {
      if (message_queue.size()>0)
      {
         ua_debug_server_message& msg = message_queue.front();
         strncpy(buffer,msg.msg_text.c_str(), bufsize);
      }
      return message_queue.size()>0;
   };

   virtual bool pop_message()
   {
      if (message_queue.size()>0)
      {
         message_queue.pop_front();
      }
      return message_queue.size()>0;
   }

   virtual void add_message(ua_debug_server_message& msg)
   {
      message_queue.push_back(msg);
   }

   virtual double get_player_pos_info(unsigned int idx)
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

   virtual void set_player_pos_info(unsigned int idx, double val)
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

   virtual unsigned int get_player_attr(unsigned int idx)
   {
      if (game == NULL) return 0;
      return game->get_underworld().get_player().get_attr((ua_player_attributes)idx);
   }

   virtual void set_player_attr(unsigned int idx, unsigned int val)
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

   virtual unsigned int get_num_levels()
   {
      if (game == NULL) return 0;
      return game->get_underworld().get_num_levels();
   }

   virtual double get_tile_height(unsigned int level, double xpos,
      double ypos)
   {
      if (game == NULL) return 0.0;
      return game->get_underworld().get_level(level).get_floor_height(xpos,ypos);
   }

/*   virtual void get_tile_info(unsigned int level, unsigned int xpos,
      unsigned int ypos, int a)
   {
   }
*/
protected:
   //! DLL module handle
   HMODULE dll;

   //! pointer to game interface
   ua_game_interface* game;

   //! is true when new level textures should be prepared at next tick()
   bool schedule_prepare;

   //! message queue
   std::deque<ua_debug_server_message> message_queue;
};


//! creates new debug server implementation object
ua_debug_server_impl* ua_create_debug_server_impl()
{
   return new ua_debug_server_impl_win32;
}

#endif
