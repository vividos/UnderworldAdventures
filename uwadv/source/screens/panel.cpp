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
/*! \file panel.cpp

   \brief inventory/stats/rune bag panel

*/

// needed includes
#include "common.hpp"
#include "panel.hpp"
#include "ingame_orig.hpp"
#include <sstream>


// tables

ua_screen_area_data ua_panel_area_table[] =
{
   { ua_area_inv_slot0, 242, 260,  83,  99 },
   { ua_area_inv_slot1, 261, 279,  83,  99 },
   { ua_area_inv_slot2, 280, 298,  83,  99 },
   { ua_area_inv_slot3, 299, 317,  83,  99 },
   { ua_area_inv_slot4, 242, 260, 100, 116 },
   { ua_area_inv_slot5, 261, 279, 100, 116 },
   { ua_area_inv_slot6, 280, 298, 100, 116 },
   { ua_area_inv_slot7, 299, 317, 100, 116 },

   { ua_area_inv_container, 241, 256, 64, 79 },
   { ua_area_inv_scroll_up, 294, 304, 69, 81 },
   { ua_area_inv_scroll_down, 305, 315, 69, 81 },

   { ua_area_equip_left_hand,     243, 258, 36, 51 },
   { ua_area_equip_left_shoulder, 246, 261, 14, 28 },
   { ua_area_equip_left_ring,     260, 265, 56, 63 },

   { ua_area_equip_right_hand,     297, 311, 36, 51 },
   { ua_area_equip_right_shoulder, 295, 309, 14, 28 },
   { ua_area_equip_right_ring,     292, 297, 56, 63 },

   { ua_area_paperdoll_head,  271, 285, 13, 31 },
   { ua_area_paperdoll_chest, 270, 285, 31, 53 },
   { ua_area_paperdoll_hand,  265, 271, 46, 55 },
   { ua_area_paperdoll_hand,  286, 292, 46, 55 },
   { ua_area_paperdoll_legs,  271, 286, 54, 65 },
   { ua_area_paperdoll_feet,  270, 286, 66, 80 },

   { ua_area_none, 0,0, 320,200 }
};


// ua_panel methods

ua_panel::ua_panel()
{
}

void ua_panel::init_panel(ua_game_core_interface* thecore,
   ua_ingame_orig_screen* theingame_orig)
{
   init(thecore);

   ingame_orig = theingame_orig;

   panel_type = 0;
   slot_start = 0;
   check_dragging = false;

   ua_settings& settings = core->get_settings();

   // load images
   img_panels.load(settings,"panels",0,3);
   img_inv_bagpanel.load(settings,"inv",6);
   img_inv_updown.load(settings,"buttons",27,29);

   armor_female = core->get_underworld().get_player().get_attr(ua_attr_gender)!=0;
   img_armor.load(settings,armor_female ? "armor_f" : "armor_m");

   img_bodies.load(settings,"bodies");
   img_objects.load(settings,"objects");

   // fonts
   font_normal.init(settings,ua_font_normal);
}

void ua_panel::suspend()
{
   tex_panel.done();
}

void ua_panel::resume()
{
   tex_panel.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);
   update_panel_texture();
}

void ua_panel::done()
{
}

void ua_panel::mouse_action(bool click, bool left_button, bool pressed)
{
   // check inventory item areas
   unsigned int area = get_area(ua_panel_area_table,cursorx,cursory);

   if (click)
      inventory_click(pressed,left_button,area);
   else
   {
      // check item dragging
      if (check_dragging && drag_area != area)
         inventory_dragged_item();
   }
}

void ua_panel::render()
{
   tex_panel.use();
   double u = tex_panel.get_tex_u(), v = tex_panel.get_tex_v();

   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(236,   79);
   glTexCoord2d(u  , v  ); glVertex2i(236+83,79);
   glTexCoord2d(u  , 0.0); glVertex2i(236+83,79+114);
   glTexCoord2d(0.0, 0.0); glVertex2i(236,   79+114);
   glEnd();
}

void ua_panel::tick()
{
}

