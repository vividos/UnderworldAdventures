/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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

   ua_conv_globals contains all conversation globals for each conversation
   slot and should be persisted when conversation or game exits.

   ua_conv_stack implements the stack used in the virtual machine. it does
   range checking and throws one of the exceptions in ua_conv_vm_exception
   when needed.

   ua_conv_code_vm is the virtual machine the whole code excutes. the function
   ua_conv_code_vm::step() executes one opcode instruction and can be executed
   in a loop to run the code. an exception of type ua_conv_vm_exception is
   thrown on error. the virtual functions are called when a special event is
   happening, such as printing a say string.

   for more info about the underworld conversation assembler script, look in
   the file docs/uw-formats.txt

   a gnu-gdb style debugger named "cnvdbg" and a disassembler and experimental
   decompiler (produces C-like code) is available in the "tools" folder.

*/

// include guard
#ifndef __uwadv_codevm_hpp_
#define __uwadv_codevm_hpp_

// needed includes
#include <string>
#include <vector>
#include "savegame.hpp"


// enums

//! imported function return type
typedef enum
{
   ua_rt_void, ua_rt_int, ua_rt_string
} ua_conv_ret_type;


//! conv code virtual machine code exceptions
typedef enum
{
   ua_ex_error_loading, // error while loading
   ua_ex_div_by_zero,   // division by 0
   ua_ex_code_access,   // invalid code segment access
   ua_ex_globals_access,// invalid globals access
   ua_ex_stack_access,  // invalid stack access
   ua_ex_unk_opcode,    // unknown opcode
   ua_ex_imported_na,   // imported function not available
} ua_conv_vm_exception;


// structs

//! imported item info
typedef struct
{
   //! type of the function/global memory location
   ua_conv_ret_type ret_type;

   // name of imported item
   std::string name;

} ua_conv_imported_item;


// classes

//! stores all conversation globals
class ua_conv_globals
{
public:
   //! ctor
   ua_conv_globals(){}

   //! returns a list of globals for a given conv slot
   std::vector<Uint16> &get_globals(Uint16 conv)
   {
      if (conv>allglobals.size()) throw ua_ex_globals_access;
      return allglobals[conv];
   }

   // loading / saving / importing

   //! loads a savegame
   void load_game(ua_savegame &sg);

   //! saves to a savegame
   void save_game(ua_savegame &sg);

   //! imports a globals file; init=true: load file without actual globals
   void import(const char *bgname, bool init);

protected:
   //! list with all globals from all conversations
   std::vector< std::vector<Uint16> > allglobals;
};


//! conversation code stack
class ua_conv_stack
{
public:
   ua_conv_stack(){ stackp = 0xffff; };

   //! reserves stack space
   void init(Uint16 stacksize){ stack.clear(); stackp = 0xffff; stack.resize(stacksize,0); }

   //! pushes a value onto the stack
   void push(Uint16 val)
   {
      if (Uint16(stackp+1)>stack.size())
         throw ua_ex_stack_access;
      stack[++stackp] = val;
   }

   //! pops a value from the stack
   Uint16 pop()
   {
      if (stackp>stack.size()) throw ua_ex_stack_access;
      return stack[stackp--];
   }

   //! returns value at stack position
   Uint16 at(Uint16 pos)
   {
      if (pos>stack.size()) throw ua_ex_stack_access;
      return stack[pos];
   }

   //! returns value at stack position
   void set(Uint16 pos, Uint16 val)
   {
      if (pos>stack.size()) throw ua_ex_stack_access;
      stack[pos]=val;
   }

   //! gets new stack pointer
   Uint16 get_stackp(){ return stackp; }

   //! sets new stack pointer
   void set_stackp(Uint16 val)
   {
      if (val>stack.size()) throw ua_ex_stack_access;
      stackp = val;
   }

protected:
   //! stack pointer; always points to top element of stack
   Uint16 stackp;

   //! the stack itself
   std::vector<Uint16> stack;
};


//! conversation code virtual machine
/*! the order to call the member functions is: load_code(), then init(),
    then step() (maybe in a for loop), and when done, done() */
class ua_conv_code_vm
{
public:
   //! ctor
   ua_conv_code_vm();
   //! dtor
   virtual ~ua_conv_code_vm();

   //! conv code loader; returns false if there is no conversation slot
   bool load_code(const char *cnvfile, Uint16 conv);

   //! inits virtual machine after filling code segment
   void init(ua_conv_globals &cg, std::vector<std::string>& stringblock);

   //! does a step in code
   void step() throw(ua_conv_vm_exception);

   //! writes back conv globals
   void done(ua_conv_globals &cg);

   //! replaces all @ placeholder in the given string
   void replace_placeholder(std::string& str);

   //! allocates new string on local strings heap and returns handle
   Uint16 alloc_string(const std::string& str);

   // virtual functions

   //! called when calling an imported function
   virtual void imported_func(const std::string& funcname);

   //! called when saying a string
   virtual void say_op(Uint16 str_id);

   //! queries for a global variable value
   virtual Uint16 get_global(const std::string& globname);

   //! sers global variable value
   virtual void set_global(const std::string& globname, Uint16 val);

   //! called when storing a value at the stack
   virtual void store_value(Uint16 at, Uint16 val);

   //! called when fetching a value from stack
   virtual void fetch_value(Uint16 at);

protected:
   //! reads all imported function entries
   void load_imported_funcs(FILE *fd);

protected:
   //! number of conversation
   Uint16 conv_nr;

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

   // all imported functions
   std::map<Uint16,ua_conv_imported_item> imported_funcs;

   // names of all imported globals
   std::map<Uint16,ua_conv_imported_item> imported_globals;

   //! all current local strings
   std::vector<std::string> localstrings;
};

#endif
