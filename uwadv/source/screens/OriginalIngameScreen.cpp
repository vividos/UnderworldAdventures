//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019,2021,2022 Underworld Adventures Team
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
/// \file OriginalIngameScreen.cpp
/// \brief ingame user interface, from the original uw1
//
#include "pch.hpp"
#include "OriginalIngameScreen.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"
#include "RenderWindow.hpp"
#include "Viewport.hpp"
#include "Underworld.hpp"
#include "Audio.hpp"
#include "SaveGameScreen.hpp"
#include "CutsceneViewScreen.hpp"
#include "MapViewScreen.hpp"
#include "ConversationScreen.hpp"
#include "Model3D.hpp"
#include "physics/PhysicsModel.hpp"
#include "physics/GeometryProvider.hpp"
#include "ImageManager.hpp"
#include "FileSystem.hpp"

/// time to fade in/out
const double OriginalIngameScreen::s_fadeTime = 0.5;

/// x resolution of savegame screenshot
const unsigned int c_ingameScreenshotXRes = 160;
/// y resolution of savegame screenshot
const unsigned int c_ingameScreenshotYRes = 100;

/// ctor; sets parent pointers for ingame controls
OriginalIngameScreen::OriginalIngameScreen(IGame& game)
   :ImageScreen(game, 0, s_fadeTime),
   m_compass(*this),
   m_runeShelf(*this),
   m_spellArea(*this),
   m_vitalityFlask(*this, true),
   m_manaFlask(*this, false),
   m_gargoyleEyes(*this),
   m_leftDragon(*this, true),
   m_rightDragon(*this, false),
   m_commandButtons(*this),
   m_view3d(*this),
   m_powerGem(*this),
   m_moveArrows(*this),
   m_playerPhysics(m_gameInstance.GetUnderworld().GetPlayer(), m_gameInstance.GetSettings().GetBool(Base::settingUwadvFeatures))
{
   ImageScreen::SetClearDrawBuffer(false);
}