void ua_panel::update_panel_texture()
{
   ua_image panel(img_panels.get_image(panel_type));

   // update inventory panel?
   if (panel_type==0)
   {
      ua_inventory& inv = core->get_underworld().get_inventory();
      ua_player& pl = core->get_underworld().get_player();

      bool female = pl.get_attr(ua_attr_gender)!=0;

      // player appearance
      {
         unsigned int bodyimg = pl.get_attr(ua_attr_appearance)%5
            + ( female ? 5 : 0 );

         panel.paste_image(this->img_bodies.get_image(bodyimg),24,4);
      }

      // check if paperdoll images should be reloaded
      if (female != armor_female)
      {
         armor_female = female;
         img_armor.load(core->get_settings(),armor_female ? "armor_f" : "armor_m");
      }

      unsigned int start = 0;

      if (inv.get_container_item_id()!=0xffff)
      {
         // inside a container

         // draw alternative inventory panel
         panel.paste_image(img_inv_bagpanel,0,73);

         // draw up/down arrows
         if (slot_start>0)
            panel.paste_image(img_inv_updown.get_image(0),60,63,true); // up

         if (inv.get_num_slots()-slot_start>=8)
            panel.paste_image(img_inv_updown.get_image(1),70,63,true); // down

         // draw container we're in
         Uint16 cont_id = inv.get_container_item_id();
         panel.paste_image(img_objects.get_image(cont_id),5,57,true);

         // begin at current slot start
         start = slot_start;
      }

      // paste inventory slots
      for(unsigned int i=0; i<8; i++)
      {
         if (start+i < inv.get_num_slots())
         {
            Uint16 item = inv.get_slot_item(start+i);
            Uint16 item_id = inv.get_item(item).item_id;

            // draw item
            if (item_id != 0xffff)
            {
               unsigned int
                  destx = 6 + (i&3)*19,
                  desty = 75 + (i>>2)*18;

               panel.paste_image(img_objects.get_image(item_id),
                  destx,desty,true);
            }
         }
      }

      // do paperdoll items
      for(unsigned int j=ua_slot_lefthand; j<ua_slot_max; j++)
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
         destx = slot_coords[(j-ua_slot_lefthand)*2];
         desty = slot_coords[(j-ua_slot_lefthand)*2+1];

         // paste item
         Uint16 item_id = inv.get_item(j).item_id;
         if (item_id != 0xffff)
         {
            if (j<ua_slot_paperdoll_start)
            {
               // normal object
               panel.paste_image(img_objects.get_image(item_id),
                  destx,desty,true);
            }
            else
            {
               // paperdoll image
               unsigned int quality = 3; // can be between 0..3
               Uint16 armorimg = item_id < 0x002f
                  ? (item_id-0x0020)+15*quality
                  : (item_id-0x002f+60);

               panel.paste_image(img_armor.get_image(armorimg),
                  destx,desty,true);
            }
         }
      }

      // inventory weight
      {
         std::ostringstream buffer;
         buffer << static_cast<unsigned int>(inv.get_inventory_weight()) << std::ends;

         ua_image img_weight;
         font_normal.create_string(img_weight,buffer.str().c_str(),224);
         panel.paste_image(img_weight,69-img_weight.get_xres()/2,52,true);
      }
   }

   // upload new panel
   tex_panel.convert(panel);
   tex_panel.upload();
}

void ua_panel::update_cursor_image()
{
   ua_inventory& inv = core->get_underworld().get_inventory();

   Uint16 cursor_object = 0;
   bool cursor_is_object = false;

   // check if we still have a floating object
   if (inv.get_floating_item() != ua_slot_no_item)
   {
      // still floating? then set new cursor object
      cursor_object =  inv.get_item(inv.get_floating_item()).item_id;
      cursor_is_object = cursor_object != ua_slot_no_item;
   }

   if (cursor_is_object)
      ingame_orig->set_cursor_image(true,cursor_object,true);
   else
      ingame_orig->set_cursor_image(false,0xffff,true);
}

