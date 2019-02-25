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
/// \file Screen.cpp
/// \brief screen base implementation
//
#include "common.hpp"
#include "Screen.hpp"

Screen::Screen(IGame& gameInterface)
   :m_game(gameInterface), m_screenKeymap(NULL)
{
}

Screen::~Screen()
{
}

void Screen::Init()
{
   Window::Create(0, 0, 320, 200);
}

void Screen::Destroy()
{
   // destroy all m_subWindows
   unsigned int max = m_subWindows.size();
   for (unsigned int i = 0; i < max; i++)
      m_subWindows[i]->Destroy();
}

void Screen::Draw()
{
   // draw all m_subWindows
   unsigned int max = m_subWindows.size();
   for (unsigned int i = 0; i < max; i++)
      m_subWindows[i]->Draw();
}

bool Screen::ProcessEvent(SDL_Event& event)
{
   // send event to all m_subWindows
   {
      unsigned int max = m_subWindows.size();
      for (unsigned int i = 0; i < max; i++)
      {
         if (m_subWindows[i]->ProcessEvent(event))
            break; // no further processing
      }
   }

   // check if a mouse event occured
   if (event.type == SDL_MOUSEMOTION ||
      event.type == SDL_MOUSEBUTTONDOWN ||
      event.type == SDL_MOUSEBUTTONUP)
   {
      // get coordinates
      unsigned int xpos = 0, ypos = 0;
      CalcMousePosition(event, xpos, ypos);

      // first, send mouse event to main screen window
      bool leftButton = event.type != SDL_MOUSEMOTION &&
         event.button.button == SDL_BUTTON_LEFT;

      MouseEvent(event.type != SDL_MOUSEMOTION,
         leftButton,
         event.type == SDL_MOUSEBUTTONDOWN,
         xpos, ypos);

      // send event to m_subWindows that are in that area
      unsigned int max = m_subWindows.size();
      for (unsigned int i = 0; i < max; i++)
      {
         Window& wnd = *m_subWindows[i];

         // mouse in area?
         if (wnd.IsInWindow(xpos, ypos))
         {
            wnd.MouseEvent(event.type != SDL_MOUSEMOTION,
               leftButton,
               event.type == SDL_MOUSEBUTTONDOWN,
               xpos, ypos);
         }
      }
   }

   // key event
   if (m_screenKeymap != NULL && (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP))
   {
      // mask out numlock mod key
      Uint16 mod = event.key.keysym.mod & ~KMOD_NUM;

      // be sure to set left and right keys for alt and ctrl
      if ((mod & KMOD_ALT) != 0) mod |= KMOD_ALT;
      if ((mod & KMOD_CTRL) != 0) mod |= KMOD_CTRL;

      Base::KeyType key = m_screenKeymap->FindKey(event.key.keysym.sym, (SDL_Keymod)mod);

      // process key
      if (key != Base::keyNone)
         KeyEvent(event.type == SDL_KEYDOWN, key);
   }

   return false;
}

/// note that SDL_events must be passed to Screen::ProcessEvent() to
/// let key messages to be processed and sent to KeyEvent()
void Screen::KeyEvent(bool keyDown, Base::KeyType key)
{
}

void Screen::Tick()
{
   // send tick to all m_subWindows
   unsigned int max = m_subWindows.size();
   for (unsigned int i = 0; i < max; i++)
      m_subWindows[i]->Tick();
}

/// note that for all m_subWindows the draw() and destroy() functions are
/// called
void Screen::RegisterWindow(Window* window)
{
   m_subWindows.push_back(window);
}

void Screen::RegisterKeymap(Base::Keymap* keymap)
{
   m_screenKeymap = keymap;
}
