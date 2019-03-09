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
//
/// \file FileSystem.cpp
/// \brief file system implementation
/// Parts of this file's code was taken from Exult
//
#include <cerrno>
#include "Base.hpp"
#include "FileSystem.hpp"

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

#if defined(HAVE_WIN32)
const char* Base::FileSystem::PathSeparator = "\\";
#else
const char* Base::FileSystem::PathSeparator = "/";
#endif

/// common error text when folder cannot be created
const char* c_strCreateFolderError = "couldn't create folder";

/// common error text when folder cannot be removed
const char* c_strRemoveFolderError = "couldn't remove folder";


#if defined(HAVE_WIN32) // use win32 API (or better, Microsoft's C runtime)

/// The meta-files "." and ".." are not added to the file list.
void Base::FileSystem::FindFiles(const std::string& searchPath, std::vector<std::string>& fileList)
{
   // find out base path
   std::string basePath(searchPath);
   std::string::size_type pos = basePath.find_last_of("\\/");
   if (pos != std::string::npos)
      basePath.erase(pos + 1);

   // try to find files by pathname
   _finddata_t fileinfo;

   long handle = _findfirst(searchPath.c_str(), &fileinfo);
   if (handle != -1)
   {
      std::string filename;

      do
      {
         // check for the meta-files
         if (strncmp(fileinfo.name, ".", 1) != 0 &&
            strncmp(fileinfo.name, "..", 2) != 0)
         {
            filename = basePath + fileinfo.name;

            // add to filelist
            fileList.push_back(filename);
         }

      } while (0 == _findnext(handle, &fileinfo));

      _findclose(handle);
   }
}

#elif defined(HAVE_GLOB_H) // this system has glob.h

#include <glob.h>

