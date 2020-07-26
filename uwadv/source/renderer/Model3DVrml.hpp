//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2020 Underworld Adventures Team
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
/// \file Model3DVrml.hpp
/// \brief VRML97 model
//
#pragma once

#include "Model3D.hpp"

namespace Import
{
   class VrmlImporter;
}

/// A model loaded from a .wrl file
class Model3DVrml : public Model3D
{
public:
   /// ctor
   Model3DVrml() {}
   /// dtor
   virtual ~Model3DVrml() {}

   /// renders model
   virtual void Render(const Underworld::Object& object, TextureManager& textureManager,
      Vector3d& base) override;

   /// returns bounding triangles for collision detection
   virtual void GetBoundingTriangles(const Underworld::Object& object,
      Vector3d& base, std::vector<Triangle3dTextured>& allTriangles) override;

private:
   friend Import::VrmlImporter;

   /// vertex coordinates
   std::vector<Vector3d> m_coords;

   /// texture coordinates
   std::vector<Vector2d> m_texCoords;

   /// coordinates indices
   std::vector<unsigned int> m_coordIndex;

   /// texture coordinates indices
   std::vector<unsigned int> m_texCoordIndex;

   /// model texture
   Texture m_texture;
};
