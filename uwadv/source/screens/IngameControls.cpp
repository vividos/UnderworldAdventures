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
/// \file IngameControls.cpp
/// \brief ingame screen controls
//
#include "pch.hpp"
#include "IngameControls.hpp"
#include "OriginalIngameScreen.hpp"
#include "Underworld.hpp"
#include "GameLogic.hpp"
#include "Renderer.hpp"
#include "ImageManager.hpp"
#include "physics/PhysicsModel.hpp"

PlayerPhysicsObject& OriginalIngameControl::GetPlayerPhysicsObject()
{
   UaAssert(m_parent != NULL);
   return m_parent->GetPlayerPhysicsObject();
}

void IngameCompass::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   // init image
   GetImage().Create(52, 26);

   ImageQuad::Init(game, xpos, ypos);

   // load compass images
   {
      std::vector<IndexedImage> temp_compass;
      game.GetImageManager().LoadList(temp_compass, "compass");

      Palette256Ptr pal0 = game.GetImageManager().GetPalette(0);

      m_compassImages.resize(16);

      static unsigned int compass_tip_coords[16 * 2] =
      {
         24, 0, 16, 2,  8, 3,  4, 6,
          0,10,  0,14,  4,16, 12,19,
         24,21, 32,19, 44,16, 48,14,
         48,10, 44, 6, 40, 3, 32, 2,
      };

      // create compass images and add needle tips and right/bottom borders
      for (unsigned int n = 0; n < 16; n++)
      {
         IndexedImage& img = m_compassImages[n];
         img.Create(52, 26);
         img.SetPalette(pal0);

         img.PasteImage(temp_compass[n & 3], 0, 0);

         // compass needle
         img.PasteImage(temp_compass[n + 4],
            compass_tip_coords[n * 2], compass_tip_coords[n * 2 + 1]);
      }
   }

   m_currentCompassImageIndex = 16;
}

void IngameCompass::Draw()
{
   // check if we have to reupload the image

   // calculate current angle and images
   Underworld::Player& player = m_parent->GetGameInterface().GetUnderworld().
      GetPlayer();

   double angle = fmod(-player.GetRotateAngle() + 90.0 + 360.0, 360.0);
   unsigned int compassimg = unsigned((angle + 11.25) / 22.5) & 15;

   // prepare texture
   if (m_currentCompassImageIndex != compassimg)
   {
      // reupload compass texture
      m_currentCompassImageIndex = compassimg;

      unsigned int dest = m_hasBorder ? 1 : 0;
      GetImage().PasteImage(m_compassImages[compassimg], dest, dest);

      Update();
   }

   ImageQuad::Draw();
}

void IngameCompass::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
      m_parent->GetGameInterface().GetGameLogic().UserAction(userActionClickedCompass);
}

void IngameRuneshelf::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   GetImage().Create(46, 16);

   // load images 232..255; A-Z without X and Z
   game.GetImageManager().LoadList(m_runestoneImages, "objects", 232, 256);

   ImageQuad::Init(game, xpos, ypos);
}

/// Updates the runeshelf image from runeshelf content.
void IngameRuneshelf::UpdateRuneshelf()
{
   IndexedImage& img_shelf = GetImage();
   img_shelf.Clear(0);

   Underworld::Runeshelf& runeshelf = m_parent->GetGameInterface().GetUnderworld().
      GetPlayer().GetRuneshelf();

   size_t max = runeshelf.GetNumRunes() % 3;
   for (size_t i = 0; i < max; i++)
   {
      Underworld::RuneType rune = static_cast<Underworld::RuneType>(runeshelf.GetRune(i) % 24);

      // paste appropriate rune image
      IndexedImage& img_rune = m_runestoneImages[rune];

      unsigned int dest = m_hasBorder ? 1 : 0;
      img_shelf.PasteRect(img_rune, 0, 0, 14, 14,
         i * 15 + dest, dest, true);
   }

   Update();
}

void IngameRuneshelf::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
      m_parent->GetGameInterface().GetGameLogic().UserAction(userActionClickedRuneshelf);
}

void IngameSpellArea::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   GetImage().Create(51, 18);

   // load images 232..255; A-Z without X and Z
   game.GetImageManager().LoadList(m_spellImages, "spells");

   ImageQuad::Init(game, xpos, ypos);
}

