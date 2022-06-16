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
/// \file Screen.hpp
/// \brief user interface screens base classes
//
#pragma once

#include "Window.hpp"
#include "GameInterface.hpp"
#include "Keymap.hpp"

/// \brief screen base class
/// base class for user interface screens, e.g. main game screen, conversation
/// screen, map screen etc.
class Screen : public Window
{
public:
   /// ctor
   Screen(IGame& gameInterface);
   /// dtor
   virtual ~Screen();

   /// inits the screen
   virtual void Init();

   /// destroys window
   virtual void Destroy() override;

   /// draws window contents
   virtual void Draw() override;

   /// processes SDL events; returns true when event shouldn't processed further
   virtual bool ProcessEvent(SDL_Event& event) override;

   /// processes key event
   virtual void KeyEvent(bool keyDown, Base::KeyType key);

   /// called every game tick
   virtual void Tick() override;

   /// registers a window as subwindow
   void RegisterWindow(Window* window);

   /// unregisters a subwindow
   void UnregisterWindow(Window* window);

   /// registers a keymap
   void RegisterKeymap(Base::Keymap* keymap);

   /// converts window position to 320x200 screen coordinates
   void MapWindowPosition(int& xpos, int& ypos) const;

   /// converts 320x200 screen coordinate to real window position
   void UnmapWindowPosition(unsigned int& xpos, unsigned int& ypos) const;

   /// sets new mouse position; in real window coordinates
   void SetMousePos(unsigned int xpos, unsigned int ypos);

   /// returns game tick rate, useful for animations
   double GetTickRate() const;

protected:
   /// list of all subwindows controlled by the screen
   std::vector<Window*> m_subWindows;

   /// game interface
   IGame& m_game;

   /// screen keymap
   Base::Keymap* m_screenKeymap;
};