void OriginalIngameScreen::Init()
{
   ImageScreen::Init();

   UaTrace("orig. ingame user interface started\n");

   // init all variables
   m_moveTurnLeft = m_moveTurnRight = m_moveWalkForward =
      m_moveWalkBackwards = m_moveRunForward = false;

   m_isPriorityCursorActive = false;

   m_ingameMode = ingameModeDefault;
   m_tickCount = 0;

   m_fadeoutAction = ingameActionNone;
   m_fadeoutParameter = 0;

   m_gameInstance.GetImageManager().LoadList(m_inventoryObjectImages, "objects");

   Notify(notifyLevelChange);

   Base::Settings& settings = m_gameInstance.GetSettings();

   // load keymap
   m_keymap.Init(settings, m_gameInstance.GetResourceManager());
   RegisterKeymap(&m_keymap);

   m_gameInstance.GetPhysicsModel().Init(
      std::bind(
         &OriginalIngameScreen::GetSurroundingTriangles, this,
         std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

   // init images/subwindows

   // load background
   {
      if (settings.GetGameType() == Base::gameUw2)
      {
         m_gameInstance.GetImageManager().LoadFromArk(m_backgroundImage, "data/byt.ark", 4, 0);
      }
      else
      {
         // uw1/uw_demo
         const char* mainscreenname = "data/main.byt";

         // replace name when using uw_demo
         if (settings.GetBool(Base::settingUw1IsUwdemo))
            mainscreenname = "data/dmain.byt";

         m_gameInstance.GetImageManager().Load(m_backgroundImage,
            mainscreenname, 0, 0, imageByt);
      }

      m_backgroundImage.FillRect(236, 7, 83, 114, 0); // panel area
      m_backgroundImage.FillRect(272, 3, 10, 4, 0);
      m_backgroundImage.FillRect(272, 121, 10, 18, 0);

      GetImage() = m_backgroundImage;
      UpdateImage();
   }

   // init 3d view window
   m_view3d.Init(m_gameInstance, 54, 20);
   RegisterWindow(&m_view3d);

   m_game.GetViewport().SetViewport3D(52, 19, 172, 112);

   m_fadeout3dViewImage.Init(m_gameInstance, 52, 19);
   m_fadeout3dViewImage.Create(52, 19, 172, 112);

   // init compass
   m_compass.Init(m_gameInstance, 112, 131);
   RegisterWindow(&m_compass);

   // init textscroll
   {
      unsigned int scrollwidth = 290;

      // adjust scroll width for uw_demo
      if (m_gameInstance.GetSettings().GetBool(Base::settingUw1IsUwdemo))
         scrollwidth = 218;

      m_textScroll.Init(m_gameInstance, 15, 169, scrollwidth, 30, 42);
      //m_textScroll.init(m_game, 11,169, 299,29, 42);
      m_textScroll.SetColorCode(colorCodeBlack);

      m_textScroll.Print("Welcome to the Underworld Adventures!\n"
         " \\3https://vividos.github.com/UnderworldAdventures\\0");

      RegisterWindow(&m_textScroll);

      // fill message scroll background area
      m_backgroundImage.FillRect(16, 169, scrollwidth, 30, 42);
   }

   // runeshelf
   m_runeShelf.Init(m_gameInstance, 176, 138);
   m_runeShelf.UpdateRuneshelf();
   RegisterWindow(&m_runeShelf);

   // active spells
   m_spellArea.Init(m_gameInstance, 52, 136);
   m_spellArea.UpdateSpellArea();
   RegisterWindow(&m_spellArea);

   // vitality/mana flasks
   m_vitalityFlask.Init(m_gameInstance, 248, 125);
   RegisterWindow(&m_vitalityFlask);

   m_manaFlask.Init(m_gameInstance, 284, 125);
   RegisterWindow(&m_manaFlask);

   // gargoyle eyes
   m_gargoyleEyes.Init(m_gameInstance, 128, 4);
   m_gargoyleEyes.UpdateEyes();
   RegisterWindow(&m_gargoyleEyes);

   // left and right side dragons
   m_leftDragon.Init(m_gameInstance, 36, 65);
   RegisterWindow(&m_leftDragon);

   m_rightDragon.Init(m_gameInstance, 204, 65);
   RegisterWindow(&m_rightDragon);

   // init command buttons
   m_commandButtons.Init(m_gameInstance, 0, 0); // buttons are self-repositioning
   RegisterWindow(&m_commandButtons);

   // init panel
   m_panel.Init(this, 235, 6);
   RegisterWindow(&m_panel);

   // init powergem
   m_powerGem.Init(m_gameInstance, 4, 139);
   m_powerGem.UpdateGem();
   RegisterWindow(&m_powerGem);

   // init move arrows
   m_moveArrows.Init(m_gameInstance, 107, 154);
   RegisterWindow(&m_moveArrows);

   // init mouse cursor
   m_mouseCursor.Init(m_gameInstance, 0);
   m_mouseCursor.Show(true);

   RegisterWindow(&m_mouseCursor);

   m_gameInstance.GetPhysicsModel().AddTrackBody(&m_playerPhysics);

   Resume();
}

void OriginalIngameScreen::Suspend()
{
   UaTrace("suspending orig. ingame user interface\n\n");

   m_game.GetRenderWindow().Clear();
   m_game.GetRenderWindow().SwapBuffers();

   m_gameInstance.GetGameLogic().RegisterUserInterface(nullptr);
}

void OriginalIngameScreen::Resume()
{
   UaTrace("resuming orig. ingame user interface\n");

   m_gameInstance.GetGameLogic().RegisterUserInterface(this);

   if (m_fadeoutAction == ingameActionConversation)
   {
      // after conversations, play "Dark Abyss"
      m_game.GetAudioManager().StartMusicTrack(Audio::musicUw1_DarkAbyss, true);
   }
   else
   {
      // normal start, play "Descent"
      m_game.GetAudioManager().StartMusicTrack(Audio::musicUw1_Descent, true);
   }

   StartFadein();
}

void OriginalIngameScreen::Destroy()
{
   Suspend();

   m_gameInstance.GetPhysicsModel().RemoveTrackBody(&m_playerPhysics);

   UaTrace("orig. ingame user interface finished\n\n");
}

void OriginalIngameScreen::Draw()
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   // 3d world
   {
      // set up 3d world
      Vector3d m_viewOffset(0.0, 0.0, 20.0);
      m_game.GetRenderer().SetupFor3D(m_viewOffset);

      // render a const world
      m_game.GetRenderer().RenderUnderworld(m_gameInstance.GetUnderworld());
   }

   // render 2d user interface
   {
      m_game.GetRenderer().SetupForUserInterface();

      glEnable(GL_BLEND);

      Uint8 alpha = GetCurentFadeAlphaValue();
      if (IsFadeInProgress() || alpha == 0)
      {
         // when fading in/out, lay a quad over the already rendered 3D view
         if (alpha < 255)
         {
            glColor4ub(0, 0, 0, 255 - alpha);
            glBindTexture(GL_TEXTURE_2D, 0);

            m_fadeout3dViewImage.Draw();
         }

         glColor3ub(alpha, alpha, alpha);
      }
      else
         glColor3ub(255, 255, 255);

      // draw all registered windows
      ImageScreen::Draw();

      glDisable(GL_BLEND);
   }

   if (m_takeScreenshot)
   {
      m_takeScreenshot = false;
      SaveScreenshot();
   }
}

bool OriginalIngameScreen::ProcessEvent(SDL_Event& event)
{
   // user event?
   if (event.type == SDL_USEREVENT && event.user.code == gameEventResumeScreen)
      Resume();

   return ImageScreen::ProcessEvent(event);
}

void OriginalIngameScreen::KeyEvent(bool keyDown, Base::KeyType key)
{
   Underworld::Player& pl = m_gameInstance.GetUnderworld().GetPlayer();
   IngameGameMode lastIngameMode = m_ingameMode;

   switch (key)
   {
      // run forward keys
   case Base::keyRunForward:
   case Base::keyRunForwardEasymove:
      m_moveRunForward = keyDown;
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveWalk, 1.0);
         m_playerPhysics.SetMovementMode(Physics::moveWalk);
      }
      else
      {
         if (m_moveWalkBackwards)
            m_playerPhysics.SetMovementFactor(Physics::moveWalk, -0.4);
         else if (m_moveWalkForward)
            m_playerPhysics.SetMovementFactor(Physics::moveWalk, 0.6);
         else if (!m_view3d.GetMouseMove())
            m_playerPhysics.SetMovementMode(0, Physics::moveWalk);
      }

      break;

      // walk forward keys
   case Base::keyWalkForward:
   case Base::keyWalkForwardEasymove:
      m_moveWalkForward = keyDown;
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveWalk, 0.6);
         m_playerPhysics.SetMovementMode(Physics::moveWalk);
      }
      else
      {
         if (m_moveWalkBackwards)
            m_playerPhysics.SetMovementFactor(Physics::moveWalk, -0.4);
         else if (m_moveRunForward)
            m_playerPhysics.SetMovementFactor(Physics::moveWalk, 1.0);
         else if (!m_view3d.GetMouseMove())
            m_playerPhysics.SetMovementMode(0, Physics::moveWalk);
      }
      break;

      // turn left keys
   case Base::keyTurnLeft:
   case Base::keyTurnLeftEasymove:
      m_moveTurnLeft = keyDown;
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveRotate, 1.0);
         m_playerPhysics.SetMovementMode(Physics::moveRotate);
      }
      else
      {
         if (m_moveTurnRight)
            m_playerPhysics.SetMovementFactor(Physics::moveRotate, -1.0);
         else if (!m_view3d.GetMouseMove())
            m_playerPhysics.SetMovementMode(0, Physics::moveRotate);
      }
      break;

      // turn right keys
   case Base::keyTurnRight:
   case Base::keyTurnRightEasymove:
      m_moveTurnRight = keyDown;
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveRotate, -1.0);
         m_playerPhysics.SetMovementMode(Physics::moveRotate);
      }
      else
      {
         if (m_moveTurnLeft)
            m_playerPhysics.SetMovementFactor(Physics::moveRotate, 1.0);
         else if (!m_view3d.GetMouseMove())
            m_playerPhysics.SetMovementMode(0, Physics::moveRotate);
      }
      break;

      // slide left key
   case Base::keySlideLeft:
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveSlide, -1.0);
         m_playerPhysics.SetMovementMode(Physics::moveSlide);
      }
      else if (!m_view3d.GetMouseMove())
         m_playerPhysics.SetMovementMode(0, Physics::moveSlide);
      break;

      // slide right key
   case Base::keySlideRight:
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveSlide, 1.0);
         m_playerPhysics.SetMovementMode(Physics::moveSlide);
      }
      else if (!m_view3d.GetMouseMove())
         m_playerPhysics.SetMovementMode(0, Physics::moveSlide);
      break;

      // walk backwards keys
   case Base::keyWalkBackwards:
   case Base::keyWalkBackwardsEasymove:
      m_moveWalkBackwards = keyDown;

      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveWalk, -0.4);
         m_playerPhysics.SetMovementMode(Physics::moveWalk);
      }
      else
      {
         if (m_moveWalkForward || m_moveRunForward)
            m_playerPhysics.SetMovementFactor(Physics::moveWalk, m_moveRunForward ? 1.0 : 0.6);
         else if (!m_view3d.GetMouseMove())
            m_playerPhysics.SetMovementMode(0, Physics::moveWalk);
      }
      break;

      // look up key
   case Base::keyLookUp:
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveLookUpDown, 1.0);
         m_playerPhysics.SetMovementMode(Physics::moveLookUpDown);
      }
      else
         m_playerPhysics.SetMovementMode(0, Physics::moveLookUpDown);
      break;

      // look down key
   case Base::keyLookDown:
      if (keyDown)
      {
         m_playerPhysics.SetMovementFactor(Physics::moveLookUpDown, -1.0);
         m_playerPhysics.SetMovementMode(Physics::moveLookUpDown);
      }
      else
         m_playerPhysics.SetMovementMode(0, Physics::moveLookUpDown);
      break;

      // center look key
   case Base::keyCenterView:
      if (keyDown)
      {
         m_playerPhysics.SetMovementMode(0, Physics::moveLookUpDown);
         m_playerPhysics.SetMovementFactor(Physics::moveLookUpDown, 0.0);
         pl.SetPanAngle(0.0);
      }
      break;

   case Base::keyMenuUp:
      if (keyDown && m_ingameMode == ingameModeOptions)
         m_commandButtons.SelectPreviousButton();
      break;

   case Base::keyMenuDown:
      if (keyDown && m_ingameMode == ingameModeOptions)
         m_commandButtons.SelectNextButton();
      break;

   case Base::keyMenuLeft:
   case Base::keyMenuRight:
      // note: as we don't have menus with 2 columns, these are unused here
      break;

   case Base::keyMenuTopOfList:
   case Base::keyMenuTopOfList2:
      if (keyDown && m_ingameMode == ingameModeOptions)
         m_commandButtons.SelectPreviousButton(true);
      break;

   case Base::keyMenuBottomOfList:
   case Base::keyMenuBottomOfList2:
      if (keyDown && m_ingameMode == ingameModeOptions)
         m_commandButtons.SelectNextButton(true);
      break;

   case Base::keyMenuPressButton:
      if (keyDown && m_ingameMode == ingameModeOptions)
         m_commandButtons.DoButtonAction();
      break;

      // combat keys
   case Base::keyCombatBash:
   case Base::keyCombatSlash:
   case Base::keyCombatThrust:
      if (m_ingameMode == ingameModeFight)
      {
         if (keyDown)
         {
            // start combat weapon drawback
            m_gameInstance.GetGameLogic().UserAction(userActionCombatDrawBack,
               key == Base::keyCombatBash ? 0 :
               key == Base::keyCombatSlash ? 1 : 2);
         }
         else
         {
            // end combat weapon drawback
            m_gameInstance.GetGameLogic().UserAction(userActionCombatRelease, 0);
         }
      }
      break;

      // options menu
   case Base::keySpecialOptions:
      if (keyDown && m_ingameMode != ingameModeOptions)
      {
         m_commandButtons.SelectButton(0);
         m_commandButtons.DoButtonAction();
      }
      break;

      // talk mode
   case Base::keySpecialTalkMode:
      if (keyDown || m_ingameMode == ingameModeOptions)
         break;

      if (m_ingameMode != ingameModeTalk)
      {
         m_commandButtons.SelectButton(1);
         m_ingameMode = ingameModeTalk;
      }
      else
      {
         m_commandButtons.SelectButton(-1);
         m_ingameMode = ingameModeDefault;
      }
      break;

      // get mode
   case Base::keySpecialGetMode:
      if (keyDown || m_ingameMode == ingameModeOptions)
         break;

      if (m_ingameMode != ingameModeGet)
      {
         m_commandButtons.SelectButton(2);
         m_ingameMode = ingameModeGet;
      }
      else
      {
         m_commandButtons.SelectButton(-1);
         m_ingameMode = ingameModeDefault;
      }
      break;

      // look mode
   case Base::keySpecialLookMode:
      if (keyDown || m_ingameMode == ingameModeOptions)
         break;

      if (m_ingameMode != ingameModeLook)
      {
         m_commandButtons.SelectButton(3);
         m_ingameMode = ingameModeLook;
      }
      else
      {
         m_commandButtons.SelectButton(-1);
         m_ingameMode = ingameModeDefault;
      }
      break;

      // fight mode
   case Base::keySpecialFightMode:
      if (keyDown || m_ingameMode == ingameModeOptions)
         break;

      if (m_ingameMode != ingameModeFight)
      {
         /*bool ret = */m_gameInstance.GetGameLogic().UserAction(userActionCombatEnter);
         //if (ret)
         {
            m_commandButtons.SelectButton(4);
            m_ingameMode = ingameModeFight;
         }
      }
      else
      {
         m_commandButtons.SelectButton(-1);
         m_ingameMode = ingameModeDefault;
      }

      break;

      // use mode
   case Base::keySpecialUseMode:
      if (keyDown || m_ingameMode == ingameModeOptions)
         break;

      if (m_ingameMode != ingameModeUse)
      {
         m_commandButtons.SelectButton(5);
         m_ingameMode = ingameModeUse;
      }
      else
      {
         m_commandButtons.SelectButton(-1);
         m_ingameMode = ingameModeDefault;
      }
      break;

   case Base::keySpecialFlipPanel:
      break;

   case Base::keySpecialCastSpell:
      if (!keyDown)
         m_gameInstance.GetGameLogic().UserAction(userActionClickedActiveSpell);
      break;

   case Base::keySpecialUseTrack:
      if (!keyDown)
         m_gameInstance.GetGameLogic().UserAction(userActionTrackCreatures);
      break;

   case Base::keySpecialSleep:
      if (!keyDown)
         m_gameInstance.GetGameLogic().UserAction(userActionSleep);
      break;

      // quicksave key
   case Base::keySpecialQuicksave:
      if (keyDown)
      {
         PrintScroll("\\0quicksaving...");
         ScheduleAction(ingameActionQuicksave, false);
      }
      break;

      // quickload key
   case Base::keySpecialQuickload:
      if (keyDown && m_gameInstance.GetSavegamesManager().IsQuicksaveAvail())
      {
         PrintScroll("\\0quickloading...");
         ScheduleAction(ingameActionQuickload, false);
      }
      break;

      // "load game" key
   case Base::keyGameRestoreGame:
      if (keyDown)
         ScheduleAction(ingameActionLoadGame, true);
      break;

      // "save game" key
   case Base::keyGameSaveGame:
      if (keyDown)
         ScheduleAction(ingameActionSaveGame, true);
      break;

      // returns from menu to game
   case Base::keyGameReturnToGame:
      break;

      // "debugger" key
   case Base::keyUaDebug:
      if (keyDown)
         m_gameInstance.GetDebugger().StartDebugger(&m_gameInstance);
      break;

      // exit screen key
   case Base::keyUaReturnMenu:
      if (keyDown)
         ScheduleAction(ingameActionExit, true);
      break;

      // "screenshot" key
   case Base::keyUaScreenshot:
      if (keyDown)
         m_takeScreenshot = true;
      break;

   default:
      break;
   }

   // leaving fight mode?
   if (lastIngameMode != m_ingameMode && lastIngameMode == ingameModeFight)
   {
      m_gameInstance.GetGameLogic().UserAction(userActionCombatLeave);
   }
}

