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

   \brief object method implementation

*/

// needed includes
#include "common.hpp"
#include "objects.hpp"
#include "level.hpp"
#include "uamath.hpp"


// ua_object methods

ua_object::ua_object(unsigned int myxpos,unsigned int myypos,Uint16 link1,Uint16 item_id)
:type(ua_obj_invisible),xpos(myxpos),ypos(myypos)
{
   info.item_id = item_id;
   info.link1 = link1;
}

/*! each object is rendered using "view coordinates", that means the camera is
    at 0,0,0 and the camera views towards the positive y axis. objects that
    should be perpendicular to the view vector can just be rendered as quads
    with a fixed y value. the needed modelview matrix should be set up by the
    caller, e.g. from ua_level::render().
*/
void ua_object::render(unsigned int x, unsigned int y,
   ua_texture_manager &texmgr, ua_frustum &fr, ua_level &lev)
{
   // don't render invisible objects
   if (type==ua_obj_invisible)
      return;

   // get object position
   double objxpos = x + (xpos+0.5)/8.0;
   double objypos = y + (ypos+0.5)/8.0;
   double height = -fr.get_pos(2)+lev.get_floor_height(objxpos,objypos);

   // get object angle and distance, in respect to the camera
   double objdx = objxpos-fr.get_pos(0), objdy = objypos-fr.get_pos(1);
   double objangle = ua_rad2deg(atan2(objdy,objdx));
   double objdist = sqrt(objdx*objdx+objdy*objdy);

   // calculate angle relative to view
   double camangle = fr.get_xangle();
   double relangle = objangle - camangle + 90.0;

   // calculate object position in transformed coordinates
   double relx, rely;
   relx = objdist*cos(ua_deg2rad(relangle));
   rely = objdist*sin(ua_deg2rad(relangle));

   // get object texture coords
   double u1,v1,u2,v2;
   texmgr.object_tex(info.item_id,u1,v1,u2,v2);

   // calculate quad y rotation (e.g. when looking at it from above)
   double boxwidth=0.13;
   double camangle2 = fr.get_yangle();
   double rely2 = rely-boxwidth*sin(ua_deg2rad(camangle2));
   double height2 = height+boxwidth*2*cos(ua_deg2rad(camangle2));

   // draw object quad

//   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);

   glBegin(GL_QUADS);
   glColor3ub(255,255,255);

   glTexCoord2d(u1,v2); glVertex3d(relx-boxwidth, rely, height);
   glTexCoord2d(u2,v2); glVertex3d(relx+boxwidth, rely, height);
   glTexCoord2d(u2,v1); glVertex3d(relx+boxwidth, rely2, height2);
   glTexCoord2d(u1,v1); glVertex3d(relx-boxwidth, rely2, height2);

   glEnd();

   glDisable(GL_BLEND);
//   glEnable(GL_DEPTH_TEST);
}


// ua_npc_object methods

ua_npc_object::ua_npc_object(unsigned int xpos,unsigned int ypos,Uint16 link1,Uint16 id)
:ua_object(xpos,ypos,link1,id)
{
}


// ua_object_list methods

ua_object_list::~ua_object_list()
{
   master_obj_list.clear();
}

void ua_object_list::get_object_list(unsigned int xpos, unsigned int ypos,
   std::vector<ua_object_ptr> &objlist)
{
   // collect all object in that tile
   Uint16 idx=tile_index[ypos*64+xpos];

   while(idx!=0)
   {
      ua_object_ptr objptr = master_obj_list[idx];
      if (objptr.get()==0)
      {
         idx=0; break;
      }

      // remember that object
      objlist.push_back(objptr);

      // follow link
      idx = objptr->get_object_info().link1;
   }
}