/// Updates the active spell area image.
/// \todo actually get spells from Underworld
void IngameSpellArea::UpdateSpellArea()
{
   IndexedImage& img_area = GetImage();
   img_area.Clear(0);

   unsigned int spell[3] = { 0, 0, 0 };

   for (unsigned int i = 0; i < 3; i++)
   {
      if (spell[i] == 0)
         continue;

      // paste appropriate spell image
      IndexedImage& img_spell = m_spellImages[(spell[i] - 1) % 21];

      unsigned int dest = m_hasBorder ? 1 : 0;
      img_area.PasteRect(img_spell, 0, 0, 16, 18,
         i * 17 + dest, dest, true);
   }

   Update();
}

void IngameSpellArea::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
      m_parent->GetGameInterface().GetGameLogic().UserAction(userActionClickedActiveSpell);
}

void IngameFlask::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   GetImage().Create(24, 33);

   // load flask images
   {
      std::vector<IndexedImage> temp_flasks;
      game.GetImageManager().LoadList(temp_flasks, "flasks");

      unsigned int maximg = m_isVitalityFlask ? 28 : 14;
      m_flaskImages.resize(maximg);

      // paste together all flask fill heights
      for (unsigned int i = 0; i < maximg; i += 14)
      {
         IndexedImage base_img = temp_flasks[75];

         static unsigned int flask_pos[13] =
         { 26, 24, 22, 20, 18, 16, 15, 14, 13, 11, 9, 7, 5 };

         unsigned int offset = m_isVitalityFlask ? (i == 0 ? 0 : 50) : 25;

         // image 0 is the empty flask
         m_flaskImages[i] = base_img;

         // generate all images
         for (unsigned int j = 0; j < 13; j++)
         {
            base_img.PasteImage(temp_flasks[offset + j], 0, flask_pos[j]);
            m_flaskImages[i + j + 1] = base_img;
         }
      }
   }

   ImageQuad::Init(game, xpos, ypos);

   m_lastFlaskImageIndex = 14 * 2;
   m_isPoisoned = false;
}

void IngameFlask::Draw()
{
   Underworld::Player& player = m_parent->GetGameInterface().GetUnderworld().
      GetPlayer();
   m_isPoisoned = player.GetAttribute(Underworld::attrPoisoned) != 0;

   unsigned int curval = player.GetAttribute(
      m_isVitalityFlask ? Underworld::attrVitality : Underworld::attrMana);
   unsigned int maxval = player.GetAttribute(
      m_isVitalityFlask ? Underworld::attrMaxVitality : Underworld::attrMaxMana);
   unsigned int curimg = maxval == 0 ? 0 : unsigned((curval*13.0) / maxval);

   // check if flask image has to be update
   unsigned int new_image = m_isVitalityFlask && m_isPoisoned ? curimg + 14 : curimg;

   if (m_lastFlaskImageIndex != new_image)
   {
      m_lastFlaskImageIndex = new_image;
      UpdateFlask();
   }

   ImageQuad::Draw();
}

void IngameFlask::UpdateFlask()
{
   unsigned int dest = m_hasBorder ? 1 : 0;
   GetImage().PasteImage(m_flaskImages[m_lastFlaskImageIndex], dest, dest);
   Update();
}

void IngameFlask::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
      m_parent->GetGameInterface().GetGameLogic().UserAction(
         m_isVitalityFlask ? userActionClickedVitalityFlash : userActionClickedManaFlask);
}

void IngameGargoyleEyes::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   GetImage().Create(20, 3);

   game.GetImageManager().LoadList(m_eyesImages, "eyes");

   ImageQuad::Init(game, xpos, ypos);
}

/// \todo update eyes from data in Underworld
void IngameGargoyleEyes::UpdateEyes()
{
   unsigned int new_image = 0;

   unsigned int dest = m_hasBorder ? 1 : 0;
   GetImage().PasteImage(m_eyesImages[new_image], dest, dest);
   Update();
}

void IngameGargoyleEyes::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
      m_parent->GetGameInterface().GetGameLogic().UserAction(userActionClickedGargoyle);
}

IngameDragon::IngameDragon(bool drg_left)
   :m_leftDragon(drg_left)
{
}

void IngameDragon::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   // load images
   game.GetImageManager().LoadList(m_dragonImages, "dragons",
      m_leftDragon ? 0 : 18, m_leftDragon ? 18 : 36);

   // prepare image
   IndexedImage& img = GetImage();

   img.Create(37, 104);
   img.SetPalette(game.GetImageManager().GetPalette(0));
   img.Clear(0);


   ImageQuad::Init(game, xpos, ypos);


   // dragon part that's never moving
   img.PasteImage(m_dragonImages[0], m_leftDragon ? 0 : 24, 69);

   UpdateDragon();
}

