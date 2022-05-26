//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022 Underworld Adventures Team
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
/// \file IngameCommandButtons.cpp
/// \brief ingame command buttons
//
#include "pch.hpp"
#include "IngameCommandButtons.hpp"
#include "../OriginalIngameScreen.hpp"
#include "ImageManager.hpp"

/// menu info table struct for command panels
struct IngameCommandMenuInfo
{
   int base_image;         ///< number of the base image in optbtns.gr
   unsigned int images[7]; ///< button images numbers
   bool selectable[7];     ///< indicates if a button can be pressed
   int opt_index[7];       ///< index to add to image number when selected

} g_ingameCommandMenuTable[8] =
{
   { // commandMenuActions; special case: images taken from lfti.gr
      0,
      { 0,2,4,6,8,10,0 },
      { true,true,true,true,true,true,true },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // commandMenuOptions
      1,
      { 6,8,10,12,14,16,18 },
      { true,true,true,true,true,true,true },
      // this line is used for showing music, sound, detail status directly in
      // the options menu, but unfortunately there are no images (see optb.gr)
      //{ -1, -1, 0, 1, 2, -1, -1 }
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // commandMenuQuit
      3,
      { 0,0, 57,59, 0,0,0 },
      { false,false,true,true,false,false,false },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // commandMenuMusic
      4,
      { 47,51,20,22,26, 0,0 },
      { false,false,true,true,true,false,false },
      { 0, -1, -1, -1, -1, -1, -1 }
   },
   { // commandMenuSound
      4,
      { 49,52,20,22,26, 0,0 },
      { false,false,true,true,true,false,false },
      { 1, -1, -1, -1, -1, -1, -1 }
   },
   { // commandMenuDetails
      5,
      { 53, 0, 38,40,42,44, 28 },
      { false,false,true,true,true,true,true },
      { 2, -1, -1, -1, -1, -1, -1 }
   },
   { // commandMenuSave
      2,
      { 0, 30,32,34,36, 24, 0 },
      { false,true,true,true,true,true,false },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
   { // commandMenuRestore
      2,
      { 46,30,32,34,36, 24, 0 },
      { false,true,true,true,true,true,false },
      { -1, -1, -1, -1, -1, -1, -1 }
   },
};

void IngameCommandButtons::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   // load images
   game.GetImageManager().LoadList(m_buttonImages, "optbtns");

   game.GetImageManager().LoadList(m_actionButtonImages, "lfti");

   // create image
   IndexedImage& img = GetImage();
   img.SetPalette(game.GetImageManager().GetPalette(0));

   ImageQuad::Init(game, xpos, ypos);

   m_options[0] = m_options[1] = m_options[2] = 0;
   m_selectedButton = -1;
   m_menu = commandMenuActions;
   m_toggleOff = false;
   m_savedActionButton = -1;

   UpdateMenu();
}

void IngameCommandButtons::SelectButton(int button)
{
   // save action menu button when entering options menu
   if (m_menu == commandMenuActions && button == 0)
      m_savedActionButton = m_selectedButton;

   m_selectedButton = button;

   UpdateMenu();
}

void IngameCommandButtons::SelectPreviousButton(bool jumpToStart)
{
   // not in actions menu
   if (m_selectedButton == -1 || m_menu == commandMenuActions)
      return;

   int lastButtonSelected = m_selectedButton;
   IngameCommandMenuInfo& menuinfo = g_ingameCommandMenuTable[m_menu];

   if (jumpToStart)
   {
      // jump to start of selectable buttons
      while (m_selectedButton > 0 && menuinfo.selectable[m_selectedButton - 1])
         m_selectedButton--;
   }
   else
   {
      // go to previous button
      if (m_selectedButton > 0 && menuinfo.selectable[m_selectedButton - 1])
         m_selectedButton--;
   }

   // update when necessary
   if (lastButtonSelected != m_selectedButton)
      UpdateMenu();
}

