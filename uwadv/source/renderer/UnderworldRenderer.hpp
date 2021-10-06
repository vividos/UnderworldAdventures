//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021 Underworld Adventures Team
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
/// \file UnderworldRenderer.hpp
/// \brief renderer for the Underworld object
//
#pragma once

#include "TextureManager.hpp"
#include "Critter.hpp"
#include "Model3D.hpp"

namespace Underworld
{
   class Level;
}

class IGame;
struct RenderOptions;

/// \brief height scale factor
/// This value scales down underworld z coordinates to coordinates in the
/// OpenGL world. It can be used to adjust the heightness of the underworld.
/// It should only appear in OpenGL function calls.
extern const double c_renderHeightScale;

/// Underworld renderer implementation
class UnderworldRenderer
{
public:
   /// ctor
   UnderworldRenderer(IGame& game);

   /// prepares renderer for rendering a new level
   void PrepareLevel(Underworld::Level& level);

   /// called for every game tick
   void Tick(double tickRate);

   /// sets selection mode on or off
   void SetSelectionMode(bool enabled) { m_selectionMode = enabled; }

   /// renders underworld level at given player pos and angles
   void Render(const RenderOptions& renderOptions, const Underworld::Level& level, Vector3d pos,
      double panAngle, double rotateAngle, double fieldOfView);

   /// returns 3d models manager
   Model3DManager& GetModel3DManager() { return m_modelManager; }

   /// calculates object position in 3d world
   static Vector3d CalcObjectPosition(unsigned int x, unsigned int y,
      const Underworld::Object& object);

private:
   /// renders all objects of a tile
   void RenderObjects(const RenderOptions& renderOptions,
      const Vector3d& viewerPos, const Underworld::Level& level,
      unsigned int x, unsigned int y);

   /// renders a single object
   void RenderObject(const RenderOptions& renderOptions,
      const Vector3d& viewerPos,  const Underworld::Level& level,
      const Underworld::Object& object,
      unsigned int x, unsigned int y);

   /// renders a billboarded sprite
   void RenderSprite(Vector3d base, double width, double height,
      bool ignoreUpVector, double u, double v,
      double moveU = 0.0, double moveV = 0.0);

   /// renders decal
   void RenderDecal(const Underworld::Object& object, unsigned int x, unsigned int y);

   /// renders tmap object
   void RenderTmapObject(const Underworld::Object& object, unsigned int x, unsigned int y);

   /// draws a billboarded quad
   void DrawBillboardQuad(Vector3d base,
      double quadwidth, double quadheight,
      double u1, double v1, double u2, double v2);

private:
   /// texture manager
   TextureManager m_textureManager;

   /// critter frames manager
   CritterFramesManager m_critterManager;

   /// 3d models manager
   Model3DManager m_modelManager;

   /// scale factor for textures
   unsigned int m_scaleFactor;

   /// indicates if in selection (picking) mode
   bool m_selectionMode;

   /// billboard right and up vectors
   Vector3d m_billboardRightVector, m_billboardUpVector;
};
