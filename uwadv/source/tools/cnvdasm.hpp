/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file cnvdasm.hpp

   \brief conversation disassembler and decompiler

*/

// include guard
#ifndef __uwadv_cnvdasm_hpp_
#define __uwadv_cnvdasm_hpp_

// needed includes
#include "common.hpp"
#include "conv/codevm.hpp"
#include "gamestrings.hpp"
#include <cstdio>


// classes

//! disassembler class
class ua_conv_dasm: private ua_conv_code_vm
{
public:
   //! ctor
   ua_conv_dasm(){}

   //! initializes disassembler; returns false if no conversation is available
   bool init(ua_gamestrings* gs, const char* cnvfile,unsigned int conv);

   //! disassembles code in assembler-like file
   void disassemble();

   //! tries to match assembler language to C constructs
   void decompile(bool with_opcodes);

protected:
   // worker functions

   //! collects cross references
   void collect_xrefs();



   // helper functions

   //! prints opcode (and argument)
   void print_opcode(Uint16 opcode, Uint16 arg, Uint16 pos);

protected:
   //! all jump/branch cross references
   std::map<Uint16,std::vector<Uint16> > all_xrefs;

   //! game strings
   ua_gamestrings* gs;
};

#endif
