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

   \brief linux debug interface

*/

// needed includes
#include "common.hpp"
#include "debug.hpp"
#include "debug/uwaccess.hpp"
#include <SDL_thread.h>


// debugger implementation class

class ua_debug_impl_linux: public ua_debug_interface
{
public:
   ua_debug_impl_linux(ua_underworld* underw);
   virtual ~ua_debug_impl_linux();

   // implemented abstraction methods from ua_debug_interface
   virtual void start_debugger();
   virtual void lock();
   virtual void unlock();
   virtual ua_underworld* get_underworld();
   virtual ua_uw_access_api* get_access_api();

protected:
   //! underworld object
   ua_underworld* underw;

   //! access api
   ua_uw_access_api api;
};


// ua_debug_interface methods

ua_debug_interface* ua_debug_interface::get_new_debug_interface(
   ua_underworld* underw)
{
   return new ua_debug_impl_linux(underw);
}


// ua_debug_impl methods

ua_debug_impl_linux::ua_debug_impl_linux(ua_underworld* theunderw)
:underw(theunderw)
{
   // todo: load dynamic debugger library
   ua_trace("linux debug impl. started; debugger is not available\n");

   // initialize access api
   api.init();
}

ua_debug_impl_linux::~ua_debug_impl_linux()
{
}

void ua_debug_impl_linux::start_debugger()
{
   // todo: check if debugger already runs, and when not,
   //       call uadebug_start() in dynamic library via own thread
   ua_trace("starting uadebug thread: not implemented yet\n");
}

void ua_debug_impl_linux::lock()
{
}

void ua_debug_impl_linux::unlock()
{
}

ua_underworld* ua_debug_impl_linux::get_underworld()
{
   return underw;
}

ua_uw_access_api* ua_debug_impl_linux::get_access_api()
{
   return &api;
}
