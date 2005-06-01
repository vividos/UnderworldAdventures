/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004,2005 Underworld Adventures Team

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
/*! \file codevm.hpp

   \brief conversation code virtual machine

   much stuff for underworld conversations

   for more info about the underworld conversation assembler script, look in
   the file docs/uw-formats.txt

   a gnu-gdb style debugger named "cnvdbg" and a disassembler and experimental
   decompiler (produces C-like code) is available in the "tools" folder.

*/
/*! \defgroup conv Conversation Code Virtual Machine

   conversation documentation yet to come ...

*/
//@{

// include guard
#ifndef uwadv_codevm_hpp_
#define uwadv_codevm_hpp_

// needed includes
#include <string>
#include <vector>
#include "savegame.hpp"
#include "convstack.hpp"
#include "convglobals.hpp"
#include "dbgserver.hpp"


// enums

//! function types
enum ua_conv_datatype
{
   ua_dt_unknown,
   ua_dt_void,
   ua_dt_int,
   ua_dt_string,
};


// structs

//! imported item info
typedef struct
{
   //! type of the function/global memory location
   ua_conv_datatype datatype;

   //! name of imported item
   std::string name;

} ua_conv_imported_item;


// classes

class ua_conv_code_callback
{
public:
   //! ctor
   ua_conv_code_callback(){}

   //! prints "say" string
   virtual void say(Uint16 index)=0;

   //! returns string in local string block
   virtual const char* get_local_string(Uint16 index)=0;

   //! executes external function
   virtual Uint16 external_func(const char* funcname, ua_conv_stack& stack)=0;

   //! returns global value
   virtual Uint16 get_global(const char* globname)=0;

   //! sets global value
   virtual void set_global(const char* globname, Uint16 val)=0;
};


//! Conversation code virtual machine
/*! Virtual machine to run conversation code loaded from cnv.ark files. It
    emulates a forth-like stack-based opcode language with intrinsic functions
    (called imported functions). The code segment contains 16-bit opcodes that
    are executed one by one using the step() method. It returns false when the
    execution is stopped due to an error or when conversation has finished.
    The class uses the ua_conv_code_callback to let the user respond to
    higher-level actions in the virtual machine.

    The conversation code first has to be loaded using
    ua_uw_import::load_conv_code(), then init() can be called. When exiting,
    done() should be called to write back conversation globals for the given
    conversation.
*/
class ua_conv_code_vm: public ua_debug_code_interface
{
public:
   //! ctor
   ua_conv_code_vm();
   //! dtor
   virtual ~ua_conv_code_vm();

   //! inits virtual machine after filling code segment
   void init(ua_conv_code_callback* code_callback, ua_conv_globals& cg);

   //! does a step in code; returns false when program has stopped
   bool step();

   //! writes back conv globals
   void done(ua_conv_globals& cg);

   //! replaces all @ placeholder in the given string
   void replace_placeholder(std::string& str);

   //! sets result register
   void set_result_register(Uint16 val);

   // access functions

   //! returns code segment
   std::vector<Uint16>& get_code_segment(){ return code; }

   //! returns map with imported functions
   std::map<Uint16,ua_conv_imported_item>& get_imported_funcs(){ return imported_funcs; }
   //! returns map with imported globals
   std::map<Uint16,ua_conv_imported_item>& get_imported_globals(){ return  imported_globals; }

   //! returns string block to use for this conversation
   Uint16 get_strblock(){ return strblock; }

   //! sets conversation slot
   void set_conv_slot(Uint16 the_conv_slot){ conv_slot = the_conv_slot; }

   //! sets string block to use
   void set_strblock(Uint16 stringblock){ strblock = stringblock; }

   //! sets number of globals reserved at start of memory
   void set_globals_reserved(Uint16 numglobals){ glob_reserved = numglobals; }

   Uint16 get_globals_reserved(){ return glob_reserved; }

protected:
   //! called when an imported function is executed
   void imported_func(const std::string& funcname);

   //! called when saying a string
   void say_op(Uint16 str_id);

   //! executes an imported function
   void imported_func(const char* funcname);

   //! queries for a global variable value
   Uint16 get_global(const char* globname);

   //! sers global variable value
   void set_global(const char* globname, Uint16 val);

   //! called when storing a value to the stack
   void store_value(Uint16 at, Uint16 val);

   //! called when fetching a value from stack
   void fetch_value(Uint16 at);

protected:
   //! conversation slot number
   Uint16 conv_slot;

   //! number of string block to use
   Uint16 strblock;

   //! code bytes
   std::vector<Uint16> code;

   //! instruction pointer
   Uint16 instrp;

   //! base (frame) pointer
   Uint16 basep;

   //! stack
   ua_conv_stack stack;

   //! number of values for globals reserved on stack
   Uint16 glob_reserved;

   //! tracks call/ret level
   unsigned int call_level;

   //! result register for (imported) functions
   Uint16 result_register;

   //! indicates if conversation has finished
   bool finished;

   //! all imported functions
   std::map<Uint16,ua_conv_imported_item> imported_funcs;

   //! names of all imported globals
   std::map<Uint16,ua_conv_imported_item> imported_globals;

   //! code callback pointer
   ua_conv_code_callback* code_callback;
};


#endif
//@}
