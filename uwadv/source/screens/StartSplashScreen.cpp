//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file StartSplashScreen.cpp
/// \brief start splash screens implementation
//
#include "pch.hpp"
#include "StartSplashScreen.hpp"
#include "Audio.hpp"
#include "Savegame.hpp"
#include "Renderer.hpp"
#include "IScripting.hpp"
#include "StartMenuScreen.hpp"
#include "OriginalIngameScreen.hpp"
#include "Import.hpp"
#include "ImageManager.hpp"

/// number of seconds the splash screen images are shown
const double StartSplashScreen::c_showTime = 4.0;

/// fade in/out time in seconds
const double StartSplashScreen::c_blendTime = 0.8;

/// animation frame rate, in frames per second
const double StartSplashScreen::c_animationFrameRate = 5.0;

/// palette shifts per second
const double StartSplashScreen::c_paletteShiftsPerSecond = 5.0;

StartSplashScreen::StartSplashScreen(IGame& game)
   :ImageScreen(game, 0, c_blendTime),
   m_stage(splashScreenShowFirstOpeningScreen),
   m_tickCount(0),
   m_currentFrame(0),
   m_animationCount(0.0),
   m_shiftCount(0.0)
{
}

void StartSplashScreen::Init()
{
   ImageScreen::Init();

   UaTrace("start splash screen started\n");

   // start intro midi music
   bool isUw2 = m_gameInstance.GetSettings().GetGameType() == Base::gameUw2;

   m_game.GetAudioManager().StartMusicTrack(
      !isUw2 ? Audio::musicUw1_Introduction : Audio::musicUw2_LabyrinthOfWorldsTheme,
      false);

   m_game.GetRenderer().SetupForUserInterface();

   SetFadeinComplete();

   // leave out first two screens when we have savegames
   if (!m_gameInstance.GetSettings().GetBool(Base::settingUw1IsUwdemo) &&
      m_gameInstance.GetSavegamesManager().GetSavegamesCount() > 0)
   {
      UaTrace("skipping splash images (savegames are available)\n");

      m_stage = splashScreenFadeinAnimation;
   }

   UpdateForNextStage();

   // switch on cursor for this screen
   SDL_ShowCursor(1);
}

void StartSplashScreen::Destroy()
{
   ImageScreen::Destroy();

   SDL_ShowCursor(0);

   UaTrace("start splash screen ended\n\n");
}

bool StartSplashScreen::ProcessEvent(SDL_Event& event)
{
   bool handled = false;

   switch (event.type)
   {
   case SDL_KEYDOWN:
   case SDL_MOUSEBUTTONDOWN:
      // when a key or mouse button was pressed, go to next stage
      switch (m_stage)
      {
         // first or second image
      case splashScreenShowFirstOpeningScreen:
      case splashScreenShowSecondOpeningScreen:
         m_tickCount = unsigned(c_showTime * GetTickRate()) + 1;
         handled = true;
         break;

      case splashScreenFadeinAnimation:
         m_stage = SplashScreenStage(m_stage + 1);
         m_tickCount = unsigned(c_blendTime * GetTickRate()) - m_tickCount;
         UpdateForNextStage();

         StartFadein();

         handled = true;
         break;

      case splashScreenShowAnimation:
         m_stage = SplashScreenStage(m_stage + 1);
         m_tickCount = 0;

         // fade out music when we have the demo (ingame starts after this)
         if (m_gameInstance.GetSettings().GetBool(Base::settingUw1IsUwdemo))
            m_game.GetAudioManager().FadeoutMusic(static_cast<int>(c_blendTime * 1000));

         StartFadeout();

         handled = true;
         break;
      }
      break;
   }

   return handled;
}