void IngameDragon::UpdateDragon()
{
   IndexedImage& img = GetImage();

   unsigned int head_frame = 1;
   img.PasteImage(m_dragonImages[head_frame], 0, 80);

   unsigned int tail_frame = 14;
   img.PasteImage(m_dragonImages[tail_frame], m_leftDragon ? 4 : 20, 0);

   Update();
}

void IngameDragon::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   if (buttonClicked && !buttonDown)
      m_parent->GetGameInterface().GetGameLogic().UserAction(userActionClickedDragons);
}

void Ingame3DView::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   ImageQuad::Init(game, xpos, ypos);

   m_windowWidth = 224 - 54 + 1;
   m_windowHeight = 131 - 20 + 1;

   m_mouseMove = false;
   m_in3dView = false;
}

void Ingame3DView::Draw()
{
   // do nothing, we're invisible
}

bool Ingame3DView::ProcessEvent(SDL_Event& event)
{
   if (event.type == SDL_MOUSEMOTION)
   {
      // check if user leaves the 3d view
      unsigned int xpos = 0, ypos = 0;
      CalcMousePosition(event, xpos, ypos);
      if (m_in3dView && !IsInWindow(xpos, ypos))
      {
         if (m_mouseMove)
         {
            // limit mouse position
            if (xpos < m_windowXPos) xpos = m_windowXPos;
            if (xpos >= m_windowXPos + m_windowWidth) xpos = m_windowXPos + m_windowWidth - 1;
            if (ypos < m_windowYPos) ypos = m_windowYPos;
            if (ypos >= m_windowYPos + m_windowHeight) ypos = m_windowYPos + m_windowHeight - 1;

            // calculate real screen coordinates
            int windowWidth = 320, windowHeight = 200;
            // TODO check if needed
            //SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

            {
               xpos = unsigned((xpos / 320.0) * windowWidth);
               ypos = unsigned((ypos / 200.0) * windowHeight);

               // TODO replacement? SDL_WarpMouse(xpos,ypos);
            }

            // update event mouse pos values
            event.motion.xrel += Sint16(xpos) - event.motion.x;
            event.motion.yrel += Sint16(ypos) - event.motion.y;
            event.motion.x = xpos;
            event.motion.y = ypos;
         }
         else
         {
            // user left the window
            m_in3dView = false;
            m_parent->SetCursor(0, false);
         }
      }
   }

   return ImageQuad::ProcessEvent(event);
}

