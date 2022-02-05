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
/// \file AcknowledgementsScreen.hpp
/// \brief acknowledgements screen
//
#pragma once

#include "Screen.hpp"
#include "ImageQuad.hpp"
#include "Cutscene.hpp"
#include "FadingHelper.hpp"

/// cutscene view screen
class AcknowledgementsScreen : public Screen
{
public:
   /// ctor
   AcknowledgementsScreen(IGame& game)
      :Screen(game)
   {
   }

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void Tick() override;

protected:
   /// inits fadeout at end
   void FadeoutEnd();

protected:
   /// time to show one credits page
   static const double s_showTime;

   /// time to crossfade between two screens
   static const double s_crossfadeTime;

   /// current view stage
   unsigned int m_stage;

   /// tick count for every stage
   unsigned int m_tickCount;

   /// current visible frame
   unsigned int m_currentFrame;

   /// indicates if ended
   bool m_isEnded;

   /// fading helper
   FadingHelper m_fader;

   /// acknowledgements cutscene
   Cutscene m_ackCutscene;

   /// cutscene image
   ImageQuad m_image;

   /// image to fade out
   ImageQuad m_fadeoutImage;
};