void StartSplashScreen::Tick()
{
   ImageScreen::Tick();

   m_tickCount++;
   bool isShowTimeElapsed = m_tickCount >= c_showTime * GetTickRate();

   bool isUw2 = m_gameInstance.GetSettings().GetGameType() == Base::gameUw2;

   switch (m_stage)
   {
   case splashScreenShowFirstOpeningScreen:
      if (isShowTimeElapsed)
      {
         if (m_gameInstance.GetSettings().GetBool(Base::settingUw1IsUwdemo))
            m_stage = SplashScreenStage(m_stage + 2);
         else
            m_stage = SplashScreenStage(m_stage + 1);

         m_tickCount = 0;

         UpdateForNextStage();
      }
      break;

   case splashScreenShowSecondOpeningScreen:
      if (isShowTimeElapsed)
      {
         m_stage = SplashScreenStage(m_stage + 1);
         m_tickCount = 0;

         UpdateForNextStage();
      }
      break;

   case splashScreenFadeinAnimation:
   case splashScreenShowAnimation:
   case splashScreenFadeoutAnimation:
      // check if we have to do a new animation frame
      m_animationCount += 1.0 / GetTickRate();
      if (m_animationCount >= 1.0 / c_animationFrameRate)
      {
         // do next frame
         m_currentFrame++;
         m_animationCount -= 1.0 / c_animationFrameRate;

         if (!isUw2)
         {
            if (m_currentFrame >= m_currentCutscene.GetMaxFrames() - 2)
               m_currentFrame = 0;
         }
         else
         {
            // in uw2, just stop at frame 24; all later frames are corrupted
            if (m_currentFrame > m_currentCutscene.GetMaxFrames() - 14)
               m_currentFrame = m_currentCutscene.GetMaxFrames() - 14;
         }

         // prepare and convert animation frame
         m_currentCutscene.GetFrame(GetImage(), m_currentFrame);
         UpdateImage();
      }
      break;

   case splashScreenFadeoutFinished:
      // nothing to do
      break;

   default:
      UaAssertMsg(false, "invalid splash screen stage!");
      break;
   }

   // shift palette when uw2 animation is shown
   if (isUw2 && m_stage >= splashScreenFadeinAnimation && m_stage < splashScreenFadeoutFinished)
   {
      m_shiftCount += 1.0 / GetTickRate();
      if (m_shiftCount >= 1.0 / c_paletteShiftsPerSecond)
      {
         m_shiftCount -= 1.0 / c_paletteShiftsPerSecond;

         IndexedImage& cutsceneImage = GetImage();

         cutsceneImage.GetPalette()->Rotate(43, 6, false);
         cutsceneImage.GetPalette()->Rotate(49, 3, false);
         cutsceneImage.GetPalette()->Rotate(57, 9, false);

         UpdateImage();
      }
   }
}

void StartSplashScreen::OnFadeInEnded()
{
   m_stage = SplashScreenStage(m_stage + 1);
   UpdateForNextStage();
}

void StartSplashScreen::OnFadeOutEnded()
{
   // start next screen
   if (m_gameInstance.GetSettings().GetBool(Base::settingUw1IsUwdemo))
   {
      // when we have the demo, we immediately go to the ingame screen

      // load and init new game
      Import::LoadUnderworld(m_gameInstance.GetSettings(), m_gameInstance.GetResourceManager(), m_gameInstance.GetUnderworld());

      m_gameInstance.GetScripting().InitNewGame();

      m_game.ReplaceScreen(new OriginalIngameScreen(m_game), false);
      return;
   }
   else
      m_game.ReplaceScreen(new StartMenuScreen(m_game), false);
}

void StartSplashScreen::UpdateForNextStage()
{
   bool isUw2 = m_gameInstance.GetSettings().GetGameType() == Base::gameUw2;

   switch (m_stage)
   {
   case splashScreenShowFirstOpeningScreen:
      UaTrace("loading first image\n");
      if (!isUw2)
      {
         const char* firstImageName = "data/pres1.byt";
         if (m_gameInstance.GetSettings().GetBool(Base::settingUw1IsUwdemo))
            firstImageName = "data/presd.byt";

         // load image, palette 5
         m_gameInstance.GetImageManager().Load(GetImage(),
            firstImageName, 0, 5, imageByt);
      }
      else
      {
         m_gameInstance.GetImageManager().LoadFromArk(GetImage(), "data/byt.ark", 6, 5);
      }

      // demo game?
      if (!isUw2 &&
         m_gameInstance.GetSettings().GetBool(Base::settingUw1IsUwdemo))
      {
         // write a string under the demo title
         Font font;
         IndexedImage tempImage;
         font.Load(m_gameInstance.GetResourceManager(), fontBig);
         font.CreateString(tempImage, "Underworld Adventures", 198);

         double scale = 0.9;
         unsigned int xpos = unsigned((320 - tempImage.GetXRes() * scale) / 2);

         GetImage().PasteRect(tempImage, 0, 0,
            tempImage.GetXRes(), tempImage.GetYRes(),
            xpos, 200 - 16);
      }

      UpdateImage();
      break;

   case splashScreenShowSecondOpeningScreen:
      UaTrace("loading second image\n");

      if (!isUw2)
      {
         m_gameInstance.GetImageManager().Load(GetImage(),
            "data/pres2.byt", 0, 5, imageByt);
      }
      else
      {
         m_gameInstance.GetImageManager().LoadFromArk(GetImage(), "data/byt.ark", 7, 5);
      }

      UpdateImage();
      break;

   case splashScreenFadeinAnimation:
      UaTrace("loading animation\n");

      m_currentFrame = 0;
      m_animationCount = 0.0;
      m_tickCount = 0;

      m_currentCutscene.Load(m_gameInstance.GetResourceManager(), "cuts/cs011.n01", GetImage());
      m_currentCutscene.GetFrame(GetImage(), m_currentFrame);
      UpdateImage();

      StartFadein();
      break;

   case splashScreenShowAnimation:
      // do nothing; wait for timer to advance stage
      break;

   case splashScreenFadeoutAnimation:
      // do nothing; wait for fadeout to finish
      break;

   case splashScreenFadeoutFinished:
      // do nothing
      break;

   default:
      UaAssertMsg(false, "invalid splash screen stage!");
      break;
   }
}
