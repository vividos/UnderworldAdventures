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
/// \file Settings.cpp
/// \brief game settings implementation
//
#include "pch.hpp"
#include "Settings.hpp"
#include <sstream>

namespace Detail
{
   /// \brief Mapping of all settings keywords to settings enum
   /// The mapping contains strings that are to be mapped to SettingsType enum
   /// values when loading the settings file. Add new entries at the end when
   /// needed.
   struct SettingsMapping
   {
      /// option name
      const char* optionName;

      /// settings type for option name
      Base::SettingsType type;

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
using Base::SettingsType;

/// Sets default values for some settings.
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

/// Can be called more than once; settings that are already set are
/// overwritten with settings from file.
void Settings::Load(const std::string& filename)
{
   // load config file
   Base::ConfigFile configFile;
   configFile.Load(filename);

   // go through all keys in the file and read the value
   Base::ConfigValueMap& cfgMap = configFile.GetValueMap();

   Base::ConfigValueMap::const_iterator stop = cfgMap.end();
   for (Base::ConfigValueMap::const_iterator iter = cfgMap.begin(); iter != stop; iter++)
   {
      Base::SettingsType type = Base::settingUnderworldPath;
      if (SearchTypeFromString(iter->first, type))
         SetValue(type, iter->second);
      else
         UaAssert(false);
   }
}

/// Saves settings file, using an old version of the file as "template". The
/// new file will have all comments and such, only the values of the settings
/// are updated. If a template file doesn't contain a key, the value isn't
/// written to the file.
///
/// \param filenameNew new filename of the settings file; will be overwritten
/// \param filenameOld filename of the template file that is used to rewrite
///
/// \todo append key/value pairs not found in the template file at the end of
/// the new file.
void Settings::Save(const std::string& filenameNew, const std::string& filenameOld)
{
   // load config file
   Base::ConfigFile configFile;
   configFile.Load(filenameOld);

   // put all settings into loaded file
   Base::ConfigValueMap& cfgMap = configFile.GetValueMap();

   Base::ConfigValueMap::iterator stop = cfgMap.end();
   for (Base::ConfigValueMap::iterator iter = cfgMap.begin(); iter != stop; iter++)
   {
      Base::SettingsType type = Base::settingUnderworldPath;
      if (SearchTypeFromString(iter->first, type))
         iter->second = GetString(type);
      else
         UaAssert(false);
   }

   // save file under new name
   configFile.Save(filenameOld, filenameNew);
}

/// Default value when the setting is not found is an empty string.
std::string Settings::GetString(SettingsType type) const
{
   SettingsMap::const_iterator iter = m_settings.find(type);

   if (iter == m_settings.end())
      return std::string("");

   return iter->second;
}

/// Default value when the setting is not found is 0.
int Settings::GetInt(SettingsType type) const
{
   SettingsMap::const_iterator iter = m_settings.find(type);

   if (iter == m_settings.end())
      return 0;

   return static_cast<int>(strtol(iter->second.c_str(), NULL, 10));
}

/// Boolean values in the settings can be expressed either with the words
/// "true", "yes" or "1". All other values mean false. Default value when
/// the setting is not found is false.
bool Settings::GetBool(SettingsType type) const
{
   SettingsMap::const_iterator iter = m_settings.find(type);

   if (iter == m_settings.end())
      return false;

   std::string strValue(iter->second);

   Base::String::Lowercase(strValue);

   // check for boolean keywords
   return (strValue.compare("true") == 0 || strValue.compare("1") == 0 || strValue.compare("yes") == 0);
}

void Settings::SetValue(SettingsType type, std::string value)
{
   m_settings[type] = value;
}

/// The keywords "true" and "false" are used when writing out to a settings
/// config file again.
void Settings::SetValue(SettingsType type, bool value)
{
   std::string textValue(value ? "true" : "false");
   SetValue(type, textValue);
}

void Settings::SetValue(SettingsType type, int value)
{
   std::ostringstream buffer;
   buffer << value;
   SetValue(type, buffer.str());
}

bool Settings::SearchTypeFromString(const std::string& keyName, SettingsType& type)
{
   for (unsigned int i = 0; i < SDL_TABLESIZE(Detail::SettingsMapping); i++)
   {
      if (keyName == Detail::SettingsMapping[i].optionName)
      {
         type = Detail::SettingsMapping[i].type;
         return true;
      }
   }
   return false;
}
