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
/*! \file player.hpp

   player character representation

*/

// include guard
#ifndef __uwadv_player_hpp_
#define __uwadv_player_hpp_

// needed includes



// classes

class ua_player
{
public:
   //! ctor
   ua_player(){}

   void init(double x,double y){ move_player(x,y); }

   double get_xpos(){ return xpos; }
   double get_ypos(){ return ypos; }

   void move_player(double x, double y){ xpos=x; ypos=y; }

protected:
   float xpos,ypos;
};

#endif
