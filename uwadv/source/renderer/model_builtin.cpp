/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004 Underworld Adventures Team

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
/*! \file model_builtin.cpp

   \brief builtin models

*/

// needed includes
#include "common.hpp"
#include "model_builtin.hpp"


// external functions

extern bool ua_model_decode_builtins(const char* filename,
   std::vector<ua_model3d_ptr>& allmodels, bool dump=false);


// ua_model3d_builtin methods

void ua_model3d_builtin::render(ua_object& obj, ua_vector3d& base)
{
   glDisable(GL_CULL_FACE);
   glDisable(GL_TEXTURE_2D);

   unsigned int max = triangles.size();

   ua_vector3d origin;

   for(unsigned int i=0; i<max; i++)
   {
      // select debug triangle color
      switch(triangles[i].texnum)
      {
      case 1:
         glColor3ub(255,0,0); // red
         break;
      case 2:
         glColor3ub(0,255,0); // green
         break;
      case 3:
         glColor3ub(0,0,255); // blue
         break;
      case 4:
         glColor3ub(255,0,255); // violet
         break;
      }

      const ua_triangle3d_textured& tri = triangles[i];

//      glBegin(GL_LINE_LOOP);
      glBegin(GL_TRIANGLES);

      // TODO generate normal vector
      ua_vector3d normal,vec1(tri.vertices[1].pos),vec2(tri.vertices[2].pos);
      vec1 -= tri.vertices[0].pos;
      vec2 -= tri.vertices[0].pos;

      normal.cross(vec1,vec2);
      normal.normalize();
      normal*= -1;

      glNormal3d(normal.x,normal.y,normal.z);

      for(unsigned j=0; j<3; j++)
         glVertex3d(
            base.x-origin.x+tri.vertices[j].pos.x,
            base.y-origin.y+tri.vertices[j].pos.y,
            base.z-origin.z+tri.vertices[j].pos.z
         );

      glEnd();
/*
#ifdef HAVE_DEBUG
      glColor3ub(255,255,255);
      glBegin(GL_LINE_LOOP);
      for(unsigned n=0; n<3; n++)
         glVertex3d(
            base.x-origin.x+tri.vertices[n].pos.x,
            base.y-origin.y+tri.vertices[n].pos.y,
            base.z-origin.z+tri.vertices[n].pos.z
         );
      glEnd();
#endif
*/
   }

#ifdef HAVE_DEBUG
/*
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
*/
#endif

   glEnable(GL_CULL_FACE);
   glEnable(GL_TEXTURE_2D);
}

void ua_model3d_builtin::get_bounding_triangles(ua_object& obj,
   ua_vector3d& base, std::vector<ua_triangle3d_textured>& trilist)
{
   // just hand over the tessellated triangles
   trilist.insert(trilist.end(),triangles.begin(),triangles.end());
}
