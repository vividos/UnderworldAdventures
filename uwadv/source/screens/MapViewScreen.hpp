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
/// \file MapViewScreen.hpp
/// \brief level map view
//
#pragma once

#include "ImageScreen.hpp"
#include "MouseCursor.hpp"
#include "ClickArea.hpp"
#include "TextEditWindow.hpp"

namespace Underworld
{
   struct MapNote;
}

/// level map view screen class
class MapViewScreen : public ImageScreen
{
public:
   /// ctor
   MapViewScreen(IGame& gameInterface);
   /// dtor
   virtual ~MapViewScreen() {}

   // virtual functions from Screen
   virtual void Init() override;
   virtual bool ProcessEvent(SDL_Event& event) override;

private:
   /// mode of map editing
   enum MapEditMode
   {
      mapEditModeNone,     ///< no editing yet
      mapEditModeEditNote, ///< editing a map note
      mapEditModeErase,    ///< erasing mode
   };

   /// sets new map edit mode
   void SetMapEditMode(MapEditMode mapEditMode);

   /// changes the displayed level up or down
   void UpDownLevel(bool up);

   /// called when one of the gem areas has been clicked
   void OnGemClicked(unsigned int gemIndex);

   /// displays level map with given index
   void DisplayLevelMap(size_t levelIndex);

   /// starts editing a new map note
   void StartEditing(unsigned int xpos, unsigned int ypos);

   /// adds new map note
   void AddNewMapNote(unsigned int xpos, unsigned int ypos, std::string noteText);

   /// selects a map note by drawing a border around it
   void SelectMapNote(unsigned int xpos, unsigned int ypos);

   /// creates an image with a selected map note text
   void CreateMapNoteSelectionImage(const Underworld::MapNote& mapNoteToSelect,
      IndexedImage& selectedMapNoteImage);

   /// erases map note at given position
   void EraseMapNote(unsigned int xpos, unsigned int ypos);

   /// removes map note selection image from screen
   void RemoveMapNoteSelectionImage();

   /// finds a map note at given position; returns end iterator when none was found
   std::vector<Underworld::MapNote>::const_iterator
      FindMapNote(unsigned int xpos, unsigned int ypos);

private:
   /// currently displayed level
   size_t m_displayedLevel;

   /// current map edit mode
   MapEditMode m_mapEditMode = mapEditModeNone;

   /// mouse cursor
   MouseCursor m_mouseCursor;

   /// up button click area
   ClickArea m_upButton;

   /// down button click area
   ClickArea m_downButton;

   /// erase button click area
   ClickArea m_eraseButton;

   /// close button click area
   ClickArea m_closeButton;

   /// the uw2 gem click areas
   std::vector<std::shared_ptr<ClickArea>> m_uw2GemClickAreas;

   /// text edit window
   std::shared_ptr<TextEditWindow> m_textEditWindow;

   /// cursor X position when starting editing
   unsigned int m_editStartCursorPosX = 0;

   /// font for selected map notes
   Font m_fontSelectedMapNote;

   /// image quad containing the currently selected map note, or nullptr when
   /// none is selected
   std::shared_ptr<ImageQuad> m_selectedMapNote;

   /// index of currently selected map note
   std::vector<Underworld::MapNote>::difference_type m_selectedMapNotesIndex = -1;
};
