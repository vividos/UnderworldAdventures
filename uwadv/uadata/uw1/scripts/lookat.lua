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
-- lookat.lua - lua underworld script; generic "look at" functionality
--

-- constants

-- tables


-- functions


-- formats item name and article
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


-- called to print a generic "look at" description
function look_at_objinfo(objinfo,is_inv)

   local quantity = 0

   -- treat some objects specially

   -- special tmap object
   if objinfo.item_id == 366 or objinfo.item_id == 367
   then
      -- simulate "look at wall"
      lua_wall_look(objinfo.owner)

      return
   end

   if objinfo.is_link ~= 1
   then
      quantity = objinfo.quantity
   end

   print(" quantity=" .. quantity)

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
      attitude = ui_get_gamestring(5,96+objinfo.npc_attitude) .. " ";

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
   if is_inv <= 0
   then
      -- check item properties
      prop_item = prop_get_common(objinfo.item_id)

      if prop_item.can_have_owner > 0 and objinfo.owner > 0
      then
         -- do owner string
         owner = " belonging to" .. ui_get_gamestring(1,objinfo.owner+370)
      end
   end

   ui_print_string(
     "You see " .. article .. attitude .. name .. named .. owner)

end


function lua_wall_look(tilex,tiley,tex_id)

   -- print description
   ui_print_string( "You see " .. ui_get_gamestring(10,tex_id) )

end
