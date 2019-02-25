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
/// \file Model3D.cpp
/// \brief 3D models handling
//
#include "common.hpp"
#include "Model3D.hpp"
#include "Model3DBuiltIn.hpp"
#include "GameInterface.hpp"
#include "ResourceManager.hpp"
#include "TextFile.hpp"

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

   // loading %game%/model3d.cfg
   std::string configFilename(settings.GetString(Base::settingGamePrefix));
   configFilename.append("/model3d.cfg");

   UaTrace(" loading config file: %s\n", configFilename.c_str());
   Base::TextFile textFile(game.GetResourceManager().GetResourceFile(configFilename.c_str()));
   Base::ConfigFile::Load(textFile);
}

void Model3DManager::load_value(const char* the_name, const char* the_value)
{
   std::string name(the_name);
   std::string value(the_value);

   // get item id
   Uint16 itemId;
   if (name.find("0x") == 0)
      itemId = strtol(name.c_str() + 2, NULL, 16);
   else
      itemId = strtol(name.c_str(), NULL, 10);

   // get filename/builtin number
   if (value.find("builtin") == 0)
   {
      // we have a builtin graphics
      unsigned int builtInNumber = strtol(value.c_str() + 7, NULL, 16);

      // insert builtin model
      Model3DPtr model = m_allBuiltInModels[builtInNumber];

      m_allModels.insert(
         std::make_pair(itemId, model));
   }
   if (value.find("special") == 0)
   {
      Model3DSpecial* modelSpecial = new Model3DSpecial;
      Model3DPtr model = Model3DPtr(modelSpecial);

      m_allModels.insert(
         std::make_pair(itemId, model));
   }
   else
   {
/* TODO reactivate loading 3D models
      std::string model_path(value);

      // load .wrl model
      Model3dWrl* model = new Model3dWrl;
      SDL_RWops* rwops = NULL;

      UaTrace(" loading model %s\n",model_path.c_str());

      rwops = core->get_filesmgr().GetResourceFile(model_path.c_str());

      // import model
      model->import_wrl(core,rwops,model_path);

      m_allModels.insert(
         std::make_pair<Uint16,Model3DPtr>(itemId,Model3DPtr(model)) );
*/
   }
}

bool Model3DManager::IsModelAvailable(Uint16 itemId) const
{
   return m_allModels.find(itemId) != m_allModels.end();
}

void Model3DManager::Render(const Underworld::Object& object,
   TextureManager& textureManager, Vector3d& base)
{
   Uint16 itemId = object.GetObjectInfo().m_itemID;

   // try to find model object
   std::map<Uint16, Model3DPtr>::iterator iter =
      m_allModels.find(itemId);

   // render
   if (iter != m_allModels.end())
      iter->second->Render(object, textureManager, base);
}

void Model3DManager::GetBoundingTriangles(const Underworld::Object& object,
   Vector3d& base, std::vector<Triangle3dTextured>& allTriangles)
{
   Uint16 itemId = object.GetObjectInfo().m_itemID;

   // try to find model object
   std::map<Uint16, Model3DPtr>::iterator iter =
      m_allModels.find(itemId);

   if (iter != m_allModels.end())
      iter->second->GetBoundingTriangles(object, base, allTriangles);
}
