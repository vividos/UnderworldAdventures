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
/// \file Viewport.hpp
/// \brief Renderer viewport
//
#pragma once

class RenderWindow;

/// \brief render viewport
/// \details the viewport can be configured for 2D and 3D rendering. The 2D
/// viewport uses a 320x200 coordinate system, with 0/0 on the upper left of
/// the screen. The 3D viewport uses a rectangle inside this 320x200 sized
/// window to render, since the underworld games used this approach to not
/// render to the whole screen.
class Viewport
{
public:
   /// ctor
   Viewport(RenderWindow& window)
      :m_window(window)
   {
   }

   /// sets viewport to render 3d scene
   void SetViewport3D(unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height);

   /// sets up OpenGL for 2D user interface rendering using an ortho cam
   void SetupCamera2D();

   /// sets up camera for 3d scene rendering
   void SetupCamera3D(double fieldOfView = 90.0, double farDistance = 16.0,
      bool setPickMatrix = false, int pickPosX = 0, int pickPosY = 0);

private:
   /// render window
   RenderWindow& m_window;

   /// 3D viewport to use
   GLint m_viewport[4];

   /// near plane distance
   static const double c_nearDistance;
};
