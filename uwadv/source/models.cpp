/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file models.cpp

   \brief 3d models handling

*/

// needed includes
#include "common.hpp"
#include "models.hpp"
#include "core.hpp"


// external functions

extern bool ua_model_decode_builtins(const char* filename,
   std::vector<ua_model3d_ptr>& allmodels);


// ua_model3d methods

void ua_model3d_builtin::render(ua_vector3d& base)
{
   // render all triangles
   glColor3ub(192,192,192);
/*
   unsigned int max = alltriangles.size();
   for(unsigned int i=0; i<max; i++)
   {
      ua_triangle3d_textured& tri = alltriangles[i];

//      texmgr->use(tri.texnum);

      glBegin(GL_TRIANGLES);
      for(int j=0; j<3; j++)
      {
         //glTexCoord2d(tri.tex_u[j],tri.tex_v[j]);
         glVertex3d(tri.points[j].x, tri.points[j].y, tri.points[j].z);
      }
      glEnd();
   }*/
}


// ua_model3d_manager methods

   void init(const char* uwexe_filename);

void ua_model3d_manager::init(ua_game_core_interface* thecore)
{
   ua_trace("initializing model3d manager\n");

   core = thecore;

   // loading builtin models
   std::string uwexe_filename(core->get_settings().get_string(ua_setting_uw_path));
   uwexe_filename.append("uw.exe");

   ua_model_decode_builtins(uwexe_filename.c_str(), allbuiltins);

   // loading model3d.cfg
   std::string cfgfile_name("uw1"); // TODO: replace with current game name
   cfgfile_name.append("/model3d.cfg");

   ua_trace(" loading config file: %s\n",cfgfile_name.c_str());
   ua_cfgfile::load(core->get_filesmgr().get_uadata_file(cfgfile_name.c_str()));
}

void ua_model3d_manager::load_value(const std::string& name, const std::string& value)
{
   // get item id
   Uint16 item_id;
   if (name.find("0x")==0)
      item_id = strtol(name.c_str()+2,NULL, 16);
   else
      item_id = strtol(name.c_str(),NULL, 10);

   // get filename/builtin number
   if (value.find("builtin")==0)
   {
      // we have a builtin graphics
      unsigned int builtin_nr = strtol(value.c_str()+7,NULL,10);

      // TODO: insert builtin model
   }
   else
   {
      std::string model_path(value);

      // load .wrl model
      ua_model3d_wrl* model = new ua_model3d_wrl;
      SDL_RWops* rwops = NULL;

      ua_trace(" loading model %s\n",model_path.c_str());

      if (model_path.find("%uadata%") == 0)
      {
         model_path.erase(0,8);
         rwops = core->get_filesmgr().get_uadata_file(model_path.c_str());
      }
      else
      {
         core->get_filesmgr().replace_system_vars(model_path);
         rwops = SDL_RWFromFile(model_path.c_str(),"rb");
      }

      // import model
      model->import_wrl(core,rwops);

      allmodels.insert(
         std::make_pair<Uint16,ua_model3d_ptr>(item_id,ua_model3d_ptr(model)) );
   }
}

bool ua_model3d_manager::model_avail(Uint16 item_id)
{
   return allmodels.find(item_id) != allmodels.end();
}

void ua_model3d_manager::render(Uint16 item_id, ua_vector3d& base)
{
   std::map<Uint16,ua_model3d_ptr>::iterator iter =
      allmodels.find(item_id);

   if (iter != allmodels.end())
      iter->second->render(base);
}
