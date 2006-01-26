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
/*! \file settings.hpp

   \brief game settings class

*/

// include guard
#ifndef uwadv_base_settings_hpp_
#define uwadv_base_settings_hpp_

// needed includes
#include <string>
#include <map>
#include "cfgfile.hpp"

namespace Base
{

class Settings;

// enums

//! settings type key
enum ESettingsType
{
   //! path to the game files (dependent on which game is currently active)
   settingUnderworldPath = 0,

   //! path to the original uw1 game files (uw1 or uw_demo)
   settingUw1Path,

   //! path to the uw2 game files
   settingUw2Path,

   //! true when the uw1 installation is the uw_demo
   settingUw1IsUwdemo,

   //! path to the "uadata" folder
   settingUadataPath,

   //! path to the savegame folder
   settingSavegameFolder,

   //! custom keymap file to load
   settingCustomKeymap,

   //! boolean value that indicates if uwadv-specific features are enabled
   settingUwadvFeatures,

   //! prefix for the uadata folder path to locate the game.cfg and other cfg files
   settingGamePrefix,

   //! screen resolution string, in the form "<xres> x <yres>"
   settingScreenResolution,

   //! boolean value that indicates if the ui is drawn with "smooth" pixels
   settingUISmooth,

   //! boolean value that indicates fullscreen mode
   settingFullscreen,

   //! string with narration type; one of "sound", "subtitles" or "both"
   settingCutsceneNarration,

   //! boolean value that is true when audio system should be enabled
   settingAudioEnabled,

   //! int value with midi device to use; -1 for default
   settingWin32MidiDevice,
};

//! base game type enum
enum EUwGameType
{
   //! we have ultima underworld 1 (or demo)
   gameUw1,

   //! we have ultima underworld 2
   gameUw2
};


//! loads all config files into the settings object
void LoadSettings(Settings& settings);


// classes

//! Settings class
/*! Manages uwadv's global settings values. The values can have the types
    "boolean", "int" or "std::string". The values are stored in a config file
    that is read via the ConfigFile class. The Settings class also manages a
    game type value that determines which type of game is currently running.
*/
class Settings
{
public:
   //! ctor
   Settings();

   //! loads settings from file
   void Load(const std::string& strFilename);

   //! save settings to file
   void Save(const std::string& strFilenameNew, const std::string& strFilenameOld);

   // settings value access

   //! returns the gametype
   EUwGameType GetGametype() const { return m_gameType; }

   //! returns string settings value
   std::string GetString(ESettingsType type) const;

   //! returns an integer settings value
   int GetInt(ESettingsType type) const;

   //! returns a boolean settings value
   bool GetBool(ESettingsType type) const;

   //! sets the gametype
   void SetGametype(EUwGameType gameType){ m_gameType = gameType; }

   //! inserts a settings key/value pair
   void SetValue(ESettingsType type, std::string strValue);

   //! inserts a boolean value
   void SetValue(ESettingsType type, bool bValue);

   //! inserts an integer value
   void SetValue(ESettingsType type, int iValue);

protected:
   //! searches type name from string name
   bool SearchTypeFromString(const std::string& strKeyname, ESettingsType& type);

protected:
   //! settings map typedef
   typedef std::map<ESettingsType, std::string> SettingsMap;

   //! game type
   EUwGameType m_gameType;

   //! settings map
   SettingsMap m_settings;
};

} // namespace Base

#endif
