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
/*! \file settings.cpp

   \brief game settings implementation

*/

// needed includes
#include "base.hpp"
#include "settings.hpp"
#include <sstream>

namespace Detail
{

// structs

//! mapping of all settings keywords to settings enum
/*! The mapping contains strings that are to be mapped to ESettingsType enum
    values when loading the settings file. Add new entries at the end when
    needed.
*/
struct SettingsMapping
{
   //! option name
   const char* cstrOptionName;

   //! settings type for option name
   Base::ESettingsType type;

} SettingsMapping[] =
{
   { "uw1-path",              Base::settingUw1Path },
   { "uw2-path",              Base::settingUw2Path },
   { "uadata-path",           Base::settingUadataPath },
   { "savegame-folder",       Base::settingSavegameFolder },
   { "custom-keymap",         Base::settingCustomKeymap },
   { "enable-uwadv-features", Base::settingUwadvFeatures },
   { "screen-resolution",     Base::settingScreenResolution },
   { "smooth-ui",             Base::settingUISmooth },
   { "fullscreen",            Base::settingFullscreen },
   { "cutscene-narration",    Base::settingCutsceneNarration },
   { "audio-enabled",         Base::settingAudioEnabled },
   { "win32-midi-device",     Base::settingWin32MidiDevice },
};

} // namespace Detail


using Base::Settings;
using Base::ESettingsType;

// Settings methods

/*! Sets default values for some settings. */
Settings::Settings()
:m_gameType(gameUw1)
{
   // set some initial values
   SetValue(settingUadataPath, std::string("./uadata/"));
   SetValue(settingSavegameFolder, std::string("./uasave/"));
   SetValue(settingFullscreen, false);
   SetValue(settingCutsceneNarration, std::string("sound"));
   SetValue(settingWin32MidiDevice, -1);
}

/*! Can be called more than once; settings that are already set are
    overwritten with settings from file. */
void Settings::Load(const std::string& strFilename)
{
   // load config file
   Base::ConfigFile cfgFile;
   cfgFile.Load(strFilename);

   // go through all keys in the file and read the value
   Base::ConfigValueMap& cfgMap = cfgFile.GetValueMap();

   Base::ConfigValueMap::const_iterator stop = cfgMap.end();
   for(Base::ConfigValueMap::const_iterator iter = cfgMap.begin(); iter != stop; iter++)
   {
      Base::ESettingsType type = Base::settingUnderworldPath;
      if (SearchTypeFromString(iter->first, type))
         SetValue(type, iter->second);
      else
         UaAssert(false);
   }
}

/*! Saves settings file, using an old version of the file as "template". The
    new file will have all comments and such, only the values of the settings
    are updated. If a template file doesn't contain a key, the value isn't
    written to the file.

    \param strFilenameNew new filename of the settings file; will be overwritten
    \param strFilenameOld filename of the template file that is used to rewrite

    \todo append key/value pairs not found in the template file at the end of
    the new file.
*/
void Settings::Save(const std::string& strFilenameNew, const std::string& strFilenameOld)
{
   // load config file
   Base::ConfigFile cfgFile;
   cfgFile.Load(strFilenameOld);

   // put all settings into loaded file
   Base::ConfigValueMap& cfgMap = cfgFile.GetValueMap();

   Base::ConfigValueMap::iterator stop = cfgMap.end();
   for(Base::ConfigValueMap::iterator iter = cfgMap.begin(); iter != stop; iter++)
   {
      Base::ESettingsType type = Base::settingUnderworldPath;
      if (SearchTypeFromString(iter->first, type))
         iter->second = GetString(type);
      else
         UaAssert(false);
   }

   // save file under new name
   cfgFile.Save(strFilenameOld, strFilenameNew);
}

/*! Default value when the setting is not found is an empty string. */
std::string Settings::GetString(ESettingsType type) const
{
   // try to find key
   SettingsMap::const_iterator iter = m_settings.find(type);

   if (iter == m_settings.end())
      return std::string("");

   // return string
   return iter->second;
}

/*! Default value when the setting is not found is 0. */
int Settings::GetInt(ESettingsType type) const
{
   // try to find key
   SettingsMap::const_iterator iter = m_settings.find(type);

   if (iter == m_settings.end())
      return 0;

   // return integer
   return static_cast<int>(strtol(iter->second.c_str(), NULL, 10));
}

/*! Boolean values in the settings can be expressed either with the words
    "true", "yes" or "1". All other values mean false. Default value when
    the setting is not found is false.
*/
bool Settings::GetBool(ESettingsType type) const
{
   // try to find key
   SettingsMap::const_iterator iter = m_settings.find(type);

   if (iter == m_settings.end())
      return false;

   std::string strValue(iter->second);

   // make lowercase
   Base::String::Lowercase(strValue);

   // check for boolean keywords
   return (strValue.compare("true")==0 || strValue.compare("1")==0 || strValue.compare("yes")==0);
}

void Settings::SetValue(ESettingsType type, std::string strValue)
{
   m_settings[type] = strValue;
}

/*! The keywords "true" and "false" are used when writing out to a settings
    config file again.
*/
void Settings::SetValue(ESettingsType type, bool bValue)
{
   std::string strValue(bValue ? "true" : "false");
   SetValue(type, strValue);
}

void Settings::SetValue(ESettingsType type, int iValue)
{
   std::ostringstream buffer;
   buffer << iValue;
   SetValue(type, buffer.str());
}

bool Settings::SearchTypeFromString(const std::string& strKeyname, ESettingsType& type)
{
   // search the whole settings table
   for(unsigned int i=0; i < SDL_TABLESIZE(Detail::SettingsMapping); i++)
   {
      // search through all option entries
      if (strKeyname == Detail::SettingsMapping[i].cstrOptionName)
      {
         // found entry
         type = Detail::SettingsMapping[i].type;
         return true;
      }
   }
   return false;
}
