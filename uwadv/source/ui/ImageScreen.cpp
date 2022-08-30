//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2021,2022 Underworld Adventures Team
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
/// \file ImageScreen.cpp
/// \brief image screen implementation
//
#include "pch.hpp"
#include "ImageScreen.hpp"
#include "ImageManager.hpp"
#include "Renderer.hpp"

ImageScreen::ImageScreen(IGame& game, unsigned int paletteIndex, double fadeInOutTime)
   :Screen(game),
   m_fadeInOutTime(fadeInOutTime),
   m_fadeState(fadeStateNotStartedFadein)
{
   m_image.Init(game.GetGameInstance(), 0, 0);
   m_image.Create(0, 0, 320, 200);

   Palette256Ptr palette = m_gameInstance.GetImageManager().GetPalette(paletteIndex);
   m_image.GetImage().SetPalette(palette);
}

IndexedImage& ImageScreen::GetImage()
{
   return m_image.GetImage();
}

void ImageScreen::UpdateImage()
{
   return m_image.Update();
}

void ImageScreen::SetFadeinComplete()
{
   m_fadeState = fadeStateShow;
}

void ImageScreen::StartFadein()
{
   if (IsFadeInProgress())
      return;

   m_fader.Init(true, m_game.GetTickRate(), m_fadeInOutTime);

   m_fadeState = fadeStateFadein;
}

void ImageScreen::StartFadeout()
{
   if (IsFadeInProgress())
      return;

   m_fader.Init(false, m_game.GetTickRate(), m_fadeInOutTime);

   m_fadeState = fadeStateFadeout;
   OnFadeOutStarted();
}

bool ImageScreen::IsFadeInProgress() const
{
   return m_fadeState == fadeStateFadein ||
      m_fadeState == fadeStateFadeout;
}

Uint8 ImageScreen::GetCurentFadeAlphaValue() const
{
   if (m_fadeState == fadeStateNotStartedFadein ||
      m_fadeState == fadeStateEndedFadeout)
      return 0;

   if (!IsFadeInProgress())
      return 255;

   return m_fader.GetFadeValue();
}

void ImageScreen::OnFadeInEnded()
{
   // empty
}

void ImageScreen::OnFadeOutStarted()
{
   // empty
}

void ImageScreen::OnFadeOutEnded()
{
   m_game.RemoveScreen();
}

void ImageScreen::Init()
{
   Screen::Init();

   m_game.GetRenderer().SetupForUserInterface();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   StartFadein();
}

void ImageScreen::Draw()
{
   if (m_clearDrawBuffer)
      glClear(GL_COLOR_BUFFER_BIT);

   glEnable(GL_BLEND);

   Uint8 alpha = GetCurentFadeAlphaValue();
   if (alpha != 0)
      glEnable(GL_BLEND);

   glColor4ub(255, 255, 255, alpha);
   m_image.Draw();

   Screen::Draw();

   if (alpha != 0)
      glDisable(GL_BLEND);
}

void ImageScreen::Tick()
{
   if (IsFadeInProgress() && m_fader.Tick())
   {
      m_fadeState = ImageScreenFadeState(m_fadeState + 1);

      if (m_fadeState == fadeStateEndedFadeout)
         OnFadeOutEnded();
      else if (m_fadeState == fadeStateShow)
         OnFadeInEnded();
   }
}
