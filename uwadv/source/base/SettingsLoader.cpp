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
// Foundation, Inc., 59 Temple Place, Sindexte 330, Boston, MA  02111-1307  USA
//
/// \file SettingsLoader.cpp
/// \brief settings loader implementation
//
#include "pch.hpp"
#include "Settings.hpp"
#include "FileSystem.hpp"

using Base::Settings;

namespace Detail
{
   std::string GetPersonalFolderPath();

   /// settings loader
   class SettingsLoader
   {
   public:
      /// ctor
      SettingsLoader(Base::Settings& settings)
         :m_settings(settings),
         m_homePath(Base::FileSystem::GetHomePath()),
         m_personalFolder(GetPersonalFolderPath())
      {
      }

      /// initializes list of all config files to load
      void InitConfigFilesList();

      /// loads all config files in list
      void LoadConfigFiles();

      /// checks paths of various settings variables
      void CheckPaths();

   protected:
      /// resolves placeholder in path to create a valid path
      void ResolvePath(std::string& path);

   private:
      /// deleted copy ctor
      SettingsLoader(const SettingsLoader&) = delete;

      /// deleted assignment operator
      SettingsLoader& operator=(const SettingsLoader&) = delete;

   private:
      /// settings to load
      Base::Settings& m_settings;

      /// config files list
      std::vector<std::string> m_configFilesList;

      /// home path
      std::string m_homePath;

      /// uadata path
      std::string m_uadataPath;

      /// personal folder, or ./ if not available
      std::string m_personalFolder;
   };

   /// Config files (always named uwadv.cfg) can be found in these locations:
   /// If CONFIGDIR is defined (all automake-using targets), the file in this
   /// folder is tried to be loaded.
   /// When the home folder (retrieved with GetHomePath()) contains a config
   /// file, it is loaded, too.
   void SettingsLoader::InitConfigFilesList()
   {
#ifdef CONFIGDIR

      std::string globalConfigFile = CONFIGDIR;
      globalConfigFile += "uwadv.cfg";

      m_configFilesList.push_back(globalConfigFile);

#endif

      // add cfg file in uahome path
      std::string homeConfigFile(m_homePath + "/uwadv.cfg");
      m_configFilesList.push_back(homeConfigFile);
   }

   void SettingsLoader::LoadConfigFiles()
   {
      // go through the list of all config files
      bool loadedConfigFile = false;
      size_t maxIndex = m_configFilesList.size();
      for (size_t index = 0; index < maxIndex; index++)
      {
         const std::string& configFilename = m_configFilesList[index];
         if (Base::FileSystem::FileExists(configFilename))
         {
            // try to load config file
            try
            {
               UaTrace("loading %s\n", configFilename.c_str());
               m_settings.Load(configFilename);
               loadedConfigFile = true;
            }
            catch (const Base::FileSystemException& ex)
            {
               UNUSED(ex);
               UaTrace("couldn't load config file: %s (%s)\n", configFilename.c_str(), ex.what());
            }
         }
      }

      // error when no config file could be loaded at all
      if (!loadedConfigFile)
         throw Base::RuntimeException("Couldn't load any configuration files");
   }

   void SettingsLoader::CheckPaths()
   {
      // resolve some paths in the settings values
      Base::SettingsType types[] =
      {
         Base::settingUadataPath,
         Base::settingSavegameFolder,
         Base::settingUw1Path,
         Base::settingUw2Path,
         Base::settingCustomKeymap
      };

      std::string path;
      for (unsigned int index = 0; index < SDL_TABLESIZE(types); index++)
      {
         Base::SettingsType type = types[index];

         // resolve path
         path = m_settings.GetString(type);

         // %uadata% must not appear in this value
         if (type == Base::settingUadataPath)
            UaAssert(path.find("%uadata%") == std::string::npos);

         ResolvePath(path);

         // check if path ends with slash
         std::string::size_type pos = path.find_last_of("\\/");
         if (pos != std::string::npos && pos != path.size() - 1)
         {
            path += Base::FileSystem::PathSeparator;
         }

         m_settings.SetValue(type, path);

         // special handling for uadata path
         if (type == Base::settingUadataPath)
            m_uadataPath = path;
      }
   }

   /// Replaces the following variables:
   /// - ~ replaced with HOME environment variable (when available)
   /// - %uahome% replaced with uwadv's home path
   /// - %uadata% replaced with the uadata resource path
   /// - %personal-folder% replaced with Win32's "Personal Folder" folder
   ///   (same as home folder on other systems)
   void SettingsLoader::ResolvePath(std::string& path)
   {
      std::string::size_type pos = 0;

      // replace ~ at start of path
      if (0 == path.find("~"))
      {
         const char* home = getenv("HOME");
         if (home != NULL)
            path.replace(pos, 1, home);
      }

      Base::String::Replace(path, "%uahome%", m_homePath);
      Base::String::Replace(path, "%uadata%", m_uadataPath);
      Base::String::Replace(path, "%personal-folder%", m_personalFolder);

      // replace any double-slashes from previous replacements
      Base::String::Replace(path, "//", "\\");
      Base::String::Replace(path, "\\", Base::FileSystem::PathSeparator);
   }

} // namespace Detail

/// Settings are loaded from uwadv.cfg file that can be found in several
/// folders. See Detail::SettingsLoader::InitConfigFilesList for a list of
/// all places where the file can be from.
void Base::LoadSettings(Base::Settings& settings)
{
   UaTrace("loading all config files ...\n");

   Detail::SettingsLoader loader(settings);

   loader.InitConfigFilesList();
   loader.LoadConfigFiles();
   loader.CheckPaths();
}
