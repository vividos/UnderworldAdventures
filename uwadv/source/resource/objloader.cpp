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

   // read in master object list
   Uint32 objprop[0x400*2];
   Uint8 npcinfo[0x100*19];

   for(Uint16 item=0; item<0x400; item++)
   {
      objprop[item*2+0] = fread32(fd);
      objprop[item*2+1] = fread32(fd);

      if (item<0x100)
      {
         // read NPC info bytes
         for(unsigned int n=0; n<19; n++)
            npcinfo[item*19+n] = fgetc(fd);
      }
   }

   // now that we have the two lists, follow each tile ref
   for(unsigned int n=0; n<64*64; n++)
      if (tile_index[n] != 0)
         addobj_follow(objprop,npcinfo,tile_index[n]);
}

//! retrieves "count" bits from "value", starting at bit "start"
Uint32 ua_get_bits(Uint32 value, unsigned int start, unsigned int count)
{
   return (value>>start) & ((1<<count)-1);
}

void ua_object_list::addobj_follow(Uint32 objprop[0x400*2],
   Uint8 npcinfo[0x100*19],Uint16 objpos)
{
   while(objpos!=0)
   {
      if (master_obj_list[objpos].get_object_info().item_id != ua_item_none)
         break; // we already had that object

      // get object properties
      Uint32 word1 = objprop[objpos*2+0];
      Uint32 word2 = objprop[objpos*2+1];

      // word 1
      Uint16 item_id =     ua_get_bits(word1, 0, 9);
      //Uint16 flags =       ua_get_bits(word1, 9, 6);
      Uint16 enchanted =   ua_get_bits(word1, 12, 1);
      Uint16 is_quantity = ua_get_bits(word1, 15, 1);

      Uint16 zpos = ua_get_bits(word1, 16, 7);
      Uint16 dir  = ua_get_bits(word1, 23, 3);
      Uint16 ypos = ua_get_bits(word1, 26, 3);
      Uint16 xpos = ua_get_bits(word1, 29, 3);

      // word 2
      Uint16 quality =  ua_get_bits(word2, 0, 6);
      Uint16 link  =    ua_get_bits(word2, 6, 10);
      Uint16 owner =    ua_get_bits(word2, 16, 6);
      Uint16 quantity = ua_get_bits(word2, 22, 10);

      // generate object
      ua_object obj;

      // basic object info
      {
         ua_object_info& info = obj.get_object_info();

         info.item_id = item_id;
         info.enchanted = enchanted == 1;
         info.is_link = is_quantity == 0;

         info.quality = quality;
         info.link = link;
         info.owner = owner;
         info.quantity = quantity;

         if (objpos<0x0100)
         {
            Uint8* data = &npcinfo[objpos*19];

            // npc_whoami
            info.data.push_back(data[18]);

            // npc_attitude
            info.data.push_back(data[6]>>6);

            // npc_hp
            info.data.push_back(data[0]);

            // npc_xhome, npc_yhome
            info.data.push_back(((data[14]|(data[15]<<8))>>5)&0x3f);
            info.data.push_back(data[14]&0x3f);

            // TODO: store more data from NPC
         }
      }

      // extended object info
      {
         ua_object_info_ext& extinfo = obj.get_ext_object_info();
         extinfo.xpos = (xpos+0.5)/8.0;
         extinfo.ypos = (ypos+0.5)/8.0;
         extinfo.zpos = zpos/4.0;
         extinfo.dir = dir;
      }

      // add to master object list
      master_obj_list[objpos] = obj;

      // examine special property and add recursively
      if (is_quantity==0)
         addobj_follow(objprop,npcinfo,quantity);

      // add next object in chain
      objpos = link;
   }
}