/// The meta-files "." and ".." are not added to the file list.
void Base::FileSystem::FindFiles(const std::string& searchPath, std::vector<std::string>& fileList)
{
   std::string path(searchPath);

   glob_t globres;
   int err = glob(path.c_str(), GLOB_NOSORT, 0, &globres);

   // check for return code
   switch (err)
   {
   case 0: // ok
      for (unsigned int i = 0; i < globres.gl_pathc; i++)
         fileList.push_back(globres.gl_pathv[i]);

      globfree(&globres);
      return;

   case 3: // no matches
      break;

   default: // error
      UaTrace("glob error: %u\n", err);
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

void Base::FileSystem::RemoveFile(const std::string& filename)
{
#ifdef HAVE_WIN32

#ifdef HAVE_UNICODE
   std::wstring wfilename;
   String::ConvertToUnicode(filename, wfilename);
   DeleteFileW(wfilename.c_str());
#else
   DeleteFileA(filename.c_str());
#endif

#else
   remove(filename.c_str());
#endif
}

/// implementation borrowed from Exult, files/utils.cc
void Base::FileSystem::MakeFolder(const std::string& folderName)
{
   std::string name(folderName);

   std::string::size_type pos = name.find_last_not_of('/');

   if (pos != std::string::npos)
      name.resize(pos + 1);

   {
      std::string parent(name);

      std::string::size_type pos2 = parent.find_last_of('/');

      if (pos2 != std::string::npos)
         parent.erase(pos2);

      bool exists = FolderExists(parent);
      if (!exists)
      {
         // call recursively
         MakeFolder(parent);
      }
   }

#if defined(HAVE_WIN32)

   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wname;
   String::ConvertToUnicode(name, wname);

   BOOL ret = CreateDirectoryW(wname.c_str(), NULL);
#else
   BOOL ret = CreateDirectoryA(name.c_str(), NULL);
#endif
   if (ret == FALSE)
   {
      DWORD errror = GetLastError();
      if (errror != ERROR_ALREADY_EXISTS)
         throw Base::FileSystemException(c_strCreateFolderError, name, errror);
   }

#else

   // posix (?)
   int iRet = mkdir(name.c_str(), 0700); // create dir if not already there
   if (iRet != 0 && errno != EEXIST)
      throw Base::FileSystemException(c_strCreateFolderError, name, errno);

#endif
}

void Base::FileSystem::RemoveFolder(const std::string& folderName)
{
#if defined(HAVE_WIN32)

   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wfolderName;
   String::ConvertToUnicode(folderName, wfolderName);

   BOOL ret = RemoveDirectoryW(wfolderName.c_str());
#else
   BOOL ret = RemoveDirectoryA(folderName.c_str());
#endif
   if (ret == FALSE)
      throw Base::FileSystemException(c_strRemoveFolderError, folderName, GetLastError());

#else

   // posix (?)
   int iRet = rmdir(folderName.c_str());
   if (iRet != 0 && errno != EEXIST)
      throw Base::FileSystemException(c_strRemoveFolderError, folderName, errno);

#endif
}

bool Base::FileSystem::FileExists(const std::string& filename)
{
#ifdef HAVE_WIN32
   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wfilename;
   String::ConvertToUnicode(filename, wfilename);

   DWORD ret = GetFileAttributesW(wfilename.c_str());
#else
   DWORD ret = GetFileAttributesA(filename.c_str());
#endif
   return (ret != (DWORD)-1) &&
      (ret & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
   // every other sane system
   struct stat sbuf;
   return
      (stat(filename.c_str(), &sbuf) == 0) &&
      !S_ISDIR(sbuf.st_mode);
#endif
}

bool Base::FileSystem::FolderExists(const std::string& folderName)
{
#ifdef HAVE_MSVC
   // win32 API
#ifdef HAVE_UNICODE
   std::wstring wfolderName;
   String::ConvertToUnicode(folderName, wfolderName);

   DWORD ret = GetFileAttributesW(wfolderName.c_str());
#else
   DWORD ret = GetFileAttributesA(folderName.c_str());
#endif
   return (ret != (DWORD)-1) &&
      (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
   // every other sane system
   struct stat sbuf;
   return
      (stat(folderName.c_str(), &sbuf) == 0) &&
      S_ISDIR(sbuf.st_mode);
#endif
}

void Base::FileSystem::RecursiveRemoveFolder(const std::string& pathname)
{
   // collect all files and folders
   std::vector<std::string> fileList;
   std::string searchPath(pathname);
   searchPath += "/*.*";
   FindFiles(searchPath, fileList);

   std::vector<std::string>::size_type max = fileList.size();
   for (std::vector<std::string>::size_type i = 0; i < max; i++)
   {
      std::string filename(fileList[i]);

      // is a file? then try to remove it
      if (FileExists(filename))
      {
         try
         {
            RemoveFile(filename);
         }
         catch (const Base::Exception& ex)
         {
            ex;
         }
      }
      else
         // is a folder? try to remove contents
         if (FolderExists(filename))
         {
            RecursiveRemoveFolder(filename);
         }
   }

   // try to remove this folder
   try
   {
      RemoveFolder(pathname);
   }
   catch (const Base::Exception& ex)
   {
      ex;
   }
}

/// \brief returns system specific home path
/// Win32: returns a subfolder of the "common application data" folder stored under
///    the user profile of the logged in user, commonly found under
///    "Documents and Settings", or the current directory, if the folder
///    doesn't exist or is not supported by the system.
/// Linux (and all with HAVE_HOME defined): returns the contents of the HOME
///    environment variable. Special cases:
/// MacOS X: "Library/Preferences/Underworld Adventures/" subfolder of HOME folder
/// BeOS: "config/settings/uwadv/" subfolder ofHOME folder
/// All other systems: returns the current folder
std::string Base::FileSystem::GetHomePath()
{
   std::string homePath = "./";

#ifdef HAVE_HOME
   {
      const char* homedir = getenv("HOME");
      if (homedir != NULL)
      {
         // User has a home directory
         homePath = homedir;

#ifdef HAVE_MACOSX
         // JCD: in Mac OS X, we put user config into user's Library folder
         homePath += "/Library/Preferences/Underworld Adventures/";
#elif defined( HAVE_BEOS )
         homePath += "/config/settings/uwadv/";
#else
         homePath += "/.uwadv/";
#endif
      }

      // try to create home folder
      UaTrace("creating uahome folder \"%s\"\n", homePath.c_str());
      Base::FileSystem::MakeFolder(homePath.c_str());
   }

#elif defined(HAVE_WIN32) // Win32 API

   char appdataPath[MAX_PATH];
   HRESULT hr = ::SHGetFolderPathA(NULL, CSIDL_COMMON_APPDATA | CSIDL_FLAG_CREATE,
      NULL, SHGFP_TYPE_CURRENT, appdataPath);
   if (hr == S_OK && appdataPath[0] != 0)
   {
      homePath = appdataPath;
      homePath += "\\Underworld Adventures\\";

      std::string configFile(homePath);
      configFile += "uwadv.cfg";

      // check if the path really exists, and that it contains uwadv.cfg
      if (!Base::FileSystem::FolderExists(homePath) ||
         !Base::FileSystem::FileExists(configFile))
         homePath = "./"; // revert to current dir
   }

#else // all other OSes

   // assume current working folder as home dir
   homePath = "./";

#endif

   return homePath;
}

namespace Detail
{
   /// \brief Returns system specific personal folder
   /// Win32: returns the "My Documents" folder, or the home folder returned by
   ///    GetHomePath(), if the folder doesn't exist or is not supported by the
   ///    system.
   /// All other systems: returns the home folder returned by GetHomePath()
   std::string GetPersonalFolderPath()
   {
      std::string personalFolderPath = Base::FileSystem::GetHomePath();

#ifdef HAVE_WIN32 // Win32 API

      char personalFolderPathBuffer[MAX_PATH];
      HRESULT hr = ::SHGetFolderPathA(NULL, CSIDL_PERSONAL | CSIDL_FLAG_CREATE,
         NULL, SHGFP_TYPE_CURRENT, personalFolderPathBuffer);

      if (hr == S_OK && personalFolderPathBuffer[0] != 0)
      {
         personalFolderPath = personalFolderPathBuffer;
      }
#endif

      return personalFolderPath;
   }

} // namespace Detail
