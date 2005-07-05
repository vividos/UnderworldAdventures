/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2005 Underworld Adventures Team

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
/*! \file conv.hpp

   \brief conversation

*/
//! \ingroup conv

//@{

// include guard
#ifndef uwadv_conv_hpp_
#define uwadv_conv_hpp_

// needed includes
#include "codevm.hpp"
#include "dbgserver.hpp"

// forward references

class ua_basic_game_interface;
class ua_conv_code_callback;


// classes

class ua_basic_conversation: public ua_conv_code_vm
{
public:
   //! ctor
   ua_basic_conversation(){}

   //! inits basic conversation
   virtual void init(unsigned int conv_level, Uint16 conv_objpos,
      ua_basic_game_interface& game, ua_conv_code_callback* code_callback,
      std::vector<std::string>& localstrings);

   //! cleans up basic conversation
   virtual void done(ua_basic_game_interface& game);

   //! returns local string
   virtual std::string get_local_string(Uint16 str_nr);

   //! allocates new local string
   Uint16 alloc_string(const char* the_str);

protected:
   // virtual methods from ua_conv_code_vm
   virtual void imported_func(const char* funcname);
   virtual Uint16 get_global(const char* globname);
   virtual void set_global(const char* globname, Uint16 val);

protected:
   //! game interface
   ua_basic_game_interface* game;

   //! underworld level of conversation partner
   unsigned int conv_level;

   //! object position of conversation partner
   Uint16 conv_objpos;

   //! all current local strings
   std::vector<std::string> localstrings;
};


class ua_conversation_debug: public ua_basic_conversation, public ua_debug_code_interface
{
public:
   ua_conversation_debug();

   //! inits debuggable conversation
   virtual void init(unsigned int conv_level, Uint16 conv_objpos,
      ua_basic_game_interface& game, ua_conv_code_callback* code_callback,
      std::vector<std::string>& localstrings);

   //! cleans up debuggable conversation
   virtual void done(ua_basic_game_interface& game);

protected:
   // virtual methods from ua_debug_code_interface
   virtual ua_debug_code_debugger_type get_debugger_type();
   virtual void prepare_debug_info();
   virtual ua_debug_code_debugger_state get_debugger_state() const;
   virtual void set_debugger_state(ua_debug_code_debugger_state state);
   virtual ua_debug_code_debugger_command get_debugger_command() const;
   virtual void set_debugger_command(ua_debug_code_debugger_command command);
   virtual void get_current_pos(unsigned int& sourcefile_index, unsigned int& sourcefile_line,
      unsigned int& code_pos, bool& sourcefile_is_valid);
   virtual unsigned int get_num_sourcefiles() const;
   virtual unsigned int get_sourcefile_name(unsigned int index, char* buffer, unsigned int len);
   virtual unsigned int get_num_breakpoints();
   virtual void get_breakpoint_info(unsigned int breakpoint_index,
      unsigned int& sourcefile_index, unsigned int& sourcefile_line,
      unsigned int& code_pos, bool& visible);

protected:
   std::string temp_decompile;

   ua_debug_code_debugger_state state;
   ua_debug_code_debugger_command command;
};


#endif
//@}
