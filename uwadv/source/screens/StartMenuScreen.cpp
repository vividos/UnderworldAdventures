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
/// \file StartMenuScreen.cpp
/// \brief start menu screen implementation
//
#include "common.hpp"
#include "StartMenuScreen.hpp"
#include "Audio.hpp"
#include "Renderer.hpp"
#include "Savegame.hpp"
#include "CutsceneViewScreen.hpp"
#include "AcknowledgementsScreen.hpp"
#include "CreateCharacterScreen.hpp"
#include "OriginalIngameScreen.hpp"
#include "SaveGameScreen.hpp"

/// time to fade in/out
const double StartMenuScreen::s_fadeTime = 0.5;

/// palette shifts per second
const double StartMenuScreen::s_paletteShiftsPerSecond = 20.0;

StartMenuScreen::StartMenuScreen(IGame& game)
   :Screen(game)
{
}

void StartMenuScreen::Init()
{
   Screen::Init();

   UaTrace("start menu screen started\n");

   // load background image
   m_game.GetImageManager().Load(m_screenImage.GetImage(), "data/opscr.byt",
      0, 2, imageByt);
   m_screenImage.GetImage().ClonePalette();

   m_screenImage.Init(m_game, 0, 0);

   // load button graphics
   m_game.GetImageManager().LoadList(m_buttonImages, "opbtn", 0, 8, 2);

   // set up mouse cursor
   m_mouseCursor.Init(m_game, 0);
   m_mouseCursor.Show(true);

   RegisterWindow(&m_mouseCursor);

   Resume();
}

void StartMenuScreen::Resume()
{
   UaTrace("resuming start menu screen\n");

   m_game.GetRenderer().SetupCamera2D();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   m_screenImage.Update();

   m_fader.Init(true, m_game.GetTickRate(), s_fadeTime);

   // set other flags/values
   m_stage = 0;
   m_isJourneyOnwardAvailable = m_game.GetSavegamesManager().GetSavegamesCount() > 0;
   m_selectedArea = -1;
   m_shiftCount = 0.0;
   m_reuploadImage = true;
}

void StartMenuScreen::Destroy()
{
   UaTrace("start menu screen ended\n\n");

   m_screenImage.Destroy();
}

void StartMenuScreen::Draw()
{
   // no need to clear the screen, since we overdraw all of it anyway

   // do we need to reupload the image quad texture?
   if (m_reuploadImage)
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
         m_screenImage.GetImage().PasteRect(img,
            0, 0, img.GetXRes(), img.GetYRes(),
            buttonCoordinates[i * 2], buttonCoordinates[i * 2 + 1]);
      }

      m_screenImage.Update();

      m_reuploadImage = false;
   }

   // calculate brightness of texture quad
   Uint8 light = m_fader.GetFadeValue();

   glColor3ub(light, light, light);

   // render screen image and mouse
   m_screenImage.Draw();

   // draw subwindows
   Screen::Draw();
}

bool StartMenuScreen::ProcessEvent(SDL_Event& event)
{
   bool ret = false;

   ret |= Screen::ProcessEvent(event);

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
         if (m_stage == 1)
         {
            m_stage++;
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
   // when fading in or out, check if blend time is over
   if ((m_stage == 0 || m_stage == 2) && m_fader.Tick())
   {
      // do next stage
      m_stage++;
   }

   // do palette shifting
   m_shiftCount += 1.0 / m_game.GetTickRate();
   if (m_shiftCount >= 1.0 / s_paletteShiftsPerSecond)
   {
      m_shiftCount -= 1.0 / s_paletteShiftsPerSecond;

      // shift palette
      m_screenImage.GetImage().GetPalette()->Rotate(64, 64, false);

      // initiate new upload
      m_reuploadImage = true;
   }

   // in stage 3, we really press the selected button
   if (m_stage == 3)
   {
      PressButton();
      m_stage = 0;

      // fade in, in case user returns to menu screen
      m_fader.Init(true, m_game.GetTickRate(), s_fadeTime);
   }
}

void StartMenuScreen::PressButton()
{
   UaTrace("button %u was pressed\n", m_selectedArea);

   switch (m_selectedArea)
   {
   case 0: // "introduction"
      m_game.ReplaceScreen(new CutsceneViewScreen(m_game, 0), true);
      break;

   case 1: // "create character"
      m_game.ReplaceScreen(new CreateCharacterScreen(m_game), true);
      break;

   case 2: // "acknowledgements"
      m_game.ReplaceScreen(new AcknowledgementsScreen(m_game), true);
      break;

   case 3: // "journey onward"
      if (m_isJourneyOnwardAvailable)
      {
         // "load game" screen (with later starting "orig. ingame ui")
         m_game.ReplaceScreen(new SaveGameScreen(m_game, true, true), true);
      }
      break;
   }
}

void StartMenuScreen::MouseEvent(bool buttonClicked, bool leftButton, bool buttonDown,
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
      return;

   // a button click action?
   if (buttonClicked)
   {
      // only in stage 1
      if (m_stage == 1)
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
               m_stage++; // next stage
               m_fader.Init(false, m_game.GetTickRate(), s_fadeTime);

               // fade out music when selecting "introduction"
               if (m_selectedArea == 0)
                  m_game.GetAudioManager().FadeoutMusic(s_fadeTime);
            }
         }
      }
   }
   else
   {
      // a mouse move action with at least one button down
      Uint8 mouse_state = SDL_GetMouseState(NULL, NULL);
      if (m_stage == 1 &&
         (mouse_state & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) != 0 &&
         area != -1)
         m_selectedArea = area;
   }
}
