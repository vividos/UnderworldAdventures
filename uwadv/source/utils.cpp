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
/*! \file utils.cpp

   \brief misc utilities

*/

// needed includes
#include "common.hpp"
#include <string>
#include <cstdarg>
#include <cstdio>

#ifdef WIN32
#include <direct.h> // for mkdir
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif


//! creates a folder
/*! borrowed from Exult, files/utils.cc */
int ua_mkdir(const char *dirname, int mode)
{
   std::string name(dirname);

   // remove any trailing slashes
   std::string::size_type pos = name.find_last_not_of('/');

   if (pos != std::string::npos)
      name.resize(pos+1);

   // check if parent folder exists
   {
      std::string parent(name);

      // remove next slash
      std::string::size_type pos = parent.find_last_of('/');

      if (pos != std::string::npos)
         parent.erase(pos);

      // test parent if it's a folder
#ifdef _MSC_VER
      // msvc case
      bool exists = false;

      DWORD ret = GetFileAttributes(parent.c_str());
      exists = (ret!= (DWORD)-1) &&
         (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
      // every other sane system
      bool exists =
         (stat(parent.c_str(), &sbuf) == 0) &&
         S_ISDIR(sbuf.st_mode);
#endif

      if (!exists)
      {
         // call recursively
         ua_mkdir(parent.c_str(),mode);
      }
   }

#if defined(WIN32) && defined(UNICODE)

   // unicode case
   const char *n = name.c_str();
   int nLen = std::strlen(n)+1;
   LPTSTR lpszT = (LPTSTR) alloca(nLen*2);
   MultiByteToWideChar(CP_ACP, 0, n, -1, lpszT, nLen);
   return CreateDirectory(lpszT, NULL);

#elif defined(WIN32)

   // normal win32 calse
   return mkdir(name.c_str());

#else

   // posix (?)
   return mkdir(name.c_str(), mode); // create dir if not already there

#endif
}

//! prints out a ua_trace message (when enabled)
int ua_trace_printf(const char *fmt,...)
{
   va_list args;
   va_start(args,fmt);

   int ret = vfprintf(stdout,fmt,args);

   va_end(args);
   return ret;
}
