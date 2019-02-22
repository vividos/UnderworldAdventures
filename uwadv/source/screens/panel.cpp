//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2004,2019 Underworld Adventures Team
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
/// \file panel.cpp
/// \brief inventory/stats/rune bag panel
//
#include "common.hpp"
#include "panel.hpp"
#include "underworld.hpp"
#include "scripting.hpp"
#include "gamestrings.hpp"
#include "uamath.hpp"
#include <sstream>

/// time to rotate panel
const double ua_panel_rotate_time = 2.5;

/// inventory area types
enum ua_panel_inventory_area_id
{
   ua_area_inv_slot0 = 0,
   ua_area_inv_slot1,
   ua_area_inv_slot2,
   ua_area_inv_slot3,
   ua_area_inv_slot4,
   ua_area_inv_slot5,
   ua_area_inv_slot6,
   ua_area_inv_slot7,

   ua_area_inv_container,
   ua_area_inv_scroll_up,
   ua_area_inv_scroll_down,

   ua_area_equip_left_hand,
   ua_area_equip_left_shoulder,
   ua_area_equip_left_ring,

   ua_area_equip_right_hand,
   ua_area_equip_right_shoulder,
   ua_area_equip_right_ring,

   ua_area_paperdoll_head,
   ua_area_paperdoll_chest,
   ua_area_paperdoll_hand,
   ua_area_paperdoll_legs,
   ua_area_paperdoll_feet,

   ua_area_none
};

/// inventory area table
struct {
   ua_panel_inventory_area_id area_id;
   unsigned int xmin, xmax, ymin, ymax;
} ua_panel_inventory_areas[] =
{
   { ua_area_inv_slot0,  5, 24, 76,  93 },
   { ua_area_inv_slot1, 25, 43, 76,  93 },
   { ua_area_inv_slot2, 44, 62, 76,  93 },
   { ua_area_inv_slot3, 63, 80, 76,  93 },
   { ua_area_inv_slot4,  5, 24, 94, 110 },
   { ua_area_inv_slot5, 25, 43, 94, 110 },
   { ua_area_inv_slot6, 44, 62, 94, 110 },
   { ua_area_inv_slot7, 63, 80, 94, 110 },

   { ua_area_inv_container,    5, 24, 60, 75 },
   { ua_area_inv_scroll_up,   61, 70, 64, 75 },
   { ua_area_inv_scroll_down, 71, 80, 64, 75 },

   { ua_area_equip_left_hand,      7, 24, 29, 45 },
   { ua_area_equip_left_shoulder, 10, 27,  7, 23 },
   { ua_area_equip_left_ring,     24, 31, 50, 57 },

   { ua_area_equip_right_hand,     61, 77, 29, 45 },
   { ua_area_equip_right_shoulder, 59, 75,  7, 23 },
   { ua_area_equip_right_ring,     55, 62, 50, 57 },

   { ua_area_paperdoll_head,  36, 49,  5, 22 },
   { ua_area_paperdoll_chest, 34, 51, 23, 44 },
   { ua_area_paperdoll_hand,  27, 33, 38, 49 },
   { ua_area_paperdoll_hand,  52, 58, 38, 49 }, // 2nd hand area
   { ua_area_paperdoll_legs,  34, 51, 45, 60 },
   { ua_area_paperdoll_feet,  32, 54, 61, 75 },
};

ua_panel::ua_panel()
{
}

