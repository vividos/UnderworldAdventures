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
#include <SDL_thread.h>


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
   //! debugger thread procedure
   static int thread_proc(void* ptr);

protected:
   //! indicates if debugger dll is available
   bool avail;

   //! DLL module handle
   HMODULE dll;

   //! semaphore that indicates if debugger still runs
   SDL_sem* sem_debugger;

   //! debug thread
   SDL_Thread* thread_debug;
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

   avail &= (NULL != ::GetProcAddress(dll,"uadebug_start"));

   ua_trace("win32 debug impl. started; debugger is %savailable\n",
      avail ? "" : "not ");

   sem_debugger = SDL_CreateSemaphore(0);
   thread_debug = NULL;
}

ua_debug_impl_win32::~ua_debug_impl_win32()
{
   // free DLL instance
   ::FreeLibrary(dll);

   avail = false;

   SDL_DestroySemaphore(sem_debugger);

   if (thread_debug!=NULL)
      SDL_KillThread(thread_debug);
}

void ua_debug_impl_win32::start_debugger()
{
   // check if debugger already runs
   if (avail && SDL_SemValue(sem_debugger)==0)
   {
      ua_trace("starting uadebug thread\n");

      // start new thread
      thread_debug = SDL_CreateThread(thread_proc,this);
   }
}

int ua_debug_impl_win32::thread_proc(void* ptr)
{
   ua_debug_impl_win32* This = reinterpret_cast<ua_debug_impl_win32*>(ptr);

   // increase semaphore count
   SDL_SemPost(This->sem_debugger);

   if (This->avail)
   {
      // get function pointer
      void* funcptr = ::GetProcAddress(This->dll,"uadebug_start");

      uadebug_start_func uadebug_start = (uadebug_start_func)funcptr;

      // start debugger
      uadebug_start();
   }

   ua_trace("uadebug thread ended\n");

   // decrease semaphore count
   SDL_SemWait(This->sem_debugger);

   return 0;
}
