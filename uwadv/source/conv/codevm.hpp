/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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

// include guard
#ifndef uwadv_codevm_hpp_
#define uwadv_codevm_hpp_

// needed includes
#include <string>
#include <vector>
#include "savegame.hpp"
#include "convstack.hpp"
#include "convglobals.hpp"


// forward references
class ua_conv_code_vm;


// enums

//! function types
enum ua_conv_datatype
{
   ua_rt_void,
   ua_rt_int,
   ua_rt_string
};

//! conv code virtual machine code exceptions
typedef enum
{
   ua_ex_error_loading, //!< error while loading
   ua_ex_div_by_zero,   //!< division by 0
   ua_ex_code_access,   //!< invalid code segment access
   ua_ex_globals_access,//!< invalid globals access
   ua_ex_stack_access,  //!< invalid stack access
   ua_ex_unk_opcode,    //!< unknown opcode
   ua_ex_imported_na,   //!< imported function not available
} ua_conv_vm_exception;


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


//! conversation code virtual machine
/*! ua_conv_code_vm is the virtual machine the whole code excutes. The
    method step() executes one opcode instruction and can be executed in a
    loop to run the code. An exception of type ua_conv_vm_exception is thrown
    on error. The virtual functions are called when a special event is
    happening, such as printing a "say" string.

    The order to call the member functions is: load_code(), then init(),
    then step() (maybe in a for loop), and when done, done()
*/
class ua_conv_code_vm
{
public:
   //! ctor
   ua_conv_code_vm();
   //! dtor
   virtual ~ua_conv_code_vm();

   //! conv code loader; returns false if there is no conversation slot
//   bool load_code(const char* cnvfile, Uint16 conv);

   //! inits virtual machine after filling code segment
   void init(ua_conv_code_callback* code_callback, ua_conv_globals& cg);

   //! does a step in code
   void step();

   //! writes back conv globals
   void done(ua_conv_globals& cg);

   //! replaces all @ placeholder in the given string
   void replace_placeholder(std::string& str);

   //! sets result register
   void set_result_register(Uint16 val);

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
   //! reads all imported function entries
//   void load_imported_funcs(FILE *fd);

protected:
   //! number of conversation slot
   Uint16 conv_slot;

   //! number of string block to use
   Uint16 strblock;

   //! size of code
   Uint16 codesize;

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
