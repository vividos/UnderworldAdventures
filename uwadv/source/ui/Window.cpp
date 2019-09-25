//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file Window.cpp
/// \brief window base class implementation
//
#include "pch.hpp"
#include "Window.hpp"
#include "Screen.hpp"

Window::Window()
   :m_windowXPos(0), m_windowYPos(0), m_windowWidth(0), m_windowHeight(0)
{
}

Window::~Window()
{
}

void Window::Create(unsigned int xpos, unsigned int ypos,
   unsigned int width, unsigned int height)
{
   m_windowXPos = xpos;
   m_windowYPos = ypos;
   m_windowWidth = width;
   m_windowHeight = height;
}

void Window::Destroy()
{
}

void Window::Draw()
{
}

bool Window::ProcessEvent(SDL_Event& event)
{
   return false;
}

/// note: mouseX and mouseY parameters are in virtual screen coordinates,
/// where the screen is a 320x200 pixel screen, with the origin in the upper
/// left.
void Window::MouseEvent(bool buttonClicked, bool leftButton,
   bool buttonDown, unsigned int mouseX, unsigned int mouseY)
{
}

void Window::Tick()
{
}

bool Window::IsInWindow(unsigned int xpos, unsigned int ypos)
{
   return (xpos >= m_windowXPos) && (xpos < m_windowXPos + m_windowWidth) &&
      (ypos >= m_windowYPos) && (ypos < m_windowYPos + m_windowHeight);
}

/// note: the necessary fields in event are only set if it holds an event of
/// type SDL_MOUSEMOTION, SDL_MOUSEBUTTONDOWN or SDL_MOUSEBUTTONUP
void Window::CalcMousePosition(SDL_Event& event, unsigned int& xpos, unsigned int& ypos)
{
   int posx = event.type == SDL_MOUSEMOTION ? event.motion.x : event.button.x;
   int posy = event.type == SDL_MOUSEMOTION ? event.motion.y : event.button.y;

   if (m_screen != NULL)
   {
      m_screen->MapWindowPosition(posx, posy);

      xpos = posx;
      ypos = posy;
   }
   else
   {
      UaAssert(false); // screen should have been set
      xpos = ypos = 0;
   }
}