void OriginalIngameScreen::Tick()
{
   ImageScreen::Tick();

   // evaluate underworld;
   // only evaluate when the user is not in the options menu
   if (!IsFadeInProgress() && m_ingameMode != ingameModeOptions)
   {
      m_gameInstance.GetGameLogic().EvaluateUnderworld(double(m_tickCount) / GetTickRate());

      double elapsedTime = 1.0 / GetTickRate();
      m_playerPhysics.RotateMove(elapsedTime);
      m_gameInstance.GetPhysicsModel().EvaluatePhysics(elapsedTime);

      m_tickCount++;

      // do renderer-specific tick processing
      m_game.GetRenderer().Tick(GetTickRate());
   }
}

void OriginalIngameScreen::OnFadeOutEnded()
{
   DoAction(m_fadeoutAction);

   m_fadeoutAction = ingameActionNone;
}

void OriginalIngameScreen::ScheduleAction(IngameAction action, bool fadeoutBefore)
{
   m_fadeoutAction = action;

   if (fadeoutBefore)
      StartFadeout();
   else
      DoAction(action); // do action immediately

   // check which action we scheduled
   switch (action)
   {
   case ingameActionSaveGame:
   case ingameActionQuicksave:
      // render savegame preview image
      DoSavegameScreenshot(c_ingameScreenshotXRes,
         c_ingameScreenshotYRes);
      break;

   case ingameActionExit:
      m_game.GetAudioManager().FadeoutMusic(static_cast<int>(s_fadeTime * 1000));
      break;

   default:
      break;
   }
}

