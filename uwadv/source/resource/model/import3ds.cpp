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
/*! \file import3ds.cpp

   \brief 3ds file importer (not used in uwadv)

*/

// needed includes
#include "common.hpp"
#include "models.hpp"

#undef HAVE_LIB3DS

#ifdef HAVE_LIB3DS
#include "lib3ds/file.h"
#include "lib3ds/mesh.h"
#include "lib3ds/material.h"
#endif


ua_model3d_3ds::~ua_model3d_3ds()
{
   lib3ds_file_free((Lib3dsFile*)file);
}

void ua_model3d_3ds::load_3ds(const char* filename)
{
#ifdef HAVE_LIB3DS

   file = (void*)lib3ds_file_load(filename);

#endif
}

void ua_model3d_3ds::render(ua_vector3d& base)
{
   Lib3dsFile* f = (Lib3dsFile*)file;

   Lib3dsMesh* m = f->meshes;

   glDisable(GL_TEXTURE_2D);

   do
   {
      // render mesh
      for(Lib3dsDword n=0; n<m->faces; n++)
      {
         Lib3dsFace& f = m->faceL[n];

         glBegin(GL_LINE_LOOP);
         //glBegin(GL_TRIANGLES);

         for(int i=0; i<3; i++)
         {
            Lib3dsVector& p = m->pointL[f.points[i]].pos;

            glTexCoord2d(base.x+p[0]*0.01,base.y+p[1]*0.01);

            glVertex3d(base.x+p[0]*0.01,base.y+p[1]*0.01,base.z+p[2]*0.01);
         }

         glEnd();
      }

      m = m->next;
   } while(m != NULL);

   glEnable(GL_TEXTURE_2D);
}
