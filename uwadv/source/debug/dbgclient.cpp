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
/*! \file dbgclient.cpp

   \brief debug client implementation

*/

// needed includes
#include "dbgcommon.hpp"
#include "dbgclient.hpp"
#include "dbgserver.hpp"


// ua_debug_client_interface methods

void ua_debug_client_interface::init_client(void* data)
{
   server = reinterpret_cast<ua_debug_server_interface*>(data);
}

void ua_debug_client_interface::lock(bool set_lock)
{
   server->lock(set_lock);
}

double ua_debug_client_interface::get_player_pos_info(unsigned int idx)
{
   return server->get_player_pos_info(idx);
}

void ua_debug_client_interface::set_player_pos_info(unsigned int idx, double val)
{
   server->set_player_pos_info(idx,val);
}

unsigned int ua_debug_client_interface::get_player_attr(unsigned int idx)
{
   return server->get_player_attr(idx);
}

void ua_debug_client_interface::set_player_attr(unsigned int idx, unsigned int val)
{
   server->set_player_attr(idx,val);
}

unsigned int ua_debug_client_interface::get_num_levels()
{
   return server->get_num_levels();
}

double ua_debug_client_interface::get_tile_height(unsigned int level, double xpos, double ypos)
{
   return server->get_tile_height(level,xpos,ypos);
}
