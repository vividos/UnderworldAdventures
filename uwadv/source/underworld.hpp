/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file underworld.hpp

   underworld class

*/

// include guard
#ifndef __uwadv_underworld_hpp_
#define __uwadv_underworld_hpp_

// needed includes
#include "levelmap.hpp"
#include "texture.hpp"


// classes

//! underworld class
class ua_underworld
{
public:
   ua_underworld(){}

   //! initializes underworld
   //! \todo game state loading, from char generation or saved game
   void init(ua_settings &settings, ua_texture_manager &texmgr);

   //! clean up the underworld
   void done();

   float get_player_height(float x, float y){ return curmap->get_floor_height(x,y)+0.6; }

   //! saves game
   //void save();

   //! moves player according to the speed vector
   void walk_player(){}

   //! transfers player to other location
   void move_player(float x, float y, int level=-1);

   //! renders the current game map and all objects
   void render(ua_texture_manager &texmgr);

protected:
   //! current level
   int curlevel;

   //! current level map
   ua_levelmap *curmap;

   //! all level maps
   std::vector<ua_levelmap*> allmaps;
};

#endif
