/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file ingame_orig.cpp

   \brief ingame user interface, from the original uw1

*/

// needed includes
#include "common.hpp"
#include "ingame_orig.hpp"
#include "uamath.hpp"


// constants

//! speed of view angle change in degree / second
const double ua_ingame_orig_viewangle_speed = 60;


// tables

struct
{
   ua_ingame_orig_area areacode;
   unsigned int xmin, xmax, ymin, ymax;

} ua_ingame_orig_area_table[] =
{
   { ua_area_screen3d, 53,224, 20,131 },

   { ua_area_menu_button0, 7, 38,  11,  28 },
   { ua_area_menu_button1, 7, 38,  29,  45 },
   { ua_area_menu_button2, 7, 38,  46,  64 },
   { ua_area_menu_button3, 7, 38,  65,  82 },
   { ua_area_menu_button4, 7, 38,  83, 100 },
   { ua_area_menu_button5, 7, 38, 101, 119 },

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


// ua_ingame_orig_screen methods

void ua_ingame_orig_screen::init()
{
   ua_trace("orig. ingame user interface started\n");

   // clear screen; this init() can take quite some time
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

   fov = 90.0;
   viewangle = 0.0;
   cursor_image = 0;
   cursorx = cursory = 0;
   cursor_is_object = false;
   cursor_object = 0;
   slot_start = 0;
   check_dragging = false;

   tickcount = 0;
   look_down = look_up = false;
   gamemode = ua_mode_default;

   setup_opengl();

   keymap.init(core->get_settings());

   core->get_underworld().import_savegame(core->get_settings(),"data/",true);
   core->get_underworld().change_level(0);

   // load all needed images
   const char *mainscreenname = "data/main.byt";

   // replace name when using uw_demo
   if (core->get_settings().gtype == ua_game_uw_demo)
      mainscreenname = "data/dmain.byt";

   // disable normal cursor
   SDL_ShowCursor(0);

   // load some images
   ua_settings &settings = core->get_settings();

   img_back.load_raw(settings,mainscreenname,0);
   tex.init(&core->get_texmgr());
   tex.convert(img_back);
   tex.use();
   tex.upload();

   img_temp.create(img_back.get_xres(),img_back.get_yres(),0,
      img_back.get_palette());

   img_compass.load(settings,"compass");
   img_bodies.load(settings,"bodies");
   img_cursors.load(settings,"cursors");
   img_objects.load(settings,"objects");

   bool female = core->get_underworld().get_player().get_attr(ua_attr_gender)!=0;
   img_armor.load(settings,female ? "armor_f" : "armor_m");

   font_normal.init(settings,ua_font_normal);

   img_cmd_btns.load(settings,"lfti");

   img_inv_bagpanel.load(settings,"inv",6);
   img_inv_updown.load(settings,"buttons",27,29);
}

void ua_ingame_orig_screen::done()
{
   tex.done();

   glDisable(GL_SCISSOR_TEST);
   glDisable(GL_FOG);
}

void ua_ingame_orig_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
   case SDL_KEYUP:
      // handle key presses
      handle_key_action(event.type,event.key.keysym);
      break;

   case SDL_MOUSEMOTION: // mouse has moved
      {
         // calculate cursor position
         int x,y;
         SDL_GetMouseState(&x,&y);
         cursorx = unsigned(double(x)/core->get_screen_width()*320.0);
         cursory = unsigned(double(y)/core->get_screen_height()*200.0);

         mouse_action(false,false,false);
      }
      break;

   case SDL_MOUSEBUTTONDOWN: // mouse button was pressed down
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);

         if (SDL_BUTTON(state)==SDL_BUTTON_LEFT) leftbuttondown = true;
         else rightbuttondown = true;

         mouse_action(true,SDL_BUTTON(state)==SDL_BUTTON_LEFT,true);
      }
      break;

   case SDL_MOUSEBUTTONUP: // mouse button was released
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);

         if (SDL_BUTTON(state)==SDL_BUTTON_LEFT) leftbuttondown = false;
         else rightbuttondown = false;

         mouse_action(true,SDL_BUTTON(state)==SDL_BUTTON_LEFT,false);
      }
      break;
   }
}