void Ingame3DView::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   // we only get this call when we're inside the window
   m_in3dView = true;

   // calculate relative mouse pos in window
   double relx = double(mouseX - m_windowXPos) / m_windowWidth;
   double rely = double(mouseY - m_windowYPos) / m_windowHeight;

   PlayerPhysicsObject& playerPhysics = GetPlayerPhysicsObject();

   // when pressing left mouse button, start mouse move mode
   if (buttonClicked && leftButton)
   {
      // mouse move is started on pressing mouse button
      m_mouseMove = (SDL_GetMouseState(NULL, NULL) & (SDL_BUTTON_LMASK)) != 0;

      if (!m_mouseMove)
      {
         // disable all modes (when possible)
         if (!m_parent->GetMoveState(ingameMoveWalkForward) &&
            !m_parent->GetMoveState(ingameMoveRunForward) &&
            !m_parent->GetMoveState(ingameMoveWalkBackwards))
            playerPhysics.SetMovementMode(0, moveWalk);

         if (!m_parent->GetMoveState(ingameMoveTurnLeft) &&
            !m_parent->GetMoveState(ingameMoveTurnRight))
            playerPhysics.SetMovementMode(0, moveRotate);

         playerPhysics.SetMovementMode(0, moveSlide);
      }
   }

   // determine new cursor image
   double slide, rotate, walk;
   {
      slide = rotate = walk = 10.0;

      int new_cursor_image = -1;

      if (rely >= 0.75)
      {
         // lower part of screen
         if (relx < 0.33) { slide = -1.0; new_cursor_image = 3; }
         else
            if (relx >= 0.66) { slide = 1.0; new_cursor_image = 4; }
            else
            {
               walk = -0.4*(rely - 0.75) / 0.25; new_cursor_image = 2;
            }
      }
      else
         if (rely >= 0.6)
         {
            // middle part
            if (relx < 0.33) { rotate = (0.33 - relx) / 0.33; new_cursor_image = 5; }
            else
               if (relx >= 0.66) { rotate = -(relx - 0.66) / 0.33; new_cursor_image = 6; }
               else
                  new_cursor_image = 0;
         }
         else
         {
            // upper part
            if (relx < 0.33) { rotate = (0.33 - relx) / 0.33; new_cursor_image = 7; }
            else
               if (relx >= 0.66) { rotate = -(relx - 0.66) / 0.33; new_cursor_image = 8; }
               else
                  new_cursor_image = 1;

            // walking speed increases in range [0.6; 0.2] only
            walk = (0.6 - rely) / 0.4;
            if (walk > 1.0) walk = 1.0;
         }

      if (new_cursor_image != -1)
         m_parent->SetCursor(new_cursor_image, false);
   }

   // mouse move mode?
   if (m_mouseMove)
   {
      // disable all modes (when not active through keyboard movement)
      // and update movement modes and factors
      if (!m_parent->GetMoveState(ingameMoveWalkForward) &&
         !m_parent->GetMoveState(ingameMoveRunForward) &&
         !m_parent->GetMoveState(ingameMoveWalkBackwards))
      {
         playerPhysics.SetMovementMode(0, moveWalk);

         if (walk < 10.0)
         {
            playerPhysics.SetMovementMode(moveWalk);
            playerPhysics.SetMovementFactor(moveWalk, walk);
         }
      }

      if (!m_parent->GetMoveState(ingameMoveTurnLeft) &&
         !m_parent->GetMoveState(ingameMoveTurnRight))
      {
         playerPhysics.SetMovementMode(0, moveRotate);

         if (rotate < 10.0)
         {
            playerPhysics.SetMovementMode(moveRotate);
            playerPhysics.SetMovementFactor(moveRotate, rotate);
         }
      }

      {
         playerPhysics.SetMovementMode(0, moveSlide);

         if (slide < 10.0)
         {
            playerPhysics.SetMovementMode(moveSlide);
            playerPhysics.SetMovementFactor(moveSlide, slide);
         }
      }
   }

   // check combat start
   if (m_parent->GetGameMode() == ingameModeFight && buttonClicked && !leftButton)
   {
      if (buttonDown)
      {
         // start combat weapon drawback
         m_parent->GetGameInterface().GetGameLogic().
            UserAction(userActionCombatDrawBack,
               rely < 0.33 ? 0 : rely < 0.67 ? 1 : 2);
      }
      else
      {
         // end combat weapon drawback
         m_parent->GetGameInterface().GetGameLogic().
            UserAction(userActionCombatRelease, 0);
      }
   }

   // check right mouse button down
   if (buttonClicked && buttonDown && !leftButton)
   {
      int x, y;
      SDL_GetMouseState(&x, &y);

      unsigned int tilex = 0, tiley = 0, id = 0;
      bool is_object = true;

      Renderer& renderer = m_parent->GetGameInterface().GetRenderer();
      renderer.SelectPick(m_parent->GetGameInterface().GetUnderworld(),
         x, y, tilex, tiley, is_object, id);

      switch (m_parent->GetGameMode())
      {
         // "look" or default action
      case ingameModeDefault:
      case ingameModeLook:
         if (is_object)
         {
            if (id != 0)
               m_parent->GetGameInterface().GetGameLogic().
               UserAction(userActionLookObject, id);
         }
         else
         {
            // user clicked on a texture
            UaTrace("looking at wall/ceiling, tile=%02x/%02x, id=%04x\n",
               tilex, tiley, id);

            m_parent->GetGameInterface().GetGameLogic().
               UserAction(userActionLookWall, id);
         }
         break;

         // "use" action
      case ingameModeUse:
         if (is_object)
         {
            m_parent->GetGameInterface().GetGameLogic().
               UserAction(userActionUseObject, id);
         }
         else
         {
            m_parent->GetGameInterface().GetGameLogic().
               UserAction(userActionUseWall, id);
         }
         break;

         // "get" action
      case ingameModeGet:
         if (is_object)
         {
            m_parent->GetGameInterface().GetGameLogic().
               UserAction(userActionGetObject, id);
         }
         break;

         // "talk" action
      case ingameModeTalk:
         if (is_object)
         {
            m_parent->GetGameInterface().GetGameLogic().
               UserAction(userActionTalkObject, id);
         }
         break;

      default:
         break;
      }
   }
}

void IngamePowerGem::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   m_attackMode = false;

   game.GetImageManager().LoadList(m_powerGemImages, "power");

   GetImage().Create(31, 12);
   GetImage().SetPalette(game.GetImageManager().GetPalette(0));

   ImageQuad::Init(game, xpos, ypos);
}

