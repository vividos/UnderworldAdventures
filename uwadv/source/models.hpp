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
/*! \file models.hpp

   \brief 3d models and model manager

*/

// include guard
#ifndef uwadv_models_hpp_
#define uwadv_models_hpp_

// needed includes
#include "renderer.hpp"
#include <map>
#include <vector>


// classes

//! 3d model class
class ua_model3d
{
public:
   //! ctor
   ua_model3d(){}

   //! renders model
   void render();

   //! returns list of triangles
   std::vector<ua_triangle3d_textured>& get_triangles(){ return alltriangles; }

protected:
   //! list of triangles
   std::vector<ua_triangle3d_textured> alltriangles;
};


//! 3d model manager
class ua_model3d_manager
{
public:
   //! ctor
   ua_model3d_manager(){}

   //! init manager
   void init(const char* uwexe_filename);

   //! renders a model
   void render(Uint16 model);

protected:
   //! map with all 3d model objects
   std::map<Uint16,ua_model3d> allmodels;
};

#endif
