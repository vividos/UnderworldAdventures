/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2002,2003,2004,2005,2006 Michael Fink

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
/*! \file filesystem.cpp

   \brief file system implementation

   Parts of this file's code was taken from Exult

*/

// needed includes
#include <cerrno>
#include "base.hpp"
#include "filesystem.hpp"

#ifdef HAVE_WIN32
#ifdef HAVE_MINGW
#define _WIN32_IE 0x0500 // define this to find SHGFP_TYPE_CURRENT in shlobj.h
#endif

#include <windows.h> // for GetFileAttribtes, CreateDirectory
#include <shlobj.h> // for SHGetFolderPathA
#include <io.h> // for _findfirst, _findnext, _findclose
#endif

#if defined(HAVE_MINGW) && !defined(HAVE_CONFIG_H)
#define HAVE_SYS_STAT_H
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

//! common error text when folder cannot be created
const char* c_strCreateFolderError = "couldn't create folder";
//! common error text when folder cannot be removed
const char* c_strRemoveFolderError = "couldn't remove folder";


#if defined(HAVE_WIN32) // use win32 API (or better, Microsoft's C runtime)

/*! The meta-files "." and ".." are not added to the file list.
*/
void Base::FileSystem::FindFiles(const std::string& strSearchPath, std::vector<std::string>& vecFileList)
{
   // find out base path
   std::string strBasePath(strSearchPath);
   std::string::size_type pos = strBasePath.find_last_of("\\/");
   if (pos != std::string::npos)
      strBasePath.erase(pos+1);

   // try to find files by pathname
   _finddata_t fileinfo;

   long handle = _findfirst(strSearchPath.c_str(),&fileinfo);
   if (handle!=-1)
   {
      std::string strFilename;

      do
      {
         // check for the meta-files
         if (strncmp(fileinfo.name, ".", 1) != 0 &&
            strncmp(fileinfo.name, "..", 2) != 0)
         {
            strFilename = strBasePath + fileinfo.name;

            // add to filelist
            vecFileList.push_back(strFilename);
         }

      } while(0 == _findnext(handle, &fileinfo));

      _findclose(handle);
   }
}

#elif defined(HAVE_GLOB_H) // this system has glob.h

#include <glob.h>

/*! The meta-files "." and ".." are not added to the file list.
*/
void Base::FileSystem::FindFiles(const std::string& strSearchPath, std::vector<std::string>& vecFileList)
{
   std::string strPath(strSearchPath);

   glob_t globres;
   int err = glob(strPath.c_str(), GLOB_NOSORT, 0, &globres);

   // check for return code
   switch (err)
   {
      case 0: // ok
         // add all found files to the list
         for (unsigned int i=0; i<globres.gl_pathc; i++)
            vecFileList.push_back(globres.gl_pathv[i]);

         globfree(&globres);
         return;

      case 3: //no matches
         break;

      default: // error
         ua_trace("glob error: %u\n",err);
         break;
   }
}

#elif defined(HAVE_BEOS)

#error "please port Exult's U7ListFiles() function in files/listfiles.cc to uwadv!"

#else

#error "no implementation for FileSystem::FindFiles()!"

// if you get this error, you have to supply a custom version of
// FileSystem::FindFiles() for your system, together with the proper ifdef's to
// enable it.

#endif

void Base::FileSystem::RemoveFile(const std::string& strFilename)
{
#ifdef HAVE_WIN32

#ifdef HAVE_UNICODE
   std::wstring wstrFilename;
   String::ConvertToUnicode(strFilename, wstrFilename);
   DeleteFileW(wstrFilename.c_str());
#else
   DeleteFileA(strFilename.c_str());
#endif

#else
   remove(strFilename.c_str());
#endif
}

/*! implementation borrowed from Exult, files/utils.cc */
void Base::FileSystem::MakeFolder(const std::string& strFolderName)
{
   std::string strName(strFolderName);

   // remove any trailing slashes
   std::string::size_type pos = strName.find_last_not_of('/');

   if (pos != std::string::npos)
      strName.resize(pos+1);

   // check if parent folder exists
   {
      std::string strParent(strName);

      // remove next slash
      std::string::size_type pos = strParent.find_last_of('/');

      if (pos != std::string::npos)
         strParent.erase(pos);

      // test parent if it's a folder
      bool bExists = FolderExists(strParent);
      if (!bExists)
      {
         // call recursively
         MakeFolder(strParent);
      }
   }

#if defined(HAVE_WIN32)

   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wstrName;
   String::ConvertToUnicode(strName, wstrName);

   BOOL bRet = CreateDirectoryW(wstrName.c_str(), NULL);
#else
   BOOL bRet = CreateDirectoryA(strName.c_str(), NULL);
#endif
   if (bRet == FALSE)
   {
      DWORD dwError = GetLastError();
      if (dwError != ERROR_ALREADY_EXISTS)
         throw Base::FileSystemException(c_strCreateFolderError, strName, dwError);
   }

#else

   // posix (?)
   int iRet = mkdir(strName.c_str(), 0700); // create dir if not already there
   if (iRet != 0 && errno != EEXIST)
      throw Base::FileSystemException(c_strCreateFolderError, strName, errno);

#endif
}

