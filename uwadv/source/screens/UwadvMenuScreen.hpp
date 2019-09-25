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
/// \file UwadvMenuScreen.hpp
/// \brief underworld adventures menu
//
#pragma once

#include "Screen.hpp"
#include "Texture.hpp"

/// \brief uwadv menu screen class
/// A screen for selecting which game should be started.
/// Currently this screen just shows a "loading ..." image and immediately
/// loads the game with prefix "uw1". In future this screen could be used to
/// select if "uw1", "uw2" or any custom games should be started.
class UwadvMenuScreen : public Screen
{
public:
   /// ctor
   UwadvMenuScreen(IGame& game)
      :Screen(game)
   {
   }
   /// dtor
   virtual ~UwadvMenuScreen() {}

   // virtual functions from Screen

   virtual void Init() override;
   virtual void Draw() override;
   virtual void Tick() override;

protected:
   /// "loading..." textures
   Texture m_texture;

   /// indicates if texture was already rendered
   bool m_isRendered;
};
