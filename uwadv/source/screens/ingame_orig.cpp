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
#include "load_game.hpp"
#include "save_game.hpp"
#include "cutscene_view.hpp"
#include "conversation.hpp"
#include <sstream>
#include <iomanip>


// constants

//! time to fade in/out
const double ua_ingame_orig_screen::fade_time = 0.5;


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

   // start ingame music
   core->get_audio().start_music(2,true);

   cursor_image = 0;
   cursor_image_current = 0;
   cursorx = cursory = 0;
   cursor_is_object = false;
   mouse_move = false;

   move_turn_left = move_turn_right = move_walk_forward = move_walk_backwards = false;

   slot_start = 0;
   check_dragging = false;

   in_screen3d = false;

   tickcount = 0;
   gamemode = ua_mode_default;
   panel_type = 0;

   // adjust scroll width for uw_demo
   scrollwidth = 289;

   if (core->get_settings().get_gametype() == ua_game_uw_demo)
      scrollwidth = 218;

   dbgint = ua_debug_interface::get_new_debug_interface(&core->get_underworld());

   // load keymap
   ua_trace("loading keymaps ...\n");
   {
      keymap.init(core->get_settings());

      // load keymap from uadata resources
      std::string keymap_name("uw1"); // TODO: replace with current game name
      keymap_name.append("/keymap.cfg");

      ua_trace(" keymap: %s\n",keymap_name.c_str());
      keymap.load(core->get_filesmgr().get_uadata_file(keymap_name.c_str()));

      // load custom keymap
      keymap_name = core->get_settings().get_string(ua_setting_custom_keymap);

      ua_trace(" keymap: %s",keymap_name.c_str());
      SDL_RWops* rwops = SDL_RWFromFile(keymap_name.c_str(),"rb");

      if (rwops!=NULL)
         keymap.load(rwops);
      else
         ua_trace(" => not available");

      ua_trace("\n");
   }

   // load all needed images
   const char *mainscreenname = "data/main.byt";

   // replace name when using uw_demo
   if (core->get_settings().get_gametype() == ua_game_uw_demo)
      mainscreenname = "data/dmain.byt";

   // load some images
   ua_settings &settings = core->get_settings();

   // background image
   {
      ua_image img_back;
      img_back.load_raw(settings,mainscreenname,0);

      // fill message scroll area
      img_back.fill_rect(16,169, scrollwidth,30, 42);

      // fill panel area
      img_back.fill_rect(236,7, 83,114, 1);

      img_back.copy_rect(img_back1,0,0, 256,200);
      img_back.copy_rect(img_back2,256,0, 320-256,200);
   }

   // compass images
   {
      ua_image_list img_temp;
      img_temp.load(settings,"compass");

      static int ua_compass_tip_coords[16*2] =
      {
         24, 0, 16, 2,  8, 3,  4, 6,
          0,10,  0,14,  4,16, 12,19,
         24,21, 32,19, 44,16, 48,14,
         48,10, 44, 6, 40, 3, 32, 2,
      };

      std::vector<ua_image>& compass = img_compass.get_allimages();

      // create compass images and add needle tips and right/bottom borders
      for(unsigned int n=0; n<16; n++)
      {
         ua_image img;
         img.create(52+1,26+1);
         img.paste_image(img_temp.get_image(n&3),0,0);

         // compass needle
         img.paste_image(img_temp.get_image(n+4),
            ua_compass_tip_coords[n*2],ua_compass_tip_coords[n*2+1]);

         // right border
         img.paste_rect(img,51,0, 1,img.get_yres(), 52,0);

         // bottom border
         img.paste_rect(img,0,25, img.get_xres(),1, 0,26);

         compass.push_back(img);
      }
   }

   // vitality/mana/poison flasks
   {
      ua_image_list flasks;
      flasks.load(settings,"flasks");

      ua_image& baseimg = flasks.get_image(75);

      // load all 3 flasks
      for(unsigned int i=0; i<3; i++)
      {
         std::vector<ua_image>& allimages = img_flasks[i].get_allimages();

         ua_image img(baseimg);
         allimages.push_back(img);

         static unsigned int ua_flasks_pos[13] =
         { 26, 24, 22, 20, 18, 16, 15, 14, 13, 11, 9, 7, 5 };

         // generate all images
         for(unsigned int j=0; j<13; j++)
         {
            img.paste_image(flasks.get_image(i*25+j), 0, ua_flasks_pos[j]);
            allimages.push_back(img);
         }
      }
   }

   // command buttons
   {
      ua_image_list buttons;
      buttons.load(settings,"lfti");

      img_cmd_buttons.create(32,108);
      img_back1.copy_rect(img_cmd_buttons,6,10,32,108);

      static unsigned int ua_cmd_btn_offsets[] =
      {
         2,0, 1,17, 0,37, 0,55, 2,70, 2,89
      };

      for(unsigned int i=0; i<6; i++)
      {
         img_cmd_buttons.paste_image(
            buttons.get_image(i*2+1),
            ua_cmd_btn_offsets[i*2],ua_cmd_btn_offsets[i*2+1]);
      }
   }

   // panels
   {
      img_panels.load(settings,"panels",0,3);
      img_inv_bagpanel.load(settings,"inv",6);
      img_inv_updown.load(settings,"buttons",27,29);

      armor_female = core->get_underworld().get_player().get_attr(ua_attr_gender)!=0;
      img_armor.load(settings,armor_female ? "armor_f" : "armor_m");

      img_bodies.load(settings,"bodies");
      img_objects.load(settings,"objects");
   }

   // fonts
   font_normal.init(settings,ua_font_normal);

   resume();

   textscroll.print("Welcome to the Underworld Adventures!\n http://uwadv.sourceforge.net/");
}

