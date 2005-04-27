--
-- Underworld Adventures - an Ultima Underworld hacking project
-- Copyright (c) 2002,2003,2004 Underworld Adventures Team
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; either version 2 of the License, or
-- (at your option) any later version.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
--
-- $Id$
--

--
-- objlist.lua - lua underworld script; object list functions
--

-- constants


-- tables


-- functions

-- called when looking at object in object list
function object_look(objpos)

   print("looking at object in world")
   dump_objinfo_table(objpos)

   objinfo = objlist.get_info(objpos)

   -- print info
   look_at_objinfo(objinfo,0)

   -- check for look trigger
   trigger_check_handle(trig_look,objpos)

end


-- called when talking to object in object list
function object_talk(objpos)

   print("talking with object\n")
   dump_objinfo_table(objpos)

   objinfo = objlist.get_info(objpos)

   -- check if npc
   if objinfo.item_id < 0x0040 or objinfo.item_id >= 0x0080
   then
      -- no, print "You cannot talk to that!"
      uw.print(uw.get_string(7,0))
      return
   end

   if objinfo.npc_whoami ~= nil
   then
      local conv_block = objinfo.npc_whoami

      -- generic conversation?
      if conv_block == 0
      then
         conv_block = objinfo.item_id-64+256  -- 0x0f00
      end

      -- check if conversation is available
      if conv.is_avail(conv_block)
      then
         -- start talking
         uw.start_conv(objpos);
      else
         -- no, print "You get no response."
         uw.print(uw.get_string(7,1))
      end
   end

end


-- called when using object in object list
function object_use(objpos)

   print( "using object " .. format("%04x",objpos) .. " name=" ..
      uw.get_string(4, objlist.get_info(objpos).item_id) )

   -- check for use trigger
   trigger_check_handle(trig_use,objpos)

end


-- called when (trying to) picking up an object
function object_get(objpos)

   -- check if the user already has item in hand
   if inventory.float_get_item() ~= inv_item_none
   then
      return
   end

   objinfo = objlist.get_info(objpos)

   -- check if object can be picked up
   if prop.get_common( objinfo.item_id ).can_be_picked_up ~= 1
   then
      -- print: "You cannot pick that up."
      uw.print(uw.get_string(1,96))
      return
   end

   -- check if object is near enough
   local xdist,ydist = player.get_pos()
   xdist = xdist - (objinfo.tilex + objinfo.xpos / 8.0)
   ydist = ydist - (objinfo.tiley + objinfo.ypos / 8.0)

   -- further away than 1.4 tiles?
   local dist = xdist*xdist + ydist*ydist
   if dist > 2.0
   then
      -- print: "That is too far away to take." or "You cannot reach that."
      uw.print(uw.get_string(1,93+random()*2))
      print( "object too far away; distance: " .. dist )
      return
   end

   -- add object to inventory and float it
   if inventory.float_add_item(objpos) ~= ua_slot_no_item
   then
      -- remove from master object list
      objlist.delete(objpos)
   end

end
