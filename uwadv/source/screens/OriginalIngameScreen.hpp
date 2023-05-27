//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022 Underworld Adventures Team
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
/// \file OriginalIngameScreen.hpp
/// \brief original, old style game screen user interface
//
#pragma once

#include "ImageScreen.hpp"
#include "IUserInterface.hpp"
#include "Keymap.hpp"
#include "MouseCursor.hpp"
#include "ImageQuad.hpp"
#include "TextScroll.hpp"
#include "IngameControls.hpp"
#include "Panel.hpp"
#include "Underworld.hpp"
#include "Math.hpp"
#include "physics/PlayerPhysicsObject.hpp"
#include "Triangle3d.hpp"

/// game modes
enum IngameGameMode
{
   ingameModeDefault = 0, ///< nothing selected
   ingameModeOptions,   ///< in the options menu
   ingameModeTalk,      ///< talk mode
   ingameModeGet,       ///< get mode
   ingameModeLook,      ///< look mode
   ingameModeFight,     ///< fight mode
   ingameModeUse        ///< use mode
};

/// actions to perform by screen
enum IngameAction
{
   ingameActionNone = 0,   ///< no action to perform
   ingameActionExit,       ///< exits the game
   ingameActionLoadGame,   ///< shows savegame loading screen
   ingameActionSaveGame,   ///< shows savegame loading screen
   ingameActionQuickload,  ///< performs quickloading
   ingameActionQuicksave,  ///< performs quicksaving
   ingameActionConversation, ///< starts conversation
   ingameActionCutscene,   ///< shows cutscene
   ingameActionShowMap,    ///< shows map view
};

/// player move state
enum IngameMoveState
{
   ingameMoveTurnLeft = 0,
   ingameMoveTurnRight,
   ingameMoveWalkForward,
   ingameMoveRunForward,
   ingameMoveWalkBackwards
};

/// original ingame screen
class OriginalIngameScreen :
   public ImageScreen,
   public IUserInterface,
   public IPanelParent
{
public:
   /// ctor
   OriginalIngameScreen(IGame& game);
   /// dtor
   virtual ~OriginalIngameScreen() {}

   // virtual methods from ImageScreen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void KeyEvent(bool keyDown, Base::KeyType key) override;
   virtual void Tick() override;
   virtual void OnFadeOutEnded() override;

   /// schedules action and starts fadeout if specified
   void ScheduleAction(IngameAction action, bool fadeoutBefore);

   /// actually performs scheduled action
   void DoAction(IngameAction action);

   /// returns game mode
   IngameGameMode GetGameMode() { return m_ingameMode; }

   /// sets game mode
   void SetGameMode(IngameGameMode ingameMode) { m_ingameMode = ingameMode; }

   /// returns move state
   bool GetMoveState(IngameMoveState state);

   /// returns player physics tracking object
   Physics::PlayerPhysicsObject& GetPlayerPhysicsObject()
   {
      return m_playerPhysics;
   }

   // virtual methods from IPanelParent

   /// sets cursor image
   virtual void SetCursor(int index, bool priority = false) override;

   /// returns game interface; for controls
   virtual IGame& GetGameInterface() override { return m_game; }

   /// returns game instance; for controls
   virtual IGameInstance& GetGameInstance() override { return m_game.GetGameInstance(); }

private:
   /// suspends game resources while showing another screen
   void Suspend();

   /// resumes gameplay and restores resources
   void Resume();

   // virtual methods from IUserInterface

   /// notifies callback class
   virtual void Notify(UserInterfaceNotification notify,
      unsigned int param = 0) override;

   /// called to print text to textscroll
   virtual void PrintScroll(const char* text) override;

   /// starts conversation with object in current level, on list position
   virtual void StartConversation(Uint16 listPos) override;

   /// starts showing cutscene with given number
   virtual void ShowCutscene(unsigned int cutsceneNumber) override;

   /// shows map
   virtual void ShowMap() override;

   /// takes a screenshot for savegame preview
   void DoSavegameScreenshot(unsigned int xres, unsigned int yres);

   /// returns triangles surrounding a tile on the current level
   void GetSurroundingTriangles(unsigned int xpos,
      unsigned int ypos, std::vector<Triangle3dTextured>& allTriangles);

private:
   // constants

   /// time to fade in/out
   static const double s_fadeTime;


   // screen related

   /// keymap
   Base::Keymap m_keymap;


   /// background image
   IndexedImage m_backgroundImage;

   /// mouse cursor
   MouseCursor m_mouseCursor;

   /// indicates if there is a movement key pressed
   bool m_moveTurnLeft, m_moveTurnRight,
      m_moveWalkForward, m_moveRunForward, m_moveWalkBackwards;

   /// indicates if a priority cursor image is in effect
   bool m_isPriorityCursorActive;

   /// all inventory objects
   std::vector<IndexedImage> m_inventoryObjectImages;


   /// action to perform after fadeout
   IngameAction m_fadeoutAction;

   /// optional parameter for fadeout action
   unsigned int m_fadeoutParameter;


   // controls

   /// panel for inventory, stats or runebag
   Panel m_panel;

   /// compass window
   IngameCompass m_compass;

   /// message text scroll
   TextScroll m_textScroll;

   /// runeshelf
   IngameRuneshelf m_runeShelf;

   /// spell area
   IngameSpellArea m_spellArea;

   /// vitality flask
   IngameFlask m_vitalityFlask;

   /// mana flask
   IngameFlask m_manaFlask;

   /// gargoyle eyes
   IngameGargoyleEyes m_gargoyleEyes;

   /// dragon on the left side
   IngameDragon m_leftDragon;

   /// dragon on the right side
   IngameDragon m_rightDragon;

   /// command buttons
   IngameCommandButtons m_commandButtons;

   /// 3d view area (invisible)
   Ingame3DView m_view3d;

   /// powergem
   IngamePowerGem m_powerGem;

   /// move arrows
   IngameMoveArrows m_moveArrows;

   /// image to draw over the 3D view area when fading in/out
   ImageQuad m_fadeout3dViewImage;


   // game related

   /// selected game mode
   IngameGameMode m_ingameMode;

   /// tickcount used for time bookkeeping
   unsigned int m_tickCount;

   // classes that need direct access to this class
   friend class IngameCommandButtons;

   /// player physics tracking object
   Physics::PlayerPhysicsObject m_playerPhysics;
};
