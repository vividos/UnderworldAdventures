--
-- Underworld Adventures - an Ultima Underworld hacking project
-- Copyright (c) 2002,2003 Underworld Adventures Team
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
function lua_objlist_look(obj_handle)

   print("looking at object in world")
   dump_objinfo_table(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   -- print info
   look_at_objinfo(objinfo,0)

   -- check for look trigger
   trig_check_handle(trig_look,obj_handle)

end


-- called when talking to object in object list
function lua_objlist_talk(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   print("talking with object\n")
   dump_objinfo_table(obj_handle)

   -- check if npc
   if objinfo.item_id < 0x0040 or objinfo.item_id >= 0x0080
   then
      -- no, print "You cannot talk to that!"
      ui_print_string(ui_get_gamestring(7,0))
      return
   end

   if objinfo.npc_whoami ~= nil
   then
      local conv = objinfo.npc_whoami

      -- generic conversation?
      if conv == 0
      then
         conv = objinfo.item_id-64+256  -- 0x0f00
      end

      -- check if conversation is available
      if conv_is_avail(conv)
      then
         -- start talking
         ui_start_conv(obj_handle);
      else
         -- no, print "You get no response."
         ui_print_string(ui_get_gamestring(7,1))
      end
   end

end


-- called when using object in object list
function lua_objlist_use(obj_handle)

   print( "using object " .. format("%04x",obj_handle) .. " name=" ..
      ui_get_gamestring(4, objlist_get_obj_info(obj_handle).item_id) )

   -- check for use trigger
   trig_check_handle(trig_use,obj_handle)

end


-- called when (trying to) picking up an object
function lua_objlist_get(obj_handle)

   -- check if the user already has item in hand
   if inv_float_get_item() ~= inv_item_none
   then
      return
   end

   objinfo = objlist_get_obj_info(obj_handle)

   -- check if object can be picked up
   if prop_get_common( objinfo.item_id ).can_be_picked_up ~= 1
   then
      -- print: "You cannot pick that up."
      ui_print_string(ui_get_gamestring(1,96))
      return
   end

   -- check if object is near enough
   local xdist,ydist = player_get_pos()
   xdist = xdist - (objinfo.tilex + objinfo.xpos / 8.0)
   ydist = ydist - (objinfo.tiley + objinfo.ypos / 8.0)

   -- further away than 1.4 tiles?
   local dist = xdist*xdist + ydist*ydist
   if dist > 2.0
   then
      -- print: "That is too far away to take." or "You cannot reach that."
      ui_print_string(ui_get_gamestring(1,93+random()*2))
      print( "object too far away; distance: " .. dist )
      return
   end

   -- add object to inventory and float it
   if inv_float_add_item(obj_handle) ~= ua_slot_no_item
   then
      -- remove from master object list
      objlist_delete_obj(obj_handle)
   end

end