/// \todo implement all actions
void OriginalIngameScreen::DoAction(IngameAction action)
{
   switch (action)
   {
   case ingameActionNone:
      break;

      // leave the ingame_orig screen
   case ingameActionExit:
      m_game.GetScreenHost().RemoveScreen();
      break;

      // start "load game" screen
   case ingameActionLoadGame:
      m_game.GetScreenHost().ReplaceScreen(new SaveGameScreen(m_game, false, true), true);
      break;

      // start "save game" screen
   case ingameActionSaveGame:
      m_game.GetScreenHost().ReplaceScreen(new SaveGameScreen(m_game, false, false), true);
      break;

      // quickloading
   case ingameActionQuickload:
      if (m_gameInstance.GetSavegamesManager().IsQuicksaveAvail())
      {
         Base::Savegame sg = m_gameInstance.GetSavegamesManager().LoadQuicksaveSavegame();
         m_gameInstance.GetUnderworld().Load(sg);
         PrintScroll("quickloading done.");
      }
      break;

      // quicksaving
   case ingameActionQuicksave:
   {
      // set player infos
      Base::SavegameInfo info;
      Underworld::Player& pl = m_gameInstance.GetUnderworld().GetPlayer();
      pl.FillSavegamePlayerInfos(info);
      info.m_gamePrefix = m_gameInstance.GetSettings().GetString(Base::settingGamePrefix);

      Base::Savegame sg = m_gameInstance.GetSavegamesManager().SaveQuicksaveSavegame(info);

      m_gameInstance.GetUnderworld().Save(sg);
      PrintScroll("quicksaving done.");
   }
   break;

   // starts conversation
   case ingameActionConversation:
      m_game.GetScreenHost().ReplaceScreen(new ConversationScreen(m_game, m_fadeoutParameter), true);
      break;

      // shows cutscene
   case ingameActionCutscene:
      m_game.GetScreenHost().ReplaceScreen(new CutsceneViewScreen(m_game, m_fadeoutParameter), true);
      break;

   case ingameActionShowMap:
      m_game.GetScreenHost().ReplaceScreen(new MapViewScreen(m_game), true);
      break;

   default:
      break;
   }
}

