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
/// \file StartMenuScreen.hpp
/// \brief start menu screen
//
#pragma once

#include "ImageScreen.hpp"
#include "MouseCursor.hpp"

/// \brief start menu screen class
/// The screen displays the main game menu. The user can select one of the
/// menu items "Introduction", "Create Character", "Acknowledgements" or
/// "Journey Onward" (only available when savegames are detected). Above
/// the menu there is a "warping" Ultima Underworld logo.
class StartMenuScreen : public ImageScreen
{
public:
   /// ctor
   StartMenuScreen(IGame& game);
   /// dtor
   virtual ~StartMenuScreen() {}

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void Tick() override;

   virtual bool MouseEvent(bool buttonClicked, bool leftButton, bool buttonDown,
      unsigned int mouseX, unsigned int mouseY) override;

   // virtual functions from ImageScreen
   virtual void OnFadeOutEnded() override;

private:
   /// called when resuming the screen
   void Resume();

   /// determines selected area by mouse coordinates
   int GetSelectedArea();

   /// updates background image, e.g. when a selected button has changed
   void UpdateBackgroundImage();

protected:
   // constants

   /// time to fade in/out
   static const double s_fadeTime;

   /// palette shifts per second
   static const double s_paletteShiftsPerSecond;


   /// mouse cursor
   MouseCursor m_mouseCursor;

   /// count for palette shifting
   double m_shiftCount;

   /// number of selected area, or -1 if none
   int m_selectedArea;

   /// indicates if "journey onward" is available
   bool m_isJourneyOnwardAvailable;

   /// image list with buttons
   std::vector<IndexedImage> m_buttonImages;

   /// indicates if image quad texture should be reuploaded
   bool m_reuploadImage;
};
