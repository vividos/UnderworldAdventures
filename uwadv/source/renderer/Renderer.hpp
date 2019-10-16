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
/// \file Renderer.hpp
/// \brief underworld renderer
//
#pragma once

#include "Math.hpp"
#include "Triangle3d.hpp"
#include "Texture.hpp"
#include "Settings.hpp"
#include "RenderOptions.hpp"

namespace Underworld
{
   class Underworld;
   class Level;
   class Object;
}

class Viewport;
class UnderworldRenderer;
class CritterFramesManager;
class Model3DManager;

/// underworld renderer
class Renderer
{
public:
   /// ctor
   Renderer();
   /// dtor
   ~Renderer();

   /// deleted copy ctor
   Renderer(const Renderer&) = delete;
   /// deleted assignment operator
   Renderer& operator=(const Renderer&) = delete;

   /// sets viewport, once available
   void SetViewport(Viewport* viewport)
   {
      m_viewport = viewport;
   }

   /// initializes renderer
   void InitGame(IGame& game);

   /// output some OpenGL diagnostics
   static void PrintOpenGLDiagnostics();

   /// cleans up renderer
   void Done();

   /// returns current render options
   RenderOptions& GetRenderOptions() { return m_renderOptions; }

   /// sets up camera for 2d user interface rendering
   void SetupForUserInterface();

   /// sets up camera for 3d scene rendering
   void SetupFor3D(const Vector3d& viewOffset, double fieldOfView = 90.0,
      double farDistance = 16.0);

   /// renders current view of the underworld
   void RenderUnderworld(const Underworld::Underworld& underworld);

   /// does selection/picking
   void SelectPick(const Underworld::Underworld& underworld,
      unsigned int xpos, unsigned int ypos,
      unsigned int& tileX, unsigned int& tileY,
      bool& isObject, unsigned int& id);

   /// prepares renderer for given level (e.g. when changing levels)
   void PrepareLevel(Underworld::Level& level);

   /// does renderer-specific tick processing
   void Tick(double tickRate);

   /// returns 3d model bounding triangles if a 3d model exists
   void GetModel3DBoundingTriangles(unsigned int x, unsigned int y,
      const Underworld::Object& object,
      std::vector<Triangle3dTextured>& allTriangles);

private:
   /// current render options
   RenderOptions m_renderOptions;

   /// viewport
   Viewport* m_viewport;

   /// Underworld renderer implementation
   UnderworldRenderer* m_rendererImpl;

   /// camera view offset
   Vector3d m_viewOffset;

   /// field of view in degrees
   double m_fieldOfView;

   /// distance of far plane
   double m_farDistance;
};