void ua_ingame_orig_screen::suspend()
{
   ua_trace("suspending orig. ingame user interface\n\n");

   // clear screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

   // unregister script callbacks
   core->get_underworld().get_scripts().register_callback(NULL);

   // clean up textures
   tex_back1.done();
   tex_back2.done();

   tex_compass.done();
   tex_flasks.done();
   tex_cmd_buttons.done();
   tex_panel.done();

   mousecursor.done();
   textscroll.done();

   renderer.done();

   glDisable(GL_SCISSOR_TEST);
}

void ua_ingame_orig_screen::resume()
{
   ua_trace("resuming orig. ingame user interface\n");

   fade_state = 0;
   fade_ticks = 0;

   setup_opengl();

   // init mouse cursor
   mousecursor.init(core,0);
   mousecursor.show(true);

   // init text scroll
   textscroll.init(*core,15,169, scrollwidth,30, 5, 42);
   textscroll.set_color(1);

   // register script callbacks
   core->get_underworld().get_scripts().register_callback(this);

   // prepare level textures
   ui_changed_level(core->get_underworld().get_player().get_attr(ua_attr_level));

   // init textures

   // background textures
   tex_back1.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);
   tex_back1.convert(img_back1);
   tex_back1.use();
   tex_back1.upload();

   tex_back2.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);
   tex_back2.convert(img_back2);
   tex_back2.use();
   tex_back2.upload();

   // compass texture
   tex_compass.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR);
   tex_compass.convert(img_compass.get_image(0));
   tex_compass.use();
   tex_compass.upload();
   compass_curimg = 0;

   // flasks textures
   flasks_curimg[0] = flasks_curimg[1] = 0;
   tex_flasks.init(&core->get_texmgr(),2,GL_LINEAR,GL_LINEAR);
   tex_flasks.convert(img_flasks[0].get_image(flasks_curimg[0]),0);
   tex_flasks.use(0);
   tex_flasks.upload();

   tex_flasks.convert(img_flasks[1].get_image(flasks_curimg[1]),1);
   tex_flasks.use(1);
   tex_flasks.upload();

   // command buttons
   tex_cmd_buttons.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);
   tex_cmd_buttons.convert(img_cmd_buttons);
   tex_cmd_buttons.use();
   tex_cmd_buttons.upload();

   // panel texture
   tex_panel.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);
   update_panel_texture();
}

