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


// ua_object_info methods

ua_object_info::ua_object_info()
:item_id(ua_item_none), enchanted(false),
 is_link(false),quality(0), link(0), owner(0), quantity(0)
{
}


// ua_object_info_ext methods

ua_object_info_ext::ua_object_info_ext()
:xpos(0.0), ypos(0.0), zpos(0), dir(0)
{
}


// ua_object methods

void ua_object::load_object(ua_savegame &sg)
{
   // read basic object info
   info.item_id = sg.read16();

   if (info.item_id == ua_item_none)
      return; // nothing more to read

   info.enchanted = sg.read8()==1;
   info.is_link = sg.read8()==1;

   info.quality = sg.read16();
   info.link = sg.read16();
   info.owner = sg.read16();
   info.quantity = sg.read16();

   // read extended object info
   extinfo.xpos = sg.read32()/256.0;
   extinfo.ypos = sg.read32()/256.0;
   extinfo.zpos = sg.read16();
   extinfo.dir = sg.read16();

   // read data array
   info.data.clear();
   Uint16 max = sg.read16();
   for(Uint16 i=0; i<max; i++)
      info.data.push_back(sg.read16());
}

void ua_object::save_object(ua_savegame &sg)
{
   // write basic object info
   sg.write16(info.item_id);

   if (info.item_id == ua_item_none)
      return; // nothing more to write

   sg.write8(info.enchanted? 1 : 0);
   sg.write8(info.is_link? 1 : 0);

   sg.write16(info.quality);
   sg.write16(info.link);
   sg.write16(info.owner);
   sg.write16(info.quantity);

   // write extended object info
   sg.write32(static_cast<Uint32>(extinfo.xpos*256.0));
   sg.write32(static_cast<Uint32>(extinfo.ypos*256.0));
   sg.write16(extinfo.zpos);
   sg.write16(extinfo.dir);

   // write data array
   Uint16 max = info.data.size();
   sg.write16(max);
   for(Uint16 i=0; i<max; i++)
      sg.write16(info.data[i]);
}


// ua_object_list methods

ua_object_list::ua_object_list()
{
}

ua_object_list::~ua_object_list()
{
   master_obj_list.clear();
}

Uint16 ua_object_list::get_tile_list_start(unsigned int xpos, unsigned int ypos)
{
   return tile_index[ypos*64+xpos];
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
      master_obj_list[n].load_object(sg);
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
