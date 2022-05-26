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
/// \file Model3DManager.hpp
/// \brief 3D model manager
//
#pragma once

#include "Model3D.hpp"
#include "Math.hpp"
#include "Triangle3d.hpp"
#include "TextureManager.hpp"
#include "Object.hpp"
#include <map>
#include <vector>

class IBasicGame;
struct RenderOptions;

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