void ua_ingame_orig_screen::done()
{
   suspend();
   delete dbgint;

   ua_trace("leaving orig. ingame user interface\n");
}

void ua_ingame_orig_screen::handle_event(SDL_Event &event)
{
   if (textscroll.handle_event(event))
      return;

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

         mousecursor.updatepos(cursorx,cursory);
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
   default: break;
   }
}

void ua_ingame_orig_screen::handle_key_action(Uint8 type, SDL_keysym &keysym)
{
   static int curlevel = 0;
   ua_player &pl = core->get_underworld().get_player();

   Uint32 keymod = ua_make_keymod(keysym.sym,keysym.mod & ~KMOD_NUM);

   // check for run forward keys
   if (keymap.is_key(ua_key_run_forward,keymod) ||
       keymap.is_key(ua_key_run_forward_easymove,keymod) )
   {
      move_run_forward = (type==SDL_KEYDOWN);
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_walk,1.0);
         pl.set_movement_mode(ua_move_walk);
      }
      else
      {
         if (move_walk_backwards)
            pl.set_movement_factor(ua_move_walk,-0.4);
         else
            if (move_walk_forward)
               pl.set_movement_factor(ua_move_walk,0.6);
            else
               pl.set_movement_mode(0,ua_move_walk);
      }
   }
   else
   // check for walk forward keys
   if (keymap.is_key(ua_key_walk_forward,keymod) ||
       keymap.is_key(ua_key_walk_forward_easymove,keymod) )
   {
      move_walk_forward = (type==SDL_KEYDOWN);
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_walk,0.6);
         pl.set_movement_mode(ua_move_walk);
         
      }
      else
      {
         if (move_walk_backwards)
            pl.set_movement_factor(ua_move_walk,-0.4);
         else
            if (move_run_forward)
               pl.set_movement_factor(ua_move_walk,1.0);
            else
               pl.set_movement_mode(0,ua_move_walk);
      }
   }
   else
   // check for turn left key
   if (keymap.is_key(ua_key_turn_left,keymod) ||
       keymap.is_key(ua_key_turn_left_easymove,keymod) )
   {
      move_turn_left = (type==SDL_KEYDOWN);
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_rotate,1.0);
         pl.set_movement_mode(ua_move_rotate);
      }
      else
      {
         if (move_turn_right)
            pl.set_movement_factor(ua_move_rotate,-1.0);
         else
            pl.set_movement_mode(0,ua_move_rotate);
      }
   }
   else
   // check for turn right key
   if (keymap.is_key(ua_key_turn_right,keymod) ||
       keymap.is_key(ua_key_turn_right_easymove,keymod) )
   {
      move_turn_right = (type==SDL_KEYDOWN);
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_rotate,-1.0);
         pl.set_movement_mode(ua_move_rotate);
      }
      else
      {
         if (move_turn_left)
            pl.set_movement_factor(ua_move_rotate,1.0);
         else
            pl.set_movement_mode(0,ua_move_rotate);
      }
   }
   else
   // check for slide left
   if (keymap.is_key(ua_key_slide_left,keymod))
   {
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_slide,-1.0);
         pl.set_movement_mode(ua_move_slide);
      }
      else
         pl.set_movement_mode(0,ua_move_slide);
   }
   else
   // check for slide right
   if (keymap.is_key(ua_key_slide_right,keymod))
   {
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_slide,1.0);
         pl.set_movement_mode(ua_move_slide);
      }
      else
         pl.set_movement_mode(0,ua_move_slide);
   }
   else
   // check for walk backwards
   if (keymap.is_key(ua_key_walk_backwards,keymod) ||
       keymap.is_key(ua_key_walk_backwards_easymove,keymod) )
   {
      move_walk_backwards = (type==SDL_KEYDOWN);

      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_walk,-0.4);
         pl.set_movement_mode(ua_move_walk);
      }
      else
      {
         if (move_walk_forward || move_run_forward)
            pl.set_movement_factor(ua_move_walk,move_run_forward ? 1.0 : 0.6);
         else
            pl.set_movement_mode(0,ua_move_walk);
      }
   }
   else
   // check for look up key
   if (keymap.is_key(ua_key_look_up,keymod))
   {
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_lookup,1.0);
         pl.set_movement_mode(ua_move_lookup);
      }
      else
         pl.set_movement_mode(0,ua_move_lookup);
   }
   else
   // check for look down key
   if (keymap.is_key(ua_key_look_down,keymod))
   {
      if (type==SDL_KEYDOWN)
      {
         pl.set_movement_factor(ua_move_lookup,-1.0);
         pl.set_movement_mode(ua_move_lookup);
      }
      else
         pl.set_movement_mode(0,ua_move_lookup);
   }

   // now check for keydown only keys
   if (type==SDL_KEYDOWN)

   // check for center look key
   if (keymap.is_key(ua_key_center_view,keymod))
   {
      pl.set_movement_mode(0,ua_move_lookup);
      pl.set_movement_factor(ua_move_lookup,0.0);
      pl.set_angle_pan(0.0);
   }
   else
   // check for exit screen key
   if (keymap.is_key(ua_key_ua_return_menu,keymod) &&
       core->get_settings().get_gametype() != ua_game_uw_demo)
   {
      fade_state = 2;
      fade_ticks = 0;
      fadeout_action = 0; // return to menu
   }
