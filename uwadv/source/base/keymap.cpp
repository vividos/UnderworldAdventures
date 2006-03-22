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
/*! \file keymap.cpp

   \brief keyboard key mappings

*/

// needed includes
#include <cerrno>
#include "base.hpp"
#include "keymap.hpp"
#include "cfgfile.hpp"
#include "settings.hpp"
#include "resourcemanager.hpp"
#include "textfile.hpp"
#include "filesystem.hpp"
#include <SDL_rwops.h>

using Base::Keymap;
using Base::EKeyType;

namespace Detail
{

//! creates a combined key/mod value from a SDLKey and a SDLMod value
inline Uint32 MakeKeymod(SDLKey key, SDLMod mod)
{
   return Uint32(key) | (Uint32(mod) << 16);
}


// tables

//! mapping of all key type keywords to key types
struct KeyTypeMapping
{
   //! type name that occurs in keymap.cfg
   const char* cstrKeyType;

   //! enum value of key type
   EKeyType key;

} KeyTypeMapping[] =
{
   { "menu-up",               Base::keyMenuUp },
   { "menu-down",             Base::keyMenuDown },
   { "menu-left",             Base::keyMenuLeft },
   { "menu-right",            Base::keyMenuRight },
   { "menu-top-of-list",      Base::keyMenuTopOfList },
   { "menu-top-of-list2",     Base::keyMenuTopOfList2 },
   { "menu-bottom-of-list",   Base::keyMenuBottomOfList },
   { "menu-bottom-of-list2",  Base::keyMenuBottomOfList2 },
   { "menu-press-button",     Base::keyMenuPressButton },

   { "run-forward",              Base::keyRunForward },
   { "run-forward-easymove",     Base::keyRunForwardEasymove },
   { "walk-forward",             Base::keyWalkForward },
   { "walk-forward-easymove",    Base::keyWalkForwardEasymove },
   { "turn-left",                Base::keyTurnLeft },
   { "turn-left-easymove",       Base::keyTurnLeftEasymove },
   { "turn-right",               Base::keyTurnRight },
   { "turn-right-easymove",      Base::keyTurnRightEasymove },
   { "slide-left",               Base::keySlideLeft },
   { "slide-right",              Base::keySlideRight },
   { "walk-backwards",           Base::keyWalkBackwards },
   { "walk-backwards-easymove",  Base::keyWalkBackwardsEasymove },

   { "fly-up",             Base::keyFlyUp },
   { "fly-down",           Base::keyFlyDown },
   { "look-down",          Base::keyLookDown },
   { "center-view",        Base::keyCenterView },
   { "look-up",            Base::keyLookUp },
   { "standing-long-jump", Base::keyStandingLongJump },
   { "jump",               Base::keyJump },

   { "combat-bash",        Base::keyCombatBash },
   { "combat-slash",       Base::keyCombatSlash },
   { "combat-thrust",      Base::keyCombatThrust },

   { "special-options",    Base::keySpecialOptions },
   { "special-talk-mode",  Base::keySpecialTalkMode },
   { "special-get-mode",   Base::keySpecialGetMode },
   { "special-look-mode",  Base::keySpecialLookMode },
   { "special-fight-mode", Base::keySpecialFightMode },
   { "special-use-mode",   Base::keySpecialUseMode },
   { "special-flip-panel", Base::keySpecialFlipPanel },
   { "special-cast-spell", Base::keySpecialCastSpell },
   { "special-use-track",  Base::keySpecialUseTrack },
   { "special-sleep",      Base::keySpecialSleep },

   { "special-quicksave",  Base::keySpecialQuicksave },
   { "special-quickload",  Base::keySpecialQuickload },

   { "game-save-game",      Base::keyGameSaveGame },
   { "game-restore-game",   Base::keyGameRestoreGame },
   { "game-change-music",   Base::keyGameChangeMusic },
   { "game-change-sfx",     Base::keyGameChangeSFX },
   { "game-change-lod",     Base::keyGameChangeLevelOfDetail },
   { "game-return-to-game", Base::keyGameReturnToGame },

   { "cursor-hotarea-right", Base::keyCursorHotAreaRight },
   { "cursor-hotarea-left",  Base::keyCursorHotAreaLeft },

   { "cursor-dir-sw", Base::keyCursorDir_sw },
   { "cursor-dir-s",  Base::keyCursorDir_s  },
   { "cursor-dir-se", Base::keyCursorDir_se },
   { "cursor-dir-w",  Base::keyCursorDir_w  },
   { "cursor-dir-e",  Base::keyCursorDir_e  },
   { "cursor-dir-nw", Base::keyCursorDir_nw },
   { "cursor-dir-n",  Base::keyCursorDir_n  },
   { "cursor-dir-ne", Base::keyCursorDir_ne },

   { "cursor-button-left",  Base::keyCursorButtonLeft },
   { "cursor-button-right", Base::keyCursorButtonRight },

   { "ua-debug",       Base::keyUaDebug },
   { "ua-return-menu", Base::keyUaReturnMenu },
   { "ua-screenshot",  Base::keyUaScreenshot },
   { "ua-level-up",    Base::keyUaLevelUp },
   { "ua-level-down" , Base::keyUaLevelDown },
};


//! mapping of all key keywords to SDL key values
struct KeyMapping
{
   //! key name that can occur as value in keymap.cfg
   const char* cstrKeyType;

