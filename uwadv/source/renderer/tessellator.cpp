/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file tessellator.cpp

   \brief tessellator implementation

*/

// needed includes
#include "common.hpp"
#include "tessellator.hpp"


// ua_poly_tessellator methods

ua_poly_tessellator::ua_poly_tessellator()
{
   tess = gluNewTess();
}

ua_poly_tessellator::~ua_poly_tessellator()
{
   gluDeleteTess(tess);

   // free vertices created through combining
   unsigned int max=combined_vertices.size();
   for(unsigned int i=0; i<max; i++)
      delete combined_vertices[i];

   combined_vertices.clear();
}

/*! Tessellates the polygon into triangles with the polygon vertices passed
    with add_poly_vertex().

    \param texnum texture number to use when storing triangles
*/
const std::vector<ua_triangle3d_textured>& ua_poly_tessellator::tessellate(Uint16 texnum)
{
   cur_texnum = texnum;
   vert_cache.clear();

   gluTessProperty(tess,GLU_TESS_BOUNDARY_ONLY,GL_FALSE);

   // glu callback function typedef
   typedef void (UA_GL_CALLBACK* GLU_CALLBACK)();

   // register callbacks
   gluTessCallback(tess,GLU_TESS_BEGIN,NULL);
   gluTessCallback(tess,GLU_TESS_BEGIN_DATA,
      reinterpret_cast<GLU_CALLBACK>(begin_data));

   gluTessCallback(tess,GLU_TESS_END,NULL);
   gluTessCallback(tess,GLU_TESS_END_DATA,
      reinterpret_cast<GLU_CALLBACK>(end_data));

   gluTessCallback(tess,GLU_TESS_VERTEX,NULL);
   gluTessCallback(tess,GLU_TESS_VERTEX_DATA,
      reinterpret_cast<GLU_CALLBACK>(vertex_data));

   gluTessCallback(tess,GLU_TESS_COMBINE,NULL);
   gluTessCallback(tess,GLU_TESS_COMBINE_DATA,
      reinterpret_cast<GLU_CALLBACK>(combine_data));

   gluTessBeginPolygon(tess,this);
   gluTessBeginContour(tess);

   // put all polygon vertices into tesselator
   unsigned int max = poly_vertices.size();
   for(unsigned int i=0; i<max; i++)
   {
      ua_vertex3d& vertex = poly_vertices[i];
      GLdouble coords[3] = { vertex.pos.x, vertex.pos.y, vertex.pos.z };

      gluTessVertex(tess, coords, &poly_vertices[i]);
   }

   gluTessEndContour(tess);
   gluTessEndPolygon(tess);

   return triangles;
}

void UA_GL_CALLBACK ua_poly_tessellator::begin_data(GLenum type, ua_poly_tessellator* This)
{
   This->cur_type = type;
   This->tri_start = true;
}

void UA_GL_CALLBACK ua_poly_tessellator::end_data(ua_poly_tessellator* This)
{
   This->cur_type = 0;
}

void UA_GL_CALLBACK ua_poly_tessellator::vertex_data(ua_vertex3d* vert, ua_poly_tessellator* This)
{
   if (This->vert_cache.size()==2)
   {
      // construct triangle
      ua_triangle3d_textured tri;
      tri.vertices[0] = This->vert_cache[0];
      tri.vertices[1] = This->vert_cache[1];
      tri.vertices[2] = *vert;
      tri.texnum = This->cur_texnum;

      This->triangles.push_back(tri);

      // check which vertices to throw away
      switch(This->cur_type)
      {
      case GL_TRIANGLES: // clear all; new set of vertices
         This->vert_cache.clear();
         break;

      case GL_TRIANGLE_STRIP: // erase first vertex, insert second
         This->vert_cache.erase(This->vert_cache.begin());
         This->vert_cache.push_back(*vert);
         break;

      case GL_TRIANGLE_FAN: // delete second vertex, insert second
         This->vert_cache.pop_back();
         This->vert_cache.push_back(*vert);
         break;
      default:
         break; // should not happen
      }
   }
   else
      This->vert_cache.push_back(*vert);
}

void UA_GL_CALLBACK ua_poly_tessellator::combine_data(GLdouble coords[3],
   ua_vertex3d* vertex_data[4], GLfloat weight[4], ua_vertex3d** out_data, ua_poly_tessellator* This)
{
   ua_vertex3d* vert = new ua_vertex3d;

   vert->pos.set(coords[0],coords[1],coords[2]);
   vert->u =
      weight[0]*vertex_data[0]->u +
      weight[1]*vertex_data[1]->u +
      weight[2]*vertex_data[2]->u +
      weight[3]*vertex_data[3]->u;

   vert->v =
      weight[0]*vertex_data[0]->v +
      weight[1]*vertex_data[1]->v +
      weight[2]*vertex_data[2]->v +
      weight[3]*vertex_data[3]->v;

   This->combined_vertices.push_back(vert);
   *out_data = vert;
}