#ifdef HAVE_DEBUG
   else
   // check for quicksave key
   if (keymap.is_key(ua_key_special_quicksave,keymod))
   {
      ua_trace("quicksaving ... ");
      ua_savegame sg = core->get_savegames_mgr().get_quicksave(true);
      core->get_underworld().save_game(sg);
      ua_trace("done\n");
   }
   else
   // check for quickload key
   if (keymap.is_key(ua_key_special_quickload,keymod))
   {
      ua_trace("quickloading ... ");
      ua_savegame sg = core->get_savegames_mgr().get_quicksave(false);
      core->get_underworld().load_game(sg);
      ua_trace("done\n");
   }
   else
   // check for "load game" key
   if (keymap.is_key(ua_key_game_restore_game,keymod))
   {
      fade_state = 2;
      fade_ticks = 0;
      fadeout_action = 1; // load game
   }
   else
   // check for "save game" key
   if (keymap.is_key(ua_key_game_save_game,keymod))
   {
      fade_state = 2;
      fade_ticks = 0;
      fadeout_action = 2; // save game
   }
   else
   // check for "debugger" key
   if (keymap.is_key(ua_key_ua_debug,keymod))
   {
      dbgint->start_debugger();
   }
#endif
   else
   // check for "level up" key
   if (keymap.is_key(ua_key_ua_level_up,keymod))
   {
      if (curlevel>0)
         core->get_underworld().change_level(--curlevel);
   }
   else
   // check for "level down" key
   if (keymap.is_key(ua_key_ua_level_down,keymod))
   {
      if (curlevel+1<core->get_underworld().get_num_levels())
         core->get_underworld().change_level(++curlevel);
   }
}

void ua_ingame_orig_screen::render()
{
   // clear color and depth buffers
   glDisable(GL_SCISSOR_TEST);
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   glEnable(GL_SCISSOR_TEST);

   glLoadIdentity();

   renderer.render();

   // render user interface

   // set up new orthogonal projection matrix
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   gluOrtho2D(0, 320, 0, 200);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glDisable(GL_FOG);

   // when fading in/out, lay a quad over the already rendered 3d view
   if (fade_state==0 || fade_state==2)
   {
      Uint8 alpha = 255;

      switch(fade_state)
      {
      case 0: // fade in
         alpha = Uint8(255-255*(double(fade_ticks) / (core->get_tickrate()*fade_time)));
         break;

      case 2: // fade out
         alpha = Uint8(255*(double(fade_ticks) / (core->get_tickrate()*fade_time)));
         break;
      }

      glColor4ub(0,0,0,alpha);
      glBindTexture(GL_TEXTURE_2D,0);

      // draw quad
      glBegin(GL_QUADS);
      glVertex2i( 52,  68);
      glVertex2i(226,  68);
      glVertex2i(226, 182);
      glVertex2i( 52, 182);
      glEnd();
   }

   glDisable(GL_SCISSOR_TEST);

   // render all user interface graphics
   render_ui();

   // restore old settings
   glEnable(GL_FOG);
   glEnable(GL_SCISSOR_TEST);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);

   glMatrixMode(GL_PROJECTION);
   glPopMatrix(); // restore proj. matrix
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix(); // restore modelview matrix
   // note: modelview matrix is sometimes needed for selection
}

