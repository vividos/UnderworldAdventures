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
class ua_basic_game_interface;


// constants

//! current interface version
const unsigned int ua_debug_server_interface_version = 1;


// enums

//! debug server message types; usable in ua_debug_server_message::msg_type
enum ua_debug_server_message_type
{
   //! a code debugger message
   /*!; msg_arg1 contains the code debugger handle */
   ua_msg_code_debugger=0,

   //! a message to shutdown the debugger
   ua_msg_shutdown,

   //! miscellaneous message
   ua_msg_misc,
};


// structs

//! debug server message that gets sent to client
struct ua_debug_server_message
{
   //! ctor
   ua_debug_server_message()
      :msg_type(0), msg_arg1(0), msg_arg2(0), msg_arg3(0.0){}

   //! message type; see enum ua_debug_server_message_type
   unsigned int msg_type;

   //! message argument 1
   unsigned int msg_arg1;
   //! message argument 2
   unsigned int msg_arg2;
   //! message argument 3
   double msg_arg3;

   //! message text
   std::string msg_text;
};


// classes

//! code debugger interface
/*!
*/
class ua_debug_code_debugger
{
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

   //! locks/unlocks underworld
   virtual void lock(bool set_lock)=0;

   // messaging stuff

   //! returns number of messages in the message queue
   virtual unsigned int get_message_num()=0;

   //! returns current message
   bool get_message(ua_debug_server_message& msg);

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

protected:
   // methods only callable for ua_debug_server

   //! adds message to queue; only debug server should call this!
   virtual void add_message(ua_debug_server_message& msg)=0;

   friend class ua_debug_server;
};


// inline methods

inline bool ua_debug_server_interface::get_message(ua_debug_server_message& msg)
{
   unsigned int text_size=0;
   bool ret = get_message(msg.msg_type, msg.msg_arg1, msg.msg_arg2,
      msg.msg_arg3, text_size);
   if (ret && text_size>0)
   {
      std::vector<char> buffer(text_size+1);
      ret = get_message_text(&buffer[0], text_size);

      buffer[text_size]=0;
      msg.msg_text.assign(&buffer[0]);
   }
   return ret;
}


#endif
//@}
