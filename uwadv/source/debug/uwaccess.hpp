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
   udc_nop=0,        //!< do nothing

   udc_lock=1,       //!< lock underworld object
   udc_unlock=2,     //!< unlock underworld

   udc_game_suspend=3, //!< suspends game
   udc_game_resume=4,  //!< resumes game

   udc_player_get=5, /*!< gets player value; param1.i holds array index;
                          allowed types:
                          0: get player info
                          1: get attribute
                          2: get skill */
   udc_player_set=6, /*!< sets player value; param1.uint holds index;
                          types are the same as in udc_player_get */

   udc_objlist_get=7, /*!< gets master object list value
                           "type" contains underworld level
                           param1 contains object list position to get
                           param2 contains value index to get:
                             0: item_id
                             1: link
                             2: quality
                             3: owner
                             4: quantity/sp_link
                             5: xpos
                             6: ypos
                             7: zpos
                             8: heading
                             9: flags
                             10: enchanted
                             11: is_quant
                             12: is_hidden */

   udc_objlist_set=8, /*!< sets master object list value
                           "type" contains "stage" in setting a value:
                            0: set list position (underworld level in param1,
                               object list position in param2)
                            1: set value (value index in param1, see
                               udc_objlist_get, value in param2) */

   udc_tilemap_get=9,  /*!< not implemented yet */
   udc_tilemap_set=10, /*!< not implemented yet */

   udc_strings_get=11, /*!< retrieves game string; block is determined by
                            param1, string number by param2 */
   udc_strings_set=12, /*!< not implemented yet */
};

//! parameter types
enum
{
   ua_param_unk=0,
   ua_param_int=1,
   ua_param_double=2,
   ua_param_string=3
};


// structs

//! parameter struct for command function
struct ua_debug_param
{
   // ctors
   ua_debug_param():type(ua_param_unk){}
   explicit ua_debug_param(unsigned int i){ set(i); }
   explicit ua_debug_param(double d){ set(d); }
   explicit ua_debug_param(const char* str){ set(str); }
   ~ua_debug_param(){ if (type==ua_param_string) delete val.str; }

   // set functions
   void set_int(unsigned int i){ val.i = i; type = ua_param_int; }
   void set(double d){ val.d = d; type = ua_param_double; }
   void set(const char* str)
   {
      if (type == ua_param_string) delete val.str;
      val.str = new char[strlen(str)+1];
      strcpy(val.str,str);
      type = ua_param_string;
   }

   // parameter variant
   union param
   {
      unsigned int i;
      double d;
      char* str;
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
   ua_uw_access_api();

   //! initialize access api object
   void init(ua_game_core_interface* core, ua_debug_interface* debug);

   //! does debugger processing (uwadv thread)
   void tick();

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

   //! indicates if a level change should be done next tick()
   bool change_level;
};

#endif
