/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
#include "resource/models_impl.hpp"
#include "core.hpp"


// external functions

extern bool ua_model_decode_builtins(const char* filename,
   std::vector<ua_model3d_ptr>& allmodels);


// ua_model3d_builtin methods

void ua_model3d_builtin::render(ua_object& obj, ua_vector3d& base)
{
   // hack: implementation for a_bridge
   if (obj.get_object_info().item_id == 0x0164)
   {
      ua_object_info_ext& extinfo = obj.get_ext_object_info();

      base.x = extinfo.tilex + 0.5;
      base.y = extinfo.tiley + 0.5;
      base.z += 0.06;

      glBegin(GL_TRIANGLES);
      glTexCoord2d(0.0,0.0); glVertex3d(base.x-0.5, base.y-0.5, base.z);
      glTexCoord2d(1.0,0.0); glVertex3d(base.x+0.5, base.y-0.5, base.z);
      glTexCoord2d(1.0,1.0); glVertex3d(base.x+0.5, base.y+0.5, base.z);

      glTexCoord2d(0.0,0.0); glVertex3d(base.x-0.5, base.y-0.5, base.z);
      glTexCoord2d(1.0,1.0); glVertex3d(base.x+0.5, base.y+0.5, base.z);
      glTexCoord2d(0.0,1.0); glVertex3d(base.x-0.5, base.y+0.5, base.z);
      glEnd();

      return;
   }

   glDisable(GL_CULL_FACE);
   glDisable(GL_TEXTURE_2D);

   unsigned int max = coord_index.size();

   unsigned int col_cnt = 0;

   for(unsigned int i=0; i<max; i++)
   {
      glColor3ub(face_colors[col_cnt],face_colors[col_cnt+1],face_colors[col_cnt+2]);
      col_cnt+=3;

      glBegin(GL_LINE_LOOP);
      //glBegin(GL_POLYGON);
      //glBegin(GL_TRIANGLE_STRIP);

      // TODO generate normal vector
      unsigned int j=i;
      do
      {
         const ua_vector3d& vert = coords[coord_index[j]];

         glVertex3d(base.x+vert.x-origin.x, base.y+vert.y-origin.y, base.z+vert.z-origin.z);

      } while(coord_index[++j] != -1);

      i = j + 1; // go to next indices

      glEnd();
   }

#ifdef HAVE_DEBUG
   // draw extents box
   glColor3ub(255,255,255);

   ua_vector3d ext(extents);
   ext.x *= 0.5;
   ext.y *= 0.5;

   glBegin(GL_LINE_LOOP);
   glVertex3d(base.x+ext.x,  base.y+ext.y, base.z+ext.z);
   glVertex3d(base.x+ext.x,  base.y+ext.y, base.z+0);
   glVertex3d(base.x+-ext.x, base.y+ext.y, base.z+0);
   glVertex3d(base.x+-ext.x, base.y+ext.y, base.z+ext.z);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glVertex3d(base.x+ext.x,  base.y+-ext.y, base.z+ext.z);
   glVertex3d(base.x+ext.x,  base.y+-ext.y, base.z+0);
   glVertex3d(base.x+-ext.x, base.y+-ext.y, base.z+0);
   glVertex3d(base.x+-ext.x, base.y+-ext.y, base.z+ext.z);
   glEnd();

   glBegin(GL_LINES);
   glVertex3d(base.x+ext.x,  base.y+ ext.y, base.z+ext.z);
   glVertex3d(base.x+ext.x,  base.y+-ext.y, base.z+ext.z);

   glVertex3d(base.x+-ext.x, base.y+ ext.y, base.z+ext.z);
   glVertex3d(base.x+-ext.x, base.y+-ext.y, base.z+ext.z);

   glVertex3d(base.x+ext.x,  base.y+ ext.y, base.z+0);
   glVertex3d(base.x+ext.x,  base.y+-ext.y, base.z+0);

   glVertex3d(base.x+-ext.x, base.y+ ext.y, base.z+0);
   glVertex3d(base.x+-ext.x, base.y+-ext.y, base.z+0);
   glEnd();
#endif

   glEnable(GL_CULL_FACE);
   glEnable(GL_TEXTURE_2D);
}

