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
/*! \file dbgserver.hpp

   \brief debug server implementation

*/
//! \ingroup debug

//@{

// include guard
#ifndef uwadv_dbgserver_hpp_
#define uwadv_dbgserver_hpp_


// forward references
class ua_game_interface;


// classes

//! debug server implementation definition
/*! all methods of this interface class are virtual, so that they can be
    called via the vtable, from the uadebug dll.
*/
class ua_debug_server_impl
{
protected:
   // methods called from debug server

   //! inits debug server
   virtual void init()=0;
   //! cleans up debug server
   virtual void done()=0;
   //! returns if debugger is available
   virtual bool is_avail()=0;
   //! returns if debugger
   virtual void start_debugger(ua_game_interface* game)=0;
   //! does idle tick processing
   virtual void tick()=0;

   friend class ua_debug_server;

public:
   // methods called from debug client interface

   //! locks/unlocks underworld
   virtual void lock(bool set_lock)=0;

   // player stuff

   //! gets player position info; 0=xpos, 1=ypos, 2=height, 3=rotangle
   virtual double get_player_pos_info(unsigned int idx)=0;

   //! sets player position info
   virtual void set_player_pos_info(unsigned int idx, double val)=0;

   //! returns player attribute
   virtual unsigned int get_player_attr(unsigned int idx)=0;

   //! sets player attribute
   virtual void set_player_attr(unsigned int idx, unsigned int val)=0;

   // level/tile stuff

   //! returns number of levels
   virtual unsigned int get_num_levels()=0;

   //! returns tile height at given coordinates
   virtual double get_tile_height(unsigned int level, double xpos,
      double ypos)=0;
};


#endif
//@}
