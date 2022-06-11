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
/// \file IUserInterface.hpp
/// \brief interface to user interface
//
#pragma once

#include "Base.hpp"

/// actions that the user performs
enum UserInterfaceUserAction
{
   /// dummy action
   userActionNothing = 0,

   /// user looks at object
   userActionLookObject,

   /// user looks at object in inventory
   userActionLookObjectInventory,

   /// user looks at a wall in 3d world
   userActionLookWall,

   /// user uses object
   userActionUseObject,

   /// user uses object in inventory
   userActionUseObjectInventory,

   /// user looks at object in inventory
   userActionUseWall,

   /// user gets object from 3d view
   userActionGetObject,

   /// user talks to object
   userActionTalkObject,

   /// clicked on active spell; param is the spell that is clicked on
   userActionClickedActiveSpell,

   /// user clicked on runeshelf to cast a spell
   userActionClickedRuneshelf,

   /// user clicked on compass
   userActionClickedCompass,

   /// user clicked on vitality flask
   userActionClickedVitalityFlash,

   /// user clicked on mana flask
   userActionClickedManaFlask,

   /// user clicked on gargoyle
   userActionClickedGargoyle,

   /// user clicked on dragons
   userActionClickedDragons,

   /// user tracks creatures
   userActionTrackCreatures,

   /// user tries to sleep, using bedroll or bed
   userActionSleep,

   /// enters combat mode; the function returns false when it's not possible
   /// to start combat, e.g. when in water.
   userActionCombatEnter,

   /// leaves combat mode again
   userActionCombatLeave,

   /// starts drawing back the weapon (start of power gem heatup)
   userActionCombatDrawBack,

   /// user released a weapon; the parameter describes the "part" of the
   /// screen the mouse was when releasing the weapon; 0=upper third, 1=middle
   /// of screen, 2=lower part of screen
   userActionCombatRelease,

   /// selected a target, previously requested with notifySelectTarget
   userActionTargetSelected,
};

/// all notification values for user interface
enum UserInterfaceNotification
{
   notifyNothing = 0,
   notifyUpdatePowergem,
   notifyUpdateGargoyleEyes,
   notifyUpdateFlasks,
   notifyUpdateShowIngameAnimation,
   notifyUpdateSpellArea,
   notifyAnimateDragons,
   notifyLevelChange,

   /// notifies that the user either should select a target or picked up an item
   /// the parameter specifies the item ID of the picked up item
   notifySelectTarget,
   notifyPlayerHit,
   notifyPlayerDrowning,
   notifyPlayerDead,
};

/// callback interface
class IUserInterface
{
public:
   /// notifies callback class
   virtual void Notify(UserInterfaceNotification notify,
      unsigned int param = 0) = 0;

   /// called to print text to textscroll
   virtual void PrintScroll(const char* text) = 0;

   /// starts conversation with object in current level, on list position
   virtual void StartConversation(Uint16 listPos) = 0;

   /// starts showing cutscene with given number
   virtual void ShowCutscene(unsigned int cutsceneNumber) = 0;

   /// shows map
   virtual void ShowMap() = 0;
};
