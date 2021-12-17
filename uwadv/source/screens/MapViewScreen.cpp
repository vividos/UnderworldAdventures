//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019,2021 Underworld Adventures Team
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
/// \file MapViewScreen.cpp
/// \brief level map view implementation
//
#include "pch.hpp"
#include "MapViewScreen.hpp"
#include "ImageManager.hpp"
#include "Underworld.hpp"
#include "Player.hpp"
#include "Audio.hpp"

MapViewScreen::MapViewScreen(IGame& gameInterface)
   :ImageScreen(gameInterface, 0, 0.5),
   m_displayedLevel((size_t)-1)
{
}

void MapViewScreen::Init()
{
   ImageScreen::Init();

   m_displayedLevel = m_game.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrMapLevel);
   DisplayLevelMap(m_displayedLevel);

   // start audio track "maps & legends" for map
   if (!m_game.GetAudioManager().IsPlayingMusicTrack(Audio::musicUw1_MapsAndLegends))
      m_game.GetAudioManager().StartMusicTrack(Audio::musicUw1_MapsAndLegends, false);
}

bool MapViewScreen::ProcessEvent(SDL_Event& event)
{
   switch (event.type)
   {
   case SDL_MOUSEBUTTONDOWN:
      StartFadeout();
      break;

   case SDL_KEYDOWN:
      // handle key presses
      switch (event.key.keysym.sym)
      {
      case SDLK_SPACE:
      case SDLK_RETURN:
      case SDLK_ESCAPE:
         StartFadeout();
         break;

      default:
         // ignore other keys
         break;
      }
      break;

   default:
      // ignore other events
      break;
   }

   return true;
}

void MapViewScreen::DisplayLevelMap(size_t levelIndex)
{
   IndexedImage& image = GetImage();

   ImageManager& imageManager = m_game.GetImageManager();
   imageManager.Load(image, "data/blnkmap.byt", 0, 1, imageByt);

   UpdateImage();
}
