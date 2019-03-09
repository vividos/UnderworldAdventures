//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file ResourceManager.cpp
/// \brief resource manager implementation
//
#include "Base.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"
#include "FileSystem.hpp"
#include <SDL_rwops.h>
#include <algorithm>
#include <SDL_rwops_zzip.h>
#include <cerrno>
#include <deque>

using Base::ResourceManager;
using Base::Settings;

/// The settingUadataPath setting must be set in the settings object.
ResourceManager::ResourceManager(const Settings& settings)
   :m_uadataPath(settings.GetString(Base::settingUadataPath)),
   m_uwPath(settings.GetString(Base::settingUnderworldPath)),
   m_uw1Path(settings.GetString(Base::settingUw1Path)),
   m_uw2Path(settings.GetString(Base::settingUw2Path))
{
   UaAssert(m_uadataPath.size() > 0);

   Rescan(settings);
}

/// The search order for resource files is as follows:
/// If a real file exists in the "uadata-path" folder, it is opened and returned.
/// All "uadata??.zip" files in the folder are searched for the file. Search for
/// the file is started with the last .zip file. This way a user can override files
/// found in the base uadata00.zip with his own files.
/// SDL_RWFromZZIP is used to open files inside .zip files.
Base::SDL_RWopsPtr ResourceManager::GetResourceFile(const std::string& relativeFilename)
{
   UaAssert(m_uadataPath.size() > 0); // must have called LoadSettings() before

   // first, we try to open the real file
   std::string filename = m_uadataPath + relativeFilename;

   SDL_RWopsPtr rwops = MakeRWopsPtr(SDL_RWFromFile(filename.c_str(), "rb"));

   if (rwops.get() != NULL)
      return rwops; // found real file

   // find all uadata resource files
   std::string uarFileSpec = m_uadataPath + "/uadata??.zip";
   std::vector<std::string> fileList;
   Base::FileSystem::FindFiles(uarFileSpec, fileList);

   std::sort(fileList.begin(), fileList.end());

   // search for the file, starting with the last .uar file
   for (int i = static_cast<int>(fileList.size()) - 1; i >= 0; i--)
   {
      // try to open from zip file
      std::string zipPath(fileList[i] + "/" + relativeFilename);

      rwops = MakeRWopsPtr(SDL_RWFromZZIP(zipPath.c_str(), "rb"));
      if (rwops.get() != NULL)
         break;
   }

   return rwops;
}

/// \todo implement reading from a zip file, e.g. uw_demo.zip
Base::SDL_RWopsPtr ResourceManager::GetUnderworldFile(Base::UnderworldResourcePath resourcePath, const std::string& relativeFilename)
{
   std::string basePath;

   switch (resourcePath)
   {
   case resourceGameUw: basePath = m_uwPath; break;
   case resourceGameUw1: basePath = m_uw1Path; break;
   case resourceGameUw2: basePath = m_uw2Path; break;
   }

   std::string filename = basePath + relativeFilename;

   return GetFile(filename);
}

Base::SDL_RWopsPtr ResourceManager::GetFile(const std::string& absoluteFilename)
{
   std::string filename = absoluteFilename;
   MapUnderworldFilename(filename);

   if (!Base::FileSystem::FileExists(filename))
      throw Base::FileSystemException("couldn't find uw game file", filename, ENOENT);

   return MakeRWopsPtr(SDL_RWFromFile(filename.c_str(), "rb"));
}

void ResourceManager::Rescan(const Settings& settings)
{
   m_uwPath = settings.GetString(Base::settingUnderworldPath);
   if (m_uwPath.empty())
      return;

   RescanUnderworldFilenames(m_uwPath);
}

void ResourceManager::RescanUnderworldFilenames(std::string uwPath)
{
   m_mapLowercaseFilenamesToActualFilenames.clear();

   std::deque<std::string> folderList;
   folderList.push_back(uwPath);

   while (!folderList.empty())
   {
      std::string path = folderList.front();
      folderList.pop_front();

      std::vector<std::string> fileList;
      FileSystem::FindFiles(path + "*.*", fileList);

      for (auto filename : fileList)
      {
         if (FileSystem::FolderExists(filename))
         {
            folderList.push_back(filename + FileSystem::PathSeparator);
            continue;
         }

         std::string lowerFilename = filename;
         String::Lowercase(lowerFilename);

         m_mapLowercaseFilenamesToActualFilenames.insert(
            std::make_pair(lowerFilename, filename));
      }
   }
}

void ResourceManager::MapUnderworldFilename(std::string& filenameToMap)
{
   std::string lowercaseFilename = filenameToMap;
   String::Lowercase(lowercaseFilename);

   // normalize path separators to get a file system match
   std::replace(lowercaseFilename.begin(), lowercaseFilename.end(), '/', FileSystem::PathSeparator[0]);
   std::replace(lowercaseFilename.begin(), lowercaseFilename.end(), '\\', FileSystem::PathSeparator[0]);

   auto iter = m_mapLowercaseFilenamesToActualFilenames.find(lowercaseFilename);
   if (iter != m_mapLowercaseFilenamesToActualFilenames.end())
      filenameToMap = iter->second;
}