void ua_ingame_orig_screen::handle_key_action(Uint8 type, SDL_keysym &keysym)
{
   static int curlevel = 0;
   ua_player &pl = core->get_underworld().get_player();

   // check for forward key
   if (keymap.get_key(ua_key_forward)==keysym.sym)
   {
      if (type==SDL_KEYDOWN)
         pl.set_movement_mode(ua_move_walk_forward);
      else
         pl.set_movement_mode(0,ua_move_walk_forward);
   }
   else
   // check for turn left key
   if (keymap.get_key(ua_key_turn_left)==keysym.sym)
   {
      if (type==SDL_KEYDOWN)
         pl.set_movement_mode(ua_move_rotate_left);
      else
         pl.set_movement_mode(0,ua_move_rotate_left);
   }
   else
   // check for turn right key
   if (keymap.get_key(ua_key_turn_right)==keysym.sym)
   {
      if (type==SDL_KEYDOWN)
         pl.set_movement_mode(ua_move_rotate_right);
      else
         pl.set_movement_mode(0,ua_move_rotate_right);
   }
   else
   // check for look up key
   if (keymap.get_key(ua_key_look_up)==keysym.sym)
   {
      look_up = type==SDL_KEYDOWN;
   }
   else
   // check for center look key
   if (keymap.get_key(ua_key_look_center)==keysym.sym)
   {
      if (type==SDL_KEYDOWN)
      {
         look_down = look_up = false;
         viewangle=0.0;
      }
   }
   else
   // check for look down key
   if (keymap.get_key(ua_key_look_down)==keysym.sym)
   {
      look_down = type==SDL_KEYDOWN;
   }
   else

   switch(keysym.sym)
   {
   case SDLK_PAGEUP:
      if (curlevel>0 && type==SDL_KEYDOWN)
         core->get_underworld().change_level(--curlevel);
      break;

   case SDLK_PAGEDOWN:
      if (curlevel<9 && type==SDL_KEYDOWN)
         core->get_underworld().change_level(++curlevel);
      break;
   }
}

void ua_ingame_orig_screen::render()
{
   // clear color and depth buffers
   glDisable(GL_SCISSOR_TEST);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_SCISSOR_TEST);

   glLoadIdentity();

   ua_player &pl = core->get_underworld().get_player();
   double plheight = 0.6+core->get_underworld().get_player_height();
   double xangle = pl.get_angle();

   {
      // rotation
      glRotated(viewangle+270.0, 1.0, 0.0, 0.0);
      glRotated(-xangle+90.0, 0.0, 0.0, 1.0);

      ua_player &pl = core->get_underworld().get_player();

      // move to position on map
      glTranslated(-pl.get_xpos(),-pl.get_ypos(),-plheight);
   }

   ua_frustum fr(pl.get_xpos(),pl.get_ypos(),plheight,xangle,-viewangle,fov,16.0);

   // render underworld
   core->get_underworld().render(fr);

   // render user interface

   // set up new orthogonal projection matrix
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   gluOrtho2D(0.0, 1.0, 0.0, 1.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glDisable(GL_SCISSOR_TEST);
   glDisable(GL_FOG);

   // render all user interface graphics
   render_ui();

   // restore old projection matrix and settings
   glEnable(GL_FOG);
   glEnable(GL_SCISSOR_TEST);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
}