void Base::FileSystem::RemoveFolder(const std::string& strFolderName)
{
#if defined(HAVE_WIN32)

   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wstrFolderName;
   String::ConvertToUnicode(strFolderName, wstrFolderName);

   BOOL bRet = RemoveDirectoryW(wstrFolderName.c_str());
#else
   BOOL bRet = RemoveDirectoryA(strFolderName.c_str());
#endif
   if (bRet == FALSE)
      throw Base::FileSystemException(c_strRemoveFolderError, strFolderName, GetLastError());

#else

   // posix (?)
   int iRet = rmdir(strFolderName.c_str());
   if (iRet != 0 && errno != EEXIST)
      throw Base::FileSystemException(c_strRemoveFolderError, strFolderName, errno);

#endif
}

bool Base::FileSystem::FileExists(const std::string& strFilename)
{
#ifdef HAVE_WIN32
   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wstrFilename;
   String::ConvertToUnicode(strFilename, wstrFilename);

   DWORD ret = GetFileAttributesW(wstrFilename.c_str());
#else
   DWORD ret = GetFileAttributesA(strFilename.c_str());
#endif
   return (ret!= (DWORD)-1) &&
      (ret & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
   // every other sane system
   struct stat sbuf;
   return
      (stat(strFilename.c_str(), &sbuf) == 0) &&
      !S_ISDIR(sbuf.st_mode);
#endif
}

bool Base::FileSystem::FolderExists(const std::string& strFolderName)
{
#ifdef HAVE_MSVC
   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wstrFolderName;
   String::ConvertToUnicode(strFolderName, wstrFolderName);

   DWORD ret = GetFileAttributesW(wstrFolderName.c_str());
#else
   DWORD ret = GetFileAttributesA(strFolderName.c_str());
#endif
   return (ret!= (DWORD)-1) &&
      (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
   // every other sane system
   struct stat sbuf;
   return
      (stat(strFolderName.c_str(), &sbuf) == 0) &&
      S_ISDIR(sbuf.st_mode);
#endif
}

void Base::FileSystem::RecursiveRemoveFolder(const std::string& strPathname)
{
   // collect all files and folders
   std::vector<std::string> vecFileList;
   std::string strSearchPath(strPathname);
   strSearchPath += "/*.*";
   FindFiles(strSearchPath, vecFileList);

   std::vector<std::string>::size_type max = vecFileList.size();
   for (std::vector<std::string>::size_type i=0; i<max; i++)
   {
      std::string strFilename(vecFileList[i]);

      // is a file? then try to remove it
      if (FileExists(strFilename))
      {
         try
         {
            RemoveFile(strFilename);
         }
         catch(Base::Exception& e)
         {
            (e);
         }
      }
      else
      // is a folder? try to remove contents
      if (FolderExists(strFilename))
      {
         RecursiveRemoveFolder(strFilename);
      }
   }

   // try to remove this folder
   try
   {
      RemoveFolder(strPathname);
   }
   catch(Base::Exception& e)
   {
      (e);
   }
}


namespace Detail
{

//! returns system specific home path
/*! Win32: returns a subfolder of the "common application data" folder stored under
       the user profile of the logged in user, commonly found under
       "Documents and Settings", or the current directory, if the folder
       doesn't exist or is not supported by the system.
    Linux (and all with HAVE_HOME defined): returns the contents of the HOME
       environment variable. Special cases:
    MacOS X: "Library/Preferences/Underworld Adventures/" subfolder of HOME folder
    BeOS: "config/settings/uwadv/" subfolder ofHOME folder
    All other systems: returns the current folder
*/
std::string GetHomePath()
{
   std::string strHomePath = "./";

#ifdef HAVE_HOME
   {
      const char* homedir = getenv("HOME");
      if (homedir != NULL)
      {
         // User has a home directory
         strHomePath = homedir;

#ifdef HAVE_MACOSX
         // JCD: in Mac OS X, we put user config into user's Library folder
         strHomePath += "/Library/Preferences/Underworld Adventures/";
#elif defined( HAVE_BEOS )
         strHomePath += "/config/settings/uwadv/";
#else
         strHomePath += "/.uwadv/";
#endif
      }

      // try to create home folder
      UaTrace("creating uahome folder \"%s\"\n", strHomePath.c_str());
      Base::FileSystem::MakeFolder(strHomePath.c_str());
   }

#elif defined(HAVE_WIN32) // Win32 API

   char appdataPath[MAX_PATH];
   HRESULT hr = ::SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
      NULL, SHGFP_TYPE_CURRENT, appdataPath);
   if (hr == S_OK && appdataPath[0] != 0)
   {
      strHomePath = appdataPath;
      strHomePath += "\\Underworld Adventures\\";

      std::string strCfgFile(strHomePath);
      strCfgFile += "uwadv.cfg";

      // check if the path really exists, and that it contains uwadv.cfg
      if (!Base::FileSystem::FolderExists(strHomePath) ||
          !Base::FileSystem::FileExists(strCfgFile))
         strHomePath = "./"; // revert to current dir
   }

#else // all other OSes

   // assume current working folder as home dir
   strHomePath = "./";

#endif

   return strHomePath;
}

//! returns system specific personal folder
/*! Win32: returns the "My Documents" folder, or the home folder returned by
       GetHomePath(), if the folder doesn't exist or is not supported by the
       system.
    All other systems: returns the home folder returned by GetHomePath()
*/
std::string GetPersonalFolderPath()
{
   std::string strPersonalFolderPath = GetHomePath();

#ifdef HAVE_WIN32 // Win32 API

   char personalFolderPath[MAX_PATH];
   HRESULT hr = ::SHGetFolderPathA(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
      NULL, SHGFP_TYPE_CURRENT, personalFolderPath);

   if (hr == S_OK && personalFolderPath[0] != 0)
   {
      strPersonalFolderPath = personalFolderPath;
   }
#endif

   return strPersonalFolderPath;
}

} // namespace Detail