void ua_ingame_orig_screen::render_ui()
{
   // set texture brightness for all ui textures
   {
      Uint8 light = 255;

      switch(fade_state)
      {
      case 0: // fade in
         light = Uint8(255*(double(fade_ticks) / (core->get_tickrate()*fade_time)));
         break;

      case 2: // fade out
         light = Uint8(255-255*(double(fade_ticks) / (core->get_tickrate()*fade_time)));
         break;
      }

      glColor3ub(light,light,light);
   }

   // draw background texture
   {
      // first quad (256x200)
      double u = tex_back1.get_tex_u(), v = tex_back1.get_tex_v();
      tex_back1.use();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(0,  0);
      glTexCoord2d(u  , v  ); glVertex2i(256,0);
      glTexCoord2d(u  , 0.0); glVertex2i(256,200);
      glTexCoord2d(0.0, 0.0); glVertex2i(0,  200);
      glEnd();

      // second quad (64x200)
      u = tex_back2.get_tex_u(); v = tex_back2.get_tex_v();
      tex_back2.use();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(256,0);
      glTexCoord2d(u  , v  ); glVertex2i(320,0);
      glTexCoord2d(u  , 0.0); glVertex2i(320,200);
      glTexCoord2d(0.0, 0.0); glVertex2i(256,200);
      glEnd();
   }

   ua_player &pl = core->get_underworld().get_player();

   // draw compass
   {
      double angle = fmod(-pl.get_angle_rot()+90.0+360.0,360.0);

      unsigned int compassdir = (unsigned(angle/11.25)&31);
      unsigned int compassimg = ((compassdir+1)/2)&15;

      compassimg = unsigned((angle+11.25)/22.5)&15;

      // prepare texture
      if (compass_curimg != compassimg)
      {
         // reupload compass texture
         compass_curimg = compassimg;
         tex_compass.convert(img_compass.get_image(compass_curimg));
         tex_compass.use();
         tex_compass.upload();
      }
      else
         tex_compass.use();

      // draw compass quad
      double u = tex_compass.get_tex_u(), v = tex_compass.get_tex_v();
      u -= tex_compass.get_tex_u()/tex_compass.get_xres();
      v -= tex_compass.get_tex_v()/tex_compass.get_yres();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(112,   43);
      glTexCoord2d(u  , v  ); glVertex2i(112+52,43);
      glTexCoord2d(u  , 0.0); glVertex2i(112+52,43+26);
      glTexCoord2d(0.0, 0.0); glVertex2i(112,   43+26);
      glEnd();
   }

   // vitality/mana/poisoned flasks
   {
      // vitality/poisoned flask

      // calculate relative vitality and flask image to use
      double vit_rel = double(pl.get_attr(ua_attr_life)) / double(pl.get_attr(ua_attr_max_life));
      if (vit_rel>1.0) vit_rel = 0;

      unsigned int vit_flask_img = unsigned(13.0 * vit_rel)
         + (pl.get_attr(ua_attr_poisoned)!=0 ? 14 : 0);

      // prepare texture
      if (flasks_curimg[0] != vit_flask_img)
      {
         // reupload flask texture
         flasks_curimg[0] = vit_flask_img;

         tex_flasks.convert(
            img_flasks[ vit_flask_img>=14 ? 2 : 0 ].get_image(vit_flask_img%14),0);
         tex_flasks.use(0);
         tex_flasks.upload();
      }
      else
         tex_flasks.use(0);

      // draw vitality flask quad
      double u = tex_flasks.get_tex_u(), v = tex_flasks.get_tex_v();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(248,   42);
      glTexCoord2d(u  , v  ); glVertex2i(248+24,42);
      glTexCoord2d(u  , 0.0); glVertex2i(248+24,42+33);
      glTexCoord2d(0.0, 0.0); glVertex2i(248,   42+33);
      glEnd();

      // mana flask

      // calculate relative mana value and flask image to use
      double mana_rel = double(pl.get_attr(ua_attr_mana)) / double(pl.get_attr(ua_attr_max_mana));
      if (mana_rel>1.0) mana_rel = 1.0;

      unsigned int mana_flask_img = unsigned(13.0 * mana_rel);

      // prepare texture
      if (flasks_curimg[1] != mana_flask_img)
      {
         // reupload flask texture
         flasks_curimg[1] = mana_flask_img;

         tex_flasks.convert(img_flasks[1].get_image(mana_flask_img%14),1);
         tex_flasks.use(1);
         tex_flasks.upload();
      }
      else
         tex_flasks.use(1);

      // draw mana flask quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(284,   42);
      glTexCoord2d(u  , v  ); glVertex2i(284+24,42);
      glTexCoord2d(u  , 0.0); glVertex2i(284+24,42+33);
      glTexCoord2d(0.0, 0.0); glVertex2i(284,   42+33);
      glEnd();
   }

   // command buttons
   if (gamemode != ua_mode_default)
   {
      // command buttons quad
      double u = tex_cmd_buttons.get_tex_u(), v = tex_cmd_buttons.get_tex_v();

      tex_cmd_buttons.use();

      unsigned int button = 0;
      switch(gamemode)
      {
      case ua_mode_options: button = 0; break;
      case ua_mode_talk:    button = 1; break;
      case ua_mode_get:     button = 2; break;
      case ua_mode_look:    button = 3; break;
      case ua_mode_fight:   button = 4; break;
      case ua_mode_use:     button = 5; break;
      default: break;
      }

      // draw mana flask quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v/6.0*(button+0.85)); glVertex2d(6,   190-button*18-15.3);
      glTexCoord2d(u  , v/6.0*(button+0.85)); glVertex2d(6+32,190-button*18-15.3);
      glTexCoord2d(u  , v/6.0*button);        glVertex2d(6+32,190-button*18);
      glTexCoord2d(0.0, v/6.0*button);        glVertex2d(6,   190-button*18);
      glEnd();
   }

   // draw panel
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

   // draw text scroll
   textscroll.render();
/*
#ifdef HAVE_DEBUG
   // debug text quad
   {
      ua_image img_coords;
      char buffer[256];
      sprintf(buffer,"x=%u y=%u area=%u hit=%08x",cursorx,cursory,
         get_area(cursorx,cursory),hit);
      font_normal.create_string(img_coords,buffer,11);
      img_temp.paste_image(img_coords,2,2,true);
   }
#endif
*/

   // draw mouse cursor
   {
      if (cursor_image_current != cursor_image && !cursor_is_object)
      {
         // change mouse cursor type
         cursor_image_current = cursor_image;
         mousecursor.settype(cursor_image);
      }

      mousecursor.draw();
   }
}

