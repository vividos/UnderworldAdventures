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
/*! \file uwaccess.hpp

   \brief underworld access API

*/

// include guard
#ifndef uadebug_uwaccess_hpp_
#define uadebug_uwaccess_hpp_

// needed includes


// forward references
class ua_game_core_interface;
class ua_debug_interface;


// enums

//! uadebug commands
enum
{
   udc_nop=0,
   udc_lock=1,       //!< lock underworld object
   udc_unlock=2,     //!< unlock underworld

   udc_player_get=3, /*!< gets player value; param1.uint holds array index;
                          allowed types:
                          0: get player info
                          0: get attribute
                          1: get skill */
   udc_player_set=4, /*!< sets player value; param1.uint holds index;
                          types are the same as in udc_player_get */
};

//! parameter types
enum
{
   ua_param_unk=0,
   ua_param_int=1,
   ua_param_double=2
};


// structs

//! parameter struct for command function
struct ua_debug_param
{
   ua_debug_param(){ type = ua_param_unk; };
   explicit ua_debug_param(unsigned int i){ set(i); }
   explicit ua_debug_param(double d){ set(d); }

   void set(unsigned int i){ val.i = i; type = ua_param_int; }
   void set(double d){ val.d = d; type = ua_param_double; }

   // parameter variant
   union param
   {
      unsigned int i;
      double d;
      //void* v;
   } val;

   //! parameter type
   unsigned int type;
};


// typedefs

//! command func to pass commands from debugger to uwadv
typedef unsigned int (*ua_debug_command_func)(
   unsigned int cmd, unsigned int type,
   ua_debug_param* param1, ua_debug_param* param2);


// classes

//! underworld access api
class ua_uw_access_api
{
public:
   //! ctor
   ua_uw_access_api(){}

   //! initialize access api object
   void init(ua_game_core_interface* core, ua_debug_interface* debug);

   //! returns command function
   ua_debug_command_func get_command_func(){ return command_func; }

protected:
   //! command function
   static unsigned int command_func(
      unsigned int cmd, unsigned int type,
      ua_debug_param* param1, ua_debug_param* param2);

   //! current api object
   static ua_uw_access_api* cur_api;

   //! game core object
   ua_game_core_interface* core;

   //! currently used debug interface
   ua_debug_interface* debug;
};

#endif
