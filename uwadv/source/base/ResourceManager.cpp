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
#include "pch.hpp"
#include "ResourceManager.hpp"
#include "Settings.hpp"
#include "FileSystem.hpp"
#include <SDL_rwops.h>
#include <algorithm>
#include <zziplib.h>
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

/// returns a file that can contain placeholder like %uw-path% and %uadata%
Base::SDL_RWopsPtr ResourceManager::GetFileWithPlaceholder(const std::string& filename) const
{
   if (filename.find("%uadata%") == 0)
   {
      std::string uadataRelativePath = filename.substr(8);
      if (uadataRelativePath.substr(0, 1) == "/")
         uadataRelativePath = uadataRelativePath.substr(1);

      return GetResourceFile(uadataRelativePath);
   }
   else if (filename.find("%uw-path%") == 0)
   {
      std::string uwpathRelativePath = filename.substr(9);
      if (uwpathRelativePath.substr(0, 1) == "/")
         uwpathRelativePath = uwpathRelativePath.substr(1);

      return GetUnderworldFile(Base::resourceGameUw, uwpathRelativePath);
   }
   else if (Base::FileSystem::FileExists(filename.c_str()))
   {
      return GetFile(filename);
   }
   else
      throw Base::FileSystemException("couldn't find file for filename with placeholders", filename, ENOENT);
}

/// resolves a filename that contains placeholders
void ResourceManager::ResolvePlaceholderFilename(std::string& filename) const
{
   std::string::size_type pos;

   while (std::string::npos != (pos = filename.find("%uadata%")))
      filename.replace(pos, 8, m_uadataPath.c_str());

   while (std::string::npos != (pos = filename.find("%uw-path%")))
      filename.replace(pos, 9, m_uwPath.c_str());
}

/// The search order for resource files is as follows:
/// If a real file exists in the "uadata-path" folder, it is opened and returned.
/// All "uadata??.zip" files in the folder are searched for the file. Search for
/// the file is started with the last .zip file. This way a user can override files
/// found in the base uadata00.zip with his own files.
/// SDL_RWFromZZIP is used to open files inside .zip files.
Base::SDL_RWopsPtr ResourceManager::GetResourceFile(const std::string& relativeFilename) const
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

