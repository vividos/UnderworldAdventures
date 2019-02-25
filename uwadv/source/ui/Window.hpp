//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2003,2004,2019 Underworld Adventures Team
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
/// \file Window.hpp
/// \brief window base class
//
#pragma once

union SDL_Event;

/// window base class
class Window
{
public:
   /// ctor
   Window();
   /// dtor
   virtual ~Window();

   /// creates window
   virtual void Create(unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height);

   /// destroys window
   virtual void Destroy();

   /// draws window contents
   virtual void Draw();

   /// processes SDL events; returns true when event shouldn't processed further
   virtual bool ProcessEvent(SDL_Event& event);

   /// processes mouse event specific to that window
   virtual void MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY);

   /// called every game tick
   virtual void Tick();

   /// returns window position x coordinate
   unsigned int GetXPos() const { return m_windowXPos; }
   /// returns window position y coordinate
   unsigned int GetYPos() const { return m_windowYPos; }

   /// returns window width
   unsigned int GetWidth() const { return m_windowWidth; }
   /// returns window height
   unsigned int GetHeight() const { return m_windowHeight; }

   /// determines if point is inside window
   bool IsInWindow(unsigned int xpos, unsigned int ypos);

   /// calculates mouse position from event.motion.x/y
   void CalcMousePosition(SDL_Event& event, unsigned int& xpos, unsigned int& ypos);

protected:
   /// window position
   unsigned int m_windowXPos, m_windowYPos;

   /// window size
   unsigned int m_windowWidth, m_windowHeight;
};
