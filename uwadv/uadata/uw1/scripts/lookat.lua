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
-- lookat.lua - lua underworld script; generic "look at" functionality
--

-- constants

-- tables


-- functions


-- formats item name and article
function format_item_name(item_id,quantity)

   local article, name

   name = uw.get_string(4,item_id)

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


-- called to handle special cases for "look at"
function look_at_special(objinfo)

   -- some_writing
   if objinfo.item_id == 0x0166
   then
      -- print more detailed string
      uw.print( uw.get_string(8,objinfo.flags + 368) )
      uw.print( uw.get_string(8,objinfo.quantity-512) )

      return 1
   end

   -- special tmap object
   if objinfo.item_id == 0x016e or objinfo.item_id == 0x016f
   then
      -- simulate "look at wall"
      lua_wall_look(objinfo.owner)

      return 1
   end

   return 0
end


-- called to print a generic "look at" description
function look_at_objinfo(objinfo,is_inv)

   local quantity = 0

   -- treat some objects specially
   if look_at_special(objinfo) > 0
   then
      return
   end

   if objinfo.is_quantity > 0.0
   then
      quantity = objinfo.quantity
   end

   local article, name = format_item_name(objinfo.item_id, quantity)

   if strlen(article) > 0
   then
      article = article .. " "
   end

   -- is NPC?
   local attitude = ""
   local named = ""
   if objinfo.item_id >= 0x0040 and objinfo.item_id < 0x0080
   then
      -- get attitude string
      attitude = uw.get_string(5,96+objinfo.npc_attitude) .. " ";

      if attitude == "upset "
      then
         article = "an "
      end

      -- do "named" string
      if objinfo.npc_whoami ~= nil and objinfo.npc_whoami > 0
      then
         named = " named " .. uw.get_string(7,objinfo.npc_whoami+16)
      end

   end

   -- object owned by someone?
   local owner = ""
   if is_inv <= 0
   then
      -- check item properties
      prop_item = prop.get_common(objinfo.item_id)

      if prop_item.can_have_owner > 0 and objinfo.owner > 0 and objinfo.owner <= 28
      then
         -- do owner string
         owner = " belonging to" .. uw.get_string(1,objinfo.owner+370)
      end
   end

   uw.print("You see " .. article .. attitude .. name .. named .. owner)

   if is_inv > 0 - 1
   then
      -- take a more in-depth look at some objects

      if objinfo.item_id >= 0x0130 and objinfo.item_id < 0x0140
      then
         -- read book, 0x013x
         if objinfo.item_id == 0x013b
         then
            -- map scroll, "Enscribed upon the scroll is your map."
            uw.print(uw.get_string(1,151))
         else
            -- normal book
            uw.print("You read the " .. name .. "...")
            uw.print(uw.get_string(3,objinfo.quantity-512))
         end
      end

      -- TODO more look stuff
   end

end


function lua_wall_look(tex_id)

   if tex_id >= 256
   then
      tex_id = 510-(tex_id-256)
   end

   -- print description
   uw.print( "You see " .. uw.get_string(10,tex_id) )

end
