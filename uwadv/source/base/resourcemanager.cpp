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
/*! \file resourcemanager.cpp

   \brief resource manager implementation

*/

// needed includes
#include "base.hpp"
#include "resourcemanager.hpp"
#include "settings.hpp"
#include "filesystem.hpp"
#include <SDL_rwops.h>
#include <algorithm>
#include "zziplib/SDL_rwops_zzip.h"
#include <cerrno>

using Base::ResourceManager;
using Base::Settings;

namespace Detail
{
std::string GetHomePath();

} // namespace Detail


// ResourceManager methods

/*! The settingUadataPath setting must be set in the settings object. */
ResourceManager::ResourceManager(const Settings& settings)
:m_strUaDataPath(settings.GetString(Base::settingUadataPath)),
 m_strUwPath(settings.GetString(Base::settingUnderworldPath)),
 m_strUw1Path(settings.GetString(Base::settingUw1Path)),
 m_strUw2Path(settings.GetString(Base::settingUw2Path))
{
   UaAssert(m_strUaDataPath.size() > 0);
}

/*! The search order for resource files is as follows:
    If a real file exists in the "uadata-path" folder, it is opened and returned.
    All "uadata??.uar" files in the folder are searched for the file. Search for
    the file is started with the last .uar file. This way a user can override files
    found in the base uadata00.uar with his own files.
    .uar files are .zip files that contain files and subfolders.
    SDL_RWFromZZIP is used to open files inside .uar files.
*/
SDL_RWops* ResourceManager::GetResourceFile(const std::string& strRelFilename)
{
   UaAssert(m_strUaDataPath.size() > 0); // must have called LoadSettings() before

   // first, we try to open the real file
   std::string strFilename = m_strUaDataPath + "/" + strRelFilename;

   // try to open file
   SDL_RWops* rwops = SDL_RWFromFile(strFilename.c_str(), "rb");

   if (rwops != NULL)
      return rwops; // found real file

   // find all uadata resource files
   std::string strUarFileSpec = m_strUaDataPath + "/uadata??.uar";
   std::vector<std::string> vecFileList;
   Base::FileSystem::FindFiles(strUarFileSpec, vecFileList);

   std::sort(vecFileList.begin(), vecFileList.end());

   // search for the file, starting with the last .uar file
   for (int i = static_cast<int>(vecFileList.size())-1; i >= 0; i--)
   {
      // try to open from zip file
      std::string strZipPath(vecFileList[i] + "/" + strRelFilename);

      rwops = SDL_RWFromZZIP(strZipPath.c_str(), "rb");
      if (rwops != NULL)
         break;
   }

   return rwops;
}

/*! \todo implement reading from a zip file, e.g. uw_demo.zip
*/
SDL_RWops* ResourceManager::GetUnderworldFile(Base::EUnderworldResourcePath resPath, const std::string& strRelFilePath)
{
   std::string strFilename;

   switch(resPath)
   {
   case resourceGameUw: strFilename = m_strUwPath; break;
   case resourceGameUw1: strFilename = m_strUw1Path; break;
   case resourceGameUw2: strFilename = m_strUw2Path; break;
   }

   std::string strRealFilePath(strRelFilePath);
   MapUnderworldFilename(strRealFilePath);

   strFilename += strRealFilePath;

   if (!Base::FileSystem::FileExists(strFilename))
      throw Base::FileSystemException("couldn't find uw game file", strFilename, ENOENT);

   return SDL_RWFromFile(strFilename.c_str(), "rb");
}

/*! \todo implement mapping */
void ResourceManager::MapUnderworldFilename(std::string& strRelFilename)
{
   Base::String::Lowercase(strRelFilename);
}