void ua_model3d_builtin::get_bounding_triangles(ua_object& obj,
   ua_vector3d& base, std::vector<ua_triangle3d_textured>& trilist)
{
   // hack: implementation for a_bridge
   if (obj.get_object_info().item_id == 0x0164)
   {
      ua_object_info_ext& extinfo = obj.get_ext_object_info();

      base.x = extinfo.tilex + 0.5;
      base.y = extinfo.tiley + 0.5;
      base.z += 0.06*4.0;

      ua_triangle3d_textured tri1,tri2;
      tri1.set(0, base.x-0.5, base.y-0.5, base.z, 0.0,0.0);
      tri1.set(1, base.x+0.5, base.y-0.5, base.z, 0.0,0.0);
      tri1.set(2, base.x+0.5, base.y+0.5, base.z, 0.0,0.0);

      tri2.set(0, base.x-0.5, base.y-0.5, base.z, 0.0,0.0);
      tri2.set(1, base.x+0.5, base.y+0.5, base.z, 0.0,0.0);
      tri2.set(2, base.x-0.5, base.y+0.5, base.z, 0.0,0.0);

      trilist.push_back(tri1);
      trilist.push_back(tri2);
   }

/*
   unsigned int max = alltriangles.size();
   for(unsigned int i=0; i<max; i++)
   {
      ua_triangle3d_textured tri(alltriangles[i]);

      tri.points[0] += base;
      tri.points[0] += origin;

      tri.points[1] += base;
      tri.points[1] += origin;

      tri.points[2] += base;
      tri.points[2] += origin;

      ua_vector3d temp = tri.points[2];
      tri.points[2] = tri.points[1];
      tri.points[1] = temp;

      trilist.push_back(tri);
   }
*/
}


// ua_model3d_manager methods

ua_model3d_manager* ua_model3d_manager::cur_modelmgr = NULL; 

void ua_model3d_manager::init(ua_game_core_interface* thecore)
{
   ua_trace("initializing model3d manager\n");

   core = thecore;

   cur_modelmgr = this; 

   // loading builtin models
   std::string uwexe_filename(core->get_settings().get_string(ua_setting_uw_path));
   uwexe_filename.append("uw.exe");

   ua_model_decode_builtins(uwexe_filename.c_str(), allbuiltins);

   // loading %game%/model3d.cfg
   std::string cfgfile_name(core->get_settings().get_string(ua_setting_game_prefix));
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

      // insert builtin model
      ua_model3d_ptr model_ptr = allbuiltins[builtin_nr];

      allmodels.insert(
         std::make_pair<Uint16,ua_model3d_ptr>(item_id,model_ptr) );
   }
   else
   {
      std::string model_path(value);

      // load .wrl model
      ua_model3d_wrl* model = new ua_model3d_wrl;
      SDL_RWops* rwops = NULL;

      ua_trace(" loading model %s\n",model_path.c_str());

      rwops = core->get_filesmgr().get_uadata_file(model_path.c_str());

      // import model
      model->import_wrl(core,rwops,model_path);

      allmodels.insert(
         std::make_pair<Uint16,ua_model3d_ptr>(item_id,ua_model3d_ptr(model)) );
   }
}

bool ua_model3d_manager::model_avail(Uint16 item_id)
{
   return allmodels.find(item_id) != allmodels.end();
}

void ua_model3d_manager::render(ua_object& obj, ua_vector3d& base)
{
   Uint16 item_id = obj.get_object_info().item_id;

   // try to find model object
   std::map<Uint16,ua_model3d_ptr>::iterator iter =
      allmodels.find(item_id);

   // render
   if (iter != allmodels.end())
      iter->second->render(obj,base);
}

void ua_model3d_manager::get_bounding_triangles(ua_object& obj,
   ua_vector3d& base, std::vector<ua_triangle3d_textured>& alltriangles)
{
   Uint16 item_id = obj.get_object_info().item_id;

   // try to find model object
   std::map<Uint16,ua_model3d_ptr>::iterator iter =
      allmodels.find(item_id);

   if (iter != allmodels.end())
      iter->second->get_bounding_triangles(obj,base,alltriangles);
}
