/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004 Underworld Adventures Team

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
/*! \file cnvdec.hpp

   \brief conversation code decompiling

*/

// include guard
#ifndef uwdump_cnvdec_hpp_
#define uwdump_cnvdec_hpp_

// needed includes
#include "uwdump.hpp"
#include "gamestrings.hpp"
#include "convgraph.hpp"


// classes

//! dumps builtin models
class ua_conv_code_decompiler
{
public:
   //! starts decompiling for given slot
   void start(std::string& basepath, Uint16 slot);

protected:
   //! outputs processed graph
   void output_graph();

protected:
   //! conversation graph
   ua_conv_graph conv_graph;

   //! game strings
   ua_gamestrings gstr;
};

#endif
