--
-- Underworld Adventures - an Ultima Underworld remake project
-- Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
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

--
-- dump.lua - lua underworld script; dump functions
--

-- constants


-- tables


-- functions


-- dumps objlist table entries
function dump_objinfo_table(objpos)

   local objinfo = objlist.get_info(objpos)
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

   print( "dumping object info, objpos = " .. string.format("%04x",objpos) .."\n" ..

      " item_id = " .. string.format("%04x",objinfo.item_id) ..
      " (" .. uw.get_string(4,objinfo.item_id) .. ")," ..

      " link_next = " .. string.format("%04x",objinfo.link_next) .. "\n" ..

      string.format(" quality = %04x,",objinfo.quality) ..
      string.format(" owner = %04x, ",objinfo.owner) ..
      category .. string.format(" = %04x\n",objinfo.quantity) ..

      " pos = " .. objinfo.xpos .. " / " .. objinfo.ypos ..
      ", height = " .. objinfo.zpos .. ", heading  = " .. objinfo.heading ..
      string.format(", tile = %02x/%02x\n", objinfo.tilex, objinfo.tiley) ..

      " enchanted = " .. objinfo.enchanted ..
      ", is_quantity = " .. objinfo.is_quantity ..
      ", flags = " .. string.format("%04x\n",objinfo.flags)
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

-- dumps inventory item info table entries
function dump_invinfo_table(inv_pos)

   objinfo = inventory.get_info(inv_pos);

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

   print( "dumping inventory item info, inv_pos = " .. string.format("%04x",inv_pos) .."\n" ..
      " item_id = " .. string.format("%04x",objinfo.item_id) ..
      " (" .. uw.get_string(4,objinfo.item_id) .. ")," ..

      " link = " .. string.format("%04x",objinfo.link) .. "\n" ..

      string.format(" quality = %04x,",objinfo.quality) ..
      string.format(" owner = %04x, ",objinfo.owner) ..
      category .. string.format(" = %04x\n",objinfo.quantity) ..

      " enchanted = " .. objinfo.enchanted ..
      ", is_quantity = " .. objinfo.is_quantity ..
      ", flags = " .. string.format("%04x\n",objinfo.flags)
   )

end