void ua_ingame_orig_screen::render_ui()
{
   // do some replacements in the image
   img_temp.paste_image(img_back,0,0);

   // compass
   ua_player &pl = core->get_underworld().get_player();
   {
      // calculate angle
      double angle = fmod(-pl.get_angle()+90.0+360.0,360.0);
      unsigned int compassdir = (unsigned(angle/11.25)&31);

      // paste image to use
      unsigned int compassimg = ((compassdir+1)/2)&3;
      img_temp.paste_image(img_compass.get_image(compassimg),112,131);

      // paste red tip facing north
      compassimg = ((compassdir+1)/2)&15;
      ua_image &tip = img_compass.get_image(compassimg+4);

      static int tip_coords[16*2] =
      {
         24, 0, 16, 2,  8, 3,  4, 6,
          0,10,  0,14,  4,16, 12,19,
         24,21, 32,19, 44,16, 48,14,
         48,10, 44, 6, 40, 3, 32, 2,
      };

      img_temp.paste_image(tip,112+tip_coords[compassimg*2],131+tip_coords[compassimg*2+1]);
   }

   // player appearance
   {
      unsigned int app = (pl.get_attr(ua_attr_appearance) +
         pl.get_attr(ua_attr_gender)==0? 0 : 5) % 10;
      img_temp.paste_image(img_bodies.get_image(app),260,11);
   }

   // "command" buttons
   if (gamemode != ua_mode_default)
   {
      static unsigned int btn_heights[] =
      {
         8,10, 7,27, 6,47, 6,65, 8,80, 8,99
      };

      unsigned int btn = 0;
      switch(gamemode)
      {
      case ua_mode_options: btn = 1; break;
      case ua_mode_talk:    btn = 3; break;
      case ua_mode_get:     btn = 5; break;
      case ua_mode_look:    btn = 7; break;
      case ua_mode_fight:   btn = 9; break;
      case ua_mode_use:     btn = 11; break;
      }

      img_temp.paste_image(img_cmd_btns.get_image(btn),
         btn_heights[btn&~1],btn_heights[btn|1]);
   }

   // inventory
   {
      ua_inventory &inv = core->get_underworld().get_inventory();

      unsigned int start = 0;

      if (inv.get_container_item_id()!=0xffff)
      {
         // inside a container

         // draw alternative inventory panel
         img_temp.paste_image(img_inv_bagpanel,236,80);

         // draw up/down arrows
         if (slot_start>0)
            img_temp.paste_image(img_inv_updown.get_image(0),296,70,true); // up

         if (inv.get_num_slots()-slot_start>=8)
            img_temp.paste_image(img_inv_updown.get_image(1),306,70,true); // down

         // draw container we're in
         Uint16 cont_id = inv.get_container_item_id();
         img_temp.paste_image(img_objects.get_image(cont_id),241,64,true);

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
                  destx = 242 + (i&3)*19,
                  desty = 82 + (i>>2)*18;

               img_temp.paste_image(img_objects.get_image(item_id),
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
            241,34, 295,34, // hand
            244,12, 293,12, // shoulder
            254,50, 284,50, // finger
            268,26,         // legs
            262,22,         // chest
            261,42,         // hands
            266,66,         // feet
            267,10,         // head
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
               img_temp.paste_image(img_objects.get_image(item_id),
                  destx,desty,true);
            }
            else
            {
               // paperdoll image
               unsigned int quality = 3; // can be between 0..3
               Uint16 armorimg = item_id < 0x002f
                  ? (item_id-0x0020)+15*quality
                  : (item_id-0x002f+60);

               img_temp.paste_image(img_armor.get_image(armorimg),
                  destx,desty,true);
            }
         }
      }

      // inventory weight
      {
         ua_image img_weight;
         font_normal.create_string(img_weight,"42",224);
         img_temp.paste_image(img_weight,301,59,true);
      }
   }

#ifdef HAVE_DEBUG
   // mouse coords
   {
      ua_image img_coords;
      char buffer[256];
      sprintf(buffer,"x=%u y=%u area=%u",cursorx,cursory,
         get_area(cursorx,cursory));
      font_normal.create_string(img_coords,buffer,11);
      img_temp.paste_image(img_coords,2,2,true);
   }