void ua_panel::init(ua_panel_parent_interface* the_panel_parent, unsigned int xpos,
   unsigned int ypos)
{
   panel_parent = the_panel_parent;
   IGame& game = panel_parent->get_game_interface();

   panel_type = ua_panel_inventory;
   armor_female = game.GetUnderworld().GetPlayer().GetAttribute(Underworld::attrGender) != 0;
   tickrate = game.get_tickrate();
   stats_scrollstart = 0;
   rotate_panel = false;
   rotate_angle = 0.0;
   rotation_oldstyle = !game.get_settings().GetBool(Base::settingUwadvFeatures);

   slot_start = 0;
   check_dragging = false;

   // load image lists
   ImageManager& img_manager = game.get_image_manager();

   img_manager.load_list(img_panels, "panels", 0, 3);
   img_manager.load_list(img_chains, "chains");
   img_manager.load(img_inv_bagpanel, "inv", 6);
   img_manager.load_list(img_inv_updown, "buttons", 27, 29);

   img_manager.load_list(img_armor, armor_female ? "armor_f" : "armor_m");

   img_manager.load_list(img_bodies, "bodies");
   img_manager.load_list(img_objects, "objects");

   // load fonts
   font_stats.load(game.get_settings(), ua_font_italic);
   font_weight.load(game.get_settings(), ua_font_normal);

   // create image
   get_image().create(85, 116);
   get_image().set_palette(img_manager.get_palette(0));

   ua_image_quad::init(game, xpos, ypos);

   {
      img_chains_top.init(game, xpos + 37, ypos - 4);
      img_chains_top.get_image().create(10, 4);
      img_chains_top.get_image().set_palette(game.get_image_manager().get_palette(0));

      img_chains_bottom.init(game, xpos + 37, ypos + 114);
      img_chains_bottom.get_image().create(10, 18);
      img_chains_bottom.get_image().set_palette(game.get_image_manager().get_palette(0));

      update_chains();
   }

   update_panel();
}

void ua_panel::destroy()
{
   ua_image_quad::destroy();
}

void ua_panel::draw()
{
   if (rotate_panel)
   {
      // save position
      unsigned int save_xpos = wnd_xpos;
      unsigned int save_width = wnd_width;

      // calculate new position
      if (rotation_oldstyle)
      {
         // old-style: calculate angles in steps of 22.5
         double angle = (unsigned(rotate_angle / 180.0 * 8.0) / 8.0 * 180.0);

         // ensure at least a small panel
         if (angle > 89.0 && angle < 91.0) angle = 87.0;

         wnd_width = unsigned(save_width*fabs(cos(ua_deg2rad(angle))));
      }
      else
      {
         // new-style: smooth
         wnd_width = unsigned(save_width*fabs(cos(ua_deg2rad(rotate_angle))));
      }

      wnd_xpos = save_xpos + save_width / 2 - wnd_width / 2;

      ua_image_quad::draw();

      wnd_xpos = save_xpos;
      wnd_width = save_width;
   }
   else
      ua_image_quad::draw();

   // also draw chains
   img_chains_top.draw();
   img_chains_bottom.draw();
}

/// \todo when pressing bottom chain, check if user has an item on cursor
/// and prevent switching then.
bool ua_panel::process_event(SDL_Event& event)
{
   // don't process events when in rotate mode
   if (rotate_panel)
      return ua_image_quad::process_event(event);

   // check if bottom chain was pressed
   if (event.type == SDL_MOUSEBUTTONUP)
   {
      unsigned int xpos = 0, ypos = 0;
      calc_mousepos(event, xpos, ypos);

      // in window?
      if (img_chains_bottom.in_window(xpos, ypos))
      {
         rotate_panel = true;
         rotate_angle = 0.0;

         // change to stats when in inventory, and to inventory when not
         if (panel_type != ua_panel_inventory)
            rotate_panel_type = ua_panel_inventory;
         else
            rotate_panel_type = ua_panel_stats;
      }
   }

   return ua_image_quad::process_event(event);
}

