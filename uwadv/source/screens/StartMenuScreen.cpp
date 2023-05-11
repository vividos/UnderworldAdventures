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
/// \file StartMenuScreen.cpp
/// \brief start menu screen implementation
//
#include "pch.hpp"
#include "StartMenuScreen.hpp"
#include "Audio.hpp"
#include "Renderer.hpp"
#include "Savegame.hpp"
#include "CutsceneViewScreen.hpp"
#include "AcknowledgementsScreen.hpp"
#include "CreateCharacterScreen.hpp"
#include "OriginalIngameScreen.hpp"
#include "SaveGameScreen.hpp"
#include "ImageManager.hpp"

/// time to fade in/out
const double StartMenuScreen::s_fadeTime = 0.5;

/// palette shifts per second
const double StartMenuScreen::s_paletteShiftsPerSecond = 20.0;

StartMenuScreen::StartMenuScreen(IGame& game)
   :ImageScreen(game, 0, s_fadeTime)
{
}

void StartMenuScreen::Init()
{
   ImageScreen::Init();

   UaTrace("start menu screen started\n");

   bool isUw2 = m_gameInstance.GetSettings().GetGameType() == Base::gameUw2;

   // load background image
   if (!isUw2)
   {
      m_gameInstance.GetImageManager().Load(GetImage(), "data/opscr.byt", 0, 2, imageByt);
      GetImage().ClonePalette();
   }
   else
   {
      m_gameInstance.GetImageManager().LoadFromArk(GetImage(), "data/byt.ark", 5, 0);
   }

   // load button graphics
   m_gameInstance.GetImageManager().LoadList(m_buttonImages, "opbtn", 0, 8, 2);

   // set up mouse cursor
   m_mouseCursor.Init(m_gameInstance, 0);
   m_mouseCursor.Show(true);

   RegisterWindow(&m_mouseCursor);

   Resume();
}

void StartMenuScreen::Resume()
{
   UaTrace("resuming start menu screen\n");

   m_game.GetRenderer().SetupForUserInterface();

   UpdateImage();

   // set other flags/values
   m_isJourneyOnwardAvailable = m_gameInstance.GetSavegamesManager().GetSavegamesCount() > 0;
   m_selectedArea = -1;
   m_shiftCount = 0.0;
   m_reuploadImage = true;

   if (m_game.GetAudioManager().IsMusicFadeoutOrStopped())
   {
      bool isUw2 = m_gameInstance.GetSettings().GetGameType() == Base::gameUw2;

      m_game.GetAudioManager().StartMusicTrack(
         !isUw2 ? Audio::musicUw1_Introduction : Audio::musicUw2_LabyrinthOfWorldsTheme,
         false);
   }

   StartFadein();
}

void StartMenuScreen::Destroy()
{
   ImageScreen::Destroy();

   UaTrace("start menu screen ended\n\n");
}

void StartMenuScreen::Draw()
{
   // do we need to reupload the image quad texture?
   if (m_reuploadImage)
   {
      UpdateBackgroundImage();
      m_reuploadImage = false;
   }

   ImageScreen::Draw();
}

void StartMenuScreen::UpdateBackgroundImage()
{
   // combine button graphics with background image
   const unsigned int buttonCoordinates[] =
   {
      98,81, 81,104, 72,128, 85,153
   };

   unsigned int max = m_isJourneyOnwardAvailable ? 4 : 3;
   for (unsigned int i = 0; i < max; i++)
   {
      unsigned int buttonNr = i * 2;
      if (int(i) == m_selectedArea) buttonNr++;

      IndexedImage& img = m_buttonImages[buttonNr];
      GetImage().PasteRect(img,
         0, 0, img.GetXRes(), img.GetYRes(),
         buttonCoordinates[i * 2], buttonCoordinates[i * 2 + 1]);
   }

   UpdateImage();
}

