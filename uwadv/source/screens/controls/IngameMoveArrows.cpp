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
/// \file IngameMoveArrows.cpp
/// \brief ingame move arrows control
//
#include "pch.hpp"
#include "IngameMoveArrows.hpp"
#include "../OriginalIngameScreen.hpp"

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
         m_parent.KeyEvent(false, m_lastPressedKey);
         m_lastPressedKey = Base::keyNone;
      }
   }

   return ret;
}

bool IngameMoveArrows::MouseEvent(bool buttonClicked, bool leftButton,
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
      m_parent.KeyEvent(buttonDown, new_selected_key);

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
            m_parent.KeyEvent(false, m_lastPressedKey);
            m_parent.KeyEvent(true, new_selected_key);

            m_lastPressedKey = new_selected_key;
         }
      }
   }

   return true;
}