void ua_panel::mouse_event(bool button_clicked, bool left_button,
   bool button_down, unsigned int mousex, unsigned int mousey)
{
   if (panel_type == ua_panel_inventory)
   {
      ua_panel_inventory_area_id area = ua_area_none;

      mousex -= wnd_xpos; mousey -= wnd_ypos;

      // determine area
      for (unsigned int i = 0; i < SDL_TABLESIZE(ua_panel_inventory_areas); i++)
      {
         // check ranges
         if (mousex >= ua_panel_inventory_areas[i].xmin &&
            mousex <= ua_panel_inventory_areas[i].xmax &&
            mousey >= ua_panel_inventory_areas[i].ymin &&
            mousey <= ua_panel_inventory_areas[i].ymax)
         {
            area = ua_panel_inventory_areas[i].area_id;
            break;
         }
      }

      if (button_clicked)
         inventory_click(button_down, left_button, area);
      else
      {
         // check item dragging
         if (check_dragging && drag_area != static_cast<unsigned int>(area))
         {
            Underworld::Inventory& inv = panel_parent->get_game_interface().
               GetUnderworld().GetPlayer().GetInventory();

            // user dragged item out of area
            check_dragging = false;
            inv.FloatObject(drag_item);

            update_cursor_image();
            update_panel();

         }
      }
      /*
      #ifdef HAVE_DEBUG
            if (area != ua_area_none)
            {
               update_inventory();
               get_image().IndexedImage::fill_rect(
                  ua_panel_inventory_areas[i].xmin,
                  ua_panel_inventory_areas[i].ymin,
                  ua_panel_inventory_areas[i].xmax-ua_panel_inventory_areas[i].xmin,
                  ua_panel_inventory_areas[i].ymax-ua_panel_inventory_areas[i].ymin, 13);
               update();
            }
      #endif
      */
   }

   if (panel_type == ua_panel_stats && button_clicked && !button_down)
   {
      mousex -= wnd_xpos; mousey -= wnd_ypos;
      if (mousex >= 29 && mousex <= 56 && mousey >= 106 && mousey <= 114)
      {
         bool leftbutton = mousex < 43;
         if (leftbutton)
         {
            if (stats_scrollstart > 0)
               stats_scrollstart--;
         }
         else
         {
            if (stats_scrollstart < 14)
               stats_scrollstart++;
         }
         update_panel();
      }
   }

   if (panel_type == ua_panel_runebag && button_clicked && !button_down)
   {
      mousex -= wnd_xpos; mousey -= wnd_ypos;
      if (mousex >= 8 && mousex < 80 && mousey >= 7 && mousey < 96)
      {
         // calculate which rune we hit
         mousex -= 8;
         mousey -= 7;
         mousex /= 18;
         mousey /= 15;

         UaTrace("runebag: mouse hit rune %u\n", mousex + mousey * 4);
      }
      else
      {
         if (mousex >= 21 && mousex < 68 && mousey >= 96 && mousey < 111)
         {
            UaTrace("runebag: mouse hit \"reset shelf\" area\n");
         }
         else
            UaTrace("runebag: mouse hit outside\n");
      }
   }
}

void ua_panel::tick()
{
   if (rotate_panel)
   {
      double old_angle = rotate_angle;
      rotate_angle += 180.0 / (ua_panel_rotate_time * tickrate);

      // check if we have to update the chains
      if (unsigned(old_angle / 180.0*8.0) != unsigned(rotate_angle / 180.0*8.0))
         update_chains();

      // do we have to change panel image?
      if (rotate_angle >= 90.0)
      {
         panel_type = rotate_panel_type;
         update_panel();
      }

      // check for rotation end
      if (rotate_angle >= 180.0)
      {
         rotate_panel = false;
         update_chains();
      }
   }
}

void ua_panel::update_panel()
{
   // inventory panel?
   if (panel_type == ua_panel_inventory)
      update_inventory();
   else
      // stats panel?
      if (panel_type == ua_panel_stats)
         update_stats();
      else
         // runebag panel?
         if (panel_type == ua_panel_runebag)
            update_runebag();

   update();
}

void ua_panel::update_chains()
{
   unsigned int chain_add = rotate_panel ?
      unsigned(rotate_angle / 180.0*8.0) % 8 : 0;

   // set chains images
   img_chains_top.get_image().paste_image(img_chains[8 + (chain_add) % 8], 0, 0);
   img_chains_bottom.get_image().paste_image(img_chains[chain_add % 8], 0, 0);

   img_chains_top.update();
   img_chains_bottom.update();
}

