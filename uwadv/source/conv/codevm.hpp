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

   a

*/

// include guard
#ifndef __uwadv_codevm_hpp_
#define __uwadv_codevm_hpp_

// needed includes
#include <vector>


// enums

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
} ua_conv_vm_exception;


// structs

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

   //! loads a globals file; init=true: load file without actual globals
   void load(const char *bgname, bool init);

   //! returns a list of globals for a given conv slot
   std::vector<Uint8> &get_globals(Uint16 conv)
   {
      if (conv>allglobals.size()) throw ua_ex_globals_access;
      return allglobals.at(conv);
   }

protected:
   //! list with all globals from all conversations
   std::vector< std::vector<Uint8> > allglobals;
};


//! conversation code stack
class ua_conv_stack
{
public:
   ua_conv_stack(){ stackp = -1; };

   //! reserves stack space
   void init(Uint16 stacksize){ stack.clear(); stackp = -1; stack.resize(stacksize,0); }

   //! pushes a value onto the stack
   void push(Uint16 val){ stack.at(++stackp) = val; }

   //! pops a value from the stack
   Uint16 pop()
   {
      if (stackp>stack.size()) throw ua_ex_stack_access;
      return stack.at(stackp--);
   }

   //! returns value at stack position
   Uint16 at(Uint16 pos)
   {
      if (pos>stack.size()) throw ua_ex_stack_access;
      return stack.at(pos);
   }

   //! returns value at stack position
   void set(Uint16 pos, Uint16 val)
   {
      if (pos>stack.size()) throw ua_ex_stack_access;
      stack.at(pos)=val;
   }

   //! gets new stack pointer
   Uint16 get_stackp(){ return stackp; }

   //! sets new stack pointer
   void set_stackp(Uint16 val)
   {
      if (stackp>stack.size()) throw ua_ex_stack_access;
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
   void init(ua_conv_globals &cg);

   //! does a step in code
   void step() throw(ua_conv_vm_exception);

   //! writes back conv globals
   void done(ua_conv_globals &cg);

   // virtual functions

   //! called when calling an imported function
   virtual void imported_func(Uint16 number);

   //! called when saying a string
   virtual void say_op(Uint16 str_id);

   //! called when storing a value at private globals
   virtual void sto_priv(Uint16 at, Uint16 val);

   //! called when fetching a value from private globals
   virtual void fetchm_priv(Uint16 at);

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
   std::vector<ua_conv_imported_item> imported_funcs;

   // names of all imported globals
   std::vector<ua_conv_imported_item> imported_globals;
};

#endif
