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
/*! \file cnvdasm.hpp

   conversation disassembler and decompiler

*/

// include guard
#ifndef __uwadv_cnvdasm_hpp_
#define __uwadv_cnvdasm_hpp_

// needed includes
#include "common.hpp"
#include "conv/codevm.hpp"
#include "gamestrings.hpp"
#include <cstdio>


// structs

//! decompiler entry
typedef struct
{
   Uint16 instrp;

   //! code byte
   Uint16 code;

   //! optional argument
   Uint16 arg;

   //! indicates if code was translated
   bool translated;

   //! translated command
   std::string command;

   //! possible label
   std::string label;

   //! indendation level
   unsigned int indent_level;

   //! indicates if an empty line should be printed before "command"
   bool empty_line;

} ua_conv_dec_entry;


// classes

//! disassembler class
class ua_conv_dasm: private ua_conv_code_vm
{
public:
   //! ctor
   ua_conv_dasm(){}

   //! initializes disassembler; returns false if no conversation is available
   bool init(const char *cnvfile,unsigned int conv);

   //! disassembles code in assembler-like file
   void disassemble(FILE *out);

   //! tries to match assembler language to C constructs
   void decompile(FILE *out);

protected:
   //! loads code in the dec_entries struct
   void load_dec();

   //! resolves labels of jump opcodes
   void resolve_labels();

   // replaces constructs in the dec_entries list
   bool replace_constructs(unsigned int type, unsigned int entry, unsigned int max);

   //! formats a string with the global's variable name
   std::string format_global(Uint16 offset);

   //! searches for instrp and returns entry index in dec_entries
   unsigned int search_instrp(unsigned int instrp);

   //! replaces stuff with if or switch statement
   bool if_switch_replace(unsigned int entry, unsigned int max);

protected:
   //! input file
   FILE *fd;

   //! decoding entries
   std::vector<ua_conv_dec_entry> dec_entries;

   //! all function names which should be forward declared
   std::vector<std::string> forward_decl;

   //! all game strings
   ua_gamestrings gs;
};

#endif