bool OriginalIngameScreen::GetMoveState(IngameMoveState movestate)
{
   bool state = false;
   switch (movestate)
   {
   case ingameMoveTurnLeft: state = m_moveTurnLeft; break;
   case ingameMoveTurnRight: state = m_moveTurnRight; break;
   case ingameMoveWalkForward: state = m_moveWalkForward; break;
   case ingameMoveRunForward: state = m_moveRunForward; break;
   case ingameMoveWalkBackwards: state = m_moveWalkBackwards; break;
   default: break;
   }
   return state;
}

/// Sets a new cursor image for the mouse cursor. There can be normal cursor
/// images and prioritized cursor images. The last ones cannot be changed or
/// resetted by unprioritized ones. Passing a negative index resets priority
/// mode and sets the negated index as normal cursor index. If the cursor
/// index is > 0x100, an object image is used for the cursor image.
/// \param index cursor image index to set
/// \param priority indicates if priority cursor image should be set
void OriginalIngameScreen::SetCursor(int index, bool priority)
{
   if (!priority && m_isPriorityCursorActive)
      return; // ignore new non-priority cursor

   m_isPriorityCursorActive = priority;

   // reset priority cursor
   if (index < 0 && priority)
   {
      m_isPriorityCursorActive = false;
      index = -index;
   }

   if (index > 0x100)
   {
      // change mouse cursor type to object image
      m_mouseCursor.SetCustom(m_inventoryObjectImages[unsigned(index) - 0x100]);
   }
   else
   {
      // set image from cursors
      m_mouseCursor.SetType(unsigned(index));
   }
}

