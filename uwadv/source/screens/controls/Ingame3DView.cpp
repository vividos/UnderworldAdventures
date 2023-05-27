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
/// \file Ingame3DView.cpp
/// \brief ingame 3D view control
//
#include "pch.hpp"
#include "Ingame3DView.hpp"
#include "../OriginalIngameScreen.hpp"
#include "GameLogic.hpp"
#include "Renderer.hpp"

void Ingame3DView::Init(IGameInstance& game, unsigned int xpos,
   unsigned int ypos)
{
   ImageWindow::Init(xpos, ypos,
      224 - 54 + 1, 131 - 20 + 1);

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

            // calculate real screen coordinates and set new pos
            m_parent.UnmapWindowPosition(xpos, ypos);
            m_parent.SetMousePos(xpos, ypos);

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
            m_parent.SetCursor(0, false);
         }
      }
   }

   return ImageWindow::ProcessEvent(event);
}

bool Ingame3DView::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
   // we only get this call when we're inside the window
   m_in3dView = true;

   // calculate relative mouse pos in window
   double relx = double(mouseX - m_windowXPos) / m_windowWidth;
   double rely = double(mouseY - m_windowYPos) / m_windowHeight;

   Physics::PlayerPhysicsObject& playerPhysics = GetPlayerPhysicsObject();

   // when pressing left mouse button, start mouse move mode
   if (buttonClicked && leftButton)
   {
      // mouse move is started on pressing mouse button
      m_mouseMove = (SDL_GetMouseState(NULL, NULL) & (SDL_BUTTON_LMASK)) != 0;

      if (!m_mouseMove)
      {
         // disable all modes (when possible)
         if (!m_parent.GetMoveState(ingameMoveWalkForward) &&
            !m_parent.GetMoveState(ingameMoveRunForward) &&
            !m_parent.GetMoveState(ingameMoveWalkBackwards))
            playerPhysics.SetMovementMode(0, Physics::moveWalk);

         if (!m_parent.GetMoveState(ingameMoveTurnLeft) &&
            !m_parent.GetMoveState(ingameMoveTurnRight))
            playerPhysics.SetMovementMode(0, Physics::moveRotate);

         playerPhysics.SetMovementMode(0, Physics::moveSlide);
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
               walk = -0.4 * (rely - 0.75) / 0.25; new_cursor_image = 2;
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
         m_parent.SetCursor(new_cursor_image, false);
   }

   // mouse move mode?
   if (m_mouseMove)
   {
      // disable all modes (when not active through keyboard movement)
      // and update movement modes and factors
      if (!m_parent.GetMoveState(ingameMoveWalkForward) &&
         !m_parent.GetMoveState(ingameMoveRunForward) &&
         !m_parent.GetMoveState(ingameMoveWalkBackwards))
      {
         playerPhysics.SetMovementMode(0, Physics::moveWalk);

         if (walk < 10.0)
         {
            playerPhysics.SetMovementMode(Physics::moveWalk);
            playerPhysics.SetMovementFactor(Physics::moveWalk, walk);
         }
      }

      if (!m_parent.GetMoveState(ingameMoveTurnLeft) &&
         !m_parent.GetMoveState(ingameMoveTurnRight))
      {
         playerPhysics.SetMovementMode(0, Physics::moveRotate);

         if (rotate < 10.0)
         {
            playerPhysics.SetMovementMode(Physics::moveRotate);
            playerPhysics.SetMovementFactor(Physics::moveRotate, rotate);
         }
      }

      {
         playerPhysics.SetMovementMode(0, Physics::moveSlide);

         if (slide < 10.0)
         {
            playerPhysics.SetMovementMode(Physics::moveSlide);
            playerPhysics.SetMovementFactor(Physics::moveSlide, slide);
         }
      }
   }

   // check combat start
   if (m_parent.GetGameMode() == ingameModeFight && buttonClicked && !leftButton)
   {
      if (buttonDown)
      {
         // start combat weapon drawback
         m_gameInstance.GetGameLogic().
            UserAction(userActionCombatDrawBack,
               rely < 0.33 ? 0 : rely < 0.67 ? 1 : 2);
      }
      else
      {
         // end combat weapon drawback
         m_gameInstance.GetGameLogic().
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

      Renderer& renderer = m_parent.GetGameInterface().GetRenderer();
      if (!renderer.SelectPick(m_gameInstance.GetUnderworld(),
         x, y, tilex, tiley, is_object, id))
         return true;

      switch (m_parent.GetGameMode())
      {
         // "look" or default action
      case ingameModeDefault:
      case ingameModeLook:
         if (is_object)
         {
            if (id != 0)
               m_gameInstance.GetGameLogic().
               UserAction(userActionLookObject, id);
         }
         else
         {
            // user clicked on a texture
            UaTrace("looking at wall/ceiling, tile=%02x/%02x, id=%04x\n",
               tilex, tiley, id);

            m_gameInstance.GetGameLogic().
               UserAction(userActionLookWall, id);
         }
         break;

         // "use" action
      case ingameModeUse:
         if (is_object)
         {
            m_gameInstance.GetGameLogic().
               UserAction(userActionUseObject, id);
         }
         else
         {
            m_gameInstance.GetGameLogic().
               UserAction(userActionUseWall, id);
         }
         break;

         // "get" action
      case ingameModeGet:
         if (is_object)
         {
            m_gameInstance.GetGameLogic().
               UserAction(userActionGetObject, id);
         }
         break;

         // "talk" action
      case ingameModeTalk:
         if (is_object)
         {
            m_gameInstance.GetGameLogic().
               UserAction(userActionTalkObject, id);
         }
         break;

      default:
         break;
      }
   }

   return true;
}
