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


// external functions

extern bool ua_model_decode_all(const char* filename,
   std::map<Uint16,ua_model3d>& allmodels);


// ua_model3d methods

void ua_model3d::render()
{
   // render all triangles
   glColor3ub(192,192,192);

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
   }
}


// ua_model3d_manager methods

   void init(const char* uwexe_filename);

void ua_model3d_manager::init(ua_settings& settings)
{
   ua_trace("initializing model3d manager\n");

   std::string uwexe_filename(settings.get_string(ua_setting_uw_path));
   uwexe_filename.append("uw.exe");

   //ua_model_decode_all(uwexe_filename.c_str(), allmodels);
}

void ua_model3d_manager::render(Uint16 model)
{
   std::map<Uint16,ua_model3d>::iterator iter =
      allmodels.find(model);

   if (iter != allmodels.end())
      iter->second.render();
}
