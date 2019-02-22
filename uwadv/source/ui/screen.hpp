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
/// \file screen.hpp
/// \brief user interface screens base classes
//
#pragma once

#include "window.hpp"
#include "game_interface.hpp"
#include "keymap.hpp"

/// \brief screen base class
/// base class for user interface screens, e.g. main game screen, conversation
/// screen, map screen etc.
class ua_screen : public ua_window
{
public:
   /// ctor
   ua_screen(IGame& game_interface);
   /// dtor
   virtual ~ua_screen();

   /// inits the screen
   virtual void init();

   /// destroys window
   virtual void destroy() override;

   /// draws window contents
   virtual void draw() override;

   /// processes SDL events; returns true when event shouldn't processed further
   virtual bool process_event(SDL_Event& event) override;

   /// processes key event
   virtual void key_event(bool key_down, Base::EKeyType key);

   /// called every game tick
   virtual void tick() override;

   /// registers a window as subwindow
   void register_window(ua_window* window);

   /// registers a keymap
   void register_keymap(Base::Keymap* keymap);

protected:
   /// list of all subwindows controlled by the screen
   std::vector<ua_window*> subwindows;

   /// game interface
   IGame& game;

   /// screen keymap
   Base::Keymap* scr_keymap;
};