void ua_panel::update_inventory()
{
   IndexedImage& img = get_image();

   Underworld::Inventory& inv = panel_parent->get_game_interface().
      GetUnderworld().GetPlayer().GetInventory();
   Underworld::Player& pl = panel_parent->get_game_interface().
      GetUnderworld().GetPlayer();

   // background image
   img.paste_image(img_panels[0], 1, 1);

   bool female = pl.GetAttribute(Underworld::attrGender) != 0;

   // player appearance
   {
      unsigned int bodyimg = pl.GetAttribute(Underworld::attrAppearance) % 5 +
         (female ? 5 : 0);

      img.paste_image(img_bodies[bodyimg], 25, 5);
   }

   // check if paperdoll images have to be reloaded (e.g. when changed in
   // debugger)
   if (female != armor_female)
   {
      armor_female = female;

      img_armor.clear();
      panel_parent->get_game_interface().get_image_manager().
         load_list(img_armor, armor_female ? "armor_f" : "armor_m");
   }

   // start index; 0 is used in the main inventory
   unsigned int start = 0;

   Uint16 cont_id = inv.GetObjectInfo(inv.GetContainerPos()).m_itemID;
   if (cont_id != Underworld::c_itemIDNone)
   {
      // inside a container

      // draw alternative inventory panel
      img.paste_image(img_inv_bagpanel, 1, 74);

      // draw up/down arrows
      if (slot_start > 0)
         img.paste_image(img_inv_updown[0], 61, 64, true); // up

      if (inv.GetNumSlots() - slot_start >= 8)
         img.paste_image(img_inv_updown[1], 71, 64, true); // down

      // draw container we're in
      img.paste_image(img_objects[cont_id], 6, 58, true);

      // begin at current slot start
      start = slot_start;
   }
   // paste inventory slots
   for (unsigned int i = 0; i < 8; i++)
   {
      if (start + i < inv.GetNumSlots())
      {
         Uint16 item_pos = inv.GetSlotListPos(start + i);
         Uint16 item_id = inv.GetObjectInfo(item_pos).m_itemID;

         // draw item
         if (item_id != 0xffff)
         {
            unsigned int
               destx = 7 + (i & 3) * 19,
               desty = 76 + (i >> 2) * 18;

            img.paste_image(img_objects[item_id], destx, desty, true);
         }
      }
   }

   // do paperdoll items
   for (unsigned int j = Underworld::slotLeftHand; j < Underworld::slotMax; j++)
   {
      // paperdoll image coordinates
      static unsigned int slot_coords[] =
      {
           5,27,  59,27, // hand
           8, 5,  57, 5, // shoulder
          18,43,  48,43, // finger
          32,19,         // legs
          26,15,         // chest
          25,35,         // hands
          30,59,         // feet
          31, 3,         // head
      };

      unsigned int destx, desty;
      destx = slot_coords[(j - Underworld::slotLeftHand) * 2] + 1;
      desty = slot_coords[(j - Underworld::slotLeftHand) * 2 + 1] + 1;

      // paste item
      Uint16 item_id = inv.GetObjectInfo(j).m_itemID;
      if (item_id != 0xffff)
      {
         if (j < Underworld::slotPaperdollStart)
         {
            // normal object
            img.paste_image(img_objects[item_id], destx, desty, true);
         }
         else
         {
            // paperdoll image
            unsigned int quality = 3; // can be between 0..3
            Uint16 armorimg = item_id < 0x002f ?
               (item_id - 0x0020) + 15 * quality : (item_id - 0x002f + 60);

            img.paste_image(img_armor[armorimg], destx, desty, true);
         }
      }
   }

   // inventory weight
   {
      std::ostringstream buffer;
      unsigned int weight = 0;
      // TODO
      //unsigned int weight = panel_parent->get_game_interface().GetGameLogic().GetInventoryWeight(inv);
      buffer << weight;

      IndexedImage img_weight;
      font_weight.create_string(img_weight, buffer.str().c_str(), 224);
      img.paste_image(img_weight, 70 - img_weight.get_xres() / 2, 53, true);
   }
}