void ua_panel::inventory_click(
   bool pressed, bool left_button, unsigned int area)
{
   ua_inventory& inv = core->get_underworld().get_inventory();

   // check scroll up/down buttons
   if (!pressed && area==ua_area_inv_scroll_up && slot_start>0)
   {
      slot_start -= slot_start > 4? 4 : slot_start;
      update_panel_texture();
      return;
   }

   if (!pressed && area==ua_area_inv_scroll_down &&
      inv.get_num_slots()-slot_start>=8)
   {
      slot_start += 4;
      update_panel_texture();
      return;
   }

   // find out itemlist pos
   Uint16 item = ua_slot_no_item;
   {
      switch(area)
      {
      case ua_area_inv_slot0: item = inv.get_slot_item(slot_start+0); break;
      case ua_area_inv_slot1: item = inv.get_slot_item(slot_start+1); break;
      case ua_area_inv_slot2: item = inv.get_slot_item(slot_start+2); break;
      case ua_area_inv_slot3: item = inv.get_slot_item(slot_start+3); break;
      case ua_area_inv_slot4: item = inv.get_slot_item(slot_start+4); break;
      case ua_area_inv_slot5: item = inv.get_slot_item(slot_start+5); break;
      case ua_area_inv_slot6: item = inv.get_slot_item(slot_start+6); break;
      case ua_area_inv_slot7: item = inv.get_slot_item(slot_start+7); break;

      case ua_area_equip_left_hand: item = ua_slot_lefthand; break;
      case ua_area_equip_left_shoulder: item = ua_slot_leftshoulder; break;
      case ua_area_equip_left_ring: item = ua_slot_leftfinger; break;

      case ua_area_equip_right_hand: item = ua_slot_righthand; break;
      case ua_area_equip_right_shoulder: item = ua_slot_rightshoulder; break;
      case ua_area_equip_right_ring: item = ua_slot_rightfinger; break;

      case ua_area_paperdoll_head: item = ua_slot_paperdoll_head; break;
      case ua_area_paperdoll_chest: item = ua_slot_paperdoll_chest; break;
      case ua_area_paperdoll_hand: item = ua_slot_paperdoll_hands; break;
      case ua_area_paperdoll_legs: item = ua_slot_paperdoll_legs; break;
      case ua_area_paperdoll_feet: item = ua_slot_paperdoll_feet; break;

      case ua_area_inv_container: break;
      default: area = ua_area_none; break;
      }
   }

   if (area == ua_area_none)
      return; // no pos that is interesting for us

   // left/right button pressed
   if (pressed && !check_dragging && area!=ua_area_inv_container)
   {
      // start checking for dragging items
      check_dragging = true;
      drag_item = item;
      drag_area = (ua_screen_area_id)area;
      return;
   }

   if (pressed)
      return; // no more checks for pressed mouse button

   // left/right button release

   // stop dragging
   check_dragging = false;

   // check if we have a floating item
   if (inv.get_floating_item() != ua_slot_no_item)
   {
      // yes, floating

      // check for container icon "slot"
      if (area==ua_area_inv_container)
      {
         if (inv.get_container_item_id() != ua_slot_no_item)
         {
            // put item into parent's container, if possible
            inv.drop_floating_item_parent();
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
         inv.drop_floating_item(item);
      }

      update_cursor_image();

      update_panel_texture();
      return;
   }

   // no floating object

   // click on container icon
   if (area==ua_area_inv_container)
   {
      // close container when not topmost
      if (inv.get_container_item_id() != ua_slot_no_item)
         inv.close_container();

      update_panel_texture();
      return;
   }

   // check if container
   if (item != ua_slot_no_item &&
       inv.is_container(inv.get_item(item).item_id) &&
       ingame_orig->get_gamemode() == ua_mode_default)
   {
      // open container
      inv.open_container(item);
      //cursor_is_object = false;
      slot_start = 0;

      update_panel_texture();
      return;
   }

   // perform left/right click action
   if (left_button)
   {
      // trigger "use" action
      if (item != ua_item_none)
         core->get_underworld().get_scripts().lua_inventory_use(item);
   }
   else
   {
      // trigger "look" action
      if (item != ua_item_none)
         core->get_underworld().get_scripts().lua_inventory_look(item);
   }
}

void ua_panel::inventory_dragged_item()
{
   ua_inventory& inv = core->get_underworld().get_inventory();

   // user dragged item out of area
   check_dragging = false;
   inv.float_item(drag_item);

   update_cursor_image();

   update_panel_texture();
}
