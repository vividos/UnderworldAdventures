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
/*! \file models_impl.hpp

   \brief 3d model class implementations

*/

// include guard
#ifndef uwadv_models_impl_hpp_
#define uwadv_models_impl_hpp_

// needed includes
#include "models.hpp"


// classes

//! static object model
class ua_model3d_wrl: public ua_model3d
{
public:
   //! ctor
   ua_model3d_wrl(){}

   //! dtor
   virtual ~ua_model3d_wrl(){}

   //! loads vrml97 .wrl file
   void import_wrl(ua_game_core_interface* core, SDL_RWops* rwops,
      std::string relpath);

   //! renders model
   virtual void render(ua_object& obj, ua_vector3d& base);

protected:

   std::vector<ua_vector3d> coords;

   std::vector<ua_vector2d> texcoords;

   std::vector<unsigned int> coord_index;

   std::vector<unsigned int> texcoord_index;

   //! model texture
   ua_texture tex;
};

#endif