bool StartMenuScreen::ProcessEvent(SDL_Event& event)
{
   bool ret = false;

   ret |= ImageScreen::ProcessEvent(event);

   int lastSelectedArea = m_selectedArea;

   if (event.type == SDL_KEYDOWN)
   {
      // handle key presses
      switch (event.key.keysym.sym)
      {
      case SDLK_UP:
         // select the area above, if possible
         if (m_selectedArea == -1) m_selectedArea = 0;
         if (m_selectedArea > 0) m_selectedArea--;
         ret = true;
         break;

      case SDLK_DOWN:
         // select the area below, if possible
         if (m_selectedArea + 1 < (m_isJourneyOnwardAvailable ? 4 : 3))
            m_selectedArea++;
         ret = true;
         break;

      case SDLK_RETURN:
         // simulate clicking on that area
         if (!IsFadeInProgress())
         {
            StartFadeout();
            ret = true;
         }
         break;

      default:
         break;
      }
   }

   // user event?
   if (event.type == SDL_USEREVENT && event.user.code == gameEventResumeScreen)
      Resume();

   // check if selected area changed
   if (m_selectedArea != lastSelectedArea)
      m_reuploadImage = true;

   return ret;
}

void StartMenuScreen::Tick()
{
   ImageScreen::Tick();

   // uw1: do palette shifting
   bool isUw2 = m_gameInstance.GetSettings().GetGameType() == Base::gameUw2;
   if (!isUw2)
   {
      m_shiftCount += 1.0 / GetTickRate();
      if (m_shiftCount >= 1.0 / s_paletteShiftsPerSecond)
      {
         m_shiftCount -= 1.0 / s_paletteShiftsPerSecond;

         // shift palette
         GetImage().GetPalette()->Rotate(64, 64, false);

         // initiate new upload
         m_reuploadImage = true;
      }
   }
}

void StartMenuScreen::OnFadeOutEnded()
{
   UaTrace("button %u was pressed\n", m_selectedArea);

   switch (m_selectedArea)
   {
   case 0: // "introduction"
      m_game.GetScreenHost().ReplaceScreen(new CutsceneViewScreen(m_game, 0), true);
      break;

   case 1: // "create character"
      m_game.GetScreenHost().ReplaceScreen(new CreateCharacterScreen(m_game), true);
      break;

   case 2: // "acknowledgements"
      m_game.GetScreenHost().ReplaceScreen(new AcknowledgementsScreen(m_game), true);
      break;

   case 3: // "journey onward"
      if (m_isJourneyOnwardAvailable)
      {
         // "load game" screen (with later starting "orig. ingame ui")
         m_game.GetScreenHost().ReplaceScreen(new SaveGameScreen(m_game, true, true), true);
      }
      break;
   }
}

bool StartMenuScreen::MouseEvent(bool buttonClicked, bool leftButton, bool buttonDown,
   unsigned int mouseX, unsigned int mouseY)
{
   // check over which area we are
   int area = -1;

   if (mouseX > 64 && mouseX < 240 && mouseY >= 81 && mouseY <= 180)
   {
      if (mouseY < 104) area = 0;
      else if (mouseY < 128) area = 1;
      else if (mouseY < 153) area = 2;
      else area = 3;
   }

   if (area == 3 && !m_isJourneyOnwardAvailable)
      return false;

   // a button click action?
   if (buttonClicked)
   {
      if (!IsFadeInProgress())
      {
         if (buttonDown)
         {
            // mouse button down
            m_selectedArea = area;
         }
         else
         {
            // mouse button up

            // determine if user released the mouse button over the same area
            if (area != -1 && m_selectedArea == area)
            {
               StartFadeout();

               // fade out music when selecting "introduction"
               if (m_selectedArea == 0)
                  m_game.GetAudioManager().FadeoutMusic(static_cast<int>(s_fadeTime * 1000));
            }
         }
      }
   }
   else
   {
      // a mouse move action with at least one button down
      Uint8 mouseState = SDL_GetMouseState(NULL, NULL);
      if (!IsFadeInProgress() &&
         (mouseState & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) != 0 &&
         area != -1)
         m_selectedArea = area;
   }

   return true;
}
