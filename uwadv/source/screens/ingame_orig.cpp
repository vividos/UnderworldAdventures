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
/*! \file ingame_orig.cpp

   \brief ingame user interface, from the original uw1

*/

// needed includes
#include "common.hpp"
#include "ingame_orig.hpp"
#include "files.hpp"
#include "renderer.hpp"
#include "underworld.hpp"
/*
#include "uamath.hpp"
#include "save_game.hpp"
#include "cutscene_view.hpp"
#include "conversation.hpp"
#include <sstream>
#include <iomanip>
*/


// constants

//! time to fade in/out
const double ua_ingame_orig_screen::fade_time = 0.5;


// ua_ingame_orig_screen methods

void ua_ingame_orig_screen::init()
{
   ua_screen::init();

   ua_trace("orig. ingame user interface started\n");

   // init all variables
   move_turn_left = move_turn_right = move_walk_forward =
      move_walk_backwards = move_run_forward = false;

   gamemode = ua_mode_default;
   tickcount = 0;

   fade_state = 0;

   fadeout_action = ua_action_none;
   fadeout_param = 0;



   // set OpenGL flags
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);



   ua_settings &settings = game->get_settings();

   // load keymap
   ua_trace("loading keymaps ...\n");
   {
      keymap.init(settings);

      // load keymap from uadata resources
      std::string keymap_name(settings.get_string(ua_setting_game_prefix));
      keymap_name.append("/keymap.cfg");

      ua_trace(" keymap: %s\n",keymap_name.c_str());
      keymap.load(game->get_files_manager().get_uadata_file(keymap_name.c_str()));

      // load custom keymap
      keymap_name = settings.get_string(ua_setting_custom_keymap);

      ua_trace(" keymap: %s",keymap_name.c_str());
      SDL_RWops* rwops = SDL_RWFromFile(keymap_name.c_str(),"rb");

      if (rwops!=NULL)
         keymap.load(rwops);
      else
         ua_trace(" => not available");

      ua_trace("\n");
   }


   register_keymap(&keymap);



   // TODO move to level changing code
   game->get_renderer().prepare_level(game->get_underworld().get_current_level());



   // init images/subwindows

   // load background
   {
      if (settings.get_gametype() == ua_game_uw2)
      {
         // uw2: no background for now
         img_background.get_image().create(320,200);
      }
      else
      {
         // uw1/uw_demo
         const char* mainscreenname = "data/main.byt";

         // replace name when using uw_demo
         if (settings.get_gametype() == ua_game_uw_demo)
            mainscreenname = "data/dmain.byt";

         game->get_image_manager().ua_image_manager::load(img_background.get_image(),
            mainscreenname, 0, 0, ua_img_byt);
      }

      img_background.init(*game, 0,0);
      img_background.update();

      //register_window(&img_background);
   }



   // init mouse cursor
   mousecursor.init(*game,0);
   mousecursor.show(true);

   register_window(&mousecursor);

   resume();
}

void ua_ingame_orig_screen::suspend()
{
}

void ua_ingame_orig_screen::resume()
{


   // setup fade-in
   fade_state = 0;
   fading.init(true, game->get_tickrate(), fade_time);
}

void ua_ingame_orig_screen::destroy()
{
   suspend();

   // only windows and other stuff has to be destroyed that wasn't registered
   // with ua_screen::register_window()



   ua_trace("orig. ingame user interface finished\n\n");
}

