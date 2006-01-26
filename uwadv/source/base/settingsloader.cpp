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
/*! \file settingsloader.cpp

   \brief settings loader implementation

*/

// needed includes
#include "base.hpp"
#include "settings.hpp"
#include "filesystem.hpp"

using Base::Settings;

namespace Detail
{
std::string GetHomePath();
std::string GetPersonalFolderPath();

//! settings loader
class SettingsLoader: public Base::NonCopyable
{
public:
   //! ctor
   SettingsLoader(Base::Settings& settings)
      :m_settings(settings),
      m_strHomePath(GetHomePath()),
      m_strPersonalFolder(GetPersonalFolderPath())
   {
   }

   //! initializes list of all config files to load
   void InitConfigFilesList();

   //! loads all config files in list
   void LoadConfigFiles();

   //! checks paths of various settings variables
   void CheckPaths();

protected:
   //! resolves placeholder in path to create a valid path
   void ResolvePath(std::string& strPath);

private:
   //! settings to load
   Base::Settings& m_settings;

   //! config files list
   std::vector<std::string> m_vecConfigFilesList;

   //! home path
   std::string m_strHomePath;

   //! uadata path
   std::string m_strUaDataPath;

   //! personal folder, or ./ if not available
   std::string m_strPersonalFolder;
};

/*! Config files (always named uwadv.cfg) can be found in these locations:
    If CONFIGDIR is defined (all automake-using targets), the file in this
    folder is tried to be loaded.
    When the home folder (retrieved with GetHomePath()) contains a config
    file, it is loaded, too.
*/
void SettingsLoader::InitConfigFilesList()
{
#ifdef CONFIGDIR

   std::string strGlobalConfigFile = CONFIGDIR;
   strGlobalConfigFile += "uwadv.cfg";

   m_vecConfigFilesList.push_back(strGlobalConfigFile);

#endif

   // add cfg file in uahome path
   std::string strHomeConfigFile(m_strHomePath + "/uwadv.cfg");
   m_vecConfigFilesList.push_back(strHomeConfigFile);
}

void SettingsLoader::LoadConfigFiles()
{
   // go through the list of all config files
   bool bLoadedCfgFile = false;
   unsigned int uiMax = m_vecConfigFilesList.size();
   for(unsigned int ui=0; ui<uiMax; ui++)
   {
      const std::string& strConfigFilename = m_vecConfigFilesList[ui];
      if (Base::FileSystem::FileExists(strConfigFilename))
      {
         // try to load config file
         try
         {
            UaTrace("loading %s\n", strConfigFilename.c_str());
            m_settings.Load(strConfigFilename);
            bLoadedCfgFile = true;
         }
         catch(Base::FileSystemException& e)
         {
            (e);
            UaTrace("couldn't load config file: %s\n", strConfigFilename.c_str());
         }
      }
   }

   // error when no config file could be loaded at all
   if (!bLoadedCfgFile)
      throw Base::RuntimeException("Couldn't load any configuration files");
}

void SettingsLoader::CheckPaths()
{
   // resolve some paths in the settings values
   Base::ESettingsType types[] =
   {
      Base::settingUadataPath,
      Base::settingSavegameFolder,
      Base::settingUw1Path,
      Base::settingUw2Path,
      Base::settingCustomKeymap
   };

   std::string strPath;
   for(unsigned int i=0; i<SDL_TABLESIZE(types); i++)
   {
      Base::ESettingsType type = types[i];

      // resolve path
      strPath = m_settings.GetString(type);

      // %uadata% must not appear in this value
      if (type == Base::settingUadataPath)
         UaAssert(m_strUaDataPath.find("%uadata%") == std::string::npos);

      ResolvePath(strPath);

      // check if path ends with slash
      std::string::size_type pos = strPath.find_last_of("\\/");
      if (pos != std::string::npos && pos != strPath.size()-1)
      {
         // add slash
         strPath += "/";
      }

      m_settings.SetValue(type, strPath);

      // special handling for some types
      switch(type)
      {
      case Base::settingUadataPath:
         m_strUaDataPath = strPath;
         break;

      case Base::settingSavegameFolder:
         // try to create folder (when not already present)
         if (!Base::FileSystem::FolderExists(strPath))
         {
            UaTrace("creating savegame folder \"%s\"\n", strPath.c_str());
            Base::FileSystem::MakeFolder(strPath.c_str());
         }
         break;
      }
   }
}

/*! Replaces the following variables:
    - ~ replaced with HOME environment variable (when available)
    - %uahome% replaced with uwadv's home path
    - %uadata% replaced with the uadata resource path
    - %personal-folder% replaced with Win32's "Personal Folder" folder
      (same as home folder on other systems)
*/
void SettingsLoader::ResolvePath(std::string& strPath)
{
   std::string::size_type pos = 0;

   // replace ~ at start of path
   if ( 0 == strPath.find("~") )
   {
      const char* home = getenv("HOME");
      if (home != NULL)
         strPath.replace(pos, 1, home);
   }

   // replace %uahome%
   while( std::string::npos != (pos = strPath.find("%uahome%") ) )
      strPath.replace(pos, 8, m_strHomePath.c_str());

   // replace %uadata%
   while( std::string::npos != (pos = strPath.find("%uadata%") ) )
      strPath.replace(pos, 8, m_strUaDataPath.c_str());

   // replace %personal-folder%
   while( std::string::npos != (pos = strPath.find("%personal-folder%") ) )
      strPath.replace(pos, 17, m_strPersonalFolder.c_str());

   // replace any double-slashes occured from previous replacements
   while( std::string::npos != (pos = strPath.find("//") ) ||
          std::string::npos != (pos = strPath.find("\\\\") )  )
      strPath.replace(pos, 2, "/");
}

} // namespace Detail

/*! Settings are loaded from uwadv.cfg file that can be found in several
    folders. See Detail::SettingsLoader::InitConfigFilesList for a list of
    all places where the file can be from.
*/
void Base::LoadSettings(Base::Settings& settings)
{
   UaTrace("loading all config files ...\n");

   Detail::SettingsLoader loader(settings);

   loader.InitConfigFilesList();
   loader.LoadConfigFiles();
   loader.CheckPaths();
}
