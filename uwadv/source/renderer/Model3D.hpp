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
/// \file Model3D.hpp
/// \brief 3D models and model manager
//
#pragma once

#include "Math.hpp"
#include "Triangle3d.hpp"
#include "Texture.hpp"
#include "Object.hpp"
#include <map>
#include <vector>
#include <memory>

class IBasicGame;
struct RenderOptions;

/// \brief 3d model base class
class Model3D
{
public:
   /// ctor
   Model3D() {}

   /// dtor
   virtual ~Model3D() {}

   /// returns unique model name
   const char* GetModelName() { return ""; }

   /// renders model
   virtual void Render(const RenderOptions& renderOptions,
      const Vector3d& viewerPos, const Underworld::Object& object,
      TextureManager& textureManager, Vector3d& base)
   {
      UNUSED(renderOptions);
      UNUSED(viewerPos);
      UNUSED(object);
      UNUSED(textureManager);
      UNUSED(base);
   }

   /// returns bounding triangles for collision detection
   virtual void GetBoundingTriangles(const Underworld::Object& object,
      Vector3d& base, std::vector<Triangle3dTextured>& allTriangles)
   {
      UNUSED(object);
      UNUSED(base);
      UNUSED(allTriangles);
   }
};

/// smart pointer to model object
typedef std::shared_ptr<Model3D> Model3DPtr;

/// 3d model manager
class Model3DManager
{
public:
   /// ctor
   Model3DManager() {}

   /// init manager
   void Init(IBasicGame& game);

   /// returns if a 3d model for a certain item_id is available
   bool IsModelAvailable(Uint16 itemId) const;

   /// renders a model
   void Render(const RenderOptions& renderOptions,
      const Vector3d& viewerPos, const Underworld::Object& object,
      TextureManager& textureManager, Vector3d& base);

   /// returns bounding triangles for collision detection with given item_id
   void GetBoundingTriangles(const Underworld::Object& obj, Vector3d& base,
      std::vector<Triangle3dTextured>& allTriangles);

private:
   /// loads model3d.cfg file
   void LoadModelConfigFile(const Base::Settings& settings, const Base::ResourceManager& resourceManager);

   /// adds a new model
   void AddModel(const std::string& name, const std::string& value, const Base::ResourceManager& resourceManager);

private:
   /// map with all 3d model objects
   std::map<Uint16, Model3DPtr> m_allModels;

   /// all builtin models from the exe
   std::vector<Model3DPtr> m_allBuiltInModels;
};
