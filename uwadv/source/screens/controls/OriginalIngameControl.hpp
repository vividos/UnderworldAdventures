//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file OriginalIngameControl.hpp
/// \brief original ingame control base class
//
#pragma once

#include "ImageWindow.hpp"
#include "physics/PlayerPhysicsObject.hpp"

class OriginalIngameScreen;

/// ingame screen control base class
class OriginalIngameControl : public ImageWindow
{
public:
   /// ctor
   OriginalIngameControl(OriginalIngameScreen& screen);

   /// returns player physics tracking object
   Physics::PlayerPhysicsObject& GetPlayerPhysicsObject();

   /// initializes ingame control
   virtual void Init(IBasicGame&, unsigned int xpos, unsigned int ypos) = 0;

protected:
   /// parent screen
   OriginalIngameScreen& m_parent;

   /// reference to the game's instance
   IBasicGame& m_gameInstance;
};