void ua_ingame_orig_screen::draw()
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   // setup 3d world
   ua_vector3d view_offset(0.0, 0.0, 0.0);
   game->get_renderer().setup_camera3d(view_offset);

   // set up viewport for small window
   {
      SDL_Surface* surf = SDL_GetVideoSurface();

      // calculate viewport for window at 52/68 to 226/182
      unsigned int xpos, ypos, width, height;
      xpos = (surf->w / 320.0) * 52;
      ypos = (surf->h / 200.0) * 68;
      width = (surf->w / 320.0) * (226-52);
      height = (surf->h / 200.0) * (182-68);

      glViewport(xpos,ypos, width,height);

      // render a const world
      game->get_renderer().render_underworld(game->get_underworld());

      // reset viewport
      glViewport(0,0, surf->w,surf->h);
   }

   // render 2d user interface
   {
      game->get_renderer().setup_camera2d();

      glEnable(GL_BLEND);

      if (fade_state == 0 || fade_state == 2)
      {
         // when fading in/out, lay a quad over the already rendered 3d view
         Uint8 alpha = fading.get_fade_value();
         if (alpha < 255)
         {
            glColor4ub(0,0,0,255-alpha);
            glBindTexture(GL_TEXTURE_2D,0);

            // draw quad
            glBegin(GL_QUADS);
            glVertex2i( 52,  68);
            glVertex2i(226,  68);
            glVertex2i(226, 182);
            glVertex2i( 52, 182);
            glEnd();
         }

         glColor3ub(alpha,alpha,alpha);
      }
      else
         glColor3ub(255,255,255);

      // draw background
      img_background.draw();
      glDisable(GL_BLEND);

      // all other registered windows
      ua_screen::draw();
   }
}

bool ua_ingame_orig_screen::process_event(SDL_Event& event)
{
   return ua_screen::process_event(event);
}

void ua_ingame_orig_screen::key_event(bool key_down, ua_key_value key)
{
   ua_player& pl = game->get_underworld().get_player();

   switch(key)
   {
      // run forward keys
   case ua_key_run_forward:
   case ua_key_run_forward_easymove:
      move_run_forward = key_down;
      if (key_down)
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

      break;

      // walk forward keys
   case ua_key_walk_forward:
   case ua_key_walk_forward_easymove:
      move_walk_forward = key_down;
      if (key_down)
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
      break;

      // turn left keys
   case ua_key_turn_left:
   case ua_key_turn_left_easymove:
      move_turn_left = key_down;
      if (key_down)
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
      break;

      // turn right keys
   case ua_key_turn_right:
   case ua_key_turn_right_easymove:
      move_turn_right = key_down;
      if (key_down)
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
      break;

      // slide left key
   case ua_key_slide_left:
      if (key_down)
      {
         pl.set_movement_factor(ua_move_slide,-1.0);
         pl.set_movement_mode(ua_move_slide);
      }
      else
         pl.set_movement_mode(0,ua_move_slide);
      break;

      // slide right key
   case ua_key_slide_right:
      if (key_down)
      {
         pl.set_movement_factor(ua_move_slide,1.0);
         pl.set_movement_mode(ua_move_slide);
      }
      else
         pl.set_movement_mode(0,ua_move_slide);
      break;

      // walk backwards keys
   case ua_key_walk_backwards:
   case ua_key_walk_backwards_easymove:
      move_walk_backwards = key_down;

      if (key_down)
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
      break;

      // look up key
   case ua_key_look_up:
      if (key_down)
      {
         pl.set_movement_factor(ua_move_lookup,1.0);
         pl.set_movement_mode(ua_move_lookup);
      }
      else
         pl.set_movement_mode(0,ua_move_lookup);
      break;

      // look down key
   case ua_key_look_down:
      if (key_down)
      {
         pl.set_movement_factor(ua_move_lookup,-1.0);
         pl.set_movement_mode(ua_move_lookup);
      }
      else
         pl.set_movement_mode(0,ua_move_lookup);
      break;

      // center look key
   case ua_key_center_view:
      if (key_down)
      {
         pl.set_movement_mode(0,ua_move_lookup);
         pl.set_movement_factor(ua_move_lookup,0.0);
         pl.set_angle_pan(0.0);
      }
      break;

      // exit screen key
   case ua_key_ua_return_menu:
      if (key_down)
         schedule_action(ua_action_exit, true);
      break;

      // quicksave key
   case ua_key_special_quicksave:
      if (key_down)
      {
         ua_trace("quicksaving ... ");
         //ui_print_string("quicksaving ...");

         schedule_action(ua_action_quicksave,false);
         ua_trace("done\n");
      }
      break;

      // quickload key
   case ua_key_special_quickload:
      if (key_down)
      {
         ua_trace("quickloading ... ");
         //ui_print_string("quickloading ...");

         schedule_action(ua_action_quickload,false);
         ua_trace("done\n");
      }
      break;

      // "load game" key
   case ua_key_game_restore_game:
      if (key_down)
      {
/*
         fade_state = 2;
         fade_ticks = 0;
         fadeout_action = 1; // load game

         // TODO why render a preview image when restoring?
         // render savegame preview image
         do_screenshot(false,80,50);
         core->get_savegames_mgr().set_save_screenshot(
            screenshot_rgba,screenshot_xres,screenshot_yres);
*/
      }
      break;

      // "save game" key
   case ua_key_game_save_game:
      if (key_down)
      {
/*
         fade_state = 2;
         fade_ticks = 0;
         fadeout_action = 2; // save game

         // render savegame preview image
         do_screenshot(false,80,50);
         core->get_savegames_mgr().set_save_screenshot(
            screenshot_rgba,screenshot_xres,screenshot_yres);
*/
      }
      break;

      // "debugger" key
   case ua_key_ua_debug:
      if (key_down)
      {
//         game->get_debug_interface()->start_debugger();
      }
      break;

      // "screenshot" key
   case ua_key_ua_screenshot:
      if (key_down)
      {
         // take a screenshot
//         do_screenshot(true,0,0);
//         save_screenshot();
      }
      break;

   default:
      break;
   }
}

