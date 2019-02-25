//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2019 Underworld Adventures Team
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
/// \file MapViewScreen.hpp
/// \brief level map view
//
#pragma once

#include "Screen.hpp"

/// level map view screen class
class MapViewScreen : public Screen
{
public:
   /// ctor
   MapViewScreen(IGame& gameInterface)
      :Screen(gameInterface)
   {
   }
   /// dtor
   virtual ~MapViewScreen() {}

   // virtual functions from Screen
   virtual void Init();
   //virtual void Destroy() override;
   //virtual void ProcessEvent(SDL_Event& event) override;
   //virtual void Draw() override;
   virtual void Tick() override;
};
