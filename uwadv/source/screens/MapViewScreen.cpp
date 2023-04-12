//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2019,2021,2022,2023 Underworld Adventures Team
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

/// coordinates for gem click areas, in order of uw2 maps (first castle, then
/// prison tower, etc.); tuple values are xpos, ypos, width, height
static std::array<
   std::tuple<unsigned int, unsigned int, unsigned int, unsigned int>,
   9> g_gemCoordinates =
{
   {
      { 268, 69, 21, 23}, // center
      { 270, 58, 17, 11}, // N
      { 287, 58, 13, 13, }, // NE
      { 289, 71, 11, 19, }, // E
      { 287, 90, 13, 13, }, // SE
      { 270, 92, 17, 11, }, // S
      { 257, 90, 13, 13, }, // SW
      { 257, 71, 11, 19, }, // W
      { 257, 58, 13, 13, }, // NW
   }
};

MapViewScreen::MapViewScreen(IGame& gameInterface)
   :ImageScreen(gameInterface, 0, 0.5),
   m_displayedLevel((size_t)-1),
   m_upButton(*this,
      std::bind(&MapViewScreen::UpDownLevel, this, true)),
   m_downButton(*this,
      std::bind(&MapViewScreen::UpDownLevel, this, false)),
   m_eraseButton(*this,
      std::bind(&MapViewScreen::SetMapEditMode, this, MapEditMode::mapEditModeErase)),
   m_closeButton(*this,
      std::bind(&MapViewScreen::StartFadeout, this))
{
   m_fontSelectedMapNote.Load(
      gameInterface.GetGameInstance().GetResourceManager(), fontSmall);
}

void MapViewScreen::Init()
{
   ImageScreen::Init();

   bool isUw2 = m_gameInstance.GetSettings().IsGameUw2();

   // add selection image here so that it's always below the mouse cursor
   m_selectedMapNote = std::make_shared<ImageQuad>();
   RegisterWindow(m_selectedMapNote.get());

   // init mouse cursor
   m_mouseCursor.Init(m_gameInstance, 0, isUw2 ? 1 : 0);
   m_mouseCursor.Show(true);
   RegisterWindow(&m_mouseCursor);
   SetMapEditMode(MapEditMode::mapEditModeNone);

   // init click areas and buttons
   RegisterWindow(&m_upButton);
   RegisterWindow(&m_downButton);
   RegisterWindow(&m_eraseButton);
   RegisterWindow(&m_closeButton);

   if (!isUw2)
   {
      m_upButton.Create(290, 0, 30, 22);
      m_downButton.Create(290, 178, 30, 22);
      m_eraseButton.Create(272, 118, 29, 26);
      m_closeButton.Create(263, 150, 48, 28);
   }
   else
   {
      m_upButton.Create(284, 0, 36, 32);
      m_downButton.Create(284, 168, 36, 32);
      m_eraseButton.Create(264, 109, 29, 26);
      m_closeButton.Create(255, 141, 48, 28);

      for (unsigned int gemIndex = 0; gemIndex < SDL_TABLESIZE(g_gemCoordinates); gemIndex++)
      {
         auto gemClickArea = std::make_shared<ClickArea>(*this,
            std::bind(&MapViewScreen::OnGemClicked, this, gemIndex));

         const auto& gemCoordTuple = g_gemCoordinates[gemIndex];

         gemClickArea->Create(
            std::get<0>(gemCoordTuple),
            std::get<1>(gemCoordTuple),
            std::get<2>(gemCoordTuple),
            std::get<3>(gemCoordTuple));

         m_uw2GemClickAreas.push_back(gemClickArea);
         RegisterWindow(gemClickArea.get());
      }
   }

   m_displayedLevel = m_gameInstance.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrMapLevel);
   DisplayLevelMap(m_displayedLevel);

   // start audio track "maps & legends" for map
   if (!m_game.GetAudioManager().IsPlayingMusicTrack(Audio::musicUw1_MapsAndLegends))
      m_game.GetAudioManager().StartMusicTrack(Audio::musicUw1_MapsAndLegends, false);
}

