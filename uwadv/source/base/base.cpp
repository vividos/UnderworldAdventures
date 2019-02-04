//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file base.cpp
/// \brief common classes and functions implementation
//
#include "base.hpp"
#include <cstdio>
#include <cstdarg>
#include <sstream>
#include <SDL_rwops.h>
#ifdef HAVE_WIN32
#include <windows.h> // for OutputDebugStringA
#endif

void Base::SDL_RWopsDeletor::operator()(SDL_RWops* rwops)
{
   if (rwops != NULL)
      SDL_RWclose(rwops);
}

/// Throws a RuntimeException after printing out the error on the trace channel.
void UaAssertCheck(bool cond, const char* cond_str, const char* file, int line)
{
   if (!cond)
   {
      // write out all pending messages
      fflush(stdout);
      fflush(stderr);

      // format string
      std::ostringstream buffer;
      buffer << file << "(" << line << "): assertion failed (" << cond_str << ")";

      UaTrace("%s\n", buffer.str().c_str());

#ifdef HAVE_WIN32
      OutputDebugStringA(buffer.str().c_str());
#endif
      throw Base::RuntimeException(buffer.str().c_str());
   }
}

/// Prints out trace message on stdout.
int UaTracePrintf(const char* format, ...)
{
   va_list args;

   va_start(args, format);
   int ret = vfprintf(stdout, format, args);
   va_end(args);

#ifdef HAVE_WIN32
   va_start(args, format);
   char buffer[512];
   vsnprintf(buffer, sizeof(buffer), format, args);
   OutputDebugStringA(buffer);
   va_end(args);
#endif

   return ret;
}
