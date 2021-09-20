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
-- inventory.lua - lua underworld script; inventory functions
--

-- constants


-- globals


-- tables


-- functions

-- prints "look" description
function inventory_look(inv_pos)

   print("looking at object in inventory")

   dump_invinfo_table(inv_pos)

   inv_objinfo = inventory.get_info(inv_pos)

   if inv_objinfo.item_id ~= inventory_slot_no_item
   then
      look_at_objinfo(inv_objinfo,1)
   end
end


-- prints "use" description
function inventory_use(inv_pos)

   print("using object in inventory")

   dump_invinfo_table(inv_pos)

   inv_objinfo = inventory.get_info(inv_pos)

   if inv_objinfo.item_id == 0x013b -- a_map
   then
      uw.show_map();
   else
      -- TODO implement using items
   end

end