void ua_panel::update_stats()
{
   IndexedImage& img = get_image();

   Underworld::Player& pl = panel_parent->get_game_interface().
      GetUnderworld().GetPlayer();

   GameStrings& gstr = panel_parent->get_game_interface().
      GetGameStrings();

   img.paste_image(img_panels[2], 1, 1);

   IndexedImage img_temp;
   unsigned int xpos;
   std::string text;

   // player name
   text = pl.GetName();
   xpos = (83 - font_stats.calc_length(text.c_str())) / 2;
   font_stats.create_string(img_temp, pl.GetName().c_str(), 1);
   img.paste_rect(img_temp, 0, 0, img_temp.get_xres(), img_temp.get_yres(), xpos, 7, true);

   // player profession
   unsigned int prof = pl.GetAttribute(Underworld::attrProfession) & 7;
   text = gstr.GetString(2, prof + 23);
   Base::String::Uppercase(text);

   font_stats.create_string(img_temp, text.c_str(), 1);
   img.paste_rect(img_temp, 0, 0, img_temp.get_xres(), img_temp.get_yres(), 7, 15, true);

   // player main stats
   for (unsigned int i = 0; i < 7; i++)
   {
      std::ostringstream buffer;
      switch (i)
      {
      case 0: buffer << pl.GetAttribute(Underworld::attrExperienceLevel) << "TH"; break;
      case 1: buffer << pl.GetAttribute(Underworld::attrStrength); break;
      case 2: buffer << pl.GetAttribute(Underworld::attrDexterity); break;
      case 3: buffer << pl.GetAttribute(Underworld::attrIntelligence); break;
      case 4: buffer << pl.GetAttribute(Underworld::attrVitality) << "/" <<
         pl.GetAttribute(Underworld::attrMaxVitality); break;
      case 5: buffer << pl.GetAttribute(Underworld::attrMana) << "/" <<
         pl.GetAttribute(Underworld::attrMaxMana); break;

      case 6: buffer << pl.GetAttribute(Underworld::attrExperiencePoints); break;
      }

      xpos = 77 - font_stats.calc_length(buffer.str().c_str());

      font_stats.create_string(img_temp, buffer.str().c_str(), 1);
      img.paste_rect(img_temp, 0, 0, img_temp.get_xres(), img_temp.get_yres(), xpos, i * 7 + 15, true);

      buffer.str().erase();
   }

   // player skills
   for (unsigned int n = 0; n < 6; n++)
   {
      std::ostringstream buffer;

      // skill name
      text = gstr.GetString(2, n + stats_scrollstart + 31);
      Base::String::Uppercase(text);

      font_stats.create_string(img_temp, text.c_str(), 104);
      img.paste_rect(img_temp, 0, 0, img_temp.get_xres(), img_temp.get_yres(), 7, n * 7 + 65, true);

      // skill value
      unsigned int skill = pl.GetSkill(
         static_cast<Underworld::EPlayerSkill>(Underworld::skillAttack + n + stats_scrollstart));

      buffer << skill;

      xpos = 77 - font_stats.calc_length(buffer.str().c_str());

      font_stats.create_string(img_temp, buffer.str().c_str(), 104);
      img.paste_rect(img_temp, 0, 0, img_temp.get_xres(), img_temp.get_yres(), xpos, n * 7 + 65, true);
   }
}

void ua_panel::update_runebag()
{
   IndexedImage& img = get_image();
   img.paste_image(img_panels[1], 1, 1);

   Underworld::Runebag& runebag = panel_parent->get_game_interface().
      GetUnderworld().GetPlayer().GetRunebag();

   for (unsigned int i = 0; i < 24; i++)
      if (runebag.IsInBag(static_cast<Underworld::ERuneType>(i)))
         img.paste_rect(img_objects[0x00e8 + i], 0, 0, 14, 14,
         (i & 3) * 18 + 9, (i >> 2) * 15 + 6, true);
}