void OriginalIngameScreen::Notify(UserInterfaceNotification notify,
   unsigned int param)
{
   switch (notify)
   {
   case notifyUpdatePowergem:
      m_powerGem.UpdateGem();
      break;

   case notifyUpdateGargoyleEyes:
      m_gargoyleEyes.UpdateEyes();
      break;

   case notifyUpdateFlasks:
      m_vitalityFlask.UpdateFlask();
      m_manaFlask.UpdateFlask();
      break;

   case notifyUpdateShowIngameAnimation:
      break;

   case notifyUpdateSpellArea:
      m_spellArea.UpdateSpellArea();
      break;

   case notifyAnimateDragons:
      //m_leftDragon.animate();
      //m_rightDragon.animate();
      break;

   case notifyLevelChange:
      m_game.GetRenderer().PrepareLevel(m_gameInstance.GetUnderworld().GetCurrentLevel());
      break;

   case notifySelectTarget:
      //target_select_mode = true
      SetCursor(param + 0x100, true);
      break;

   default:
      break;
   }
}

void OriginalIngameScreen::PrintScroll(const char* text)
{
   m_textScroll.Print(text);
}

void OriginalIngameScreen::StartConversation(Uint16 listPos)
{
   m_fadeoutParameter = listPos;
   ScheduleAction(ingameActionConversation, true);
}

