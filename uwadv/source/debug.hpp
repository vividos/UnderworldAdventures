/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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

   \brief debug interface

*/

// include guard
#ifndef uwadv_debug_hpp_
#define uwadv_debug_hpp_

// needed includes


// forward references
class ua_game_core_interface;
class ua_underworld;


// classes

//! debug interface
class ua_debug_interface
{
public:
   //! returns a new debug interface
   static ua_debug_interface* get_new_debug_interface(
      ua_game_core_interface* core);

   //! starts visual debugger
   virtual void start_debugger()=0;

   //! locks underworld object
   virtual void lock()=0;

   //! unlocks underworld object
   virtual void unlock()=0;


   //! dtor
   virtual ~ua_debug_interface(){}

protected:
   //! ctor
   ua_debug_interface(){}
};

#endif
