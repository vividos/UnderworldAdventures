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
/*! \file objloader.cpp

   \brief level objects loader

*/

// needed includes
#include "common.hpp"
#include "objects.hpp"
#include "fread_endian.hpp"


// ua_object_list methods

void ua_object_list::import_objs(FILE *fd)
{
   tile_index.resize(64*64,0);
   master_obj_list.resize(0x400);

   // read in all tile indices
   for(unsigned int tile=0; tile<64*64; tile++)
   {
      Uint32 tileword = fread32(fd);
      tile_index[tile] = (tileword & 0xFFC00000) >> 22;
   }

   // now read in the master object list
   std::vector<Uint32> objprop;
   objprop.resize(0x400*2,0);

//   std::vector<Uint8> npcinfo;
//   npcinfo.resize(0x100*19,0);

   for(Uint16 item=0; item<0x400; item++)
   {
      objprop[item*2+0] = fread32(fd);
      objprop[item*2+1] = fread32(fd);

      if (item<0x100)
      {
         // read NPC info bytes, too
         fseek(fd,19,SEEK_CUR);
      }
   }

   // now that we have the two lists, follow each tile ref
   for(unsigned int n=0; n<64*64; n++)
   {
      if (tile_index[n]!=0)
         addobj_follow(objprop,tile_index[n]);
   }
}

void ua_object_list::addobj_follow(std::vector<Uint32> &objprop,Uint16 objpos)
{
   while(objpos!=0)
   {
      // get object properties
      Uint16 objid = objprop[objpos*2+0] & 0x000001ff;
      Uint16 ypos = (objprop[objpos*2+0] & (7<<26))>>26;
      Uint16 xpos = (objprop[objpos*2+0] & (7<<29))>>29;
      Uint16 link1 = (objprop[objpos*2+1] & (0x3ff<<6))>>6;

      ua_object *obj;

      // add object to master object list
      if (objpos<0x100)
      {
         obj = new ua_npc_object(xpos,ypos,link1,objid);
         //obj->set_npcinfo();
      }
      else
      {
         obj = new ua_object(xpos,ypos,link1,objid);
      }

      if ( (objid >= 0x40 && objid < 0x80) ||
           (objid >= 0x140 && objid != 458) )
         obj->set_type(ua_obj_invisible);
      else
         obj->set_type(ua_obj_object);

      // add to master object list
      master_obj_list[objpos] = ua_object_ptr(obj);

      // examine link2 and add where appropriate/known

      if (objpos==link1)
      {
         // throw ua_exception("while importing: very strange, object links to itself");
         break;
      }

      // add next obj in list
      objpos=link1;
   }
}