void OriginalIngameScreen::ShowCutscene(unsigned int cutsceneNumber)
{
   m_fadeoutParameter = cutsceneNumber;
   ScheduleAction(ingameActionCutscene, true);
}

void OriginalIngameScreen::ShowMap()
{
   ScheduleAction(ingameActionShowMap, true);
}

void OriginalIngameScreen::SaveScreenshot()
{
   unsigned int xres = GetWidth();
   unsigned int yres = GetHeight();
   UnmapWindowPosition(xres, yres);

   std::vector<Uint32> screenshotRgbaData;
   m_game.GetRenderer().GetScreenshot(
      xres, yres,
      screenshotRgbaData);

   std::string screenshotsPath = m_gameInstance.GetResourceManager().GetHomePath() + "screenshots";
   if (!Base::FileSystem::FolderExists(screenshotsPath))
      Base::FileSystem::MakeFolder(screenshotsPath);

   unsigned int fileIndex = 0;
   std::string filename;
   do
   {
      filename = Base::String::Format(
         "%s/uw_%03u.png",
         screenshotsPath.c_str(),
         fileIndex++);

   } while (Base::FileSystem::FileExists(filename));

   ImageManager::Save(filename, xres, yres, screenshotRgbaData);

   PrintScroll(("Saved screenshot " + filename).c_str());
}

