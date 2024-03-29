//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Underworld Adventures Team
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
/// \file Keymap.hpp
/// \brief keyboard key mappings
//
#pragma once

#include <map>
#include <SDL2/SDL_keycode.h>

namespace Base
{
   class Settings;
   class ResourceManager;

   /// \brief logical key types
   /// defines all keys that are used during the game.
   enum KeyType
   {
      // menus
      keyMenuUp = 0,       ///< cursor up
      keyMenuDown,         ///< cursor down
      keyMenuLeft,         ///< cursor left
      keyMenuRight,        ///< cursor right
      keyMenuTopOfList,    ///< pgup
      keyMenuTopOfList2,      ///< home
      keyMenuBottomOfList,    ///< pgdn
      keyMenuBottomOfList2,   ///< end
      keyMenuPressButton,     ///< enter

      // normal movement
      keyRunForward,             ///< w
      keyRunForwardEasymove,     ///< shift w
      keyWalkForward,            ///< s
      keyWalkForwardEasymove,    ///< shift s
      keyTurnLeft,               ///< a
      keyTurnLeftEasymove,       ///< shift a
      keyTurnRight,              ///< d
      keyTurnRightEasymove,      ///< shift d
      keySlideLeft,              ///< z, y on non-us keyboards
      keySlideRight,             ///< c
      keyWalkBackwards,          ///< x
      keyWalkBackwardsEasymove,  ///< shift x

      // other movement
      keyFlyUp,            ///< e
      keyFlyDown,          ///< q
      keyLookDown,         ///< 1
      keyCenterView,       ///< 2
      keyLookUp,           ///< 3
      keyStandingLongJump, ///< shift j
      keyJump,             ///< j

      // combat mode keys
      keyCombatBash,       ///< p
      keyCombatSlash,      ///< ;
      keyCombatThrust,     ///< . (dot)

      // special function keys
      keySpecialOptions,   ///< f1
      keySpecialTalkMode,  ///< f2
      keySpecialGetMode,   ///< f3
      keySpecialLookMode,  ///< f4
      keySpecialFightMode, ///< f5
      keySpecialUseMode,   ///< f6
      keySpecialFlipPanel, ///< f7
      keySpecialCastSpell, ///< f8
      keySpecialUseTrack,  ///< f9
      keySpecialSleep,     ///< f10

      keySpecialQuicksave, ///< f11
      keySpecialQuickload, ///< f12

      // game options
      keyGameSaveGame,     ///< ctrl s
      keyGameRestoreGame,  ///< ctrl r
      keyGameChangeMusic,  ///< ctrl m
      keyGameChangeSFX,    ///< ctrl f
      keyGameChangeLevelOfDetail, ///< ctrl d
      keyGameReturnToGame, ///< esc

      // mouse cursor movement keys
      keyCursorHotAreaRight, ///< tab
      keyCursorHotAreaLeft,  ///< shift tab

      // numeric keypad
      keyCursorDir_sw, ///< num1
      keyCursorDir_s,  ///< num2
      keyCursorDir_se, ///< num3
      keyCursorDir_w,  ///< num4
      keyCursorDir_e,  ///< num6
      keyCursorDir_nw, ///< num7
      keyCursorDir_n,  ///< num8
      keyCursorDir_ne, ///< num9

      keyCursorButtonLeft,  ///< num0
      keyCursorButtonRight, ///< numperiod

      // special uwadv keys
      keyUaDebug,       ///< alt d, starts debugger application
      keyUaReturnMenu,  ///< alt q, returns to main menu
      keyUaScreenshot,  ///< alt c, takes screenshot
      keyUaLevelUp,     ///< alt page up, only in debug mode
      keyUaLevelDown,   ///< alt page down, only in debug mode

      keyNone
   };

   /// \brief keymap class
   /// The class keeps all key mappings from key/modifier values to KeyType
   /// values that can be processed in the game.
   class Keymap
   {
   public:
      /// ctor
      Keymap()
      {
      }

      /// initializes keymap
      void Init(const Settings& settings, Base::ResourceManager& resourceManager);

      /// finds key by given key amd modifier value
      KeyType FindKey(SDL_Keycode key, SDL_Keymod mod);

      /// inserts a new key mapping
      void InsertKeyMapping(SDL_Keycode key, SDL_Keymod mod, KeyType keyType);

   private:
      /// mapping from SDL keysym/modifier pair to key type value
      typedef std::map<Uint32, KeyType> KeymapMapping;

      /// map with combined SDL "key/mod" and key types
      KeymapMapping m_mapKeys;
   };

} // namespace Base
