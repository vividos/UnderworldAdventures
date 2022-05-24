//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019,2021,2022 Underworld Adventures Team
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
#include "AutomapGenerator.hpp"
#include <functional>

MapViewScreen::MapViewScreen(IGame& gameInterface)
   :ImageScreen(gameInterface, 0, 0.5),
   m_displayedLevel((size_t)-1),
   m_upButton(*this,
      std::bind(&MapViewScreen::UpDownLevel, this, true)),
   m_downButton(*this,
      std::bind(&MapViewScreen::UpDownLevel, this, false))
{
}

void MapViewScreen::Init()
{
   ImageScreen::Init();

   // init mouse cursor
   m_mouseCursor.Init(m_game, 0);
   m_mouseCursor.Show(true);
   RegisterWindow(&m_mouseCursor);

   // init click areas
   RegisterWindow(&m_upButton);
   RegisterWindow(&m_downButton);

   bool isUw2 = m_game.GetSettings().GetGameType() == Base::gameUw2;
   if (!isUw2)
   {
      m_upButton.Create(290, 0, 30, 22);
      m_downButton.Create(290, 178, 30, 22);
   }
   else
   {
      m_upButton.Create(284, 0, 36, 32);
      m_downButton.Create(284, 168, 36, 32);
   }

   m_displayedLevel = m_game.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrMapLevel);
   DisplayLevelMap(m_displayedLevel);

   // start audio track "maps & legends" for map
   if (!m_game.GetAudioManager().IsPlayingMusicTrack(Audio::musicUw1_MapsAndLegends))
      m_game.GetAudioManager().StartMusicTrack(Audio::musicUw1_MapsAndLegends, false);
}

bool MapViewScreen::ProcessEvent(SDL_Event& event)
{
   if (ImageScreen::ProcessEvent(event))
      return true;

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

void MapViewScreen::UpDownLevel(bool up)
{
   bool isUw2 = m_game.GetSettings().GetGameType() == Base::gameUw2;

   if (!isUw2)
   {
      if (up && m_displayedLevel > 0)
         DisplayLevelMap(--m_displayedLevel);
      else if (!up && m_displayedLevel < 8)
         DisplayLevelMap(++m_displayedLevel);
   }
   else
   {
      if (up && (m_displayedLevel % 8) > 0)
         DisplayLevelMap(--m_displayedLevel);
      else if (!up && (m_displayedLevel % 8) < 7)
         DisplayLevelMap(++m_displayedLevel);
   }
}

void MapViewScreen::DisplayLevelMap(size_t levelIndex)
{
   IndexedImage& image = GetImage();

   ImageManager& imageManager = m_game.GetImageManager();

   bool isUw2 = m_game.GetSettings().GetGameType() == Base::gameUw2;

   if (!isUw2)
      imageManager.Load(image, "data/blnkmap.byt", 0, 1, imageByt);
   else
      m_game.GetImageManager().LoadFromArk(image, "data/byt.ark", 0, 1);

   const Underworld::Level& level = m_game.GetUnderworld().GetLevelList().GetLevel(levelIndex);
   const Underworld::Tilemap& tilemap = level.GetTilemap();

   UI::AutomapGenerator generator{ m_game.GetResourceManager(), imageManager, tilemap };

   generator.DrawLevelNumber(image, levelIndex, level.GetLevelName());
   generator.DrawTiles(image);

   bool upArrow = levelIndex > 0;
   bool downArrow = levelIndex < 8;
   if (isUw2)
   {
      upArrow = (levelIndex % 8) > 0;
      downArrow = (levelIndex % 8) < 7;
   }

   generator.DrawUpDownArrows(image, upArrow, downArrow);

   generator.DrawMapNotes(image, level.GetMapNotes());

   const Underworld::Player& player = m_game.GetUnderworld().GetPlayer();

   size_t playerLevel = player.GetAttribute(Underworld::attrMapLevel);
   bool showPlayerPin = playerLevel == levelIndex;

   if (showPlayerPin)
      generator.DrawPlayerPin(image, player);

   UpdateImage();
}
