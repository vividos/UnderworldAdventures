//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2005,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file conv.hpp
/// \brief conversation
//
#pragma once

#include "codevm.hpp"
//#include "dbgserver.hpp"

class IBasicGame;

namespace Conv
{
   class ICodeCallback;

   /// \brief conversation
   /// \details implements conversation using the Underworld object
   /// to implement imported functions and globals
   class Conversation : public CodeVM
   {
   public:
      /// ctor
      Conversation()
         :m_game(NULL),
         m_conversationLevel(0),
         m_conversationObjectPos(0)
      {
      }

      /// inits basic conversation
      virtual void Init(unsigned int conversationLevel, Uint16 conversationObjectPos,
         IBasicGame& game, ICodeCallback* codeCallback,
         std::vector<std::string>& localStrings);

      /// cleans up basic conversation
      virtual void Done(IBasicGame& game);

   protected:
      // virtual methods from CodeVM
      virtual void ImportedFunc(const char* funcname);
      virtual Uint16 GetGlobal(const char* globname);
      virtual void SetGlobal(const char* globname, Uint16 val);

   protected:
      /// game interface
      IBasicGame* m_game;

      /// underworld level of conversation partner
      unsigned int m_conversationLevel;

      /// object position of conversation partner
      Uint16 m_conversationObjectPos;
   };

/*
   /// \todo move to own file
   class ua_conversation_debug : public Conversation, public ua_debug_code_interface
   {
   public:
      ua_conversation_debug();

      /// inits debuggable conversation
      virtual void Init(unsigned int conv_level, Uint16 conv_objpos,
         IBasicGame& game, ICodeCallback* m_codeCallback,
         std::vector<std::string>& localStrings);

      /// cleans up debuggable conversation
      virtual void Done(IBasicGame& game);

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
*/

} // namespace Conv
