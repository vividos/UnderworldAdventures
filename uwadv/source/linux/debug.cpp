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
/*! \file debug.cpp

   \brief debug interface

*/

// needed includes
#include "common.hpp"
#include "debug.hpp"


// debugger implementation class

class ua_debug_impl: public ua_debug_interface
{
public:
   ua_debug_impl();
   virtual ~ua_debug_impl();

   void start_debugger();
};


// ua_debug_interface methods

ua_debug_interface* ua_debug_interface::get_new_debug_interface()
{
   return new ua_debug_impl;
}


// ua_debug_impl methods

ua_debug_impl::ua_debug_impl()
{
   // load debugger library
}

ua_debug_impl::~ua_debug_impl()
{
   // call uadebug_done() and free debugger library
}

void ua_debug_impl::start_debugger()
{
   // call uadebug_start()
}
