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

void StartSplashScreen::Init()
{
   Screen::Init();

   UaTrace("start splash screen started\n");

   // start intro midi music
   bool isUw2 = m_game.GetSettings().GetGameType() == Base::gameUw2;

   m_game.GetAudioManager().StartMusicTrack(
      !isUw2 ? Audio::musicUw1_Introduction : Audio::musicUw2_LabyrinthOfWorldsTheme,
      false);

   m_game.GetRenderer().SetupForUserInterface();

   // load first image
   UaTrace("loading first image\n");
   if (!isUw2)
   {
      const char* firstImageName = "data/pres1.byt";
      if (m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
         firstImageName = "data/presd.byt";

      // load image, palette 5
      m_game.GetImageManager().Load(m_currentImage.GetImage(),
         firstImageName, 0, 5, imageByt);
   }
   else
   {
      m_game.GetImageManager().LoadFromArk(m_currentImage.GetImage(), "data/byt.ark", 6, 5);
   }

   m_currentImage.Init(m_game, 0, 0);

   // demo game?
   if (!isUw2 &&
      m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
   {
      // write a string under the demo title
      Font font;
      IndexedImage tempImage;
      font.Load(m_game.GetResourceManager(), fontBig);
      font.CreateString(tempImage, "Underworld Adventures", 198);

      double scale = 0.9;
      unsigned int xpos = unsigned((320 - tempImage.GetXRes() * scale) / 2);

      m_currentImage.GetImage().PasteRect(tempImage, 0, 0,
         tempImage.GetXRes(), tempImage.GetYRes(),
         xpos, 200 - 16);
   }

   m_currentImage.Update();

   // set up variables
   m_stage = 0;
   m_tickCount = 0;
   m_currentFrame = 0;
   m_animationCount = 0.0;

   // leave out first two screens when we have savegames
   if (!m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo) &&
      m_game.GetSavegamesManager().GetSavegamesCount() > 0)
   {
      UaTrace("skipping images (savegames available)\n");

      m_stage = 1;
      m_tickCount = unsigned(c_showTime * m_game.GetTickRate()) + 1;
      Tick();
   }

   // switch on cursor for this screen
   SDL_ShowCursor(1);
}

void StartSplashScreen::Destroy()
{
   Screen::Destroy();

   m_currentImage.Destroy();
   m_currentCutscene.Destroy();

   SDL_ShowCursor(0);

   UaTrace("start splash screen ended\n\n");
}

void StartSplashScreen::Draw()
{
   Screen::Draw();

   // calculate brightness of texture quad
   Uint8 light = 255;

   switch (m_stage)
   {
      // anim fade-in/fade-out
   case 2:
   case 4:
      light = m_fader.GetFadeValue();
      break;

      // finished
   case 5:
      light = 0;
      break;
   }

   glColor3ub(light, light, light);

   if (m_stage >= 2)
   {
      // prepare and convert animation frame
      m_currentCutscene.UpdateFrame(m_currentFrame);

      // render quad
      m_currentCutscene.Draw();
   }
   else
   {
      // render still image
      m_currentImage.Draw();
   }
}

bool StartSplashScreen::ProcessEvent(SDL_Event& event)
{
   bool ret = false;

   switch (event.type)
   {
   case SDL_KEYDOWN:
   case SDL_MOUSEBUTTONDOWN:
      // when a key or mouse button was pressed, go to next stage
      switch (m_stage)
      {
      case 0: // first or second image
      case 1:
         m_tickCount = unsigned(c_showTime * m_game.GetTickRate()) + 1;
         ret = true;
         break;

      case 2: // fading in animation
         m_stage = 4;
         m_tickCount = unsigned(c_blendTime * m_game.GetTickRate()) - m_tickCount;

         // init fadeout
         m_fader.Init(false, m_game.GetTickRate(), c_blendTime, m_tickCount);

         ret = true;
         break;

      case 3: // showing animation
         m_stage++;
         m_tickCount = 0;

         // fade out music when we have the demo (ingame starts after this)
         if (m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
            m_game.GetAudioManager().FadeoutMusic(static_cast<int>(c_blendTime * 1000));

         // init fadeout
         m_fader.Init(false, m_game.GetTickRate(), c_blendTime);

         ret = true;
         break;
      }
      break;
   }

   return ret;
}

void StartSplashScreen::Tick()
{
   Screen::Tick();

   m_tickCount++;

   // check if animation should be loaded
   if ((m_stage == 1 || (m_stage == 0 &&
      m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))) &&
      m_tickCount >= c_showTime * m_game.GetTickRate())
   {
      UaTrace("loading animation\n");

      // load animation
      m_currentCutscene.Load(m_game.GetResourceManager(), "cuts/cs011.n01");
      m_currentCutscene.Init(m_game, 0, 0);

      m_currentFrame = 0;
      m_animationCount = 0.0;
      m_stage = 2;
      m_tickCount = 0;

      // init fadein
      m_fader.Init(true, m_game.GetTickRate(), c_blendTime);
   }

   bool isUw2 = m_game.GetSettings().GetGameType() == Base::gameUw2;

   // check other stages
   switch (m_stage)
   {
   case 0:
      if (m_tickCount >= c_showTime * m_game.GetTickRate())
      {
         UaTrace("loading second image\n");

         // load second image
         if (!isUw2)
         {
            m_game.GetImageManager().Load(m_currentImage.GetImage(),
               "data/pres2.byt", 0, 5, imageByt);
         }
         else
         {
            m_game.GetImageManager().LoadFromArk(m_currentImage.GetImage(), "data/byt.ark", 7, 5);
         }

         m_currentImage.Update();

         m_stage++;
         m_tickCount = 0;
      }
      break;

      // fade-in / out
   case 2:
   case 4:
      if (m_fader.Tick())
      {
         m_stage++;
         m_tickCount = 0;
         break;
      }
      // no break, fall through

      // animation
   case 3:
      // check if we have to do a new animation frame
      m_animationCount += 1.0 / m_game.GetTickRate();
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
      }
      break;

      // finished
   case 5:
      // start next screen
      if (m_game.GetSettings().GetBool(Base::settingUw1IsUwdemo))
      {
         // when we have the demo, we immediately go to the ingame screen

         // load and init new game
         Import::LoadUnderworld(m_game.GetSettings(), m_game.GetResourceManager(), m_game.GetUnderworld());

         m_game.GetScripting().InitNewGame();

         // to ingame screen
         m_game.ReplaceScreen(new OriginalIngameScreen(m_game), false);
         return;
      }
      else
         m_game.ReplaceScreen(new StartMenuScreen(m_game), false);
      break;
   }
}
