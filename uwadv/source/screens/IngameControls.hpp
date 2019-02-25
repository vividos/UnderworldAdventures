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
/// \file IngameControls.hpp
/// \brief ingame screen controls
//
#pragma once

#include "ImageQuad.hpp"
#include "Keymap.hpp"
#include "physics/PlayerPhysicsObject.hpp"

class OriginalIngameScreen;

/// ingame screen control base class
class OriginalIngameControl : public ImageQuad
{
public:
   /// sets parent window
   void SetParent(OriginalIngameScreen* parent) { m_parent = parent; }

protected:
   /// parent screen
   OriginalIngameScreen* m_parent;
};

/// compass control
class IngameCompass : public OriginalIngameControl
{
public:
   /// initializes compass
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// draws compass
   virtual void Draw() override;

   // virtual methods from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// current compass image
   unsigned int m_currentCompassImageIndex;

   /// compass images
   std::vector<IndexedImage> m_compassImages;
};


/// runeshelf control
class IngameRuneshelf : public OriginalIngameControl
{
public:
   /// initializes runeshelf
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// updates runeshelf
   void UpdateRuneshelf();

   // virtual methods from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// all runestones
   std::vector<IndexedImage> m_runestoneImages;
};


/// active spell area control
class IngameSpellArea : public OriginalIngameControl
{
public:
   /// dtor
   virtual ~IngameSpellArea() {}

   /// initializes active spells area
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// updates spell area
   void UpdateSpellArea();

   // virtual methods from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// all runestones
   std::vector<IndexedImage> m_spellImages;
};


/// vitality / mana flask
/// \todo implement flask fluid bubbling (remaining images in flasks.gr)
class IngameFlask : public OriginalIngameControl
{
public:
   /// ctor
   IngameFlask(bool isVitalityFlask)
      :m_isVitalityFlask(isVitalityFlask)
   {
   }

   /// initializes flask
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// draws compass
   virtual void Draw() override;

   /// updates flask image
   void UpdateFlask();

   // virtual methods from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// indicates if showing a vitality or mana flask
   bool m_isVitalityFlask;

   /// indicates if player is poisoned (only when vitality flask)
   bool m_isPoisoned;

   /// indicates last flask image
   unsigned int m_lastFlaskImageIndex;

   /// flask images
   std::vector<IndexedImage> m_flaskImages;
};


/// gargoyle eyes
class IngameGargoyleEyes : public OriginalIngameControl
{
public:
   /// initializes flask
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// updates eyes image
   void UpdateEyes();

   // virtual methods from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// eyes images
   std::vector<IndexedImage> m_eyesImages;
};


/// \brief dragons
/// left/right dragon
/// images from dragons.gr:
/// image 0     / 18: lower part, left/right
/// image 1     / 19: idle looking head
/// image 2-5   / 20-23: scrolling dragon feet
/// image 6-9   / 24-27: scrolling dragon head
/// image 10-13 / 28-31: dragon looking away
/// image 14-17 / 32-35: dragon tail movement
/// \todo implement animation
class IngameDragon : public OriginalIngameControl
{
public:
   /// ctor
   IngameDragon(bool leftDragon);

   /// initializes dragon
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// updates the dragon's image
   void UpdateDragon();

   // virtual methods from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// specifies if the left or right dragon has to be drawn
   bool m_leftDragon;

   /// compass images
   std::vector<IndexedImage> m_dragonImages;
};


/// nonvisible 3d view window area
class Ingame3DView : public OriginalIngameControl
{
public:
   /// initializes 3d view window
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// returns if 3d view is in mouse move mode
   bool GetMouseMove() { return m_mouseMove; }

   // virtual methods from Window
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// is true when the mouse cursor is in 3D view
   bool m_in3dView;

   /// indicates that mouse movement is currently on
   bool m_mouseMove;

   std::unique_ptr<PlayerPhysicsObject> m_playerPhysics;
};


/// power gem
class IngamePowerGem : public OriginalIngameControl
{
public:
   /// initializes powergem
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// sets attack mode, e.g. for keyboard combat
   void set_attack_mode(bool attackMode) { m_attackMode = attackMode; }

   /// updates gem image
   void UpdateGem();

   // virtual methods from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;
   virtual void Tick() override;

protected:
   /// indicates if in attack mode, i.e. with drawn weapon
   bool m_attackMode;

   /// frame used when having maximum power
   unsigned int m_maxPowerFrameIndex;

   /// powergame images
   std::vector<IndexedImage> m_powerGemImages;
};


/// move arrows (below the compass)
class IngameMoveArrows : public OriginalIngameControl
{
public:
   /// initializes move arrows window
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   // virtual methods from Window
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

protected:
   /// last (virtually) pressed key
   Base::KeyType m_lastPressedKey;
};


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
   /// initializes command buttons
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   // virtual functions from Window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

   /// selects (but doesn't press) specified button
   void SelectButton(int button = -1);

   /// select previous button (if any)
   void SelectPreviousButton(bool jumpToStart = false);

   /// select next button (if any)
   void SelectNextButton(bool jumpToEnd = false);

   /// performs button action
   void DoButtonAction();

protected:
   /// updates menu image
   void UpdateMenu();

protected:
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