void ua_ingame_orig_screen::update_panel_texture()
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
         buffer << static_cast<unsigned int>(inv.get_weight_avail()) << std::ends;

         ua_image img_weight;
         font_normal.create_string(img_weight,buffer.str().c_str(),224);
         panel.paste_image(img_weight,65,52,true);
      }
   }

   // upload new panel
   tex_panel.convert(panel);
   tex_panel.use();
   tex_panel.upload();
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

   // check for fading in/out
   if ((fade_state==0 || fade_state==2) &&
      ++fade_ticks >= (core->get_tickrate()*fade_time))
   {
      fade_state++;
      fade_ticks=0;

      if (fade_state==3)
      {
         // carry out fadeout_action

         switch(fadeout_action)
         {
         case 0: // leave the ingame_orig screen
            core->pop_screen();
            break;

         case 1: // start "load game" screen
            core->push_screen(new ua_load_game_screen);
            break;

         case 2: // start "save game" screen
            core->push_screen(new ua_save_game_screen);
            break;

         case 3: // start conversation
            core->push_screen(new ua_conversation_screen(fadeout_param));
            break;

         case 4: // start cutscene
            // show cutscene
            core->push_screen(new ua_cutscene_view_screen(fadeout_param));
            break;
         }
      }
   }
}

void ua_ingame_orig_screen::setup_opengl()
{
   // clear color
   glClearColor(0,0,0,0);

   // alpha blending
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // camera setup
   ua_vector3d view_offset(0.0, 0.0, 0.0);
   renderer.init(&core->get_underworld(),&core->get_texmgr(),
      &core->get_critter_pool(),
      view_offset);

   renderer.setup_camera(90.0,
      double(core->get_screen_width())/core->get_screen_height(),
      16.0);

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
   // restrict area when movement mode is on
   if (mouse_move && !click)
   {
      bool modified = false;
      if (cursorx<53)
         cursorx=54;
      if (cursorx>224)
         cursorx=224;

      if (cursory<20)
         cursory=21;
      if (cursory>131)
         cursory=131;
   }

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

   default: break;
   }

   // check inventory items
   ua_ingame_orig_area area_save = area;
   do
   {
      ua_inventory& inv = core->get_underworld().get_inventory();

      if (click)
      {
         inventory_click(inv,pressed,left_button,area);
      }
      else
      {
         // check item dragging
         if (check_dragging && drag_area != area)
            inventory_dragged_item(inv);
      }

   } while (false);
   area = area_save;

   if (in_screen3d && area != ua_area_screen3d)
   {
      // cursor was moved out of 3d screen
      in_screen3d = false;
      cursor_image = 0;
   }

   // check 3d view area
   if (area == ua_area_screen3d)
   {
      in_screen3d = true;

      double relx = double(cursorx-53)/(224-53);
      double rely = double(cursory-20)/(131-20);

      {
         ua_player& player = core->get_underworld().get_player();

         // when pressing mouse, start mouse move mode
         if (click)
         {
            // mouse move is started on pressing mouse button
            mouse_move = pressed;

            if (!mouse_move)
            {
               // disable all modes
               player.set_movement_mode(0,ua_move_slide);
               player.set_movement_mode(0,ua_move_rotate);
               player.set_movement_mode(0,ua_move_walk);

               // set new mouse cursor position
               Uint16 x = unsigned(cursorx*core->get_screen_width()/320.0);
               Uint16 y = unsigned(cursory*core->get_screen_height()/200.0);
               SDL_WarpMouse(x,y);
            }
         }

         // determine cursor image
         double slide, rotate, walk;

         slide = rotate = walk = 10.0;

         if (rely>=0.75)
         {
            // lower part of screen
            if (relx<0.33){ slide = -1.0; cursor_image = 3; } else
            if (relx>=0.66){ slide = 1.0; cursor_image = 4; } else
               { walk = -0.4*(rely-0.75)/0.25; cursor_image = 2; }
         }
         else
         if (rely>=0.6)
         {
            // middle part
            if (relx<0.33){ rotate = (0.33-relx)/0.33; cursor_image = 5; } else
            if (relx>=0.66){ rotate = -(relx-0.66)/0.33; cursor_image = 6; } else
               cursor_image = 0;
         }
         else
         {
            // upper part
            if (relx<0.33){ rotate = (0.33-relx)/0.33; cursor_image = 7; } else
            if (relx>=0.66){ rotate = -(relx-0.66)/0.33; cursor_image = 8; } else
               cursor_image = 1;

            // walking speed increases in range [0.6; 0.2] only
            walk = (0.6-rely)/0.4;
            if (walk>1.0) walk = 1.0;
         }

         if (mouse_move)
         {
            // first, disable all modes
            player.set_movement_mode(0,ua_move_slide);
            player.set_movement_mode(0,ua_move_rotate);
            player.set_movement_mode(0,ua_move_walk);

            // update movement modes and factors
            if (slide<10.0)
            {
               player.set_movement_mode(ua_move_slide);
               player.set_movement_factor(ua_move_slide,slide);
            }

            if (rotate<10.0)
            {
               player.set_movement_mode(ua_move_rotate);
               player.set_movement_factor(ua_move_rotate,rotate);
            }

            if (walk<10.0)
            {
               player.set_movement_mode(ua_move_walk);
               player.set_movement_factor(ua_move_walk,walk);
            }
         }
      }

      // check click
      if (click && pressed)
      {
         int x,y;
         SDL_GetMouseState(&x,&y);

         unsigned int tilex, tiley, id;
         bool isobj;

         renderer.select_pick(x,y,tilex,tiley,isobj,id);
//         hit = ((id+(isobj?0:0x0400))<<16) | (tilex << 8) | tiley;
      }
   }
}