#endif

   // mouse cursor; should be the last one to paste
   {
      unsigned int posx,posy;
      posx = cursorx<8 ? 0 : cursorx-8;
      posy = cursory<8 ? 0 : cursory-8;

      ua_image &img = cursor_is_object
         ? img_objects.get_image(cursor_object)
         : img_cursors.get_image(cursor_image);

      img_temp.paste_image(img,posx,posy,true);
   }

   // upload ui texture
   tex.convert(img_temp);
   tex.use();
   tex.upload();

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   glColor3ub(255,255,255);

   // draw user interface image quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(0,0);
   glTexCoord2d(u  , v  ); glVertex2i(1,0);
   glTexCoord2d(u  , 0.0); glVertex2i(1,1);
   glTexCoord2d(0.0, 0.0); glVertex2i(0,1);
   glEnd();
}

void ua_ingame_orig_screen::tick()
{
   // evaluate underworld
   if (gamemode != ua_mode_options)
   {
      // only evaluate when the user is not in the options menu
      core->get_underworld().eval_underworld(double(tickcount)/core->get_tickrate());

      tickcount++;
   }

   // check for looking up or down
   if (look_up || look_down)
   {
      viewangle += (look_up ? 1.0 : -1.0)*(ua_ingame_orig_viewangle_speed/core->get_tickrate());

      // view angle has to stay between -180 and 180 degree
      while (viewangle > 180.0 || viewangle < -180.0 )
         viewangle = fmod(viewangle-360.0,360.0);

      // restrict up-down view angle
      if (viewangle < -40.0) viewangle = -40.0;
      if (viewangle > 40.0) viewangle = 40.0;
   }
}

void ua_ingame_orig_screen::setup_opengl()
{
   // culling
   glCullFace(GL_BACK);
   glFrontFace(GL_CCW);
   glEnable(GL_CULL_FACE);

   // clear color
   glClearColor(0,0,0,0);

   // z-buffer
   glEnable(GL_DEPTH_TEST);

   // enable texturing
   glEnable(GL_TEXTURE_2D);

   // smooth shading
   glShadeModel(GL_SMOOTH);

   // alpha blending
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // fog
   glEnable(GL_FOG);
   glFogi(GL_FOG_MODE,GL_EXP2);
   glFogf(GL_FOG_DENSITY,0.2f); // 0.65f
   glFogf(GL_FOG_START,0.0);
   glFogf(GL_FOG_END,1.0);
   GLint fog_color[4] = { 0,0,0,0 };
   glFogiv(GL_FOG_COLOR,fog_color);

   // give some hints
   glHint(GL_FOG_HINT,GL_DONT_CARE);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT,GL_DONT_CARE);

   // camera setup

   // set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   double aspect = double(core->get_screen_width())/core->get_screen_height();
   gluPerspective(fov, aspect, 0.25, 16.0);

   // switch back to modelview matrix
   glMatrixMode(GL_MODELVIEW);

   // set up scissor test
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);

   // calculate scissor rectangle
   const unsigned int scissor_area[4] =
   { 52,68, 174,114 };

   unsigned int xres = core->get_screen_width(),
      yres = core->get_screen_height();

   int x1,x2,y1,y2;

   // lower left coords
   x1 = int((scissor_area[0]/320.0) * xres);
   y1 = int((scissor_area[1]/200.0) * yres);

   // width/height
   x2 = int((scissor_area[2]/320.0) * xres);
   y2 = int((scissor_area[3]/200.0) * yres);

   glScissor(x1,y1,x2,y2);

   glEnable(GL_SCISSOR_TEST);
}

ua_ingame_orig_area ua_ingame_orig_screen::get_area(
   unsigned int xpos,unsigned int ypos)
{
   // search for area that first matches the coordinate range
   unsigned int idx=0;
   while(ua_ingame_orig_area_table[idx].areacode != ua_area_none)
   {
      // check ranges
      if (xpos >= ua_ingame_orig_area_table[idx].xmin &&
          xpos <= ua_ingame_orig_area_table[idx].xmax &&
          ypos >= ua_ingame_orig_area_table[idx].ymin &&
          ypos <= ua_ingame_orig_area_table[idx].ymax)
         break;

      idx++;
   }

   return ua_ingame_orig_area_table[idx].areacode;
}

