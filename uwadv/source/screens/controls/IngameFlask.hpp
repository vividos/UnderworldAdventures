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
/// \file IngameFlask.hpp
/// \brief ingame flask control
//
#pragma once

#include "OriginalIngameControl.hpp"

/// vitality / mana flask
/// \todo implement flask fluid bubbling (remaining images in flasks.gr)
class IngameFlask : public OriginalIngameControl
{
public:
   /// ctor
   IngameFlask(OriginalIngameScreen& screen, bool isVitalityFlask)
      :OriginalIngameControl(screen),
      m_isVitalityFlask(isVitalityFlask),
      m_isPoisoned(false),
      m_lastFlaskImageIndex(0)
   {
   }

   /// initializes flask
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// draws compass
   virtual void Draw() override;

   /// updates flask image
   void UpdateFlask();

   // virtual methods from Window
   virtual bool MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

private:
   /// indicates if showing a vitality or mana flask
   bool m_isVitalityFlask;

   /// indicates if player is poisoned (only when vitality flask)
   bool m_isPoisoned;

   /// indicates last flask image
   unsigned int m_lastFlaskImageIndex;

   /// flask images
   std::vector<IndexedImage> m_flaskImages;
};
