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
/*! \file opcodes.hpp

   \brief conversation code opcodes

*/

// include guard
#ifndef __uwadv_opcodes_hpp_
#define __uwadv_opcodes_hpp_


// enums

//! conversation code opcodes
enum ua_conv_opcodes
{
   // nop
   op_NOP=0x0000,

   // arthmetic operators
   op_OPADD=0x0001, op_OPMUL=0x0002, op_OPSUB=0x0003, op_OPDIV=0x0004,
   op_OPMOD=0x0005, op_OPOR= 0x0006, op_OPAND=0x0007, op_OPNOT=0x0008,

   // comparison operators
   op_TSTGT=0x0009, op_TSTGE=0x000a, op_TSTLT=0x000b, op_TSTLE=0x000c,
   op_TSTEQ=0x000d, op_TSTNE=0x000e,

   // branches, jumps
   op_JMP=0x000f, op_BEQ=0x0010, op_BNE=0x0011, op_BRA=0x0012,

   // function related
   op_CALL=0x0013, op_CALLI=0x0014, op_RET=0x0015,

   // stack manipulation
   op_PUSHI=0x0016, op_PUSHI_EFF=0x0017, op_POP=0x0018, op_SWAP=0x0019,

   // stack frame
   op_PUSHBP=0x001a, op_POPBP=0x001b, op_SPTOBP=0x001c, op_BPTOSP=0x001d, op_ADDSP=0x001e,

   // stack operators
   op_FETCHM=0x001f, op_STO=0x0020, op_OFFSET=0x0021,

   // function related
   op_START=0x0022, op_SAVE_REG=0x0023, op_PUSH_REG=0x0024,

   // misc. opcodes
   op_STRCMP=0x0025, op_EXIT_OP=0x0026, op_SAY_OP=0x0027, op_RESPOND_OP=0x0028,

   op_OPNEG=0x0029,

   // number of last opcode
   op_last=0x0029
};


// structs


// text opcodes list

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
