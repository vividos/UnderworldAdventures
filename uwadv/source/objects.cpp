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
/*! \file objects.cpp

   \brief object method implementation

*/

// needed includes
#include "common.hpp"
#include "objects.hpp"


// ua_object_info methods

ua_object_info::ua_object_info()
:item_id(ua_item_none), link(0), quality(0), owner(0), quantity(0),
 flags(0), enchanted(false), is_hidden(false), is_quantity(true)
{
}

void ua_object_info::load_info(ua_savegame& sg)
{
   // read basic object info
   item_id = sg.read16();

   if (item_id == ua_item_none)
      return; // nothing more to read

   link = sg.read16();
   quality = sg.read16();
   owner = sg.read16();
   quantity = sg.read16();

   flags = sg.read8();

   enchanted = sg.read8() != 0;
   is_hidden = sg.read8() != 0;
   is_quantity = sg.read8() != 0;

   // dummy values
   sg.read8();
   sg.read8();
}

void ua_object_info::save_info(ua_savegame& sg)
{
   // write basic object info
   sg.write16(item_id);

   if (item_id == ua_item_none)
      return; // nothing more to write

   sg.write16(link);
   sg.write16(quality);
   sg.write16(owner);
   sg.write16(quantity);

   sg.write8(flags);

   sg.write8(enchanted ? 1 : 0);
   sg.write8(is_hidden ? 1 : 0);
   sg.write8(is_quantity ? 1 : 0);

   // dummy values
   sg.write8(0);
   sg.write8(0);
}


// ua_object_info_ext methods

ua_object_info_ext::ua_object_info_ext()
:xpos(0), ypos(0), zpos(0), heading(0),
 npc_used(false),
 npc_hp(0), npc_goal(0), npc_gtarg(0), npc_level(0),
 npc_talkedto(0), npc_attitude(0), npc_xhome(0), npc_yhome(0), 
 npc_hunger(0), npc_whoami(0)
{
}

void ua_object_info_ext::load_extinfo(ua_savegame& sg)
{
   // read extended object info
   xpos = sg.read8() & 7;
   ypos = sg.read8() & 7;
   zpos = sg.read8();
   heading = sg.read8() & 7;

   npc_used = sg.read8()!=0;
   if (npc_used)
   {
      npc_hp = sg.read8();

      npc_goal = sg.read8();
      npc_gtarg = sg.read8();
      npc_level = sg.read8();
      npc_talkedto = sg.read8() != 0;
      npc_attitude = sg.read8();

      npc_xhome = sg.read8();
      npc_yhome = sg.read8();

      npc_hunger = sg.read8();
      npc_whoami = sg.read8();

      // read placeholder values
      sg.read16();
      sg.read16();
      sg.read16();
   }
}

void ua_object_info_ext::save_extinfo(ua_savegame& sg)
{
   // write extended object info
   sg.write8(xpos & 7);
   sg.write8(ypos & 7);
   sg.write8(zpos);
   sg.write8(heading & 7);

   sg.write8(npc_used ? 1 : 0);
   if (npc_used)
   {
      sg.write8(npc_hp);

      sg.write8(npc_goal);
      sg.write8(npc_gtarg);
      sg.write8(npc_level);
      sg.write8(npc_talkedto ? 1 : 0);
      sg.write8(npc_attitude);

      sg.write8(npc_xhome);
      sg.write8(npc_yhome);

      sg.write8(npc_hunger);
      sg.write8(npc_whoami);

      // write placeholder values
      sg.write16(0);
      sg.write16(0);
      sg.write16(0);
   }
}


// ua_object methods

void ua_object::load_object(ua_savegame &sg)
{
   info.load_info(sg);

   if (info.item_id != ua_item_none)
      extinfo.load_extinfo(sg);
}

void ua_object::save_object(ua_savegame &sg)
{
   info.save_info(sg);

   if (info.item_id != ua_item_none)
      extinfo.save_extinfo(sg);
}


// ua_object_list methods

ua_object_list::ua_object_list()
{
   // do empty object list
   master_obj_list.resize(0x400);
}

ua_object_list::~ua_object_list()
{
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
      master_obj_list[n].save_object(sg);
}
