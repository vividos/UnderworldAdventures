//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2004,2019,2021 Underworld Adventures Team
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
/// \file Model3DBuiltIn.hpp
/// \brief builtin models
//
#pragma once

#include "Model3D.hpp"

/// builtin model class
class Model3DBuiltIn : public Model3D
{
public:
   /// ctor
   Model3DBuiltIn() {}
   /// dtor
   virtual ~Model3DBuiltIn() {}

   /// renders model
   virtual void Render(const Vector3d& viewerPos, const Underworld::Object& object, TextureManager& textureManager,
      Vector3d& base) override;

   /// returns bounding triangles for collision detection
   virtual void GetBoundingTriangles(const Underworld::Object& object,
      Vector3d& base, std::vector<Triangle3dTextured>& allTriangles) override;

private:
   /// draws a wireframe extents bounding box around the 3D model
   void DrawExtentsBox(const Vector3d& base);

private:
   /// model extents
   Vector3d m_extents;

   /// all triangles
   std::vector<Triangle3dTextured> m_triangles;

   // friend decoding function
   friend bool DecodeBuiltInModels(const char* filename,
      std::vector<Model3DPtr>& allModels, bool dump);
};

/// special model class, like bridges and doors
class Model3DSpecial : public Model3D
{
public:
   /// ctor
   Model3DSpecial() {}
   /// dtor
   virtual ~Model3DSpecial() {}

   /// renders model
   virtual void Render(const Vector3d& viewerPos, const Underworld::Object& object, TextureManager& textureManager,
      Vector3d& base) override;

   /// returns bounding triangles for collision detection
   virtual void GetBoundingTriangles(const Underworld::Object& object,
      Vector3d& base, std::vector<Triangle3dTextured>& allTriangles) override;
};
