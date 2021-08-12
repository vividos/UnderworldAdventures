//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file CodeVM.hpp
/// \brief conversation code virtual machine
///
/// much stuff for underworld conversations
///
/// for more info about the underworld conversation assembler script, look in
/// the file docs/uw-formats.txt
///
/// a gnu-gdb style debugger named "cnvdbg" and a disassembler and experimental
/// decompiler (produces C-like code) is available in the "tools" folder.
///
//
#pragma once

#include <string>
#include <vector>
#include <random>
#include "Savegame.hpp"
#include "ConvStack.hpp"
#include "ConvGlobals.hpp"

/// \brief Conversation namespace
namespace Conv
{
   /// function types
   enum DataType
   {
      dataTypeUnknown,  ///< unknown data type
      dataTypeVoid,     ///< void data type
      dataTypeInt,      ///< integer data type
      dataTypeString,   ///< string data type
   };

   /// imported item info
   struct ImportedItem
   {
      /// type of the function/global memory location
      DataType datatype;

      /// name of imported item
      std::string name;

   };

   class ICodeCallback
   {
   public:
      /// ctor
      virtual ~ICodeCallback() {}

      /// prints "say" string
      virtual void Say(Uint16 index) = 0;

      /// shows menu, with possible answers
      virtual Uint16 BablMenu(const std::vector<Uint16>& answerStringIds) = 0;

      /// executes external function
      virtual Uint16 ExternalFunc(const char* funcname, ConvStack& stack) = 0;
   };

   /// \brief Conversation code virtual machine
   /// \details Virtual machine to run conversation code loaded from cnv.ark files.
   /// It emulates a forth-like stack-based opcode language with intrinsic functions
   /// (called imported functions). The code segment contains 16-bit opcodes that
   /// are executed one by one using the Step() method. It returns false when the
   /// execution is stopped due to an error or when conversation has finished.
   /// The class uses the ICodeCallback to let the user respond to
   /// higher-level actions in the virtual machine.
   ///
   /// The conversation code first has to be loaded using
   /// Import::LoadConvCode(), then Init() can be called. When exiting,
   /// Done() should be called to write back conversation globals for the given
   /// conversation.
   class CodeVM
   {
   public:
      /// ctor
      CodeVM();
      /// dtor
      virtual ~CodeVM() {}

      /// inits virtual machine after filling code segment
      void Init(ICodeCallback* codeCallback, const Underworld::ConvGlobals& globals);

      /// does a step in code; returns false when program has stopped
      bool Step();

      /// writes back conv globals
      void Done(Underworld::ConvGlobals& globals);

      /// replaces all @ placeholder in the given string
      void ReplacePlaceholder(std::string& text);

      /// allocates new local string
      Uint16 AllocString(const char* text);

      // get functions

      /// returns code segment
      std::vector<Uint16>& GetCodeSegment() { return m_code; }

      /// returns instruction pointer
      Uint16 GetInstructionPointer() const { return m_instructionPointer; }

      /// returns map with imported functions
      std::map<Uint16, ImportedItem>& GetImportedFunctions() { return m_mapImportedFunctions; }

      /// returns map with imported globals
      std::map<Uint16, ImportedItem>& GetImportedGlobals() { return  m_mapImportedGlobals; }

      /// returns string block to use for this conversation
      Uint16 GetStringBlock() const { return m_stringBlock; }

      /// returns number of reserved global variables
      Uint16 GetReservedGlobals() const { return m_reservedGlobals; }

      /// returns local string value
      std::string GetLocalString(Uint16 stringIndex);

      // set functions

      /// sets result register
      void SetResultRegister(Uint16 resultValue);

      /// sets conversation slot
      void SetConversationSlot(Uint16 conversationSlot) { m_conversationSlot = conversationSlot; }

      /// sets string block to use
      void SetStringBlock(Uint16 stringBlock) { m_stringBlock = stringBlock; }

      /// sets number of globals reserved at start of memory
      void SetReservedGlobals(Uint16 numGlobals) { m_reservedGlobals = numGlobals; }

   protected:
      /// called when saying a string
      void SayOp(Uint16 stringNumber);

      /// executes an imported function
      virtual void ImportedFunc(const char* functionName);

      /// queries for a global variable value
      virtual Uint16 GetGlobal(const char* globalName);

      /// sets global variable value
      virtual void SetGlobal(const char* globalName, Uint16 value);

      /// called when storing a value to the stack
      void StoreValue(Uint16 at, Uint16 value);

      /// called when fetching a value from stack
      void FetchValue(Uint16 at);

   protected:
      /// conversation slot number
      Uint16 m_conversationSlot;

      /// number of string block to use
      Uint16 m_stringBlock;

      /// code segment
      std::vector<Uint16> m_code;

      /// instruction pointer
      Uint16 m_instructionPointer;

      /// base (frame) pointer
      Uint16 m_basePointer;

      /// stack
      ConvStack m_stack;

      /// number of values for globals reserved on stack
      Uint16 m_reservedGlobals;

      /// tracks call/ret level
      unsigned int m_callLevel;

      /// result register for (imported) functions
      Uint16 m_resultRegister;

      /// indicates if conversation has finished
      bool m_finished;

      /// all imported functions
      std::map<Uint16, ImportedItem> m_mapImportedFunctions;

      /// names of all imported globals
      std::map<Uint16, ImportedItem> m_mapImportedGlobals;

      /// local strings
      std::vector<std::string> m_localStrings;

      /// code callback pointer
      ICodeCallback* m_codeCallback;

      /// random number generator
      std::mt19937 m_rng;
   };

} // namespace Conv