bool MapViewScreen::ProcessEvent(SDL_Event& event)
{
   // while entering text, ignore cursor moves
   if (event.type == SDL_MOUSEMOTION &&
      m_mapEditMode == MapEditMode::mapEditModeEditNote)
      return true;

   if (ImageScreen::ProcessEvent(event))
      return true;

   bool isHandled = false;

   switch (event.type)
   {
   case SDL_MOUSEBUTTONDOWN:
   {
      int xpos = event.button.x;
      int ypos = event.button.y;
      MapWindowPosition(xpos, ypos);

      if (m_mapEditMode == MapEditMode::mapEditModeNone)
      {
         ypos -= 7;

         // no notes on the far side
         if (xpos < 256)
         {
            StartEditing((unsigned int)xpos, (unsigned int)ypos);
            isHandled = true;
         }
      }
      else if (m_mapEditMode == MapEditMode::mapEditModeErase)
      {
         if (xpos < 256)
            EraseMapNote(xpos, ypos);

         RemoveMapNoteSelectionImage();

         SetMapEditMode(MapEditMode::mapEditModeNone);
         isHandled = true;
      }
      break;
   }

   case SDL_MOUSEMOTION:
   {
      int xpos = event.motion.x;
      int ypos = event.motion.y;
      MapWindowPosition(xpos, ypos);

      if (m_mapEditMode == MapEditMode::mapEditModeNone)
      {
         // use crosshair cursor when outside editing area
         m_mouseCursor.SetType(xpos < 256 ? 12 : 0);
         isHandled = true;
      }
      else if (m_mapEditMode == MapEditMode::mapEditModeErase)
      {
         SelectMapNote(xpos, ypos);
         isHandled = true;
      }
      break;
   }

   case SDL_KEYDOWN:
      // handle key presses
      switch (event.key.keysym.sym)
      {
      case SDLK_SPACE:
      case SDLK_RETURN:
      case SDLK_ESCAPE:
         if (m_mapEditMode == MapEditMode::mapEditModeNone)
         {
            StartFadeout();
            isHandled = true;
         }
         else if (m_mapEditMode == MapEditMode::mapEditModeErase)
         {
            SetMapEditMode(MapEditMode::mapEditModeNone);
            RemoveMapNoteSelectionImage();
            isHandled = true;
         }

         // leaving mapEditModeEditNote is handled by user events
         break;

      default:
         // ignore other keys
         break;
      }
      break;

   case SDL_USEREVENT:
      if (m_textEditWindow != nullptr)
      {
         if (event.user.code == gameEventTexteditFinished)
         {
            AddNewMapNote(
               m_textEditWindow->GetXPos(),
               m_textEditWindow->GetYPos(),
               m_textEditWindow->GetText());
         }

         m_textEditWindow->SetCursorPosChangedHandler();

         UnregisterWindow(m_textEditWindow.get());
         m_textEditWindow.reset();

         SetMapEditMode(MapEditMode::mapEditModeNone);
         isHandled = true;
      }
      break;

   default:
      // ignore other events
      break;
   }

   return isHandled;
}

void MapViewScreen::SetMapEditMode(MapEditMode mapEditMode)
{
   m_mapEditMode = mapEditMode;
   unsigned int cursorIndex = 12;
   if (mapEditMode == MapEditMode::mapEditModeEditNote)
      cursorIndex = 14;

   if (mapEditMode == MapEditMode::mapEditModeErase)
      cursorIndex = 13;

   m_mouseCursor.SetType(cursorIndex);
}

