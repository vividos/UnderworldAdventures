/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file uwaccess.hpp

   \brief underworld access API

*/

// include guard
#ifndef __uadebug_uwaccess_hpp__
#define __uadebug_uwaccess_hpp__

// needed includes


// structs

//! ua_underworld access api function struct
struct ua_uw_access_api
{
   //! gets/sets a player double value
   void (*player_value)(ua_debug_interface* inter,
      bool set, unsigned int index, double& value);

   void (*player_attribute)(ua_debug_interface* inter,
      bool set, unsigned int index, unsigned int& value);

   void (*player_skill)(ua_debug_interface* inter,
      bool set, unsigned int index, unsigned int& value);

   //! inits access api struct; must be the last member in the struct
   void init();

};

#endif
