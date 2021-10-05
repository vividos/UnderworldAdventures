//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2020 Underworld Adventures Team
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
/// \file Model3D.cpp
/// \brief 3D models handling
//
#include "pch.hpp"
#include "Model3D.hpp"
#include "Model3DBuiltIn.hpp"
#include "GameInterface.hpp"
#include "ResourceManager.hpp"
#include "TextFile.hpp"
#include "VrmlImporter.hpp"
#include "Model3DVrml.hpp"

bool DecodeBuiltInModels(const char* filename,
   std::vector<Model3DPtr>& allModels, bool dump);

void Model3DManager::Init(IGame& game)
{
   UaTrace("initializing Model3DManager\n");

   Base::Settings& settings = game.GetSettings();

   // loading builtin models
   std::string underworldExeFilename(settings.GetString(Base::settingUnderworldPath));

   if (!settings.GetBool(Base::settingUw1IsUwdemo))
      underworldExeFilename.append("uw.exe");
   else
      underworldExeFilename.append("uwdemo.exe");

   DecodeBuiltInModels(underworldExeFilename.c_str(), m_allBuiltInModels, false);

   LoadModelConfigFile(settings, game.GetResourceManager());
}

void Model3DManager::LoadModelConfigFile(const Base::Settings& settings, const Base::ResourceManager& resourceManager)
{
   std::string configFilename{ settings.GetString(Base::settingGamePrefix) };
   configFilename.append("/model3d.cfg");

   UaTrace(" loading config file: %s\n", configFilename.c_str());

   Base::SDL_RWopsPtr configFilePtr = resourceManager.GetResourceFile(configFilename.c_str());

   if (configFilePtr == nullptr)
   {
      UaTrace(" model 3D config file not available\n");
      return;
   }

   Base::TextFile textFile{ configFilePtr };
   Base::ConfigFile configFile;
   configFile.Load(textFile);

   for (const auto& pair : configFile.GetValueMap())
      AddModel(pair.first, pair.second, resourceManager);
}

void Model3DManager::AddModel(const std::string& name, const std::string& value, const Base::ResourceManager& resourceManager)
{
   Uint16 itemId;
   if (name.find("0x") == 0)
      itemId = strtol(name.c_str() + 2, NULL, 16);
   else
      itemId = strtol(name.c_str(), NULL, 10);

   if (value.find("builtin") == 0)
   {
      unsigned int builtInNumber = strtol(value.c_str() + 7, NULL, 16);

      Model3DPtr model = m_allBuiltInModels[builtInNumber];

      m_allModels.insert(std::make_pair(itemId, model));
   }
   else if (value.find("special") == 0)
   {
      Model3DPtr model = std::make_shared<Model3DSpecial>();

      m_allModels.insert(std::make_pair(itemId, model));
   }
   else
   {
      // load .wrl model
      std::string modelPath{ value };
      UaTrace(" loading model %s\n", modelPath.c_str());

      Base::SDL_RWopsPtr rwops = resourceManager.GetResourceFile(modelPath.c_str());

      Import::VrmlImporter importer{ resourceManager };
      Model3DPtr model = importer.ImportWrl(rwops.get(), modelPath);

      m_allModels.insert(std::make_pair(itemId, model));
   }
}

bool Model3DManager::IsModelAvailable(Uint16 itemId) const
{
   return m_allModels.find(itemId) != m_allModels.end();
}

void Model3DManager::Render(const Vector3d& viewerPos, const Underworld::Object& object,
   TextureManager& textureManager, Vector3d& base)
{
   Uint16 itemId = object.GetObjectInfo().m_itemID;

   std::map<Uint16, Model3DPtr>::iterator iter =
      m_allModels.find(itemId);

   if (iter != m_allModels.end())
      iter->second->Render(viewerPos, object, textureManager, base);
}

void Model3DManager::GetBoundingTriangles(const Underworld::Object& object,
   Vector3d& base, std::vector<Triangle3dTextured>& allTriangles)
{
   Uint16 itemId = object.GetObjectInfo().m_itemID;

   std::map<Uint16, Model3DPtr>::iterator iter =
      m_allModels.find(itemId);

   if (iter != m_allModels.end())
      iter->second->GetBoundingTriangles(object, base, allTriangles);
}