void IngameCommandButtons::SelectNextButton(bool jumpToEnd)
{
   // not in actions menu
   if (m_selectedButton == -1 || m_menu == commandMenuActions)
      return;

   int lastButtonSelected = m_selectedButton;
   IngameCommandMenuInfo& menuinfo = g_ingameCommandMenuTable[m_menu];

   if (jumpToEnd)
   {
      // jump to end of selectable buttons
      while (m_selectedButton < 7 && menuinfo.selectable[m_selectedButton + 1])
         m_selectedButton++;
   }
   else
   {
      // go to next button
      if (m_selectedButton < 7 && menuinfo.selectable[m_selectedButton + 1])
         m_selectedButton++;
   }

   // update when necessary
   if (lastButtonSelected != m_selectedButton)
      UpdateMenu();
}

void IngameCommandButtons::UpdateMenu()
{
   // check if in range
   if (m_menu >= commandMenuMax ||
      m_menu > SDL_TABLESIZE(g_ingameCommandMenuTable))
      return;

   IndexedImage& img = GetImage();
   IngameCommandMenuInfo& menuinfo = g_ingameCommandMenuTable[m_menu];

   // re-set window position
   static unsigned int windowPositions[6 * 2] = { 4,10, 4,10, 4,10, 4,10, 3,10, 3,9 };

   m_windowXPos = windowPositions[menuinfo.base_image * 2];
   m_windowYPos = windowPositions[menuinfo.base_image * 2 + 1];

   m_windowWidth = 35;
   m_windowHeight = 108;
   img.Create(m_windowWidth, m_windowHeight);

   // base image
   img.PasteImage(m_buttonImages[menuinfo.base_image], 0, 0);

   static unsigned int buttonPositions[7] = { 2, 17, 32, 47, 62, 77, 92 };
   static unsigned int actionPositions[6 * 2] = { 4,0, 3,17, 2,37, 2,55, 4,70, 4,89 };

   // paste button images
   if (m_menu == commandMenuActions)
   {
      // special: actions menu
      for (unsigned int i = 0; i < 6; i++)
      {
         unsigned int btn = menuinfo.images[i];

         // selected? draw selected image then
         if (m_selectedButton >= 0 && i == unsigned(m_selectedButton))
            btn++;

         img.PasteImage(m_actionButtonImages[btn], actionPositions[i * 2], actionPositions[i * 2 + 1]);
      }
   }
   else
   {
      // normal menu images
      for (unsigned int i = 0; i < 7; i++)
      {
         unsigned int btn = menuinfo.images[i];
         if (btn == 0)
            continue;

         if (menuinfo.opt_index[i] >= 0)
            btn += m_options[menuinfo.opt_index[i]];

         // selected? draw selected image then
         if (m_selectedButton >= 0 && i == unsigned(m_selectedButton))
            btn++;

         unsigned int xpos = menuinfo.selectable[i] ? 1 : 2;

         // special case: "details" menu, first image
         if (menuinfo.base_image == 5 && i == 0)
            xpos = 0;

         img.PasteImage(m_buttonImages[btn], xpos, buttonPositions[i]);
      }
   }

   // add border
   AddBorder(m_parent.m_backgroundImage.GetImage());

   Update();
}

bool IngameCommandButtons::MouseEvent(bool buttonClicked,
   bool leftButton, bool buttonDown, unsigned int mouseX,
   unsigned int mouseY)
{
   // check if a mouse button is down
   if (!buttonClicked && (SDL_GetMouseState(NULL, NULL) & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) == 0)
      return false; // no button was pressed

   // check if user is currently toggling off a button
   if (m_toggleOff && buttonClicked && !buttonDown)
   {
      m_toggleOff = false;
      return true;
   }

   unsigned int ypos = mouseY - m_windowYPos;

   static unsigned int buttonPositions[8] = { 2, 17, 32, 47, 62, 77, 92, 107 };
   static unsigned int actionPositions[8] = { 0, 17, 35, 54, 70, 90, 107, 107 };

   int lastButtonSelected = m_selectedButton;
   IngameCommandMenuInfo& menuinfo = g_ingameCommandMenuTable[m_menu];

   // find selected button
   for (unsigned int i = 0; i < 7; i++)
   {
      if (menuinfo.selectable[i] && (
         (m_menu == commandMenuActions && ypos >= (actionPositions[i]) && ypos < (actionPositions[i + 1])) ||
         (m_menu != commandMenuActions && ypos >= (buttonPositions[i]) && ypos < (buttonPositions[i + 1]))
         ))
      {
         m_selectedButton = i;
         break;
      }
   }

   // toggle buttons in action menu
   if (m_menu == commandMenuActions && lastButtonSelected == m_selectedButton &&
      buttonClicked && buttonDown)
   {
      m_selectedButton = -1;
      m_toggleOff = true;
   }

   if (lastButtonSelected != m_selectedButton)
   {
      UpdateMenu();
   }

   // store action menu button when entering options menu
   if (buttonClicked && buttonDown && m_menu == commandMenuActions && m_selectedButton == 0)
      m_savedActionButton = lastButtonSelected;

   // do button press when user released button
   if (buttonClicked && !buttonDown && m_selectedButton != -1)
   {
      // press key
      DoButtonAction();
   }

   return true;
}

