//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file SaveGameScreen.hpp
/// \brief save game screen
//
#pragma once

#include "Screen.hpp"
#include "Savegame.hpp"
#include "ImageQuad.hpp"
#include "FadingHelper.hpp"
#include "Font.hpp"
#include "TextScroll.hpp"
#include "TextEditWindow.hpp"
#include "MouseCursor.hpp"

class SaveGameScreen;

/// button id's
enum SaveGameButtonId
{
   saveGameButtonSave = 0, ///< save button
   saveGameButtonLoad,     ///< load button
   saveGameButtonRefresh,  ///< refresh button
   saveGameButtonExit,     ///< exit button
   saveGameButtonNone      ///< no button
};

/// savegame screen button class
class SaveGameButton : public ImageQuad
{
public:
   /// ctor
   SaveGameButton() {}

   /// initializes button
   void Init(SaveGameScreen* screen, IGame& game,
      unsigned int xpos, unsigned int ypos, const char* text,
      SaveGameButtonId id);

   // virtual methods from Window

   virtual bool ProcessEvent(SDL_Event& event) override;

   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// updates button image according to state
   void UpdateButton(bool buttonStatePressed);

protected:
   // constants

   /// standard button width
   static const unsigned int c_buttonWidth;

   /// pointer to savegame screen
   SaveGameScreen* m_screen;

   /// button id
   SaveGameButtonId m_buttonId;

   /// indicates if one of the mouse buttons is down
   bool m_isLeftButtonDown, m_isRightButtonDown;

   /// button images
   std::vector<IndexedImage> m_buttonImages;
};

/// savegames list class
class SaveGamesList : public ImageQuad
{
public:
   /// ctor
   SaveGamesList()
      :m_selectedSavegameItemIndex(-1)
   {
   }

   /// initializes savegames list
   void Init(SaveGameScreen* screen, IGame& game,
      unsigned int xpos, unsigned int ypos, bool showNewSavegameEntry);

   /// updates list
   void UpdateList();

   /// returns selected savegame index
   int GetSelectedSavegame() const { return m_selectedSavegameItemIndex; }

   // virtual methods from Window

   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// pointer to savegames manager
   Base::SavegamesManager* m_savegamesManager;

   /// pointer to savegame screen
   SaveGameScreen* m_screen;

   /// font for list text
   Font m_normalFont;

   /// first list item to show
   unsigned int m_firstListItemIndex;

   /// index of selected savegame (or -1 when none)
   int m_selectedSavegameItemIndex;

   /// indicates if the "new savegame" entry is shown
   bool m_showNewSavegameEntry;
};

/// save game screen class
class SaveGameScreen : public Screen
{
public:
   /// ctor
   SaveGameScreen(IGame& game, bool calledFromStartMenu,
      bool disableSaveButton);
   /// dtor
   virtual ~SaveGameScreen() {}

   /// called from SaveGameButton when a button is pressed
   void PressButton(SaveGameButtonId id);

   /// updates savegame info with selected savegame
   void UpdateInfo();

   // virtual functions from Screen
   virtual void Init() override;
   virtual void Destroy() override;
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void Tick() override;

protected:
   /// starts asking for a savegame name
   void AskForSavegameDescription();

   /// saves game to disk
   void SaveGameToDisk();

protected:
   // constants

   /// time to fade in/out
   static const double s_fadeTime;


   // buttons

   /// save button
   SaveGameButton m_saveButton;
   /// load button
   SaveGameButton m_loadButton;
   /// refresh button
   SaveGameButton m_refreshButton;
   /// exit button
   SaveGameButton m_exitButton;

   /// button id pressed
   SaveGameButtonId m_pressedButton;

   /// savegames list
   SaveGamesList m_savegamesList;

   /// indicates if screen is called from start menu
   bool m_calledFromStartMenu;

   /// indicates if save button should be invisible
   bool m_disableSaveButton;

   /// background image
   ImageQuad m_backgroundImage;

   /// savegame info area
   ImageQuad m_infoAreaImage;

   /// indicates if preview image is shown
   bool m_showPreview;

   /// savegame preview image
   Texture m_previewImageTexture;

   /// indicates that we're editing the savegame description
   bool m_isEditingDescription;

   /// textedit window for entering savegame description
   TextEditWindow m_textEdit;

   /// font for info area
   Font m_buttonFont;

   /// appearance images
   std::vector<IndexedImage> m_facesImages;

   /// mouse cursor
   MouseCursor m_mouseCursor;

   /// fading helper
   FadingHelper m_fader;

   /// fade in/out state
   unsigned int m_fadeState;
};
