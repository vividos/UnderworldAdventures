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

void ua_object::load_object(ua_savegame &sg)
{
   xpos = sg.read32()/256.0;
   ypos = sg.read32()/256.0;

   info.type = static_cast<ua_obj_type>(sg.read8());
   info.item_id = sg.read16();
   info.quality = sg.read16();
   info.link1 = sg.read16();
   info.quantity = sg.read16();

   // todo: read data
}

void ua_object::save_object(ua_savegame &sg)
{
   sg.write32(Uint32(xpos*256));
   sg.write32(Uint32(ypos*256));

   sg.write8(info.type);
   sg.write16(info.item_id);
   sg.write16(info.quality);
   sg.write16(info.link1);
   sg.write16(info.quantity);

   // todo: write data
}


/* billboard tutorials:
   http://www.lighthouse3d.com/opengl/billboarding/
   http://nate.scuzzy.net/gltut/

*/

void billboardSphericalBegin(
   ua_vector3d &cam,
   ua_vector3d &objPos,
   ua_vector3d &lookAt)
{
   ua_vector3d objToCam, objToCamProj, upAux;

   double modelview[16],angleCosine;

   glPushMatrix();

// objToCamProj is the vector in world coordinates from the 
// local origin to the camera projected in the XY plane
   objToCamProj.x = cam.x - objPos.x;
   objToCamProj.y = cam.y - objPos.y;
   objToCamProj.z = 0;//cam.z - objPos.z;

// This is the original lookAt vector for the object 
// in world coordinates
/*   lookAt.x = 0;
   lookAt.y = -1;
   lookAt.z = 0;
*/

// normalize both vectors to get the cosine directly afterwards
   objToCamProj.normalize();

// easy fix to determine wether the angle is negative or positive
// for positive angles upAux will be a vector pointing in the 
// positive y direction, otherwise upAux will point downwards
// effectively reversing the rotation.

// mathsCrossProduct(upAux,lookAt,objToCamProj);
   upAux.cross(lookAt,objToCamProj);

// compute the angle
   angleCosine = lookAt.dot(objToCamProj);

// perform the rotation. The if statement is used for stability reasons
// if the lookAt and objToCamProj vectors are too close together then 
// |angleCosine| could be bigger than 1 due to lack of precision
   if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
      glRotatef(acos(angleCosine)*180/3.14,upAux.x, upAux.y, upAux.z);

   return;
/*
// so far it is just like the cylindrical billboard. The code for the 
// second rotation comes now
// The second part tilts the object so that it faces the camera

// objToCam is the vector in world coordinates from 
// the local origin to the camera
   objToCam.x = cam.x - objPos.x;
   objToCam.y = cam.y - objPos.y;
   objToCam.z = cam.z - objPos.z;

// Normalize to get the cosine afterwards
//   mathsNormalize(objToCam);
   objToCam.normalize();

// Compute the angle between objToCamProj and objToCam, 
//i.e. compute the required angle for the lookup vector

//   angleCosine = mathsInnerProduct(objToCamProj,objToCam);
   angleCosine = objToCamProj.dot(objToCam);


// Tilt the object. The test is done to prevent instability 
// when objToCam and objToCamProj have a very small
// angle between them

   if ((angleCosine < 0.99990) && (angleCosine > -0.9999))
      if (objToCam.y < 0)
         glRotatef(acos(angleCosine)*180/3.14,1,0,0);
      else
         glRotatef(acos(angleCosine)*180/3.14,-1,0,0);
*/
}

