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
