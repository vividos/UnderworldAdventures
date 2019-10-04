//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Underworld Adventures Team
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
/// \file ResourceManager.hpp
/// \brief resource manager
//
#pragma once

#include <string>
#include <map>

namespace Base
{
   /// resource path for GetUnderworldFile()
   enum UnderworldResourcePath
   {
      resourceGameUw,   ///< load file from currently selected uw game
      resourceGameUw1,  ///< load file from uw1 game (if path is specified)
      resourceGameUw2,  ///< load file from uw2 game (if path is specified)
   };

   class Settings;

   /// \brief Resource manager
   /// Manages access to resource files.
   class ResourceManager
   {
   public:
      /// ctor
      ResourceManager(const Settings& settings);

      /// detects game type based on the settingUnderworldPath settings value and sets game type,
      /// settingGamePrefix and settingUw1IsUwdemo settings
      void DetectGameType(Settings& settings) const;

      /// returns a file that can contain placeholder like %uw-path% and %uadata%
      SDL_RWopsPtr GetFileWithPlaceholder(const std::string& filename) const;

      /// resolves a filename that contains placeholders
      void ResolvePlaceholderFilename(std::string& filename) const;

      /// returns "uadata" resource file
      SDL_RWopsPtr GetResourceFile(const std::string& relativeFilename) const;

      /// returns ultima underworld file
      SDL_RWopsPtr GetUnderworldFile(UnderworldResourcePath resourcePath, const std::string& relativeFilename) const;

      /// returns a file that already has a full path
      SDL_RWopsPtr GetFile(const std::string& absoluteFilename) const;

      /// re-scans all available files after the "underworld" path was set in the settings
      void Rescan(const Settings& settings);

      /// checks if a file with given filename is available, either as physical file or in a zip archive
      bool IsUnderworldFileAvailable(const char* relativeFilename) const;

      /// checks if uw1 game files are available (and returns if it's the uw_demo)
      bool CheckUw1GameFilesAvailable(bool& isUw1Demo) const;

      /// checks if uw2 game files are available
      bool CheckUw2GameFilesAvailable() const;

   private:
      /// re-scans all underworld data filenames in the given path
      void RescanUnderworldFilenames(std::string uwPath);

      /// re-scans all zip archies that may contain underworld data files
      void RescanUnderworldZipArchives(const std::string& uwPath);

      /// re-scans a single zip archive
      void RescanZipArchive(const std::string& zipFilename);

      /// checks contents of zip file (by checking mapping) and adds it to the global mapping
      void CheckAndAddZipArchive(const std::string& zipFilename,
         const std::map<std::string, std::string>& mapRelativeLowercaseFilenamesToZipArchiveFilename);

      /// maps a requested filename to a real file system filename, for the underworld data files
      void MapUnderworldFilename(std::string& filenameToMap) const;

   private:
      /// home path
      std::string m_homePath;

      /// path to uadata resources
      std::string m_uadataPath;

      /// path to uw game currently selected
      std::string m_uwPath;

      /// path to uw1 game
      std::string m_uw1Path;

      /// path to uw2 game
      std::string m_uw2Path;

      /// mapping from lowercase filenames to actual file system filenames
      std::map<std::string, std::string> m_mapLowercaseFilenamesToActualFilenames;

      /// maps from relative paths of underworld files to "inside" zip archive filename
      std::map<std::string, std::string> m_mapRelativeLowercaseFilenamesToZipArchiveFilename;
   };

} // namespace Base
