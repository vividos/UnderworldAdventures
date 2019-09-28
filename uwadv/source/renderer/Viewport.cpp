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
/// \file Viewport.cpp
/// \brief Renderer viewport
//
#include "pch.hpp"
#include "Viewport.hpp"
#include "RenderWindow.hpp"
#include <gl/GLU.h>

/// near plane distance from the camera
const double Viewport::c_nearDistance = 0.05;

/// Sets the viewport to use in 3D window; for 2D the whole surface size is
/// set as viewport. The viewport only has to be set once and is used in
/// SetupCamera3D().
/// \param xpos x position of viewport, in image coordinates (320x200 max.)
/// \param ypos y position of viewport
/// \param width width of viewport
/// \param height height of viewport
void Viewport::SetViewport3D(unsigned int xpos, unsigned int ypos,
   unsigned int width, unsigned int height)
{
   int windowWidth = 0, windowHeight = 0;
   m_window.GetWindowSize(windowWidth, windowHeight);

   // calculate viewport for given window
   xpos = unsigned((windowWidth / 320.0) * double(xpos));
   ypos = unsigned((windowHeight / 200.0) * double(ypos));
   width = unsigned((windowWidth / 320.0) * double(width));
   height = unsigned((windowHeight / 200.0) * double(height));

   m_viewport[0] = xpos;
   m_viewport[1] = windowHeight - ypos - height;
   m_viewport[2] = width;
   m_viewport[3] = height;
}

/// Sets up camera for 2D user interface rendering. All triangles (e.g. quads)
/// should be rendered with z coordinate = 0.
void Viewport::SetupCamera2D()
{
   // set viewport
   int windowWidth = 0, windowHeight = 0;
   m_window.GetWindowSize(windowWidth, windowHeight);
   glViewport(0, 0, windowWidth, windowHeight);

   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0, 320, 0, 200);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

/// Sets up camera for 3D scene rendering.
/// \param fieldOfView field of view angle
/// \param farDistance distance from camera to far plane
/// \param setPickMatrix indicates if pick matrix should also be set
/// \param pickPosX X position for picking
/// \param pickPosY Y position for picking
void Viewport::SetupCamera3D(double fieldOfView, double farDistance,
   bool setPickMatrix, int pickPosX, int pickPosY)
{
   glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   if (setPickMatrix)
   {
      int windowWidth = 0, windowHeight = 0;
      m_window.GetWindowSize(windowWidth, windowHeight);

      gluPickMatrix(pickPosX, windowHeight - pickPosY, 5.0, 5.0, m_viewport);
   }

   double aspectRatio = double(m_viewport[2]) / m_viewport[3];
   gluPerspective(fieldOfView, aspectRatio, c_nearDistance, farDistance);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}
