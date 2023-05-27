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
/// \file IngameDragon.hpp
/// \brief ingame dragon control
//
#pragma once

#include "OriginalIngameControl.hpp"

/// \brief dragons
/// left/right dragon
/// images from dragons.gr:
/// image 0     / 18: lower part, left/right
/// image 1     / 19: idle looking head
/// image 2-5   / 20-23: scrolling dragon feet
/// image 6-9   / 24-27: scrolling dragon head
/// image 10-13 / 28-31: dragon looking away
/// image 14-17 / 32-35: dragon tail movement
/// \todo implement animation
class IngameDragon : public OriginalIngameControl
{
public:
   /// ctor
   IngameDragon(OriginalIngameScreen& screen, bool leftDragon);

   /// initializes dragon
   virtual void Init(IGameInstance& game, unsigned int xpos,
      unsigned int ypos) override;

   /// updates the dragon's image
   void UpdateDragon();

   // virtual methods from Window
   virtual bool MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

private:
   /// specifies if the left or right dragon has to be drawn
   bool m_leftDragon;

   /// compass images
   std::vector<IndexedImage> m_dragonImages;
};
