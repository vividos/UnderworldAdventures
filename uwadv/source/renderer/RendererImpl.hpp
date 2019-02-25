//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file RendererImpl.hpp
/// \brief renderer implementation class
//
#pragma once

#include "Math.hpp"
#include "Level.hpp"
#include "Critter.hpp"
#include "Model3D.hpp"
#include "Quadtree.hpp"
#include "GeometryProvider.hpp"

/// renderer implementation
class RendererImpl
{
public:
   /// ctor
   RendererImpl();

   /// renders underworld level at given player pos and angles
   void Render(const Underworld::Level& level, Vector3d pos,
      double panAngle, double rotateAngle, double fieldOfView);

   /// returns texture manager
   TextureManager& GetTextureManager() { return m_textureManager; }

   /// returns critter frames manager
   CritterFramesManager& GetCritterFramesManager() { return m_critterManager; }

   /// returns 3d models manager
   Model3DManager& GetModel3DManager() { return m_modelManager; }

   /// renders the objects of a tile
   void RenderObjects(const Underworld::Level& level, unsigned int x, unsigned int y);

   /// sets selection mode on or off
   void SetSelectionMode(bool enabled) { m_selectionMode = enabled; }

   /// calculates object position in 3d world
   Vector3d CalcObjectPosition(unsigned int x, unsigned int y,
      const Underworld::Object& object) const;

protected:
   /// renders a single object
   void RenderObject(const Underworld::Level& level, const Underworld::Object& object,
      unsigned int x, unsigned int y);

   /// renders a billboarded sprite
   void RenderSprite(Vector3d base, double width, double height,
      bool ignoreUpVector, double u, double v,
      double moveU = 0.0, double moveV = 0.0);

   /*
      /// renders decal
      void RenderDecal(const Underworld::Object& obj, unsigned int x, unsigned int y);

      /// renders tmap object
      void RenderTmapObject(const Underworld::Object& obj, unsigned int x, unsigned int y);

      /// draws a billboarded quad
      void DrawBillboardQuad(Vector3d base,
         double quadwidth, double quadheight,
         double u1,double v1,double u2,double v2);
   */
protected:
   /// texture manager
   TextureManager m_textureManager;

   /// critter frames manager
   CritterFramesManager m_critterManager;

   /// 3d models manager
   Model3DManager m_modelManager;

   /// indicates if in selection (picking) mode
   bool m_selectionMode;

   /// billboard right and up vectors
   Vector3d m_billboardRightVector, m_billboardUpVector;
};

/// level tile renderer class
class LevelTilemapRenderer : public IQuadtreeCallback
{
public:
   /// ctor
   LevelTilemapRenderer(const Underworld::Level& level, RendererImpl& rendererImpl)
      :m_level(level), m_rendererImpl(rendererImpl), m_geometryProvider(level)
   {
   }

   /// callback function to render tile
   virtual void OnVisibleTile(unsigned int xpos, unsigned int ypos);

protected:
   /// ref to level
   const Underworld::Level& m_level;

   /// geometry provider for level
   GeometryProvider m_geometryProvider;

   /// ref back to renderer implementation
   RendererImpl& m_rendererImpl;
};