void ua_ingame_orig_screen::inventory_click(ua_inventory& inv,
   bool pressed, bool left_button, ua_ingame_orig_area area)
{
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
      drag_area = area;
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

      Uint16 cursor_object = 0;
      cursor_is_object = false;

      // check if we still have a floating object
      if (inv.get_floating_item() != ua_slot_no_item)
      {
         // still floating? then set new cursor object
         cursor_object =  inv.get_item(inv.get_floating_item()).item_id;
         cursor_is_object = cursor_object != ua_slot_no_item;
      }

      if (cursor_is_object)
         mousecursor.set_custom(img_objects.get_image(cursor_object));
      else
         cursor_image_current = (unsigned int)-1;

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
       gamemode == ua_mode_default)
   {
      // open container
      inv.open_container(item);
      cursor_is_object = false;
      slot_start = 0;

      update_panel_texture();
      return;
   }

   // perform left/right click action
   if (left_button)
   {
      // trigger "look" action
      core->get_underworld().get_scripts().lua_inventory_look(item);
   }
   else
   {
      // trigger "use" action
      core->get_underworld().get_scripts().lua_inventory_use(item);
   }
}

void ua_ingame_orig_screen::inventory_dragged_item(ua_inventory &inv)
{
   // user dragged item out of area
   check_dragging = false;
   inv.float_item(drag_item);

   Uint16 cursor_object = 0;
   cursor_is_object = false;

   // check if we still have a floating object
   if (inv.get_floating_item() != ua_slot_no_item)
   {
      // still floating? then set new cursor object
      cursor_object =  inv.get_item(inv.get_floating_item()).item_id;
      cursor_is_object = cursor_object != ua_slot_no_item;
   }

   if (cursor_is_object)
      mousecursor.set_custom(img_objects.get_image(cursor_object));
   else
      cursor_image_current = (unsigned int)-1;

   update_panel_texture();
}