   //! SDL key value that maps to the key name
   SDLKey key;

} KeyMapping[] =
{
   { "f1",  SDLK_F1 },
   { "f2",  SDLK_F2 },
   { "f3",  SDLK_F3 },
   { "f4",  SDLK_F4 },
   { "f5",  SDLK_F5 },
   { "f6",  SDLK_F6 },
   { "f7",  SDLK_F7 },
   { "f8",  SDLK_F8 },
   { "f9",  SDLK_F9 },
   { "f10", SDLK_F10 },
   { "f11", SDLK_F11 },
   { "f12", SDLK_F12 },

   { "semicolon",    SDLK_SEMICOLON },
   { "period",       SDLK_PERIOD },
   { "tab",          SDLK_TAB },
   { "esc",          SDLK_ESCAPE },
   { "pgup",         SDLK_PAGEUP },
   { "pgdown",       SDLK_PAGEDOWN },
   { "home",         SDLK_HOME },
   { "end",          SDLK_END },
   { "cursor-up",    SDLK_UP },
   { "cursor-down",  SDLK_DOWN },
   { "cursor-left",  SDLK_LEFT },
   { "cursor-right", SDLK_RIGHT },

   { "backspace", SDLK_BACKSPACE },
   { "return", SDLK_RETURN },
   { "enter",  SDLK_RETURN },
   { "pause",  SDLK_PAUSE },
   { "space",  SDLK_SPACE },
   { "comma",  SDLK_COMMA },
   { "minus",  SDLK_MINUS },
   { "colon",  SDLK_COLON },
   { "less",   SDLK_LESS },
   { "delete", SDLK_DELETE },

   { "num0", SDLK_KP0 },
   { "num1", SDLK_KP1 },
   { "num2", SDLK_KP2 },
   { "num3", SDLK_KP3 },
   { "num4", SDLK_KP4 },
   { "num5", SDLK_KP5 },
   { "num6", SDLK_KP6 },
   { "num7", SDLK_KP7 },
   { "num8", SDLK_KP8 },
   { "num9", SDLK_KP9 },

   { "num-divide",   SDLK_KP_DIVIDE },
   { "num-multipl",  SDLK_KP_MULTIPLY },
   { "num-minus",    SDLK_KP_MINUS },
   { "num-plus",     SDLK_KP_PLUS },
   { "num-enter",    SDLK_KP_ENTER },
   { "num-period",   SDLK_KP_PERIOD },
};


//! keymap config file loader
class KeymapConfigLoader: public Base::NonCopyable
{
public:
   //! ctor
   KeymapConfigLoader(Base::ConfigFile& cfgFile, Base::Keymap& keyMap)
      :m_cfgFile(cfgFile), m_keyMap(keyMap)
   {
   }

   //! loads keymap from config file into Keymap object
   void Load();

protected:
   //! initializes internal key type and key name mappings
   void InitKeyMappings();

   //! searches a key type from key type name
   bool SearchKeyFromString(const std::string& strKeyTypeName, EKeyType& key);

   //! sets key type and value in map
   void SetKeyValue(EKeyType& key, const std::string& strKeyValue);

private:
   //! reference to config file being loade
   Base::ConfigFile& m_cfgFile;

   //! reference to keymap being loaded
   Base::Keymap& m_keyMap;

   //! mapping from keytype names to key values
   typedef std::map<std::string, Base::EKeyType> KeyTypeNamesMapping;

   //! key type names mapping
   KeyTypeNamesMapping m_mapKeyTypeNames;

   //! mapping from key names to SDL key values
   typedef std::map<std::string, SDLKey> KeyNamesMapping;

