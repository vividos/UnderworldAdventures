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
/*! \file convstack.hpp

   \brief conversation code stack

*/

// include guard
#ifndef uwadv_convstack_hpp_
#define uwadv_convstack_hpp_

// needed includes


//! Conversation code stack
/*! Implements a stack that is used in the conversation code virtual machine.
    The stack pointer points to the last pushed element. The usual methods for
    pushing, popping, setting and getting data are available.
*/
class ua_conv_stack
{
public:
   //! ctor
   ua_conv_stack();

   //! reserves stack space
   void init(Uint16 stacksize);

   //! pushes a value onto the stack
   void push(Uint16 val);

   //! pops a value from the stack
   Uint16 pop();

   //! returns value at stack position
   Uint16 at(Uint16 pos);

   //! returns value at stack position
   void set(Uint16 pos, Uint16 val);

   //! returns current size of stack
   unsigned int size();

   //! gets new stack pointer
   Uint16 get_stackp();

   //! sets new stack pointer
   void set_stackp(Uint16 val);

protected:
   //! stack pointer; always points to top element of stack
   Uint16 stackp;

   //! the stack itself
   std::vector<Uint16> stack;
};


// inline methods

inline ua_conv_stack::ua_conv_stack()
{
   stackp = 0xffff;
};

inline void ua_conv_stack::init(Uint16 stacksize)
{
   stack.clear();
   stackp = 0xffff;
   stack.resize(stacksize,0);
}

inline void ua_conv_stack::push(Uint16 val)
{
   ua_assert(Uint16(stackp+1)>stack.size());
   stack[++stackp] = val;
}

inline Uint16 ua_conv_stack::pop()
{
   ua_assert(stackp<=stack.size());
   return stack[stackp--];
}

inline Uint16 ua_conv_stack::at(Uint16 pos)
{
   ua_assert(pos<=stack.size());
   return stack[pos];
}

inline void ua_conv_stack::set(Uint16 pos, Uint16 val)
{
   ua_assert(pos<=stack.size());
   stack[pos]=val;
}

inline unsigned int ua_conv_stack::size()
{
   return stack.size();
}

inline Uint16 ua_conv_stack::get_stackp()
{
   return stackp;
}

inline void ua_conv_stack::set_stackp(Uint16 val)
{
   ua_assert(val<=stack.size());
   stackp = val;
}


#endif