void billboardCheatSphericalBegin() {
   
   float modelview[16];
   int i,j;

   // save the current modelview matrix
   glPushMatrix();

   // get the current modelview matrix
   glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

   // undo all rotations
   // beware all scaling is lost as well 
   for( i=0; i<3; i++ ) 
       for( j=0; j<3; j++ ) {
      if ( i==j )
          modelview[i*4+j] = 1.0;
      else
          modelview[i*4+j] = 0.0;
       }

   // set the modelview with no rotations
   glLoadMatrixf(modelview);
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
#ifndef HAVE_DEBUG
   if (info.type==ua_obj_invisible)
      return;
#endif

   // get object position
   double objxpos = x + (xpos+0.5)/8.0;
   double objypos = y + (ypos+0.5)/8.0;
   double height = -fr.get_pos(2)+lev.get_floor_height(objxpos,objypos);

#if 1 // enabled part

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

#else // disabled part

/*
   ua_vector3d cam(fr.get_pos(0),fr.get_pos(1),fr.get_pos(2));
   ua_vector3d objpos(objxpos,objypos,height);

   ua_vector3d lookAt;
   lookAt.set(0,1,0);

   billboardSphericalBegin(cam,objpos,lookAt);
*/

   billboardCheatSphericalBegin();

   // draw object quad
   {
      double boxwidth=0.13;

      // get object texture coords
      double u1,v1,u2,v2;
      texmgr.object_tex(info.item_id,u1,v1,u2,v2);

      glEnable(GL_BLEND);

      glBegin(GL_QUADS);
      glColor3ub(255,255,255);

      glTexCoord2d(u1,v2); glVertex3d(objxpos-boxwidth, objypos, height);
      glTexCoord2d(u2,v2); glVertex3d(objxpos+boxwidth, objypos, height);
      glTexCoord2d(u2,v1); glVertex3d(objxpos+boxwidth, objypos, height+2*boxwidth);
      glTexCoord2d(u1,v1); glVertex3d(objxpos-boxwidth, objypos, height+2*boxwidth);
      glEnd();

      glDisable(GL_BLEND);
   }

   glPopMatrix();

/*
   float modelview[16];
   glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

   ua_vector3d xvec(modelview[0], modelview[4], modelview[8]), // right vector
               yvec(modelview[1], modelview[5], modelview[9]); // up vector

   // draw object quad
   {
      double boxwidth=0.13;

      // get object texture coords
      double u1,v1,u2,v2;
      texmgr.object_tex(info.item_id,u1,v1,u2,v2);

      glEnable(GL_BLEND);

      glBegin(GL_QUADS);
      glColor3ub(255,255,255);

      glTexCoord2d(u1,v2); glVertex3d(objxpos+(-xvec.x-yvec.x)*boxwidth, objypos+(-xvec.y-yvec.y)*boxwidth, height+(-xvec.z-yvec.z)*boxwidth);
      glTexCoord2d(u2,v2); glVertex3d(objxpos+(+xvec.x-yvec.x)*boxwidth, objypos+(+xvec.y-yvec.y)*boxwidth, height+(+xvec.z-yvec.z)*boxwidth);
      glTexCoord2d(u2,v1); glVertex3d(objxpos+(+xvec.x+yvec.x)*boxwidth, objypos+(+xvec.y+yvec.y)*boxwidth, height+(+xvec.z+yvec.z)*boxwidth);
      glTexCoord2d(u1,v1); glVertex3d(objxpos+(-xvec.x+yvec.x)*boxwidth, objypos+(-xvec.y+yvec.y)*boxwidth, height+(-xvec.z+yvec.z)*boxwidth);
      glEnd();

      glDisable(GL_BLEND);
   }*/

#endif
}


// ua_object_list methods

ua_object_list::ua_object_list()
{
}

ua_object_list::~ua_object_list()
{
   master_obj_list.clear();
}

bool ua_object_list::get_first_tile_object(unsigned int xpos,
   unsigned int ypos, ua_object &obj)
{
   // get first object in tile
   Uint16 idx = tile_index[ypos*64+xpos];
   if (idx==0) return false; // no object in tile

   obj = master_obj_list[idx];
   return true;
}

bool ua_object_list::get_next_tile_object(ua_object &obj)
{
   Uint16 link1 = obj.get_object_info().link1;
   if (link1==0) return false; // no more objects in tile

   obj = master_obj_list[link1];
   return true;
}

void ua_object_list::load_game(ua_savegame &sg)
{
   // read tile index table
   tile_index.clear();
   tile_index.resize(64*64,0);
   unsigned int n=0;

   for(n=0; n<64*64; n++)
      tile_index[n] = sg.read16();

   // read master object list
   master_obj_list.clear();
   master_obj_list.resize(0x400);

   for(n=0; n<0x400; n++)
   {
      master_obj_list[n].load_object(sg);
   }
}

void ua_object_list::save_game(ua_savegame &sg)
{
   // write tile index table
   unsigned int n=0;

   for(n=0; n<64*64; n++)
      sg.write16(tile_index[n]);

   // write all objects in master object list
   for(n=0; n<0x400; n++)
   {
      ua_object& obj = master_obj_list[n];
      obj.save_object(sg);
   }
}