Base::SDL_RWopsPtr ResourceManager::GetUnderworldFile(
   Base::UnderworldResourcePath resourcePath,
   const std::string& relativeFilename) const
{
   // check zip archives
   if (resourcePath == resourceGameUw)
   {
      std::string lowercaseRelativeFilename = relativeFilename;
      String::Lowercase(lowercaseRelativeFilename);

      auto iter = m_mapRelativeLowercaseFilenamesToZipArchiveFilename.find(lowercaseRelativeFilename);
      if (iter != m_mapRelativeLowercaseFilenamesToZipArchiveFilename.end())
      {
         return MakeRWopsPtr(::SDL_RWFromZZIP(iter->second.c_str(), "rb"));
      }
   }

   // check file system
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

Base::SDL_RWopsPtr ResourceManager::GetFile(const std::string& absoluteFilename) const
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
   RescanUnderworldZipArchives(m_uwPath);
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

void ResourceManager::RescanUnderworldZipArchives(const std::string& uwPath)
{
   std::vector<std::string> fileList;
   FileSystem::FindFiles(uwPath + "*.zip", fileList);

   // also find GoG.com zip archive
   FileSystem::FindFiles(uwPath + "*.gog", fileList);

   for (auto zipFilename : fileList)
      RescanZipArchive(zipFilename);
}

void ResourceManager::RescanZipArchive(const std::string& zipFilename)
{
   zzip_error_t errorCode = ZZIP_NO_ERROR;
   ZZIP_DIR* archive = zzip_dir_open(zipFilename.c_str(), &errorCode);
   std::shared_ptr<ZZIP_DIR> autoFree(archive,
      [](ZZIP_DIR* dir) { if (dir != NULL) zzip_dir_close(dir); });

   if (archive == NULL || errorCode != ZZIP_NO_ERROR)
   {
      UaTrace("couldn't open zip archive: %s (%s)\n",
         zipFilename.c_str(),
         zzip_strerror(errorCode));
      return;
   }

   std::map<std::string, std::string> mapRelativeLowercaseFilenamesToZipArchiveFilename;

   ZZIP_DIRENT dirEntry = { 0 };
   int ret;
   while ((ret = zzip_dir_read(archive, &dirEntry)) != 0)
   {
      std::string relativeFilename = dirEntry.d_name;

      // ignore non-relative entries
      if (relativeFilename.find("/") == 0 ||
         relativeFilename.find("..") != std::string::npos)
         continue;

      std::string lowercaseRelativeFilename = relativeFilename;
      String::Lowercase(lowercaseRelativeFilename);

      // the "inside" zip archive filename can be opened SDL_RWFromZZIP() directly
      std::string insideZipArchiveFilename =
         zipFilename + "/" + relativeFilename;

      mapRelativeLowercaseFilenamesToZipArchiveFilename.insert(
         std::make_pair(lowercaseRelativeFilename, insideZipArchiveFilename));
   }

   CheckAndAddZipArchive(zipFilename, mapRelativeLowercaseFilenamesToZipArchiveFilename);
}

void ResourceManager::CheckAndAddZipArchive(const std::string& zipFilename,
   const std::map<std::string, std::string>& theMap)
{
   // check neccesary files in the zip archive mapping
   auto end = theMap.end();
   bool foundDemo = theMap.find("uwdemo.exe") != end && theMap.find("data/level13.st") != end;
   bool foundUw1 = theMap.find("uw.exe") != end && theMap.find("data/lev.ark") != end;
   bool foundUw2 = theMap.find("uw2.exe") != end && theMap.find("data/sdc.ark") != end;

   /// GoG.com's Ultima Underworld has both uw1 and uw2 in a game.gog that's a zip archive
   bool foundGogUw1Uw2 =
      theMap.find("uw/uw.exe") != end && theMap.find("uw/data/lev.ark") != end &&
      theMap.find("uw2/uw2.exe") != end && theMap.find("uw2/data/sdc.ark") != end;

   if (foundDemo || foundUw1 || foundUw2 || foundGogUw1Uw2)
   {
      m_mapRelativeLowercaseFilenamesToZipArchiveFilename.insert(
         theMap.begin(),
         theMap.end());
   }
   else
   {
      std::string reason = "zip archive doesn't contain uw_demo, uw1 or uw2 game files";

      if (theMap.find("uw_demo.exe") != end &&
         theMap.size() == 1)
      {
         reason = "zip archive only contains the uw_demo.exe self extracting archive for uw_demo; please extract it with dosbox first";
      }
      else if (theMap.find("uw_demo.exe") != end &&
         theMap.find("data/level13.st") == end)
      {
         reason = "zip archive is a corrupt uw_demo zip archive that is missing its subfolder files; please use a valid zip archive";
      }
      else if (theMap.find("uw.lzh") != end &&
         theMap.find("cnv.lzh") != end)
      {
         reason = "zip archive contains the installation files for uw1, not the extracted game files itself";
      }
      else if (theMap.find("base0001.lzh") != end)
      {
         reason = "zip archive contains the installation files for uw2, not the extracted game files itself";
      }

      UaTrace("zip archive \"%s\" rejected: %s\n",
         zipFilename.c_str(),
         reason.c_str());
   }
}

bool ResourceManager::CheckUw1GameFilesAvailable(bool& isUw1Demo) const
{
   isUw1Demo = false;

   // check for uw game files
   if (IsUnderworldFileAvailable("data/cnv.ark") ||
      IsUnderworldFileAvailable("data/strings.pak") ||
      IsUnderworldFileAvailable("data/pals.dat") ||
      IsUnderworldFileAvailable("data/allpals.dat"))
   {
      // could be uw1 or uw_demo
      if (IsUnderworldFileAvailable("uw.exe"))
         return true; // found all needed files
      else
         // check if we only have the demo
         if (IsUnderworldFileAvailable("uwdemo.exe") &&
            IsUnderworldFileAvailable("data/level13.st") &&
            IsUnderworldFileAvailable("data/level13.anx") &&
            IsUnderworldFileAvailable("data/level13.txm"))
         {
            // found all needed files for uw_demo
            isUw1Demo = true;
            return true;
         }
   }

   return false;
}

bool ResourceManager::CheckUw2GameFilesAvailable() const
{
   return IsUnderworldFileAvailable("data/scd.ark") ||
      IsUnderworldFileAvailable("data/strings.pak") ||
      IsUnderworldFileAvailable("data/pals.dat") ||
      IsUnderworldFileAvailable("data/allpals.dat") ||
      IsUnderworldFileAvailable("uw2.exe") ||
      IsUnderworldFileAvailable("data/t64.tr");
}

bool ResourceManager::IsUnderworldFileAvailable(const char* relativeFilename) const
{
   std::string filename = relativeFilename;
   String::Lowercase(filename);

   if (m_mapRelativeLowercaseFilenamesToZipArchiveFilename.find(filename) !=
      m_mapRelativeLowercaseFilenamesToZipArchiveFilename.end())
      return true; // found in a zip archive

   std::string absoluteFilename = m_uw1Path + filename;
   MapUnderworldFilename(absoluteFilename);

   if (Base::FileSystem::FileExists(absoluteFilename.c_str()))
      return true;

   absoluteFilename = m_uw2Path + filename;
   MapUnderworldFilename(absoluteFilename);

   return Base::FileSystem::FileExists(absoluteFilename.c_str());
}

void ResourceManager::MapUnderworldFilename(std::string& filenameToMap) const
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
