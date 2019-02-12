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
class IBasicGame;


// constants

//! current interface version
const unsigned int ua_debug_server_interface_version = 1;


// enums

//! debug server message types; usable in ua_debug_server_message::msg_type
enum ua_debug_server_message_type
{
   //! a code debugger message
//   ua_msg_code_debugger=0,

   //! a message to shutdown the debugger
   ua_msg_shutdown=0,

   //! a code debugger has started
   /*! msg_arg1 contains the code debugger handle */
   ua_msg_start_code_debugger,

   //! a code debugger has ended
   /*! msg_arg1 contains the code debugger handle */
   ua_msg_end_code_debugger,

   //! a code debugger has updated his debug state
   /*! msg_arg1 contains the code debugger handle */
   ua_msg_code_debugger_state_update,

   //! miscellaneous message
   ua_msg_misc,

   //! unknown message
   ua_msg_unknown,
};

//! debug server flags, used in ua_debug_server_interface::get_flag()
enum ua_debug_server_flags
{
   ua_flag_is_studio_mode=0, //!< is in studio mode?
};

//! tile info type for get_tile_info_value() and set_tile_info_value()
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

//! tile info type for get_tile_info_value() and set_tile_info_value()
enum ua_debug_code_debugger_type
{
   ua_code_debugger_uwconv=0, //!< uw conversation script
   ua_code_debugger_lua,      //!< Lua script
};


enum ua_debug_code_debugger_state
{
   ua_debugger_state_inactive=0,
   ua_debugger_state_running,
   ua_debugger_state_break,
};

//! code debugger command
enum ua_debug_code_debugger_command
{
   ua_debugger_command_run=0, //!< continue running code until next breakpoint or end
   ua_debugger_command_step_over, //!< continue with one line of code, not stepping into functions
   ua_debugger_command_step_into, //!< continue with one line of code, stepping into functions
   ua_debugger_command_step_out, //!< continue to the function above
};


//! code location
struct ua_debug_code_location
{
   ua_debug_code_location(): sourcefile_index(unsigned(-1)),
      sourcefile_line(0), code_pos(unsigned(-1)){}

   unsigned int sourcefile_index;
   unsigned int sourcefile_line;
   unsigned int code_pos;
};


//! breakpoint informations
struct ua_debug_code_breakpoint_info
{
   ua_debug_code_breakpoint_info():visible(true){}

   //! breakpoint location
   ua_debug_code_location pos;

   bool visible; //!< indicates if breakpoint is visible to debugger

   //unsigned int count_before_trigger;
   //std::string condition;
};


// classes

//! code debugger interface
/*!
*/
class ua_debug_code_interface
{
public:
   //! returns code debugger type
   virtual ua_debug_code_debugger_type get_debugger_type()=0;

   //! called by client to let the code debugger prepare debug info
   virtual void prepare_debug_info()=0;

   //! returns current debugger state
   virtual ua_debug_code_debugger_state get_debugger_state() const=0;

   //! sets new debugger state
   virtual void set_debugger_state(ua_debug_code_debugger_state state)=0;

   //! returns current debugger command
   virtual ua_debug_code_debugger_command get_debugger_command() const=0;

   //! sets new debugger command
   virtual void set_debugger_command(ua_debug_code_debugger_command command)=0;

   //! returns current position
   virtual void get_current_pos(unsigned int& sourcefile_index, unsigned int& sourcefile_line,
      unsigned int& code_pos, bool& sourcefile_is_valid)=0;

   //! returns number of source files
   virtual unsigned int get_num_sourcefiles() const=0;

   //! returns sourcefile name by index
   virtual unsigned int get_sourcefile_name(unsigned int index, char* buffer, unsigned int len)=0;

   //! returns number of breakpoints
   virtual unsigned int get_num_breakpoints()=0;

   //! returns breakpoint info for a given breakpoint index
   virtual void get_breakpoint_info(unsigned int breakpoint_index,
      unsigned int& sourcefile_index, unsigned int& sourcefile_line,
      unsigned int& code_pos, bool& visible)=0;

protected:
   //! sets debugger id
   void set_debugger_id(unsigned int the_debugger_id){ debugger_id = the_debugger_id;}

   //! code debugger id
   unsigned int debugger_id;

   friend class ua_debug_server;
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

   //! pauses or unpauses game; returns previous game state
   virtual bool pause_game(bool pause)=0;

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
   virtual unsigned int GetNumLevels()=0;

   //! returns tile height at given coordinates
   virtual double get_tile_height(unsigned int level, double xpos,
      double ypos)=0;

   //! returns tile info value
   virtual unsigned int get_tile_info_value(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type)=0;

   //! sets tile info value
   virtual void set_tile_info_value(unsigned int level,
      unsigned int xpos, unsigned int ypos, unsigned int type,
      unsigned int val)=0;

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

   // code debugger

   //! retrieves code debugger interface by debugger id
   virtual ua_debug_code_interface* get_code_debugger(unsigned int debugger_id)=0;
};


#endif
//@}