void ua_ingame_orig_screen::tick()
{
   // evaluate underworld;
   // only evaluate when the user is not in the options menu
   if (fadeout_action == ua_action_none && gamemode != ua_mode_options)
   {
      game->get_underworld().eval_underworld(double(tickcount)/game->get_tickrate());

      tickcount++;

      // do renderer-specific tick processing
      game->get_renderer().tick(1.0/game->get_tickrate());
   }

   // action to perform?
   if ((fade_state == 0 || fade_state == 2) && fading.tick())
   {
      fade_state++;

      // now it's time
      do_action(fadeout_action);

      fadeout_action = ua_action_none;
   }
}

void ua_ingame_orig_screen::schedule_action(ua_ingame_orig_action action, bool fadeout_before)
{
   fadeout_action = action;

   if (fadeout_before)
   {
      fade_state = 2;
      fading.init(false,game->get_tickrate(), fade_time);
   }
}

/*! \todo implement all actions */
void ua_ingame_orig_screen::do_action(ua_ingame_orig_action action)
{
   switch(action)
   {
   case ua_action_none:
      break;

      // leave the ingame_orig screen
   case ua_action_exit:
      game->remove_screen();
      break;

      // start "load game" screen
   case ua_action_load_game:
      //game->push_screen(new ua_save_game_screen(false));
      break;

      // start "save game" screen
   case ua_action_save_game:
      //core->push_screen(new ua_save_game_screen(false));
      break;

      // quickloading
   case ua_action_quickload:
      {
/*
         ua_savegame sg = game->get_savegames_manager().get_quicksave(
            false,game->get_underworld().get_player());
         game->get_underworld().load_game(sg);
*/
      }
      break;

      // quicksaving
   case ua_action_quicksave:
      {
/*
         // render savegame preview image
         do_screenshot(false,80,50);
         game->get_savegames_managergr().set_save_screenshot(
            screenshot_rgba,screenshot_xres,screenshot_yres);

         ua_savegame sg = game->get_savegames_manager().get_quicksave(
            true,game->get_underworld().get_player());
         game->get_underworld().save_game(sg);
*/
      }
      break;

      // starts conversation
   case ua_action_conversation:
//      core->push_screen(new ua_conversation_screen(conv_level,conv_objpos));
      break;

      // shows cutscene
   case ua_action_cutscene:
//      core->push_screen(new ua_cutscene_view_screen(fadeout_param));
      break;

   default:
      break;
   }
}