   //! key names mapping
   KeyNamesMapping m_mapKeyNames;
};

void KeymapConfigLoader::Load()
{
   InitKeyMappings();

   // go through all loaded key/value pairs
   Base::ConfigValueMap& cfgMap = m_cfgFile.GetValueMap();
   Base::ConfigValueMap::const_iterator stop = cfgMap.end();

   for(Base::ConfigValueMap::const_iterator iter = cfgMap.begin(); iter != stop; iter++)
   {
      // search for key type, and add key value for type
      Base::EKeyType key = Base::keyNone;
      if (SearchKeyFromString(iter->first, key))
         SetKeyValue(key, iter->second);
   }
}

/*! Initializes two maps from the values in KeyTypeMapping and KeyMapping
    arrays to faster match key type and key strings to values.
*/
void KeymapConfigLoader::InitKeyMappings()
{
   unsigned int max = SDL_TABLESIZE(KeyTypeMapping);
   for(unsigned int i=0; i < max; i++)
      m_mapKeyTypeNames[KeyTypeMapping[i].cstrKeyType] = KeyTypeMapping[i].key;

   max = SDL_TABLESIZE(KeyMapping);
   for(unsigned int j=0; j < max; j++)
   {
      UaAssert(strlen(KeyMapping[j].cstrKeyType) > 1); // no keytype name can be 1 character only
      m_mapKeyNames[KeyMapping[j].cstrKeyType] = KeyMapping[j].key;
   }
}

bool KeymapConfigLoader::SearchKeyFromString(const std::string& strKeyTypeName, EKeyType& key)
{
   // search "keymap key" key from name
   key = Base::keyNone;
   std::string strKeyname(strKeyTypeName);

   // make lowercase
   Base::String::Lowercase(strKeyname);

   // search for key type
   KeyTypeNamesMapping::const_iterator iter = m_mapKeyTypeNames.find(strKeyname);
   if (iter == m_mapKeyTypeNames.end())
   {
      UaTrace("keymap: unknown keyboard action: %s\n", strKeyname.c_str());
      return false;
   }
   else
      key = iter->second;

   return true;
}

/*! If the key name is not recognized, no key type to key mapping is added
    to the Keymap. No exception is thrown in this case.
*/
void KeymapConfigLoader::SetKeyValue(EKeyType& keyType, const std::string& strKeyValue)
{
   std::string strKeyName(strKeyValue);
   Base::String::Lowercase(strKeyName);

   // check for key state modifiers
   SDLMod mod = KMOD_NONE;
   std::string::size_type pos = strKeyName.find("ctrl");
   if (pos != std::string::npos)
   {
      mod = static_cast<SDLMod>(mod | KMOD_CTRL);
      strKeyName.erase(pos, 4);
   }

   pos = strKeyName.find("alt");
   if (pos != std::string::npos)
   {
      mod = static_cast<SDLMod>(mod | KMOD_ALT);
      strKeyName.erase(pos, 3);
   }

   pos = strKeyName.find("shift");
   if (pos != std::string::npos)
   {
      mod = static_cast<SDLMod>(mod | KMOD_SHIFT);
      strKeyName.erase(pos, 5);
   }

   // trim string
   for(;strKeyName.size() > 0 && isspace(strKeyName[0]);)
      strKeyName.erase(0,1);

   // check for single alphanumeric char
   SDLKey key = SDLK_UNKNOWN;
   if (strKeyName.size() == 1 && isalnum(strKeyName.at(0)))
   {
      // note: assumes that a conversion from alphanumeric character to SDLKey succeeds
      key = static_cast<SDLKey>(strKeyName.at(0));
   }
   else
   {
      // check for known key keywords
      KeyNamesMapping::const_iterator iter = m_mapKeyNames.find(strKeyName);
      if (iter == m_mapKeyNames.end())
      {
         UaTrace("keymap: didn't find keycode for: %s\n", strKeyName.c_str());
         return;
      }
      else
         key = iter->second;
   }

   // insert key
   m_keyMap.InsertKeyMapping(key, mod, keyType);
}

} // namespace Detail


// Keymap methods

/*! Loads global keymap for chosen game (game prefix must be set in settings
    object) and custom keymap specified in settings.
*/
void Keymap::Init(const Base::Settings& settings)
{
   UaTrace("loading keymaps ...\n");

   // load keymap from uadata resources
   std::string strKeymapName(settings.GetString(Base::settingGamePrefix));
   UaAssert(strKeymapName.size() > 0);

   strKeymapName += "/keymap.cfg";

   UaTrace(" keymap: %s\n", strKeymapName.c_str());

   // load main keymap
   Base::ConfigFile cfgFile;
   Base::ResourceManager resManager(settings);
   {
      SDL_RWopsPtr rwops = SDL_RWopsPtr(resManager.GetResourceFile(strKeymapName));
      if (rwops.get() == NULL)
         throw Base::FileSystemException("file not found", strKeymapName, ENOENT);

      Base::TextFile textFile(rwops);
      cfgFile.Load(textFile);
   }

   // load custom keymap
   strKeymapName = settings.GetString(settingCustomKeymap);

   UaTrace(" keymap: %s", strKeymapName.c_str());
   if (Base::FileSystem::FileExists(strKeymapName))
   {
      SDL_RWopsPtr rwops = SDL_RWopsPtr(SDL_RWFromFile(strKeymapName.c_str(), "rb"));
      // note: this exception is only thrown if the file was deleted between two lines above and here
      if (rwops.get() == NULL)
         throw Base::FileSystemException("file not found", strKeymapName, ENOENT);

      UaTrace("\n");

      Base::TextFile textFile(rwops);
      cfgFile.Load(textFile);
   }
   else
      UaTrace(" => not available\n");

   // load all values into the map
   Detail::KeymapConfigLoader loader(cfgFile, *this);
   loader.Load();
}

Base::EKeyType Keymap::FindKey(SDLKey key, SDLMod mod)
{
   Uint32 keymod = Detail::MakeKeymod(key, mod);
   KeymapMapping::const_iterator iter = m_mapKeys.find(keymod);

   return iter == m_mapKeys.end() ? Base::keyNone : iter->second;
}

void Keymap::InsertKeyMapping(SDLKey key, SDLMod mod, Base::EKeyType keyType)
{
   Uint32 keymod = Detail::MakeKeymod(key, mod);
   m_mapKeys[keymod] = keyType;
}
