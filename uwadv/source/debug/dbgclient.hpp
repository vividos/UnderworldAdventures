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
/*! \file dbgclient.hpp

   \brief debugger client interface

*/

// include guard
#ifndef uadebug_dbgclient_hpp_
#define uadebug_dbgclient_hpp_


// forward references
class ua_debug_server_interface;


// classes

//! debugger client interface
class ua_debug_client_interface
{
public:
   //! inits client interface
   void init_client(void* data);

   //! locks or unlocks the interface
   void lock(bool set_lock);

   //! gets player position info; 0=xpos, 1=ypos, 2=height, 3=rotangle
   double get_player_pos_info(unsigned int idx);

   //! sets player position info
   void set_player_pos_info(unsigned int idx, double val);

   //! returns player attribute
   unsigned int get_player_attr(unsigned int idx);

   //! sets player attribute
   void set_player_attr(unsigned int idx, unsigned int val);

   // level/tile stuff

   //! returns number of levels
   unsigned int get_num_levels();

   //! returns tile height at given coordinates
   double get_tile_height(unsigned int level, double xpos, double ypos);

protected:
   //! pointer to server debugger interface
   ua_debug_server_interface* server;
};


#endif
