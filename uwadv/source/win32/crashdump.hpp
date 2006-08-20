/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file crashdump.hpp

   \brief crash dump reporting for win32 platform

*/

// include guard
#ifndef uwadv_win32_crashdump_hpp_
#define uwadv_win32_crashdump_hpp_

// needed includes
#ifdef HAVE_WIN32
#include <windows.h>

//! exception filter function that reports crash to stderr
int ReportCrashFilter(unsigned int code, struct _EXCEPTION_POINTERS *ep);

//! checked call
/*! does a call to function f and protects it from structured exceptions (e.g.
    access violations, etc.)
*/
template <typename Function>
inline void CheckedCall(Function f)
{
   __try {
      f();
   }
   __except(ReportCrashFilter(GetExceptionCode(), GetExceptionInformation()))
   {
   }
}

#endif // HAVE_WIN32

#endif