void OriginalIngameScreen::DoSavegameScreenshot(
   unsigned int xres, unsigned int yres)
{
   std::vector<Uint32> screenshotRgbaData;
   m_game.GetRenderer().TakeSavegameScreenshot(
      xres, yres,
      screenshotRgbaData,
      m_gameInstance.GetUnderworld());

   // set in savegames manager
   m_gameInstance.GetSavegamesManager().SetSaveScreenshot(
      xres, yres, screenshotRgbaData);
}

void OriginalIngameScreen::GetSurroundingTriangles(
   unsigned int xpos, unsigned int ypos,
   std::vector<Triangle3dTextured>& allTriangles)
{
   Uint8 xmin, xmax, ymin, ymax;

   xmin = static_cast<Uint8>(xpos > 0 ? xpos - 1 : 0);
   xmax = static_cast<Uint8>(xpos < 64 ? xpos + 1 : 64);
   ymin = static_cast<Uint8>(ypos > 0 ? ypos - 1 : 0);
   ymax = static_cast<Uint8>(ypos < 64 ? ypos + 1 : 64);

   // tile triangles
   Physics::GeometryProvider provider(m_gameInstance.GetGameLogic().GetCurrentLevel());

   for (unsigned int x = xmin; x < xmax; x++)
      for (unsigned int y = ymin; y < ymax; y++)
         provider.GetTileTriangles(x, y, allTriangles);

   // also collect triangles from 3d models and critter objects
   {
      const Underworld::ObjectList& objectList =
         m_gameInstance.GetGameLogic().GetCurrentLevel().GetObjectList();

      for (Uint8 x = xmin; x < xmax; x++)
      {
         for (Uint8 y = ymin; y < ymax; y++)
         {
            // get first object link
            Uint16 link = objectList.GetListStart(x, y);
            while (link != 0)
            {
               // collect triangles
               Underworld::ObjectPtr object = objectList.GetObject(link);
               if (object != NULL)
               {
                  m_game.GetRenderer().GetModel3DBoundingTriangles(x, y, *object, allTriangles);
               }

               link = objectList.GetObject(link)->GetObjectInfo().m_link;
            }
         }
      }
   }
}
