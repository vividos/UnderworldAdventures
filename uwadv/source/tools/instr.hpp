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
/*! \file instr.hpp

   conversation code instruction list

*/

// include guard
#ifndef __uwadv_instr_hpp_
#define __uwadv_instr_hpp_


// structs

//! instruction list
static struct
{
   const char *mnemonic;
   int args;
   const char *argcode;
} ua_conv_instructions[0x002a] = 
{
   { "NOP", 0,  "" },
   { "OPADD", 0, "" },
   { "OPMUL", 0, "" },
   { "OPSUB", 0, "" },
   { "OPDIV", 0, "" },
   { "OPMOD", 0, "" },
   { "OPOR", 0, "" },
   { "OPAND", 0, "" },
   { "OPNOT", 0, "" },
   { "TSTGT", 0, "" },
   { "TSTGE", 0, "" },
   { "TSTLT", 0, "" },
   { "TSTLE", 0, "" },
   { "TSTEQ", 0, "" },
   { "TSTNE", 0, "" },
   { "JMP", 1, " $%04x" },
   { "BEQ", 1, " +%04x" },
   { "BNE", 1, " +%04x" },
   { "BRA", 1, " +%04x" },
   { "CALL", 1, " $%04x" },
   { "CALLI", 1, " $%04x" },
   { "RET", 0, "" },
   { "PUSHI", 1, " #%04x" },
   { "PUSHI_EFF", 1, " #%04x" },
   { "POP", 0, "" },
   { "SWAP", 0, "" },
   { "PUSHBP", 0, "" },
   { "POPBP", 0, "" },
   { "SPTOBP", 0, "" },
   { "BPTOSP", 0, "" },
   { "ADDSP", 0, "" },
   { "FETCHM", 0, "" },
   { "STO", 0, "" },
   { "OFFSET", 0, "" },
   { "START", 0, "" },
   { "SAVE_REG", 0, "" },
   { "PUSH_REG", 0, "" },
   { "STRCMP", 0, "" },
   { "EXIT_OP", 0, "" },
   { "SAY_OP", 0, "" },
   { "RESPOND_OP", 0, "" },
   { "OPNEG", 0, "" },
};

#endif
