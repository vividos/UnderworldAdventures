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


-- dumps objlist table entries
function dump_objinfo_table(obj_handle)

   local objinfo = objlist_get_obj_info(obj_handle)
   local category = "quantity"

   if objinfo.is_quantity > 0
   then
      if objinfo.quantity >= 512
      then
         category = "special"
      end
   else
      category = "sp_link"
   end

   print( "dumping object info, obj_handle = " .. format("%04x",obj_handle) .."\n" ..

      " item_id = " .. format("%04x",objinfo.item_id) ..
      " (" .. ui_get_gamestring(4,objinfo.item_id) .. ")," ..

      " handle_next = " .. format("%04x",objinfo.handle_next) .. "\n" ..

      format(" quality = %04x,",objinfo.quality) ..
      format(" owner = %04x, ",objinfo.owner) ..
      category .. format(" = %04x\n",objinfo.quantity) ..

      " pos = " .. objinfo.xpos .. " / " .. objinfo.ypos ..
      ", height = " .. objinfo.zpos .. ", heading  = " .. objinfo.heading .. "\n" ..

      " enchanted = " .. objinfo.enchanted ..
      ", is_quantity   = " .. objinfo.is_quantity .. "\n"
   )

   if objinfo.npc_used > 0
   then
      print( " npc_hp = " .. objinfo.npc_hp .. "\n" ..
         " npc_goal = " .. objinfo.npc_goal .. "\n" ..
         " npc_gtarg = " .. objinfo.npc_gtarg .. "\n" ..
         " npc_level = " .. objinfo.npc_level .. "\n" ..
         " npc_talkedto = " .. objinfo.npc_talkedto .. "\n" ..
         " npc_attitude = " .. objinfo.npc_attitude .. "\n" ..
         " npc_xhome = " .. objinfo.npc_xhome .. "\n" ..
         " npc_yhome = " .. objinfo.npc_yhome .. "\n" ..
         " npc_hunger = " .. objinfo.npc_hunger .. "\n" ..
         " npc_whoami = " .. objinfo.npc_whoami .. "\n"
       )
   end
end

-- function to format item name and return
function format_item_name(item_id,quantity)

   local article, name

   name = ui_get_gamestring(4,item_id)

   local pos_amp = strfind(name, "&", 1, 1)

   -- more than one object?
   if pos_amp ~= nil
   then
      if quantity > 1
      then
         -- take "plural" part of description
         name = strsub(name, pos_amp+1)
      else
         -- delete plural part of string
         name = strsub(name, 1, pos_amp-1)
      end
   end

   -- find out article
   local pos_us = strfind(name, "_", 1, 1)

   if pos_us == nil
   then
      article = ""
   else
      -- take article
      article = strsub(name, 1, pos_us-1)

      -- delete article from name
      name = strsub(name, pos_us+1)
   end

   return article, name
end


-- called when looking at object in object list
function lua_objlist_look(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   print("looking at object")
   dump_objinfo_table(obj_handle)

   local quantity = 0

   if objinfo.is_link ~= 1
   then
      quantity = objinfo.quantity
   end

   local article, name = format_item_name(objinfo.item_id, quantity)

   if strlen(article) > 0
   then
      article = article .. " "
   end

   -- is NPC?
   attitude = ""
   named = ""
   if objinfo.item_id >= 64 and objinfo.item_id < 128
   then
      -- get attitude string
      --attitude = ui_get_gamestring(5,96+objinfo.npc_attitude) .. " ";

      if attitude == "upset "
      then
         article = "an "
      end

      -- do "named" string
      if objinfo.npc_whoami ~= nil and objinfo.npc_whoami > 0
      then
         named = " named " .. ui_get_gamestring(7,objinfo.npc_whoami+16)
      end

   end

   -- object owned by someone?
   owner = ""
   if (objinfo.item_id < 64 or objinfo.item_id >= 128) and
      objinfo.item_id < 320  and objinfo.owner > 0
   then
      local article,name = format_item_name(objinfo.owner-1+64, 0)

      -- do owner string
      owner = " belonging to " .. article .. " " .. name
   end

   ui_print_string(
     "You see " .. article .. attitude .. name .. named .. owner)

   -- check for look trigger
   trig_check_handle(trig_look,obj_handle)

end


-- called when talking to object in object list
function lua_objlist_talk(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   print("talking with object\n")
   dump_objinfo_table(obj_handle)

   -- check if npc
   if objinfo.item_id < 64 or objinfo.item_id >= 128
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