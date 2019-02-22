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
/// \file models.cpp
/// \brief 3d models handling
//
#include "common.hpp"
#include "models.hpp"
#include "model_builtin.hpp"
#include "game_interface.hpp"
#include "resourcemanager.hpp"
#include "textfile.hpp"

bool ua_model_decode_builtins(const char* filename,
   std::vector<ua_model3d_ptr>& allmodels, bool dump);

void ua_model3d_manager::init(IGame& game)
{
   UaTrace("initializing model3d manager\n");

   Base::Settings& settings = game.get_settings();

   // loading builtin models
   std::string uwexe_filename(settings.GetString(Base::settingUnderworldPath));

   if (!settings.GetBool(Base::settingUw1IsUwdemo))
      uwexe_filename.append("uw.exe");
   else
      uwexe_filename.append("uwdemo.exe");

   ua_model_decode_builtins(uwexe_filename.c_str(), allbuiltins, false);

   // loading %game%/model3d.cfg
   std::string cfgfile_name(settings.GetString(Base::settingGamePrefix));
   cfgfile_name.append("/model3d.cfg");

   UaTrace(" loading config file: %s\n", cfgfile_name.c_str());
   Base::TextFile textFile(game.GetResourceManager().GetResourceFile(cfgfile_name.c_str()));
   Base::ConfigFile::Load(textFile);
}

void ua_model3d_manager::load_value(const char* the_name, const char* the_value)
{
   std::string name(the_name);
   std::string value(the_value);

   // get item id
   Uint16 item_id;
   if (name.find("0x") == 0)
      item_id = strtol(name.c_str() + 2, NULL, 16);
   else
      item_id = strtol(name.c_str(), NULL, 10);

   // get filename/builtin number
   if (value.find("builtin") == 0)
   {
      // we have a builtin graphics
      unsigned int builtin_nr = strtol(value.c_str() + 7, NULL, 16);

      // insert builtin model
      ua_model3d_ptr model_ptr = allbuiltins[builtin_nr];

      allmodels.insert(
         std::make_pair(item_id, model_ptr));
   }
   if (value.find("special") == 0)
   {
      ua_model3d_special* model = new ua_model3d_special;
      ua_model3d_ptr model_ptr = ua_model3d_ptr(model);

      allmodels.insert(
         std::make_pair(item_id, model_ptr));
   }
   else
   {
/*
      std::string model_path(value);

      // load .wrl model
      ua_model3d_wrl* model = new ua_model3d_wrl;
      SDL_RWops* rwops = NULL;

      UaTrace(" loading model %s\n",model_path.c_str());

      rwops = core->get_filesmgr().GetResourceFile(model_path.c_str());

      // import model
      model->import_wrl(core,rwops,model_path);

      allmodels.insert(
         std::make_pair<Uint16,ua_model3d_ptr>(item_id,ua_model3d_ptr(model)) );
*/
   }
}

bool ua_model3d_manager::model_avail(Uint16 item_id) const
{
   return allmodels.find(item_id) != allmodels.end();
}

void ua_model3d_manager::render(const Underworld::Object& obj,
   ua_texture_manager& texmgr, ua_vector3d& base)
{
   Uint16 item_id = obj.GetObjectInfo().m_itemID;

   // try to find model object
   std::map<Uint16, ua_model3d_ptr>::iterator iter =
      allmodels.find(item_id);

   // render
   if (iter != allmodels.end())
      iter->second->render(obj, texmgr, base);
}

void ua_model3d_manager::get_bounding_triangles(const Underworld::Object& obj,
   ua_vector3d& base, std::vector<ua_triangle3d_textured>& alltriangles)
{
   Uint16 item_id = obj.GetObjectInfo().m_itemID;

   // try to find model object
   std::map<Uint16, ua_model3d_ptr>::iterator iter =
      allmodels.find(item_id);

   if (iter != allmodels.end())
      iter->second->get_bounding_triangles(obj, base, alltriangles);
}
