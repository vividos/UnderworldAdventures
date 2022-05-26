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
/// \file Ingame3DView.hpp
/// \brief ingame 3D view control
//
#pragma once

#include "OriginalIngameControl.hpp"

/// nonvisible 3d view window area
class Ingame3DView : public OriginalIngameControl
{
public:
   /// ctor
   Ingame3DView(OriginalIngameScreen& screen)
      :OriginalIngameControl(screen)
   {
   }

   /// initializes 3d view window
   virtual void Init(IGame& game, unsigned int xpos,
      unsigned int ypos) override;

   /// returns if 3d view is in mouse move mode
   bool GetMouseMove() { return m_mouseMove; }

   // virtual methods from Window
   virtual void Draw() override;
   virtual bool ProcessEvent(SDL_Event& event) override;
   virtual bool MouseEvent(bool buttonClicked, bool leftButton,
      bool buttonDown, unsigned int mouseX, unsigned int mouseY) override;

private:
   /// is true when the mouse cursor is in 3D view
   bool m_in3dView;

   /// indicates that mouse movement is currently on
   bool m_mouseMove;
};
