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
/*! \file objects.cpp

   object method implementation

*/

// needed includes
#include "common.hpp"
#include "objects.hpp"
#include "level.hpp"


// ua_object methods

ua_object::ua_object(unsigned int myxpos,unsigned int myypos,Uint16 mylink1,Uint16 myid)
:xpos(myxpos),ypos(myypos),link1(mylink1),id(myid),type(ua_obj_invisible)
{
}

void ua_object::render(unsigned int x, unsigned int y,
   ua_texture_manager &texmgr, ua_frustum &fr, ua_level &lev)
{
   // get object position
   double objxpos = x + (xpos+0.5)/8.0;
   double objypos = y + (ypos+0.5)/8.0;
   double height = lev.get_floor_height(objxpos,objypos);

   // get object texture coords
   double u1,v1,u2,v2;
   texmgr.object_tex(id,u1,v1,u2,v2);


   // render object

   double boxwidth=0.1;

   // draws a white textured box on the floor
   glBegin(GL_QUADS);
   glColor3ub(255,255,255);

   glTexCoord2d(u1,v1); glVertex3d(objxpos-boxwidth, objypos-boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v1); glVertex3d(objxpos+boxwidth, objypos-boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v2); glVertex3d(objxpos+boxwidth, objypos+boxwidth, height+boxwidth*2);
   glTexCoord2d(u1,v2); glVertex3d(objxpos-boxwidth, objypos+boxwidth, height+boxwidth*2);

   glTexCoord2d(u1,v1); glVertex3d(objxpos-boxwidth, objypos-boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v1); glVertex3d(objxpos-boxwidth, objypos+boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v2); glVertex3d(objxpos-boxwidth, objypos+boxwidth, height);
   glTexCoord2d(u1,v2); glVertex3d(objxpos-boxwidth, objypos-boxwidth, height);

   glTexCoord2d(u1,v1); glVertex3d(objxpos-boxwidth, objypos+boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v1); glVertex3d(objxpos+boxwidth, objypos+boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v2); glVertex3d(objxpos+boxwidth, objypos+boxwidth, height);
   glTexCoord2d(u1,v2); glVertex3d(objxpos-boxwidth, objypos+boxwidth, height);

   glTexCoord2d(u1,v1); glVertex3d(objxpos+boxwidth, objypos+boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v1); glVertex3d(objxpos+boxwidth, objypos-boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v2); glVertex3d(objxpos+boxwidth, objypos-boxwidth, height);
   glTexCoord2d(u1,v2); glVertex3d(objxpos+boxwidth, objypos+boxwidth, height);

   glTexCoord2d(u1,v1); glVertex3d(objxpos+boxwidth, objypos-boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v1); glVertex3d(objxpos-boxwidth, objypos-boxwidth, height+boxwidth*2);
   glTexCoord2d(u2,v2); glVertex3d(objxpos-boxwidth, objypos-boxwidth, height);
   glTexCoord2d(u1,v2); glVertex3d(objxpos+boxwidth, objypos-boxwidth, height);
   glEnd();
}


// ua_npc_object methods

ua_npc_object::ua_npc_object(unsigned int xpos,unsigned int ypos,Uint16 link1,Uint16 id)
:ua_object(xpos,ypos,link1,id)
{
}


// ua_object_list methods

std::vector<ua_object*> ua_object_list::get_object_list(unsigned int xpos,
   unsigned int ypos)
{
   std::vector<ua_object*> objlist;

   // collect all object in that tile
   Uint16 idx=tile_index[ypos*64+xpos];

   while(idx!=0)
   {
      ua_object *obj = master_obj_list[idx];
      if (obj==0)
      {
         idx=0; break;
      }

      // remember that object
      objlist.push_back(obj);

      // follow link
      idx = obj->get_link1();
   }

   return objlist;
}

ua_object *ua_object_list::get_object(Uint16 at)
{
   return master_obj_list[at];
}
