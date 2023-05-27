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
/// \file IngameCompass.hpp
/// \brief ingame compass control
//
#pragma once

#include "OriginalIngameControl.hpp"

/// compass control
class IngameCompass : public OriginalIngameControl
{
public:
   /// ctor
   IngameCompass(OriginalIngameScreen& screen)
      :OriginalIngameControl(screen)
   {
   }

   /// initializes compass
   virtual void Init(IGameInstance& game, unsigned int xpos,
      unsigned int ypos) override;

   /// draws compass
   virtual void Draw() override;

   // virtual methods from Window
   virtual bool MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

private:
   /// current compass image
   unsigned int m_currentCompassImageIndex;

   /// compass images
   std::vector<IndexedImage> m_compassImages;
};
