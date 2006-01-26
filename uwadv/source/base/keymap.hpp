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
/*! \file keymap.hpp

   \brief keyboard key mappings

*/

// include guard
#ifndef uwadv_base_keymap_hpp_
#define uwadv_base_keymap_hpp_

// needed includes
#include <map>
#include <SDL_keysym.h>

namespace Base
{

class Settings;

// enums

//! logical key types
/*! defines all keys that are used during the game. */
enum EKeyType
{
   // menus
   keyMenuUp=0,       //!< cursor up
   keyMenuDown,       //!< cursor down
   keyMenuLeft,       //!< cursor left
   keyMenuRight,      //!< cursor right
   keyMenuTopOfList,  //!< pgup
   keyMenuTopOfList2,      //!< home
   keyMenuBottomOfList,    //!< pgdn
   keyMenuBottomOfList2,   //!< end
   keyMenuPressButton,     //!< enter

   // normal movement
   keyRunForward,             //!< w
   keyRunForwardEasymove,     //!< shift w
   keyWalkForward,            //!< s
   keyWalkForwardEasymove,    //!< shift s
   keyTurnLeft,               //!< a
   keyTurnLeftEasymove,       //!< shift a
   keyTurnRight,              //!< d
   keyTurnRightEasymove,      //!< shift d
   keySlideLeft,              //!< z, y on non-us keyboards
   keySlideRight,             //!< c
   keyWalkBackwards,          //!< x
   keyWalkBackwardsEasymove,  //!< shift x

   // other movement
   keyFlyUp,            //!< e
   keyFlyDown,          //!< q
   keyLookDown,         //!< 1
   keyCenterView,       //!< 2
   keyLookUp,           //!< 3
   keyStandingLongJump, //!< shift j
   keyJump,             //!< j

   // combat mode keys
   keyCombatBash,       //!< p
   keyCombatSlash,      //!< ;
   keyCombatThrust,     //!< . (dot)

   // special function keys
   keySpecialOptions,   //!< f1
   keySpecialTalkMode,  //!< f2
   keySpecialGetMode,   //!< f3
   keySpecialLookMode,  //!< f4
   keySpecialFightMode, //!< f5
   keySpecialUseMode,   //!< f6
   keySpecialFlipPanel, //!< f7
   keySpecialCastSpell, //!< f8
   keySpecialUseTrack,  //!< f9
   keySpecialSleep,     //!< f10

   keySpecialQuicksave, //!< f11
   keySpecialQuickload, //!< f12

   // game options
   keyGameSaveGame,     //!< ctrl s
   keyGameRestoreGame,  //!< ctrl r
   keyGameChangeMusic,  //!< ctrl m
   keyGameChangeSFX,    //!< ctrl f
   keyGameChangeLevelOfDetail, //!< ctrl d
   keyGameReturnToGame, //!< esc

   // mouse cursor movement keys
   keyCursorHotAreaRight, //!< tab
   keyCursorHotAreaLeft,  //!< shift tab

   // numeric keypad
   keyCursorDir_sw, //!< num1
   keyCursorDir_s,  //!< num2
   keyCursorDir_se, //!< num3
   keyCursorDir_w,  //!< num4
   keyCursorDir_e,  //!< num6
   keyCursorDir_nw, //!< num7
   keyCursorDir_n,  //!< num8
   keyCursorDir_ne, //!< num9

   keyCursorButtonLeft,  //!< num0
   keyCursorButtonRight, //!< numperiod

   // special uwadv keys
   keyUaDebug,       //!< alt d, starts debugger application
   keyUaReturnMenu,  //!< alt q, returns to main menu
   keyUaScreenshot,  //!< alt c, takes screenshot
   keyUaLevelUp,     //!< alt page up, only in debug mode
   keyUaLevelDown,   //!< alt page down, only in debug mode

   keyNone
};


// classes

//! keymap class
/*! The class keeps all key mappings from key/modifier values to EKeyType
    values that can be processed in the game.
*/
class Keymap
{
public:
   //! ctor
   Keymap(){}

   //! initializes keymap
   void Init(const Settings& settings);

   //! finds key by given key amd modifier value
   EKeyType FindKey(SDLKey key, SDLMod mod);

   //! inserts a new key mapping
   void InsertKeyMapping(SDLKey key, SDLMod mod, EKeyType keyType);

public:
   //! mapping from SDL keysym/modifier pair to key type value
   typedef std::map<Uint32, EKeyType> KeymapMapping;

protected:
   //! map with combined SDL "key/mod" and key types
   KeymapMapping m_mapKeys;
};

} // namespace Base

#endif