void MapViewScreen::UpDownLevel(bool up)
{
   bool isUw2 = m_gameInstance.GetSettings().IsGameUw2();

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

void MapViewScreen::OnGemClicked(unsigned int gemIndex)
{
   UaTrace("clicked on map gem %u\n", gemIndex);

   unsigned int currentLevel = m_gameInstance.GetUnderworld()
      .GetPlayer().GetAttribute(Underworld::attrMapLevel);

   // back to the castle
   if (gemIndex == 0)
   {
      if (currentLevel < 8)
         m_displayedLevel = currentLevel;
      else
         m_displayedLevel = 0;

      DisplayLevelMap(m_displayedLevel);
      return;
   }

   // check if gem is accessible
   Uint16 worldFlags = m_game.GetGameInstance().GetUnderworld()
      .GetPlayer().GetQuestFlags().GetFlag(130);

   Uint16 worldBit = 1 << (gemIndex - 1);
   if ((worldFlags & worldBit) == 0)
      return; // world not available yet

   unsigned int levelStart = gemIndex * 8;

   bool isSameWorld = (levelStart / 8) == (currentLevel / 8);
   m_displayedLevel = isSameWorld
      ? currentLevel
      : levelStart;

   DisplayLevelMap(m_displayedLevel);
}

void MapViewScreen::DisplayLevelMap(size_t levelIndex)
{
   IndexedImage& image = GetImage();

   ImageManager& imageManager = m_gameInstance.GetImageManager();

   bool isUw2 = m_gameInstance.GetSettings().IsGameUw2();

   if (!isUw2)
      imageManager.Load(image, "data/blnkmap.byt", 0, 1, imageByt);
   else
      m_gameInstance.GetImageManager().LoadFromArk(image, "data/byt.ark", 0, 1);

   const Underworld::Level& level = m_gameInstance.GetUnderworld().GetLevelList().GetLevel(levelIndex);
   const Underworld::Tilemap& tilemap = level.GetTilemap();

   UI::AutomapGenerator generator{ m_gameInstance.GetResourceManager(), imageManager, tilemap };

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

   if (isUw2)
      generator.DrawUw2MapGem(image, levelIndex,
         m_gameInstance.GetUnderworld().GetPlayer());

   generator.DrawMapNotes(image, level.GetMapNotes());

   const Underworld::Player& player = m_gameInstance.GetUnderworld().GetPlayer();

   size_t playerLevel = player.GetAttribute(Underworld::attrMapLevel);
   bool showPlayerPin = playerLevel == levelIndex;

   if (showPlayerPin)
      generator.DrawPlayerPin(image, player);

   UpdateImage();
}

void MapViewScreen::StartEditing(unsigned int xpos, unsigned int ypos)
{
   SetMapEditMode(MapEditMode::mapEditModeEditNote);
   m_textEditWindow = std::make_shared<TextEditWindow>();

   bool isUw2 = m_gameInstance.GetSettings().IsGameUw2();

   Uint8 textColor = !isUw2 ? 45 : 78;
   m_textEditWindow->Init(m_game.GetGameInstance(), xpos - 1, ypos + 7,
      320 - xpos, 0, 0, textColor, "", "", false, fontSmall, 1);

   m_editStartCursorPosX = xpos - 1;

   m_textEditWindow->SetCursorPosChangedHandler(
      [&](unsigned int cursorPosX)
      {
         m_mouseCursor.SetXPos(m_editStartCursorPosX + cursorPosX);
      });

   RegisterWindow(m_textEditWindow.get());
}

void MapViewScreen::AddNewMapNote(unsigned int xpos, unsigned int ypos, std::string noteText)
{
   Underworld::MapNotes& mapNotes =
      m_game.GetGameInstance().GetUnderworld().GetLevelList()
      .GetLevel(m_displayedLevel).GetMapNotes();

   Underworld::MapNote mapNote;
   mapNote.m_xpos = xpos;
   mapNote.m_ypos = ypos;
   mapNote.m_text = noteText;

   mapNotes.GetMapNotesList().push_back(mapNote);

   DisplayLevelMap(m_displayedLevel);
}

void MapViewScreen::SelectMapNote(unsigned int xpos, unsigned int ypos)
{
   std::vector<Underworld::MapNote>::const_iterator iter =
      FindMapNote(xpos, ypos);

   const Underworld::MapNotes& mapNotes =
      m_game.GetGameInstance().GetUnderworld().GetLevelList()
      .GetLevel(m_displayedLevel).GetMapNotes();

   if (iter == mapNotes.GetMapNotesList().end())
   {
      RemoveMapNoteSelectionImage();
      return;
   }

   std::vector<Underworld::MapNote>::difference_type mapNotesIndex =
      std::distance(mapNotes.GetMapNotesList().begin(), iter);

   if (mapNotesIndex == m_selectedMapNotesIndex)
      return; // skip recreating the same select item

   m_selectedMapNotesIndex = mapNotesIndex;

   const Underworld::MapNote& mapNoteToSelect = *iter;

   CreateMapNoteSelectionImage(mapNoteToSelect,
      m_selectedMapNote->GetImage());

   m_selectedMapNote->Init(m_game.GetGameInstance(),
      mapNoteToSelect.m_xpos - 1, mapNoteToSelect.m_ypos - 1);
   m_selectedMapNote->Update();
}

void MapViewScreen::CreateMapNoteSelectionImage(const Underworld::MapNote& mapNoteToSelect,
   IndexedImage& selectedMapNoteImage)
{
   Uint8 selectedColor = 48; // blue

   IndexedImage mapNoteTextImage;
   m_fontSelectedMapNote.CreateString(
      mapNoteTextImage,
      mapNoteToSelect.m_text, selectedColor);

   selectedMapNoteImage.Create(
      mapNoteTextImage.GetXRes() + 3, mapNoteTextImage.GetYRes() + 4);

   selectedMapNoteImage.Clear(selectedColor);
   selectedMapNoteImage.FillRect(1, 1,
      selectedMapNoteImage.GetXRes() - 2, selectedMapNoteImage.GetYRes() - 2,
      0);

   selectedMapNoteImage.PasteImage(mapNoteTextImage, 2, 2, false);
}

void MapViewScreen::EraseMapNote(unsigned int xpos, unsigned int ypos)
{
   std::vector<Underworld::MapNote>::const_iterator iter =
      FindMapNote(xpos, ypos);

   Underworld::MapNotes& mapNotes =
      m_game.GetGameInstance().GetUnderworld().GetLevelList()
      .GetLevel(m_displayedLevel).GetMapNotes();

   if (iter == mapNotes.GetMapNotesList().end())
      return;

   mapNotes.GetMapNotesList().erase(iter);

   RemoveMapNoteSelectionImage();

   DisplayLevelMap(m_displayedLevel);
}

void MapViewScreen::RemoveMapNoteSelectionImage()
{
   m_selectedMapNotesIndex = -1;

   m_selectedMapNote->GetImage().Create(1, 1);
   m_selectedMapNote->Init(m_game.GetGameInstance(), 320, 200);
}

std::vector<Underworld::MapNote>::const_iterator MapViewScreen::FindMapNote(
   unsigned int xpos, unsigned int ypos)
{
   Underworld::MapNotes& mapNotes =
      m_game.GetGameInstance().GetUnderworld().GetLevelList()
      .GetLevel(m_displayedLevel).GetMapNotes();

   unsigned int height = m_fontSelectedMapNote.GetCharHeight();

   size_t maxIndex = mapNotes.GetMapNoteCount();
   for (size_t noteIndex = 0; noteIndex < maxIndex; noteIndex++)
   {
      const Underworld::MapNote& mapNote = mapNotes.GetNote(noteIndex);

      if (xpos < mapNote.m_xpos ||
         ypos < mapNote.m_ypos ||
         ypos >= mapNote.m_ypos + height)
         continue;

      unsigned int width = m_fontSelectedMapNote.CalcLength(mapNote.m_text);

      if (xpos >= mapNote.m_xpos + width)
         continue;

      std::vector<Underworld::MapNote>::const_iterator foundIter =
         mapNotes.GetMapNotesList().begin() + noteIndex;

      return foundIter;
   }

   return mapNotes.GetMapNotesList().end();
}
