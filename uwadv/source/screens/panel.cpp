/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
#include "underworld.hpp"
#include <sstream>


//! time to rotate panel
const double ua_panel_rotate_time = 2.5;


// ua_panel methods

ua_panel::ua_panel()
{
}

void ua_panel::init(ua_panel_parent_interface* the_panel_parent, unsigned int xpos,
   unsigned int ypos)
{
   panel_parent = the_panel_parent;
   ua_game_interface& game = panel_parent->get_game_interface();

   panel_type = ua_panel_inventory;
   armor_female = game.get_underworld().get_player().get_attr(ua_attr_gender) != 0;
   tickrate = game.get_tickrate();
   stats_scrollstart = 0;
   rotate_panel = false;
   rotate_angle = 0.0;
   rotation_oldstyle = !game.get_settings().get_bool(ua_setting_uwadv_features);

   // load image lists
   ua_image_manager& img_manager = game.get_image_manager();

   img_manager.load_list(img_panels, "panels", 0,3);
   img_manager.load_list(img_chains, "chains");
   img_manager.load(img_inv_bagpanel, "inv", 6);
   img_manager.load_list(img_inv_updown, "buttons", 27,28);

   img_manager.load_list(img_armor, armor_female ? "armor_f" : "armor_m");

   img_manager.load_list(img_bodies, "bodies");
   img_manager.load_list(img_objects, "objects");

   // load fonts
   font_stats.load(game.get_settings(), ua_font_italic);


   // create image
   get_image().create(85,116);
   get_image().set_palette(img_manager.get_palette(0));

   ua_image_quad::init(game,xpos,ypos);

   {
      img_chains_top.init(game,xpos+37, ypos-4);
      img_chains_top.get_image().create(10,4);
      img_chains_top.get_image().set_palette(game.get_image_manager().get_palette(0));

      img_chains_bottom.init(game,xpos+37, ypos+114);
      img_chains_bottom.get_image().create(10,18);
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

      wnd_xpos = save_xpos+save_width/2-wnd_width/2;

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

/*! \todo when pressing bottom chain, check if user has an item on cursor
    and prevent switching then. */
bool ua_panel::process_event(SDL_Event& event)
{
   // don't process events when in rotate mode
   if (rotate_panel)
      return ua_image_quad::process_event(event);

   // check if bottom chain was pressed
   if (event.type == SDL_MOUSEBUTTONUP)
   {
      unsigned int xpos,ypos;
      calc_mousepos(event, xpos, ypos);

      // in window?
      if (img_chains_bottom.in_window(xpos,ypos))
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

         ua_trace("runebag: mouse hit rune %u\n", mousex+mousey*4);
      }
      else
      {
         if (mousex >= 21 && mousex < 68 && mousey >= 96 && mousey < 111)
         {
            ua_trace("runebag: mouse hit \"reset shelf\" area\n");
         }
         else
            ua_trace("runebag: mouse hit outside\n");
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
      if (unsigned(old_angle/180.0*8.0) != unsigned(rotate_angle/180.0*8.0))
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
   ua_image& img = get_image();

   // inventory panel?
   if (panel_type == ua_panel_inventory)
   {
      img.paste_image(img_panels[0],1,1);
   }
   else
   if (panel_type == ua_panel_stats)
      update_stats();
   else
   if (panel_type == ua_panel_runebag)
      update_runebag();

   update();
}

void ua_panel::update_chains()
{
   unsigned int chain_add = rotate_panel ?
      unsigned(rotate_angle/180.0*8.0)%8 : 0;

   // set chains images
   img_chains_top.get_image().paste_image(img_chains[8+(chain_add)%8], 0,0);
   img_chains_bottom.get_image().paste_image(img_chains[chain_add%8], 0,0);

   img_chains_top.update();
   img_chains_bottom.update();
}

void ua_panel::update_stats()
{
   ua_image& img = get_image();

   ua_player& pl = panel_parent->get_game_interface().
      get_underworld().get_player();

   ua_gamestrings& gstr = panel_parent->get_game_interface().
      get_underworld().get_strings();

   img.paste_image(img_panels[2],1,1);

   ua_image img_temp;
   unsigned int xpos;
   std::string text;
   
   // player name
   text = pl.get_name();
   xpos = ( 83 - font_stats.calc_length(text.c_str()) )/2;
   font_stats.create_string(img_temp, pl.get_name().c_str(), 1);
   img.paste_rect(img_temp, 0,0, img_temp.get_xres(),img_temp.get_yres(), xpos, 7, true);

   // player profession
   unsigned int prof = pl.get_attr(ua_attr_profession)&7;
   text = gstr.get_string(2,prof+23);
   ua_str_uppercase(text);

   font_stats.create_string(img_temp, text.c_str(), 1);
   img.paste_rect(img_temp, 0,0, img_temp.get_xres(),img_temp.get_yres(), 7, 15, true);

   // player main stats
   for(unsigned int i=0; i<7; i++)
   {
      std::ostringstream buffer;
      switch(i)
      {
      case 0: buffer << pl.get_attr(ua_attr_exp_level) << "TH" << std::ends; break;
      case 1: buffer << pl.get_attr(ua_attr_strength) << std::ends; break;
      case 2: buffer << pl.get_attr(ua_attr_dexterity) << std::ends; break;
      case 3: buffer << pl.get_attr(ua_attr_intelligence) << std::ends; break;
      case 4: buffer << pl.get_attr(ua_attr_vitality) << "/" <<
                 pl.get_attr(ua_attr_max_vitality) << std::ends; break;
      case 5: buffer << pl.get_attr(ua_attr_mana) << "/" <<
                 pl.get_attr(ua_attr_max_mana) << std::ends; break;

      case 6: buffer << pl.get_attr(ua_attr_exp_points) << std::ends; break;
      }

      xpos = 77 - font_stats.calc_length(buffer.str().c_str());

      font_stats.create_string(img_temp, buffer.str().c_str(), 1);
      img.paste_rect(img_temp, 0,0, img_temp.get_xres(),img_temp.get_yres(), xpos, i*7+15, true);

      buffer.str().erase();
   }

   // player skills
   for(unsigned int n=0; n<6; n++)
   {
      std::ostringstream buffer;

      // skill name
      text = gstr.get_string(2,n+stats_scrollstart+31);
      ua_str_uppercase(text);

      font_stats.create_string(img_temp, text.c_str(), 104);
      img.paste_rect(img_temp, 0,0, img_temp.get_xres(),img_temp.get_yres(), 7, n*7+65, true);

      // skill value
      unsigned int skill = pl.get_skill(
         static_cast<ua_player_skills>(ua_skill_attack+n+stats_scrollstart));

      buffer << skill << std::ends;

      xpos = 77 - font_stats.calc_length(buffer.str().c_str());

      font_stats.create_string(img_temp, buffer.str().c_str(), 104);
      img.paste_rect(img_temp, 0,0, img_temp.get_xres(),img_temp.get_yres(), xpos, n*7+65, true);
   }
}

void ua_panel::update_runebag()
{
   ua_image& img = get_image();
   img.paste_image(img_panels[1],1,1);

   std::bitset<24>& runebag = panel_parent->get_game_interface().
      get_underworld().get_player().get_runes().get_runebag();

   for(unsigned int i=0; i<24; i++)
      if (runebag.test(i))
         img.paste_rect(img_objects[0x00e8+i], 0,0, 14,14,
            (i&3)*18+9, (i>>2)*15+6, true);
}


// old stuff


// tables
/*
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

void ua_panel::init_panel(ua_game_core_interface* thecore,
   ua_ingame_orig_screen* theingame_orig)
{
   slot_start = 0;
   check_dragging = false;

   // fonts
   font_normal.init(settings,ua_font_normal);
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
       inv.is_container(inv.get_item(item).item_id))
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
*/
