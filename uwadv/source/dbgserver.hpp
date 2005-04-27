/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004,2005 Underworld Adventures Team

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
class ua_basic_game_interface;


// constants

//! current interface version
const unsigned int ua_debug_server_interface_version = 1;


// enums

//! debug server message types; usable in ua_debug_server_message::msg_type
enum ua_debug_server_message_type
{
   //! a message to shutdown the debugger
   ua_msg_shutdown,

   //! a code debugger has started
   /*! msg_arg1 contains the code debugger handle */
   ua_msg_start_code_debugger,

   //! a code debugger has ended
   /*! msg_arg1 contains the code debugger handle */
   ua_msg_end_code_debugger,

   //! miscellaneous message
   ua_msg_misc,
};

//! debug server flags, used in ua_debug_server_interface::get_flag()
enum ua_debug_server_flags
{
   ua_flag_is_studio_mode=0, //!< is in studio mode?
};

//! tile info type for get_tile_info_value
enum ua_debug_server_tile_info
{
   ua_tile_info_type=0,
   ua_tile_info_floor_height,
   ua_tile_info_ceiling_height,
   ua_tile_info_slope,
   ua_tile_info_tex_wall,
   ua_tile_info_tex_floor,
   ua_tile_info_tex_ceil,
   ua_tile_info_objlist_start
};

//! object list info type for get_objlist_info
enum ua_debug_server_objlist_info
{
   ua_objlist_info_item_id=0,
   ua_objlist_info_link,
   ua_objlist_info_quality,
   ua_objlist_info_owner,
   ua_objlist_info_quantity,
   ua_objlist_info_xpos,
   ua_objlist_info_ypos,
   ua_objlist_info_zpos,
   ua_objlist_info_heading,
   ua_objlist_info_flags,
   ua_objlist_info_ench,
   ua_objlist_info_is_quant,
   ua_objlist_info_hidden,
};


// classes

//! code debugger interface
/*!
*/
class ua_debug_code_interface
{
public:
//   virtual unsigned int get_num_breakpoints()=0;
};


//! debug server interface definition
/*! All methods of this interface class are virtual, so that they can be
    called via the vtable, from the uadebug shared library.
*/
class ua_debug_server_interface
{
public:
   //! dtor
   virtual ~ua_debug_server_interface(){}

   // methods called from debug client

   //! checks interface version
   /*! checks interface version used in debug server; when interface used by
       the client is different from the version used by the server, the
       debugger mustn't proceed */
   virtual bool check_interface_version(
      unsigned int interface_ver=ua_debug_server_interface_version)=0;

   //! returns server flag
   virtual unsigned int get_flag(unsigned int flag_id)=0;

   //! returns current game path, or "" when none
   virtual unsigned int get_game_path(char* buffer, unsigned int bufsize)=0;

   //! loads new game, or unloads game when "" is set as path
   virtual void load_game(const char* path)=0;

   //! locks/unlocks underworld
   virtual void lock(bool set_lock)=0;

   // messaging stuff

   //! returns number of messages in the message queue
   virtual unsigned int get_message_num()=0;

   //! returns current message
   virtual bool get_message(unsigned int& msg_type,
      unsigned int& msg_arg1, unsigned int& msg_arg2, double& msg_arg3,
      unsigned int& msg_text_size)=0;

   //! returns message text of current message
   virtual bool get_message_text(char* buffer, unsigned int bufsize)=0;

   //! removes current message
   virtual bool pop_message()=0;

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

   //! returns tile info
   virtual unsigned int get_tile_info_value(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type)=0;

   //! returns master object list info
   virtual unsigned int get_objlist_info(unsigned int level,
      unsigned int pos, unsigned int type)=0;

   //! sets master object list info
   virtual void set_objlist_info(unsigned int level,
      unsigned int pos, unsigned int type, unsigned int value)=0;

   // game strings stuff

   //! enumerates game string blocks
   virtual bool enum_gamestr_block(unsigned int index,
      unsigned int& blocknum)=0;

   //! returns number of strings in the given block
   virtual unsigned int get_gamestr_blocksize(unsigned int block)=0;

   //! returns game string
   virtual unsigned int get_game_string(unsigned int block, unsigned int nr,
      char* buffer, unsigned int maxsize)=0;

   //! retrieves an object list imagelist in 32-bit RGBA format
   virtual bool get_object_list_imagelist(unsigned int& num_objects,
      unsigned char* buffer, unsigned int size)=0;
};


#endif
//@}
