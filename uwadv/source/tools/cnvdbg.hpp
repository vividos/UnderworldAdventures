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
/*! \file cnvdbg.hpp

   gdb-like conversation debugger class

*/

// include guard
#ifndef __uwadv_cnvdbg_hpp_
#define __uwadv_cnvdbg_hpp_

// needed includes
#include "conv/codevm.hpp"
#include "gamestrings.hpp"


// classes

//! conversation debugger class
class ua_conv_debugger: public ua_conv_code_vm
{
public:
   //! ctor
   ua_conv_debugger(ua_gamestrings &gstr):gs(gstr){}

   //! loads private globals from bglobals.dat file
   void load_priv_globals(const char *fname);

   //! starts the debugger
   void start(ua_gamestrings gs);

protected:
   //! lists code at position; returns number of bytes opcode occupies
   int list_code(Uint16 at);

   // virtual function implementation

   virtual void imported_func(Uint16 number);
   virtual void say_op(Uint16 str_id);
   virtual void sto_priv(Uint16 at, Uint16 val);
   virtual void fetchm_priv(Uint16 at);

protected:
   //! ref to all game strings
   ua_gamestrings &gs;

   //! private globals
   std::vector<Uint16> priv_globals;

   //! list of all breakpoints
   std::vector<Uint16> allbreakpoints;

   bool verbose;
};

#endif