void IngamePowerGem::UpdateGem()
{
   unsigned int frame = 0;

   unsigned int power =
      m_parent->GetGameInterface().GetGameLogic().GetAttackPower();

   if (power > 0)
   {
      if (power >= 100)
         frame = (m_maxPowerFrameIndex & 3) + 10;
      else
      {
         frame = (unsigned(power / 100.0*8.0) % 9) + 1;
         m_maxPowerFrameIndex = 0;
      }
   }

   unsigned int dest = m_hasBorder ? 1 : 0;
   GetImage().PasteImage(m_powerGemImages[frame], dest, dest);

   Update();
}

void IngamePowerGem::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
}

void IngamePowerGem::Tick()
{
   /*
      // check if we have to update
      unsigned int power =
         m_parent->GetGameInterface().GetUnderworld().GetAttackPower();
      if (power >= 100)
      {
         if (++m_maxPowerFrameIndex>3)
            m_maxPowerFrameIndex = 0;
         UpdateGem();
      }
   */
}

void IngameMoveArrows::Init(IGame& game, unsigned int xpos,
   unsigned int ypos)
{
   ImageQuad::Init(game, xpos, ypos);

   m_windowWidth = 63;
   m_windowHeight = 14;

   m_lastPressedKey = Base::keyNone;
}

void IngameMoveArrows::Draw()
{
   // do nothing
}

bool IngameMoveArrows::ProcessEvent(SDL_Event& event)
{
   bool ret = ImageQuad::ProcessEvent(event);

   if (event.type == SDL_MOUSEMOTION)
   {
      unsigned int xpos = 0, ypos = 0;
      CalcMousePosition(event, xpos, ypos);

      // leaving window while move arrow is pressed?
      if (m_lastPressedKey != Base::keyNone && !IsInWindow(xpos, ypos))
      {
         // unpress (release) key
         m_parent->KeyEvent(false, m_lastPressedKey);
         m_lastPressedKey = Base::keyNone;
      }
   }

   return ret;
}

void IngameMoveArrows::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   mouseX -= m_windowXPos;

   Base::KeyType new_selected_key = Base::keyNone;

   // check which button the mouse is over
   if (mouseX < 19)
      new_selected_key = Base::keyTurnLeft;
   else
      if (mouseX < 44)
         new_selected_key = Base::keyWalkForward;
      else
         new_selected_key = Base::keyTurnRight;

   // button activity?
   if (buttonClicked)
   {
      // simulate key press (or release)
      m_parent->KeyEvent(buttonDown, new_selected_key);

      if (!buttonDown)
         m_lastPressedKey = Base::keyNone;
      else
         m_lastPressedKey = new_selected_key;
   }
   else
   {
      if (m_lastPressedKey != Base::keyNone)
      {
         // mouse was moved; check if arrow changed
         if (new_selected_key != m_lastPressedKey)
         {
            // change pressed button
            m_parent->KeyEvent(false, m_lastPressedKey);
            m_parent->KeyEvent(true, new_selected_key);

            m_lastPressedKey = new_selected_key;
         }
      }
   }
}

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
   AddBorder(m_parent->m_backgroundImage.GetImage());

   Update();
}

void IngameCommandButtons::MouseEvent(bool buttonClicked,
   bool leftButton, bool buttonDown, unsigned int mouseX,
   unsigned int mouseY)
{
   // check if a mouse button is down
   if (!buttonClicked && (SDL_GetMouseState(NULL, NULL) & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) == 0)
      return; // no button was pressed

   // check if user is currently toggling off a button
   if (m_toggleOff && buttonClicked && !buttonDown)
   {
      m_toggleOff = false;
      return;
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
         m_parent->SetGameMode(ingameModeOptions);
         break;

         // all other buttons just toggle the game mode
      default:
         // update game mode; note: the code assumes that IngameGameMode
         // values are contiguous
         m_parent->SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
         break;
      }
      break;

      // options menu
   case commandMenuOptions:
      switch (m_selectedButton)
      {
      case 0: // save game
         m_parent->ScheduleAction(ingameActionSaveGame, true);
         m_menu = commandMenuActions;
         m_selectedButton = m_savedActionButton;
         m_parent->SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
         //m_menu = commandMenuSave; // old-style save menu
         //m_selectedButton = 1;
         break;

      case 1: // restore game
         m_parent->ScheduleAction(ingameActionLoadGame, true);
         m_menu = commandMenuActions;
         m_selectedButton = m_savedActionButton;
         m_parent->SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
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
         m_parent->SetGameMode(IngameGameMode(m_selectedButton - 1 + ingameModeTalk));
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
         m_parent->ScheduleAction(ingameActionExit, true);
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
