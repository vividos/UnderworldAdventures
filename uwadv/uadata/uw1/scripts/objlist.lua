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
function dump_objinfo_table(objinfo,obj_handle)

   print( " lua_objlist_look(" .. obj_handle ..")\n" ..
      "  item_id   = " .. objinfo.item_id .. " (" ..
         ui_get_gamestring(4,objinfo.item_id) .. ")\n" ..
      "  enchanted = " .. objinfo.enchanted .. "\n" ..
      "  is_link   = " .. objinfo.is_link .. "\n\n" ..

      "  zpos = " .. objinfo.zpos .. "\n" ..
      "  dir  = " .. objinfo.dir .. "\n" ..
      "  ypos = " .. objinfo.ypos .. "\n" ..
      "  xpos = " .. objinfo.xpos .. "\n\n" ..

      "  quality     = " .. objinfo.quality .. "\n" ..
      "  handle_next = " .. objinfo.handle_next .. "\n" ..
      "  owner       = " .. objinfo.owner .. "\n" ..
      "  quantity    = " .. objinfo.quantity .. "\n\n"
   )
end


-- function to format item name and return
function format_item_name(item_id,quantity)

   local article, name

   name = ui_get_gamestring(4,item_id)

   local pos_amp = strfind(name, "&", 1, 1)

   -- more than one object?
   if pos_amp ~= nil then
      if quantity > 1 then
         -- take "plural" part of description
         name = strsub(name, pos_amp+1)
      else
         -- delete plural part of string
         name = strsub(name, 1, pos_amp-1)
      end
   end

   -- find out article
   local pos_us = strfind(name, "_", 1, 1)

   if pos_us == nil then
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

   dump_objinfo_table(objinfo,obj_handle)

   local quantity = 0

   if objinfo.is_link ~= 1 then
      quantity = objinfo.quantity
   end

   local article, name = format_item_name(objinfo.item_id, quantity)

   if strlen(article) > 0 then
      article = article .. " "
   end

   -- is NPC?
   mood = ""
   named = ""
   if objinfo.item_id >= 64 and objinfo.item_id < 128 then
      -- do mood string

      -- todo: ui_get_gamestring(5,96-99) (99=friendly)

      -- do "named" string
      if objinfo.data.size > 0 and objinfo.data[0] > 0 then

         print("name byte is: " .. objinfo.data[0])
         named = " named " .. ui_get_gamestring(7,objinfo.data[0]+16)
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
     "You see " .. article ..
     mood .. name .. named .. owner)

end

function lua_objlist_talk(obj_handle)

   objinfo = objlist_get_obj_info(obj_handle)

   dump_objinfo_table(objinfo,obj_handle)

   if objinfo.item_id < 64 or objinfo.item_id >= 128
   then
      ui_print_string(ui_get_gamestring(1,156))
      return
   end

   if objinfo.data.size > 0
   then
      local conv = objinfo.data[0]

      if conv > 0 and conv < 255
      then
         ui_start_conv(obj_handle)
      end
   end

end
