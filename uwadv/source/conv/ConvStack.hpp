//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file ConvStack.hpp
/// \brief conversation code stack
//
#pragma once

#include <vector>

namespace Conv
{
   /// \brief Conversation code stack
   /// \details Implements a stack that is used in the conversation code virtual machine.
   /// The stack pointer points to the last pushed element. The usual methods for
   /// pushing, popping, setting and getting data are available.
   class ConvStack
   {
   public:
      /// ctor
      ConvStack()
         :m_stackPointer(0xffff)
      {
      }

      /// reserves stack space
      void Init(Uint16 stackSize)
      {
         m_stack.clear();
         m_stackPointer = 0xffff;
         m_stack.resize(stackSize, 0);
      }

      /// pushes a value onto the stack
      void Push(Uint16 value)
      {
         UaAssert(Uint16(m_stackPointer + 1) < m_stack.size());
         m_stack[++m_stackPointer] = value;
      }

      /// pops a value from the stack
      Uint16 Pop()
      {
         UaAssert(m_stackPointer < m_stack.size());
         return m_stack[m_stackPointer--];
      }

      /// returns value at stack position
      Uint16 At(Uint16 pos)
      {
         UaAssert(pos < m_stack.size());
         return m_stack[pos];
      }

      /// returns value at stack position
      void Set(Uint16 pos, Uint16 val)
      {
         UaAssert(pos < m_stack.size());
         m_stack[pos] = val;
      }

      /// returns current size of stack
      size_t Size()
      {
         return m_stack.size();
      }

      /// gets new stack pointer
      Uint16 GetStackPointer()
      {
         return m_stackPointer;
      }

      /// sets new stack pointer
      void SetStackPointer(Uint16 value)
      {
         UaAssert(value <= m_stack.size());
         m_stackPointer = value;
      }

   protected:
      /// stack pointer; always points to top element of stack
      Uint16 m_stackPointer;

      /// the stack itself
      std::vector<Uint16> m_stack;
   };

} // namespace Conv
