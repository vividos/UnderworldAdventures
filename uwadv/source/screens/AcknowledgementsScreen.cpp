//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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
/// \file AcknowledgementsScreen.cpp
/// \brief acknowledgements implementation
/// stage 0: show animation frame
/// stage 1: crossfade to next frame
//
#include "common.hpp"
#include "AcknowledgementsScreen.hpp"
#include "IndexedImage.hpp"
#include "Renderer.hpp"

/// time to show one credits image
const double AcknowledgementsScreen::s_showTime = 3.0;

/// time to crossfade between two screens
const double AcknowledgementsScreen::s_crossfadeTime = 0.5;

void AcknowledgementsScreen::Init()
{
   Screen::Init();

   UaTrace("acknowledgements screen started\n");

   m_game.GetRenderer().SetupCamera2D();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // set up variables
   m_isEnded = false;
   m_stage = 1; // crossfade
   m_tickCount = 0;
   m_currentFrame = 0;

   // init cutscene quad
   m_ackCutscene.Load(m_game.GetResourceManager(), "cuts/cs012.n01");
   m_ackCutscene.Init(m_game, 0, 0);
   m_ackCutscene.UpdateFrame(m_currentFrame);

   // init fadeout image
   m_fadeoutImage.Init(m_game, 0, 0);
   {
      IndexedImage& img = m_fadeoutImage.GetImage();

      img.Create(320, 200);
      img.Clear(1);
   }
   m_fadeoutImage.Update();

   // init fadeout
   m_fader.Init(false, m_game.GetTickRate(), s_crossfadeTime);

   SDL_ShowCursor(1);
}

void AcknowledgementsScreen::Destroy()
{
   SDL_ShowCursor(0);

   m_ackCutscene.Destroy();
   m_fadeoutImage.Destroy();

   UaTrace("acknowledgements screen ended\n\n");
}

void AcknowledgementsScreen::Draw()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glDisable(GL_BLEND);

   // render first quad; image to show / to fade in
   glColor3ub(255, 255, 255);

   // draw first quad
   m_ackCutscene.Draw();

   if (m_stage == 1)
   {
      // render second quad using alpha blending
      glEnable(GL_BLEND);

      Uint8 alpha = m_fader.GetFadeValue();
      glColor4ub(255, 255, 255, alpha);

      // draw second quad
      m_fadeoutImage.Draw();
   }
}

bool AcknowledgementsScreen::ProcessEvent(SDL_Event& event)
{
   switch (event.type)
   {
   case SDL_MOUSEBUTTONDOWN:
      // start crossfade immediately
      if (m_stage == 0)
         m_tickCount = unsigned(m_game.GetTickRate()*s_showTime) + 1;
      break;

   case SDL_KEYDOWN:
      // handle key presses
      switch (event.key.keysym.sym)
      {
      case SDLK_SPACE:
         m_tickCount = unsigned(s_showTime * m_game.GetTickRate()) + 1;
         break;

      case SDLK_RETURN:
      case SDLK_ESCAPE:
         UaTrace("acknowledgements ended by return/escape\n");
         m_isEnded = true;

         FadeoutEnd();
         break;

      default:
         break;
      }
   default:
      break;
   }

   return true;
}

void AcknowledgementsScreen::Tick()
{
   if (m_stage == 0 && double(m_tickCount) / m_game.GetTickRate() >= s_showTime)
   {
      // last frame? fade out and end
      if (m_currentFrame == 13)
      {
         m_isEnded = true;
         FadeoutEnd();
         return;
      }

      // switch to crossfade
      m_stage = 1;
      m_tickCount = 0;

      // reinit m_fader
      m_fader.Init(false, m_game.GetTickRate(), s_crossfadeTime);

      // copy old frame to fadeout image
      m_fadeoutImage.GetImage() = m_ackCutscene.GetImage();
      m_fadeoutImage.Update();

      // load new animation frame
      m_ackCutscene.UpdateFrame(++m_currentFrame);
      m_fadeoutImage.Update();

      //UaTrace("crossfading to frame %u\n",m_currentFrame);
      return;
   }

   if (m_stage == 1 && m_fader.Tick())
   {
      // fading complete; switch to showing image
      //UaTrace("showing frame %u\n",m_currentFrame);

      if (m_isEnded)
         m_game.RemoveScreen();

      // switch to show mode
      m_stage = 0;
      m_tickCount = 0;

      return;
   }

   ++m_tickCount;
}

void AcknowledgementsScreen::FadeoutEnd()
{
   // initiate fadeout
   if (m_stage == 1)
      m_tickCount = unsigned(s_crossfadeTime * m_game.GetTickRate()) - m_tickCount;
   else
      m_tickCount = 0;
   m_stage = 1;

   // reinit m_fader
   m_fader.Init(false, m_game.GetTickRate(), s_crossfadeTime, m_tickCount);

   // copy last frame to fadeout image
   m_fadeoutImage.GetImage() = m_ackCutscene.GetImage();
   m_fadeoutImage.Update();

   // insert black frame as fadeout frame
   m_ackCutscene.GetImage().Clear(1);
   m_ackCutscene.Update();
}
