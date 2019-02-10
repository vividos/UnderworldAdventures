//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file opcodes.hpp
/// \brief conversation code opcodes
//
#pragma once

namespace Conv
{
   /// conversation code opcodes
   enum EOpcodes
   {
      // nop
      op_NOP = 0x0000,

      // arthmetic operators
      op_OPADD = 0x0001, op_OPMUL = 0x0002, op_OPSUB = 0x0003, op_OPDIV = 0x0004,
      op_OPMOD = 0x0005, op_OPOR = 0x0006, op_OPAND = 0x0007, op_OPNOT = 0x0008,

      // comparison operators
      op_TSTGT = 0x0009, op_TSTGE = 0x000a, op_TSTLT = 0x000b, op_TSTLE = 0x000c,
      op_TSTEQ = 0x000d, op_TSTNE = 0x000e,

      // branches, jumps
      op_JMP = 0x000f, op_BEQ = 0x0010, op_BNE = 0x0011, op_BRA = 0x0012,

      // function related
      op_CALL = 0x0013, op_CALLI = 0x0014, op_RET = 0x0015,

      // stack manipulation
      op_PUSHI = 0x0016, op_PUSHI_EFF = 0x0017, op_POP = 0x0018, op_SWAP = 0x0019,

      // stack frame
      op_PUSHBP = 0x001a, op_POPBP = 0x001b, op_SPTOBP = 0x001c, op_BPTOSP = 0x001d, op_ADDSP = 0x001e,

      // stack operators
      op_FETCHM = 0x001f, op_STO = 0x0020, op_OFFSET = 0x0021,

      // function related
      op_START = 0x0022, op_SAVE_REG = 0x0023, op_PUSH_REG = 0x0024,

      // misc. opcodes
      op_STRCMP = 0x0025, op_EXIT_OP = 0x0026, op_SAY_OP = 0x0027, op_RESPOND_OP = 0x0028,

      op_OPNEG = 0x0029,

      // number of last opcode
      op_last = 0x0029
   };

   /// instruction list
   static struct
   {
      /// mnemonic name
      const char* mnemonic;
      /// number of arguments to format opcode string
      int args;
      /// opcode format string
      const char* argcode;
      /// operator text
      const char* operator_text;
      /// operator precedence level
      int op_prec_level;
   } g_convInstructions[0x002a] =
   {
      { "NOP",       0, "",       "",   -1 },
      { "OPADD",     0, "",       "+",  0x79 },
      { "OPMUL",     0, "",       "*",  0x7b },
      { "OPSUB",     0, "",       "-",  0x79 },
      { "OPDIV",     0, "",       "/",  0x7b },
      { "OPMOD",     0, "",       "%",  0x7a },
      { "OPOR",      0, "",       "||", 0x46 },
      { "OPAND",     0, "",       "&&", 0x50 },
      { "OPNOT",     0, "",       "!",  0x5a },
      { "TSTGT",     0, "",       ">",  0x64 },
      { "TSTGE",     0, "",       ">=", 0x64 },
      { "TSTLT",     0, "",       "<",  0x64 },
      { "TSTLE",     0, "",       "<=", 0x64 },
      { "TSTEQ",     0, "",       "==", 0x64 },
      { "TSTNE",     0, "",       "!=", 0x64 },
      { "JMP",       1, " $%04x", "",   -1 },
      { "BEQ",       1, " +%04x", "",   -1 },
      { "BNE",       1, " +%04x", "",   -1 },
      { "BRA",       1, " +%04x", "",   -1 },
      { "CALL",      1, " $%04x", "",   0x90 },
      { "CALLI",     1, " $%04x", "",   0x90 },
      { "RET",       0, "",       "",   -1 },
      { "PUSHI",     1, " #%04x", "",   -1 },
      { "PUSHI_EFF", 1, " #%04x", "",   -1 },
      { "POP",       0, "",       "",   -1 },
      { "SWAP",      0, "",       "",   -1 },
      { "PUSHBP",    0, "",       "",   -1 },
      { "POPBP",     0, "",       "",   -1 },
      { "SPTOBP",    0, "",       "",   -1 },
      { "BPTOSP",    0, "",       "",   -1 },
      { "ADDSP",     0, "",       "",   -1 },
      { "FETCHM",    0, "",       "",   0x85 },
      { "STO",       0, "",       "",   0x20 },
      { "OFFSET",    0, "",       "",   0xa0 },
      { "START",     0, "",       "",   -1 },
      { "SAVE_REG",  0, "",       "",   -1 },
      { "PUSH_REG",  0, "",       "",   -1 },
      { "STRCMP",    0, "",       "",   -1 },
      { "EXIT_OP",   0, "",       "",   -1 },
      { "SAY_OP",    0, "",       "",   0x90 },
      { "RESPOND_OP",0, "",       "",   -1 },
      { "OPNEG",     0, "",       "",   0x7d },
   };

} // namespace Conv
