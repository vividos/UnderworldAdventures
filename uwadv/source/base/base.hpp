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
/*! \file base.hpp

   \brief Base module includes

*/
/*! \defgroup base Base Components

   The Base component contains classes used by all other components of the
   project.
   The Base component doesn't depend on other modules.

*/

// include guard
#ifndef uwadv_base_base_hpp_
#define uwadv_base_base_hpp_

// needed includes
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* compiler macros;
   The following macros will be defined if the feature is present
   HAVE_MSVC     Microsoft Visual C++ compiler
   HAVE_GCC      Gnu gcc compiler
   HAVE_WIN32    Win32 API is available through <windows.h>
   HAVE_UNICODE  Unicode Win32 API functions should be used
   HAVE_LINUX    Linux is used as target system
   HAVE_MINGW    MinGW32 development/runtime environment
   HAVE_MACOSX   MacOS X development/runtime environment
*/

#ifdef _MSC_VER
#define HAVE_MSVC _MSC_VER
#define HAVE_WIN32
#endif

#if defined(UNICODE) || defined(_UNICODE)
#define HAVE_UNICODE
#endif

#ifdef __gcc__
#define HAVE_GCC
#endif

#ifdef __MINGW32__
#define HAVE_MINGW
#define HAVE_WIN32
#endif

#ifdef __linux__
#define HAVE_LINUX
#endif

#ifdef __MACH__
#define HAVE_MACOSX
#endif

#ifdef BEOS
#define HAVE_BEOS
#endif


//! checks for assert
void UaAssertCheck(bool cond, const char* cond_str, const char* file, int line);

//! macro to check for conditions and pass source filename and line
#define UaAssert(cond) UaAssertCheck((cond), #cond, __FILE__, __LINE__);

//! macro to verify expression/statement
#define UaVerify(cond) if (!(cond)) UaAssertCheck((cond), #cond, __FILE__, __LINE__);


// trace messages

//! prints out a trace message (don't use directly, use UaTrace instead!)
int UaTracePrintf(const char *fmt, ...);

/*! \def UaTrace
    \brief debug output
    Used to log text during the game. The text is printed on the console (the
    program has to be built with console support to show the text).

    The function has the same syntax as the printf function and uses the
    UaTracePrintf() helper function. The function can be switched off
    conditionally.
*/
#if 1 //defined(_DEBUG) || defined(DEBUG)
# define UaTrace UaTracePrintf
#else
# define UaTrace true ? 0 : UaTracePrintf
#endif

//! \brief Base classes namespace \ingroup base
/*! Contains all base classes, functions and types used in uwadv.
*/
namespace Base
{

//! \brief Base class for noncopyable classes
/*! Base class to prevent a derived class from being copyable (e.g. to put
    into a container).
*/
class NonCopyable
{
public:
   //! ctor; derived classes must be publicly constructible
   NonCopyable(){}
private:
   //! copy ctor; derived classes must not be able to copy construct
   NonCopyable(const NonCopyable&);
   //! assignment operator; same as with copy ctor
   NonCopyable& operator=(const NonCopyable&);
};

} // namespace Base

// common includes

#include "exception.hpp"
#include "string.hpp"

#endif