void ua_ingame_orig_screen::mouse_action(bool click, bool left_button, bool pressed)
{
   ua_ingame_orig_area area = get_area(cursorx,cursory);

   // check areas

   // check "command" buttons
   if (click && pressed)
   switch(area)
   {
   case ua_area_menu_button0: // "options" button
      gamemode = (gamemode == ua_mode_options) ? ua_mode_default : ua_mode_options;
      break;

   case ua_area_menu_button1: // "talk" button
      gamemode = (gamemode == ua_mode_talk) ? ua_mode_default : ua_mode_talk;
      break;

   case ua_area_menu_button2: // "get" button
      gamemode = (gamemode == ua_mode_get) ? ua_mode_default : ua_mode_get;
      break;

   case ua_area_menu_button3: // "look" button
      gamemode = (gamemode == ua_mode_look) ? ua_mode_default : ua_mode_look;
      break;

   case ua_area_menu_button4: // "fight" button
      // todo: check if we can fight (e.g. when swimming)
      gamemode = (gamemode == ua_mode_fight) ? ua_mode_default : ua_mode_fight;
      break;
   case ua_area_menu_button5: // "use" button
      gamemode = (gamemode == ua_mode_use) ? ua_mode_default : ua_mode_use;
      break;
   }

   // check inventory items
   do
   {
      ua_inventory &inv = core->get_underworld().get_inventory();

      if (click)
      {
         // check clicks

         // check scroll up/down buttons
         if (!pressed && area==ua_area_inv_scroll_up && slot_start>0)
         {
            slot_start -= slot_start > 4? 4 : slot_start;
            break;
         }

         if (!pressed && area==ua_area_inv_scroll_down &&
            inv.get_num_slots()-slot_start>=8)
         {
            slot_start += 4;
            break;
         }

         // find out itemlist pos
         Uint16 item = ua_slot_no_item;
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

         if (area == ua_area_none) break;

         // left/right click pressed
         if (pressed && !check_dragging && area!=ua_area_inv_container)
         {
            // start checking for dragging items
            check_dragging = true;
            drag_item = item;
            drag_area = area;
         }

         // left/right click release
         if (!pressed)
         {
            // stop dragging
            check_dragging = false;

            // check for floating items
            if (inv.get_floating_item() != ua_slot_no_item)
            {
               // yes, floating

               // check for container icon "slot"
               if (area==ua_area_inv_container &&
                  inv.get_container_item_id() != ua_slot_no_item)
               {
                  // put item into parent's container, if possible
                  inv.drop_floating_item_parent();
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

               // check if we still have a floating object
               if (inv.get_floating_item() != ua_slot_no_item)
               {
                  // still floating? then set new cursor object
                  cursor_object =  inv.get_item(inv.get_floating_item()).item_id;
                  cursor_is_object = cursor_object != ua_slot_no_item;
               }
               else
                  cursor_is_object = false;

               break;
            }

            // no floating object

            // click on container icon
            if (area==ua_area_inv_container)
            {
               // close container when not topmost
               if (inv.get_container_item_id() != ua_slot_no_item)
                  inv.close_container();
               break;
            }

            // check if container
            if (item != ua_slot_no_item &&
                inv.is_container(inv.get_item(item).item_id) &&
                gamemode == ua_mode_default)
            {
               // open container
               inv.open_container(item);
               cursor_is_object = false;
               slot_start = 0;
               break;
            }

            // todo: perform left/right click action
         }
      }
      else
      {
         // check item dragging
         if (check_dragging && drag_area != area)
         {
            // user dragged item out of area
            check_dragging = false;
            inv.float_item(drag_item);

            cursor_object =  inv.get_item(inv.get_floating_item()).item_id;
            cursor_is_object = cursor_object != ua_slot_no_item;
         }
      }
   } while (false);
}
