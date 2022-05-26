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
/// \file IngameSpellArea.hpp
/// \brief ingame spell area control
//
#pragma once

#include "OriginalIngameControl.hpp"

/// active spell area control
class IngameSpellArea : public OriginalIngameControl
{
public:
   /// dtor
   virtual ~IngameSpellArea() {}

   /// initializes active spells area
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// updates spell area
   void UpdateSpellArea();

   // virtual methods from Window
   virtual bool MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

private:
   /// all runestones
   std::vector<IndexedImage> m_spellImages;
};
