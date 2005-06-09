/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004,2005 Underworld Adventures Team

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

   Implementations of some functions are dependent on operating system and
   compiler, so some #ifdef magic is used. Some functions are taken from
   Exult.

*/

// needed includes
#include "common.hpp"
#include <string>
#include <cstdarg>
#include <cstdio>
#include <algorithm>
#include <cctype>
#include <sstream>


#if defined(__MINGW32__) && !defined(HAVE_CONFIG_H)
#define HAVE_SYS_STAT_H
#endif

#ifdef WIN32
#include <direct.h> // for mkdir
#include <io.h> // for _findfirst, _findnext, _findclose
#include <shlobj.h> // for SHGetFolderPathA
#pragma comment(lib, "shell32.lib")
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif


/*! borrowed from Exult, files/utils.cc */
int ua_mkdir(const char* dirname, int mode)
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
      struct stat sbuf;
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
   const char* n = name.c_str();
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
int ua_trace_printf(const char* fmt,...)
{
   va_list args;
   va_start(args,fmt);

   int ret = vfprintf(stdout,fmt,args);

   va_end(args);
   return ret;
}

void ua_assert_check(bool cond, const char* cond_str, const char* file, int line)
{
   if (!cond)
   {
      // write out all messages
      fflush(stdout);
      fflush(stderr);

      std::ostringstream buffer;
      buffer << "ua_assert failed (" << cond_str << ") at " << file << ":" << line << std::ends;
      ua_trace("%s\n", buffer.str().c_str());
      throw ua_exception(buffer.str().c_str());
   }
}

bool ua_file_exists(const char* filename)
{
   FILE* fd = fopen(filename,"rb");
   if (fd==NULL)
      return false;

   fclose(fd);
   return true;
}


// ua_find_files implementation for various platforms

#if defined(WIN32)

void ua_find_files(const char* pathname, std::vector<std::string>& filelist)
{
   ua_trace("searching for files: %s\n",pathname);

   // find out base path
   std::string basepath(pathname);
   std::string::size_type pos = basepath.find_last_of("\\/");
   if (pos != std::string::npos)
      basepath.erase(pos+1);

   // try to find files by pathname
   _finddata_t fileinfo;

   long handle = _findfirst(pathname,&fileinfo);
   if (handle!=-1)
   {
      std::string filename;

      do
      {
         // add to filelist
         filename.assign(basepath);
         filename.append(fileinfo.name);

         ua_trace(" found file: %s\n",filename.c_str());

         filelist.push_back(filename);

      } while(0==_findnext(handle,&fileinfo));

      _findclose(handle);
   }

   ua_trace("found %u files\n",filelist.size());
}

#elif defined(HAVE_GLOB_H) // this system has glob.h

#include <glob.h>

void ua_find_files(const char* pathname, std::vector<std::string>& filelist)
{
   ua_trace("searching for files: %s\n",pathname);

   std::string path(pathname);

   glob_t globres;
   int err = glob(path.c_str(), GLOB_NOSORT, 0, &globres);

   // check for return code
   switch (err)
   {
      case 0: // ok
         // add all found files to the list
         for (unsigned int i=0; i<globres.gl_pathc; i++)
            filelist.push_back(globres.gl_pathv[i]);

         globfree(&globres);
         return;

      case 3: //no matches
         break;

      default: // error
         ua_trace("glob error: %u\n",err);
         break;
   }

   ua_trace("found %u files\n",filelist.size());
}

#elif defined(BEOS)

#error "please port Exult's U7ListFiles() function in files/listfiles.cc to uwadv!"

#else

#error "no implementation for ua_find_files()!"

// if you get this error, you have to supply a custom version of
// ua_find_files() for your system, together with the proper ifdef's to
// enable it.

#endif


std::string ua_get_home_path()
{
   std::string uahome_path = "./";

#ifdef HAVE_HOME
   {
      const char *homedir = getenv("HOME");
      if (homedir != NULL)
      {
         // User has a home directory
         uahome_path = homedir;

#ifdef __MACH__
         // JCD: in Mac OS X, we put user config into user's Library folder
         uahome_path += "/Library/Preferences/Underworld Adventures/";
#elif defined( BEOS )
         uahome_path += "/config/settings/uwadv/";
#else
         uahome_path += "/.uwadv/";
#endif
      }

      // try to create home folder
      ua_trace("creating uahome folder \"%s\"\n", uahome_path.c_str());
      ua_mkdir(uahome_path.c_str(), 0700);
   }

#elif defined(WIN32) && defined(_MSC_VER) // Visual C++ under Windows

   char appdata_path[MAX_PATH];
   HRESULT hr = ::SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
      NULL, SHGFP_TYPE_CURRENT, appdata_path);
   if (hr == S_OK)
   {
      uahome_path = appdata_path;
      uahome_path += "\\Underworld Adventures\\";

      std::string cfgfile(uahome_path);
      cfgfile += "uwadv.cfg";

      // check if the path really exists, and that it contains uwadv.cfg
      if (0xFFFFFFFF == ::GetFileAttributesA(uahome_path.c_str()) ||
          0xFFFFFFFF == ::GetFileAttributesA(cfgfile.c_str()))
         uahome_path = "./"; // revert to current dir
   }

#else // all other OSes

   // assume current working folder as home dir
   uahome_path = "./";

#endif // HAVE_HOME

   return uahome_path;
}


void ua_str_lowercase(std::string& str)
{
   std::transform(str.begin(),str.end(),str.begin(),tolower);
}

void ua_str_uppercase(std::string& str)
{
   std::transform(str.begin(),str.end(),str.begin(),toupper);
}