// virtual functions from ua_underworld_script_callback

void ua_ingame_orig_screen::ui_changed_level(unsigned int level)
{
   ua_texture_manager& texmgr = core->get_texmgr();

   // reset stock texture usage
   texmgr.reset();

   // prepare all used textures
   const std::vector<Uint16>& used_textures =
      core->get_underworld().get_level(level).get_used_textures();

   unsigned int max = used_textures.size();
   for(unsigned int n=0; n<max; n++)
      texmgr.prepare(used_textures[n]);
}

void ua_ingame_orig_screen::ui_start_conv(unsigned int convslot)
{
   // start fading out
   fade_state = 2;
   fade_ticks = 0;
   fadeout_action = 3; // start conversation
   fadeout_param = convslot;
}

void ua_ingame_orig_screen::ui_show_cutscene(unsigned int cutscene)
{
   // start fading out
   fade_state = 2;
   fade_ticks = 0;
   fadeout_action = 4; // start cutscene
   fadeout_param = cutscene;
}

void ua_ingame_orig_screen::ui_print_string(const char* str)
{
   // print to text scroll
   textscroll.print(str);
}

void ua_ingame_orig_screen::ui_show_ingame_anim(unsigned int anim)
{
}

void ua_ingame_orig_screen::ui_cursor_use_item(Uint16 item_id)
{
}

void ua_ingame_orig_screen::ui_cursor_target()
{
}
