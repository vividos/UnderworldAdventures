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
/*! \file tessellator.hpp

   \brief tessellator class

*/

// include guard
#ifndef uwadv_tessellator_hpp_
#define uwadv_tessellator_hpp_

// needed includes


#ifdef WIN32
#define UA_GL_CALLBACK __stdcall
#else
#define UA_GL_CALLBACK
#endif

// classes

//! polygon tessellator class
class ua_poly_tessellator
{
public:
   //! ctor
   ua_poly_tessellator();
   //! dtor
   ~ua_poly_tessellator();

   //! adds polygon vertex
   void add_poly_vertex(const ua_vertex3d& vertex)
   {
      poly_vertices.push_back(vertex);
   }

   //! tessellates the polygon and returns triangles
   const std::vector<ua_triangle3d_textured>& tessellate(Uint16 texnum);

protected:
   // static callback functions

   static void UA_GL_CALLBACK begin_data(GLenum type, ua_poly_tessellator* This);

   static void UA_GL_CALLBACK end_data(ua_poly_tessellator* This);

   static void UA_GL_CALLBACK vertex_data(
      ua_vertex3d* vert, ua_poly_tessellator* This);

   static void UA_GL_CALLBACK combine_data(GLdouble coords[3],
      ua_vertex3d* vertex_data[4], GLfloat weight[4], ua_vertex3d** out_data,
      ua_poly_tessellator* This);

protected:
   //! GLU tessellator object
   GLUtesselator* tess;

   //! current texture number
   Uint16 cur_texnum;

   //! current glBegin() parameter type
   GLenum cur_type;

   //! indicates a fresh triangle start
   bool tri_start;

   //! list of polygon vertices; only point 0 of triangle is valid
   std::vector<ua_vertex3d> poly_vertices;

   //! temporary vertex cache to combine 3 vertices to a triangle
   std::vector<ua_vertex3d> vert_cache;

   //! list with new triangles
   std::vector<ua_triangle3d_textured> triangles;

   //! list of pointer to vertices created using combining
   std::vector<ua_vertex3d*> combined_vertices;
};


#endif