/*
// tables

ua_screen_area_data ua_ingame_orig_area_table[] =
{
   { ua_area_screen3d, 53,224, 20,131 },

   { ua_area_menu_button0, 7, 38,  11,  28 },
   { ua_area_menu_button1, 7, 38,  29,  45 },
   { ua_area_menu_button2, 7, 38,  46,  64 },
   { ua_area_menu_button3, 7, 38,  65,  82 },
   { ua_area_menu_button4, 7, 38,  83, 100 },
   { ua_area_menu_button5, 7, 38, 101, 119 },

   { ua_area_none, 0,0, 320,200 }
};

void ua_ingame_orig_screen::init(ua_game_core_interface* thecore)
{
   cursor_image = 0;
   mouse_move = false;
   prio_cursor = false;

   in_screen3d = false;

   panel.init_panel(core,this);

   fadeout_action = 0;

   // adjust scroll width for uw_demo
   scrollwidth = 289;

   if (core->get_settings().get_gametype() == ua_game_uw_demo)
      scrollwidth = 218;

   // load some images
   img_objects.load(settings,"objects");

   // background image
   {
      // fill message scroll area
      img_back.fill_rect(16,169, scrollwidth,30, 42);

      // fill panel area
      img_back.fill_rect(236,7, 83,114, 1);

      }
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
      img_back.copy_rect(img_cmd_buttons,6,10,32,108);

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

   resume();

   textscroll.print("Welcome to the Underworld Adventures!\n http://uwadv.sourceforge.net/");
}

void ua_ingame_orig_screen::suspend()
{
   ua_trace("suspending orig. ingame user interface\n\n");

   panel.suspend();

   // clear screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

   // unregister script callbacks
   core->get_underworld().get_scripts().register_callback(NULL);

   img_back.done();

   // clean up textures
   tex_compass.done();
   tex_flasks.done();
   tex_cmd_buttons.done();

   mousecursor.done();
   textscroll.done();

   renderer.done();

   // reset stock textures
   core->get_texmgr().reset();

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
   textscroll.set_color(46);

   // register script callbacks
   core->get_underworld().get_scripts().register_callback(this);

   // prepare level textures
   ui_changed_level(core->get_underworld().get_player().get_attr(ua_attr_maplevel));

   // background image
   img_back.init(&core->get_texmgr(),0,0,320,320);
   img_back.convert_upload();

   // init textures

   // compass texture
   tex_compass.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR);
   tex_compass.convert(img_compass.get_image(0));
   tex_compass.upload();
   compass_curimg = 0;

   // flasks textures
   flasks_curimg[0] = flasks_curimg[1] = 0;
   tex_flasks.init(&core->get_texmgr(),2,GL_LINEAR,GL_LINEAR);
   tex_flasks.convert(img_flasks[0].get_image(flasks_curimg[0]),0);
   tex_flasks.upload(0);

   tex_flasks.convert(img_flasks[1].get_image(flasks_curimg[1]),1);
   tex_flasks.upload(1);

   // command buttons
   tex_cmd_buttons.init(&core->get_texmgr(),1,GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);
   tex_cmd_buttons.convert(img_cmd_buttons);
   tex_cmd_buttons.upload();

   // panel texture
   panel.resume();

   if (fadeout_action == 3)
   {
      // after conversations, play "Dark Abyss"
      core->get_audio().start_music(1,true);
   }
   else
   {
      // normal start, play "Descent"
      core->get_audio().start_music(2,true);
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

   // draw background image
   img_back.render();

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
         tex_flasks.upload(0);
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
         tex_flasks.upload(1);
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
   panel.render();

   // draw text scroll
   textscroll.render();

   // draw mouse cursor
   mousecursor.draw();
}

void ua_ingame_orig_screen::setup_opengl()
{
   // set up scissor test

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

   unsigned int area = get_area(ua_ingame_orig_area_table,cursorx,cursory);

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

   if (in_screen3d && area != ua_area_screen3d)
   {
      // cursor was moved out of 3d screen
      in_screen3d = false;
      set_cursor_image(false,0);
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
         if (click && left_button)
         {
            // mouse move is started on pressing mouse button
            mouse_move = pressed;

            if (!mouse_move)
            {
               // disable all modes (when possible)
               if (!move_walk_forward && !move_run_forward && !move_walk_backwards)
                  player.set_movement_mode(0,ua_move_walk);

               if (!move_turn_left && !move_turn_right)
                  player.set_movement_mode(0,ua_move_rotate);

               player.set_movement_mode(0,ua_move_slide);

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
            if (relx<0.33){ slide = -1.0; set_cursor_image(false,3); } else
            if (relx>=0.66){ slide = 1.0; set_cursor_image(false,4); } else
               { walk = -0.4*(rely-0.75)/0.25; set_cursor_image(false,2); }
         }
         else
         if (rely>=0.6)
         {
            // middle part
            if (relx<0.33){ rotate = (0.33-relx)/0.33; set_cursor_image(false,5); } else
            if (relx>=0.66){ rotate = -(relx-0.66)/0.33; set_cursor_image(false,6); } else
               set_cursor_image(false,0);
         }
         else
         {
            // upper part
            if (relx<0.33){ rotate = (0.33-relx)/0.33; set_cursor_image(false,7); } else
            if (relx>=0.66){ rotate = -(relx-0.66)/0.33; set_cursor_image(false,8); } else
               set_cursor_image(false,1);

            // walking speed increases in range [0.6; 0.2] only
            walk = (0.6-rely)/0.4;
            if (walk>1.0) walk = 1.0;
         }

         if (mouse_move)
         {
            // disable all modes (when not active through keyboard movement)
            // and update movement modes and factors
            if (!move_walk_forward && !move_run_forward && !move_walk_backwards)
            {
               player.set_movement_mode(0,ua_move_walk);

               if (walk<10.0)
               {
                  player.set_movement_mode(ua_move_walk);
                  player.set_movement_factor(ua_move_walk,walk);
               }
            }

            if (!move_turn_left && !move_turn_right)
            {
               player.set_movement_mode(0,ua_move_rotate);

               if (rotate<10.0)
               {
                  player.set_movement_mode(ua_move_rotate);
                  player.set_movement_factor(ua_move_rotate,rotate);
               }
            }

            {
               player.set_movement_mode(0,ua_move_slide);

               if (slide<10.0)
               {
                  player.set_movement_mode(ua_move_slide);
                  player.set_movement_factor(ua_move_slide,slide);
               }
            }
         }
      }

      // check click
      if (click && pressed && !left_button)
      {
         int x,y;
         SDL_GetMouseState(&x,&y);

         unsigned int tilex=0, tiley=0, id=0;
         bool isobj = true;

         renderer.select_pick(x,y,tilex,tiley,isobj,id);

         if (gamemode == ua_mode_default || gamemode == ua_mode_look)
         {
            // "look" action
            if (isobj)
            {
               if (id != 0)
               {
                  // user clicked on an object
                  Uint32 level = core->get_underworld().get_player().get_attr(ua_attr_maplevel);
                  core->get_underworld().get_scripts().lua_objlist_look(level,id);
               }
            }
            else
            {
               // user clicked on a texture
               ua_trace("looking at wall/ceiling, tile=%02x/%02x, id=%04x\n",
                  tilex,tiley,id);

               core->get_underworld().get_scripts().lua_wall_look(id);
            }
         }
         else
         if (gamemode == ua_mode_use)
         {
            if (isobj)
            {
               // use object in map
               Uint32 level = core->get_underworld().get_player().get_attr(ua_attr_maplevel);
               core->get_underworld().get_scripts().lua_objlist_use(level,id);
            }
            else
            {
            }
         }
         else
         if (gamemode == ua_mode_get)
         {
            if (isobj)
            {
               // get object from map
               Uint32 level = core->get_underworld().get_player().get_attr(ua_attr_maplevel);
               core->get_underworld().get_scripts().lua_objlist_get(level,id);

               ua_inventory& inv = core->get_underworld().get_inventory();

               // set appropriate cursor
               if (inv.get_floating_item() != ua_item_none)
               {
                  Uint16 item_id = inv.get_item(inv.get_floating_item()).item_id;
                  set_cursor_image(true,item_id,true);
               }
               else
                  set_cursor_image(false,0xffff,true);
            }
            else
            {
               // getting tilemap wall/ceiling/floor? not possible
            }
         }

         if (gamemode == ua_mode_talk && isobj)
         {
            Uint32 level = core->get_underworld().get_player().get_attr(ua_attr_maplevel);
            core->get_underworld().get_scripts().lua_objlist_talk(level,id);
         }
      }
   }
}


// virtual functions from ua_underworld_script_callback

void ua_ingame_orig_screen::ui_changed_level(unsigned int level)
{
   ua_texture_manager& texmgr = core->get_texmgr();

   // reset stock texture usage
   texmgr.reset();

   // prepare all used wall/ceiling textures
   {
      const std::vector<Uint16>& used_textures =
         core->get_underworld().get_level(level).get_used_textures();

      unsigned int max = used_textures.size();
      for(unsigned int n=0; n<max; n++)
         texmgr.prepare(used_textures[n]);
   }

   // prepare all switch, door and tmobj textures
   {
      unsigned int n;
      for(n=0; n<16; n++) texmgr.prepare(ua_tex_stock_switches+n);
      for(n=0; n<13; n++) texmgr.prepare(ua_tex_stock_door+n);
      for(n=0; n<33; n++) texmgr.prepare(ua_tex_stock_tmobj+n);
   }
}

void ua_ingame_orig_screen::ui_start_conv(unsigned int level, unsigned int objpos)
{
   // start fading out
   fade_state = 2;
   fade_ticks = 0;
   fadeout_action = 3; // start conversation

   conv_level = level;
   conv_objpos = objpos;

   // fade out audio track
   core->get_audio().fadeout_music(fade_time);
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

void ua_ingame_orig_screen::do_screenshot(bool with_menu, unsigned int xres, unsigned int yres)
{
   if (xres==0) xres = core->get_screen_width();
   if (yres==0) yres = core->get_screen_height();

   // set up camera and viewport
   glViewport(0,0,xres,yres);
   renderer.setup_camera(90.0,double(xres)/yres,16.0);

   glClear(GL_COLOR_BUFFER_BIT);

   // render scene
   if (with_menu)
      render();
   else
   {
      glDisable(GL_SCISSOR_TEST);
      renderer.render();
      glEnable(GL_SCISSOR_TEST);
   }

   // prepare screenshot
   screenshot_xres = xres;
   screenshot_yres = yres;

   screenshot_rgba.clear();
   screenshot_rgba.resize(xres*yres,0);

   // read in scanlines
   glReadBuffer(GL_BACK);

   for(int i=yres-1; i>=0 ;i--)
   {
      glReadPixels(0, i, xres, 1, GL_RGBA, GL_UNSIGNED_BYTE,
         &screenshot_rgba[(yres-1-i)*xres]);
   }

   // reset camera and viewport
   glViewport(0,0,core->get_screen_width(),core->get_screen_height());

   renderer.setup_camera(90.0,
      double(core->get_screen_width())/core->get_screen_height(),
      16.0);
}

void ua_ingame_orig_screen::set_cursor_image(bool is_object, Uint16 image,
   bool prio)
{
   if (!prio && prio_cursor)
      return; // do nothing; priority cursor in effect

//   ua_trace("set_cursor_image(%s, %04x, %s)\n",
//      is_object?"true":"false", image, prio?"true":"false");

   if (image==0xffff)
   {
      if (prio_cursor)
      {
         // reset cursor image
         prio_cursor = false;
      }
      image = 0;
   }
   else
      prio_cursor = prio;

   cursor_image = image;

   if (!is_object)
   {
      // change mouse cursor type
//      ua_trace("mousecursor.settype(%04x);\n",cursor_image);
      mousecursor.settype(cursor_image);
   }
   else
   {
//      ua_trace("mousecursor.set_custom(img_objects.get_image(%04x));\n",cursor_image);
      mousecursor.set_custom(img_objects.get_image(cursor_image));
   }
}
*/