void IngameCommandButtons::DoButtonAction()
{
   IngameCommandMenu oldMenu = m_menu;
   bool optionHasChanged = false;

   switch (m_menu)
   {
      // action menu
   case commandMenuActions:
      switch (m_selectedButton)
      {
         // button "options"
      case 0:
         m_menu = commandMenuOptions;
         m_parent.SetGameMode(ingameModeOptions);
         break;

         // all other buttons just toggle the game mode
      default:
         // update game mode; note: the code assumes that IngameGameMode
         // values are contiguous
         m_parent.SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
         break;
      }
      break;

      // options menu
   case commandMenuOptions:
      switch (m_selectedButton)
      {
      case 0: // save game
         m_parent.ScheduleAction(ingameActionSaveGame, true);
         m_menu = commandMenuActions;
         m_selectedButton = m_savedActionButton;
         m_parent.SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
         //m_menu = commandMenuSave; // old-style save menu
         //m_selectedButton = 1;
         break;

      case 1: // restore game
         m_parent.ScheduleAction(ingameActionLoadGame, true);
         m_menu = commandMenuActions;
         m_selectedButton = m_savedActionButton;
         m_parent.SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
         //m_menu = commandMenuRestore; // old-style restore menu
         //m_selectedButton = 1;
         break;

      case 2: // music
         m_menu = commandMenuMusic;
         m_selectedButton = m_options[0] + 2;
         optionHasChanged = true;
         break;

      case 3: // sound
         m_menu = commandMenuSound;
         m_selectedButton = m_options[1] + 2;
         optionHasChanged = true;
         break;

      case 4: // detail
         m_menu = commandMenuDetails;
         m_selectedButton = m_options[2] + 2;
         optionHasChanged = true;
         break;

      case 5: // return to game
         m_menu = commandMenuActions;
         m_selectedButton = m_savedActionButton;
         m_parent.SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
         break;

      case 6: // quit game
         m_menu = commandMenuQuit;
         m_selectedButton = 3;
         break;

      default:
         break;
      }
      break;

      // "really quit?" question
   case commandMenuQuit:
      switch (m_selectedButton)
      {
      case 2: // yes
         m_parent.ScheduleAction(ingameActionExit, true);
         break;

      case 3: // no
         m_menu = commandMenuActions; // better to options menu?
         break;

      default:
         break;
      }
      break;

      // music / sound
   case commandMenuMusic:
   case commandMenuSound:
      switch (m_selectedButton)
      {
      case 2:
      case 3:
         m_options[m_menu == commandMenuMusic ? 0 : 1] = (m_selectedButton - 2) & 1;
         optionHasChanged = true;
         break;

      case 4: // done
         m_selectedButton = m_menu == commandMenuMusic ? 2 : 3;
         m_menu = commandMenuOptions;
         break;

      default:
         break;
      }
      break;

   case commandMenuDetails:
      switch (m_selectedButton)
      {
      case 2:// low ... very high
      case 3:
      case 4:
      case 5:
         m_options[2] = (m_selectedButton - 2) & 3;
         optionHasChanged = true;
         break;

      case 6: // done
         m_menu = commandMenuOptions;
         m_selectedButton = 4;
         break;

      default:
         break;
      }
      break;

   case commandMenuSave:
   case commandMenuRestore:
      switch (m_selectedButton)
      {
      case 5: // cancel
         m_selectedButton = m_menu == commandMenuSave ? 0 : 1;
         m_menu = commandMenuOptions;
         break;

      default:
         break;
      }
      break;

   default:
      break;
   }

   // update menu when necessary
   if (oldMenu != m_menu || optionHasChanged)
      UpdateMenu();
}
