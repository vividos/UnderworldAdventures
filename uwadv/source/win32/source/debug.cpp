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

   \brief win32 debug interface implementation

*/

// needed includes
#include "common.hpp"
#include "debug.hpp"


// debugger implementation class

class ua_debug_impl_win32: public ua_debug_interface
{
public:
   //! ctor
   ua_debug_impl_win32();
   //! dtor
   virtual ~ua_debug_impl_win32();

   //! starts debugger
   virtual void start_debugger();

protected:
   //! indicates if debugger dll is available
   bool avail;

   //! DLL module handle
   HMODULE dll;
};


// ua_debug_interface methods

ua_debug_interface* ua_debug_interface::get_new_debug_interface()
{
   return new ua_debug_impl_win32;
}


// types

typedef void (*uadebug_start_func)(void);
typedef void (*uadebug_done_func)(void);


// ua_debug_impl methods

ua_debug_impl_win32::ua_debug_impl_win32()
{
   // load library, check if functions are available
   dll = ::LoadLibrary("uadebug.dll");
   avail = dll != NULL;

   avail &= NULL != ::GetProcAddress(dll,"uadebug_start");
   avail &= NULL != ::GetProcAddress(dll,"uadebug_done");
}

ua_debug_impl_win32::~ua_debug_impl_win32()
{
   // call uadebug_done() and free library
   if (avail)
   {
         // get function pointer
      void* funcptr = ::GetProcAddress(dll,"uadebug_done");

      uadebug_done_func uadebug_done = (uadebug_done_func)funcptr;

      uadebug_done();

      // free DLL instance
      ::FreeLibrary(dll);
   }

   avail = false;
}

void ua_debug_impl_win32::start_debugger()
{
   if (avail)
   {
      // get function pointer
      void* funcptr = ::GetProcAddress(dll,"uadebug_start");

      uadebug_start_func uadebug_start = (uadebug_start_func)funcptr;

      uadebug_start();
   }
}
