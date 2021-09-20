//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019,2021 Underworld Adventures Team
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
/// \details Parts of this file's code was taken from Exult
//
#include "pch.hpp"
#include "FileSystem.hpp"
#include <filesystem>
#include <deque>

#ifdef HAVE_WIN32
#ifdef HAVE_MINGW
#define _WIN32_IE 0x0500 // define this to find SHGFP_TYPE_CURRENT in shlobj.h
#endif
#include <shlobj.h> // for SHGetFolderPathA
#endif

#if defined(HAVE_WIN32)
const char* Base::FileSystem::PathSeparator = "\\";
#else
const char* Base::FileSystem::PathSeparator = "/";
#endif

/// matches a wildcard pattern; case insensitive; \see
/// https://stackoverflow.com/questions/3300419/file-name-matching-with-wildcard
bool PatternMatches(const std::string& path, const std::string& pattern)
{
   if (pattern == "*" || pattern == "*.*")
      return true;

   std::string::const_iterator pathIter = path.begin();
   for (std::string::const_iterator patternIter = pattern.begin(); patternIter != pattern.end(); ++patternIter)
   {
      switch (*patternIter)
      {
      case '?':
         if (pathIter == path.end())
            return false;
         ++pathIter;
         break;

      case '*':
      {
         if (patternIter + 1 == pattern.end())
            return true;

         size_t max = strlen(&*pathIter);
         for (size_t i = 0; i < max; i++)
            if (PatternMatches(&*pathIter + i, &*patternIter + 1))
               return true;
         return false;
      }

      default:
         if (tolower(*pathIter) != tolower(*patternIter))
            return false;
         ++pathIter;
      }
   }

   return pathIter == path.end();
}

/// The meta-files "." and ".." are not added to the file list.
void Base::FileSystem::FindFiles(const std::string& searchPath, std::vector<std::string>& fileList,
   bool recursive)
{
   // find out base path
   std::string basePath{ searchPath };
   std::string::size_type pos = basePath.find_last_of("\\/");
   if (pos != std::string::npos)
      basePath.erase(pos + 1);

   std::string pattern = searchPath.substr(pos + 1);

   std::deque<std::string> filePathQueue;
   filePathQueue.push_back(basePath);

   while (!filePathQueue.empty())
   {
      std::string folder = filePathQueue.front();
      filePathQueue.pop_front();

      for (auto entry : std::filesystem::directory_iterator(std::filesystem::path{ folder }))
      {
         if (entry.is_regular_file() &&
            PatternMatches(entry.path().filename().string(), pattern))
            fileList.push_back(entry.path().string());

         if (recursive &&
            entry.is_directory())
         {
            filePathQueue.push_back(entry.path().string());
         }
      }
   }
}

void Base::FileSystem::RemoveFile(const std::string& filename)
{
   std::error_code ec;
   std::filesystem::remove(
      std::filesystem::path(filename), ec);

   if (ec)
      UaTrace("error removing file: %s (%s)", filename.c_str(), ec.message().c_str());
}

/// implementation borrowed from Exult, files/utils.cc
void Base::FileSystem::MakeFolder(const std::string& folderName)
{
   std::error_code ec;
   std::filesystem::create_directories(
      std::filesystem::path(folderName), ec);

   if (ec)
   {
      UaTrace("error creating folder: %s (%s)", folderName.c_str(), ec.message().c_str());
      throw Base::FileSystemException("couldn't create folder", folderName, ec.value());
   }
}

void Base::FileSystem::RemoveFolder(const std::string& folderName)
{
   std::error_code ec;
   std::filesystem::remove(
      std::filesystem::path(folderName), ec);

   if (ec)
   {
      UaTrace("error deleting folder: %s (%s)", folderName.c_str(), ec.message().c_str());
      throw Base::FileSystemException("couldn't delete folder", folderName, ec.value());
   }
}

bool Base::FileSystem::FileExists(const std::string& filename)
{
   std::error_code ec;
   return
      std::filesystem::exists(std::filesystem::path(filename), ec) &&
      std::filesystem::is_regular_file(filename, ec) &&
      !ec;
}

bool Base::FileSystem::FolderExists(const std::string& folderName)
{
   std::error_code ec;
   return
      std::filesystem::exists(std::filesystem::path(folderName), ec) &&
      std::filesystem::is_directory(folderName, ec) &&
      !ec;
}

void Base::FileSystem::RecursiveRemoveFolder(const std::string& pathname)
{
   std::error_code ec;
   std::filesystem::remove_all(
      std::filesystem::path(pathname), ec);

   if (ec)
   {
      UaTrace("error deleting folder: %s (%s)", pathname.c_str(), ec.message().c_str());
      throw Base::FileSystemException("couldn't delete folder recursively", pathname, ec.value());
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
