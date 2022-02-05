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
/// \file StartSplashScreen.hpp
/// \brief start splash screen
//
#pragma once

#include "Screen.hpp"
#include "ImageQuad.hpp"
#include "Font.hpp"
#include "Cutscene.hpp"
#include "FadingHelper.hpp"

/// \brief start splash screen class
/// The screen displays the splash screen sequence at start of Ultima
/// Underworld. It shows two images (or just one when using the demo) and the
/// animated logo. The images are skipped when savegames are available.
/// The screen can be in one of these stages:
/// stage 0: first opening screen
/// stage 1: second opening screen (not in uw_demo)
/// stage 2: fading in animation
/// stage 3: showing animation
/// stage 4: fading out animation
/// stage 5: screen finished
class StartSplashScreen : public Screen
{
public:
   /// ctor
   StartSplashScreen(IGame& game);
   /// dtor
   virtual ~StartSplashScreen() {}

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void Tick() override;

private:
   /// number of seconds the splash screen images are shown
   static const double c_showTime;

   /// fade in/out time in seconds
   static const double c_blendTime;

   /// animation frame rate, in frames per second
   static const double c_animationFrameRate;

   /// palette shifts per second
   static const double c_paletteShiftsPerSecond;

   /// fading helper
   FadingHelper m_fader;

   /// current stage we are in
   unsigned int m_stage;

   /// count of ticks since last stage-start
   unsigned int m_tickCount;

   /// current still image
   ImageQuad m_currentImage;

   /// current cutscene
   Cutscene m_currentCutscene;

   /// cutscene image
   ImageQuad m_cutsceneImage;

   /// current animation frame
   unsigned int m_currentFrame;

   /// animation time count
   double m_animationCount;

   /// count for palette shifting
   double m_shiftCount;
};
