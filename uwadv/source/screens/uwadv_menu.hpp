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
/// \file uwadv_menu.hpp
/// \brief underworld adventures menu
//
#pragma once

#include "screen.hpp"
#include "texture.hpp"

/// \brief uwadv menu screen class
/// A screen for selecting which game should be started.
/// Currently this screen just shows a "loading ..." image and immediately
/// loads the game with prefix "uw1". In future this screen could be used to
/// select if "uw1", "uw2" or any custom games should be started.
class ua_uwadv_menu_screen : public ua_screen
{
public:
   /// ctor
   ua_uwadv_menu_screen(IGame& game) :ua_screen(game) {}
   /// dtor
   virtual ~ua_uwadv_menu_screen() {}

   // virtual functions from ua_screen

   virtual void init() override;
   virtual void draw() override;
   virtual void tick() override;

protected:
   /// "loading..." textures
   ua_texture tex;

   /// indicates if texture was already rendered
   bool rendered;
};
