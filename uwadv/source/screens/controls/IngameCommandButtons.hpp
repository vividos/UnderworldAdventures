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
/// \file IngameCommandButtons.hpp
/// \brief ingame command buttons
//
#pragma once

#include "OriginalIngameControl.hpp"
//#include "Keymap.hpp"
//#include "physics/PlayerPhysicsObject.hpp"

/// menu currently shown by IngameCommandButtons
enum IngameCommandMenu
{
   commandMenuActions = 0,  ///< game actions menu (special)
   commandMenuOptions = 1,  ///< options menu
   commandMenuQuit = 2,     ///< quit menu
   commandMenuMusic = 3,    ///< music menu
   commandMenuSound = 4,    ///< sound menu
   commandMenuDetails = 5,  ///< details menu
   commandMenuSave = 6,     ///< save menu; not used in uwadv
   commandMenuRestore = 7,  ///< restore menu; not used in uwadv
   commandMenuMax
};

/// command buttons
/// images from optbtns.gr: <pre>
/// image 0: game actions, not selected, at 4/10, size 35/108
/// image 1: options menu, at 4/10, size 35/108
/// image 2: save game menu, at 4/10
/// image 3: quit game menu, at 4/10
/// image 4: music menu, at 3/10
/// image 5: detail level menu, at 3/9
/// image 6/7: save game button, not selected/selected
/// image 8/9: restore game
/// image 10/11: music
/// image 12/13: sound
/// image 14/15: detail
/// image 16/17: return to game
/// image 18/19: quit game
/// image 20/21: on
/// image 22/23: off
/// image 24/25: cancel
/// image 26/27: done
/// image 28/29: done
/// image 30/31 .. 36/37: I, II, III, IV
/// image 38/39 .. 44/45: low, medium, high, very high
/// image 46: "restore game:"
/// image 47/48: music is on/off.
/// image 49/50: sound is on/off.
/// image 51: "turn music:"
/// image 52: "turn sound:"
/// image 53..56: "detail level is low/medium/high/very high"
/// image 57/58: yes
/// image 59/60: no
///
/// images from lfti.gr:
/// image 0/1: options, not selected/selected
/// image 2/3: talk
/// image 4/5: get
/// image 6/7: look
/// image 8/9: fight
/// image 10/11: use
/// </pre>
class IngameCommandButtons : public OriginalIngameControl
{
public:
   /// ctor
   IngameCommandButtons(OriginalIngameScreen& screen)
      :OriginalIngameControl(screen)
   {
   }

   /// initializes command buttons
   virtual void Init(IGameInstance& game, unsigned int xpos,
      unsigned int ypos) override;

   // virtual functions from Window
   virtual bool MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

   /// selects (but doesn't press) specified button
   void SelectButton(int button = -1);

   /// select previous button (if any)
   void SelectPreviousButton(bool jumpToStart = false);

   /// select next button (if any)
   void SelectNextButton(bool jumpToEnd = false);

   /// performs button action
   void DoButtonAction();

private:
   /// updates menu image
   void UpdateMenu();

private:
   /// button images
   std::vector<IndexedImage> m_buttonImages;

   /// action buttons
   std::vector<IndexedImage> m_actionButtonImages;

   /// current menu
   IngameCommandMenu m_menu;

   /// \brief options
   /// the options are as following
   /// 0: music
   /// 1: sound
   /// 2: detail
   unsigned int m_options[3];

   /// currently selected button
   signed int m_selectedButton;

   /// indicates if user is toggling off an action type
   bool m_toggleOff;

   /// action button stored when in main menu
   int m_savedActionButton;
};