void ua_panel::inventory_click(bool button_down, bool left_button,
   unsigned int the_area)
{
   ua_panel_inventory_area_id area =
      static_cast<ua_panel_inventory_area_id>(the_area);

   Underworld::Inventory& inv = panel_parent->get_game_interface().
      GetUnderworld().GetPlayer().GetInventory();

   // check scroll up/down buttons
   if (!button_down && area == ua_area_inv_scroll_up && slot_start > 0)
   {
      slot_start -= slot_start > 4 ? 4 : slot_start;
      update_panel();
      update();
      return;
   }

   if (!button_down && area == ua_area_inv_scroll_down &&
      inv.GetNumSlots() - slot_start >= 8)
   {
      slot_start += 4;
      update_panel();
      update();
      return;
   }

   // find out itemlist pos
   Uint16 item_pos = Underworld::c_inventorySlotNoItem;
   {
      switch (area)
      {
      case ua_area_inv_slot0: case ua_area_inv_slot1:
      case ua_area_inv_slot2: case ua_area_inv_slot3:
      case ua_area_inv_slot4: case ua_area_inv_slot5:
      case ua_area_inv_slot6: case ua_area_inv_slot7:
         item_pos = inv.GetSlotListPos(slot_start + unsigned(area - ua_area_inv_slot0));
         break;

      case ua_area_equip_left_hand: item_pos = Underworld::slotLeftHand; break;
      case ua_area_equip_left_shoulder: item_pos = Underworld::slotLeftShoulder; break;
      case ua_area_equip_left_ring: item_pos = Underworld::slotLeftFinger; break;

      case ua_area_equip_right_hand: item_pos = Underworld::slotRightHand; break;
      case ua_area_equip_right_shoulder: item_pos = Underworld::slotRightShoulder; break;
      case ua_area_equip_right_ring: item_pos = Underworld::slotRightFinger; break;

      case ua_area_paperdoll_head: item_pos = Underworld::slotPaperdollHead; break;
      case ua_area_paperdoll_chest: item_pos = Underworld::slotPaperdollChest; break;
      case ua_area_paperdoll_hand: item_pos = Underworld::slotPaperdollHands; break;
      case ua_area_paperdoll_legs: item_pos = Underworld::slotPaperdollLegs; break;
      case ua_area_paperdoll_feet: item_pos = Underworld::slotPaperdollFeet; break;

      case ua_area_inv_container: break;
      default: area = ua_area_none; break;
      }
   }

   if (area == ua_area_none)
      return; // no pos that is interesting for us

   // left/right button pressed
   if (button_down && !check_dragging && area != ua_area_inv_container)
   {
      // start checking for dragging items
      check_dragging = true;
      drag_item = item_pos;
      drag_area = area;
      return;
   }

   if (button_down)
      return; // no more checks for pressed mouse button

   // left/right button release

   // stop dragging
   check_dragging = false;

   // check if we have a floating item
   if (inv.GetFloatingObjectPos() != Underworld::c_inventorySlotNoItem)
   {
      // yes, floating

      // check for container icon "slot"
      if (area == ua_area_inv_container)
      {
         if (inv.GetContainerPos() != Underworld::c_inventorySlotNoItem)
         {
            // put item into parent's container, if possible
            inv.DropFloatingObject(inv.GetParentContainerPos()); // TODO debug .drop_floating_item_parent();
         }
      }
      else
      {
         // normal inventory slot

         // just drop the item on that position
         // an item on that slot gets the next floating one
         // items dropped onto a container are put into that container
         // items that are combineable will be combined
         // items that don't fit into a paperdoll slot will be rejected
         inv.DropFloatingObject(inv.GetContainerPos(), item_pos); // TODO debug
      }

      update_cursor_image();
      update_panel();
      return;
   }

   // no floating object

   // click on container icon
   if (area == ua_area_inv_container)
   {
      // close container when not topmost
      if (inv.GetContainerPos() != Underworld::c_inventorySlotNoItem)
         inv.CloseContainer();

      update_panel();
      return;
   }

   // check if container
   if (item_pos != Underworld::c_inventorySlotNoItem &&
      inv.IsContainer(inv.GetObjectInfo(item_pos).m_itemID))
   {
      // open container
      inv.OpenContainer(item_pos);
      //cursor_is_object = false;
      slot_start = 0;

      update_panel();
      return;
   }

   IScripting& scripting = panel_parent->get_game_interface().get_scripting();

   // perform left/right click action
   if (left_button)
   {
      // trigger "use" action
      if (item_pos != Underworld::c_itemIDNone/* && scripting != NULL*/)
         scripting.UserAction(ua_action_use_object_inventory, item_pos);
   }
   else
   {
      // trigger "look" action
      if (item_pos != Underworld::c_itemIDNone/* && scripting != NULL*/)
         scripting.UserAction(ua_action_look_object_inventory, item_pos);
   }
}

void ua_panel::update_cursor_image()
{
   Underworld::Inventory& inv = panel_parent->get_game_interface().
      GetUnderworld().GetPlayer().GetInventory();

   Uint16 cursor_object = 0;
   bool cursor_is_object = false;

   // check if we still have a floating object
   if (inv.GetFloatingObjectPos() != Underworld::c_inventorySlotNoItem)
   {
      // still floating? then set new cursor object
      cursor_object = inv.GetObjectInfo(inv.GetFloatingObjectPos()).m_itemID;
      cursor_is_object = cursor_object != Underworld::c_inventorySlotNoItem;
   }

   if (cursor_is_object)
      panel_parent->set_cursor(cursor_object + 0x0100, true);
   else
   {
      panel_parent->set_cursor(-1, true);
      panel_parent->set_cursor(0, false);
   }
}
