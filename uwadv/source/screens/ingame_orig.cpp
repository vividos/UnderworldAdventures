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


// ua_ingame_orig_screen methods

void ua_ingame_orig_screen::init()
{
   ua_trace("orig. ingame user interface started\n");

   // clear screen; this init() can take quite some time
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

   walk = false;
   fov = 90.0;
   playeryangle = 0.0;
   leftbuttondown = rightbuttondown = false;
   cursor_image = 0;
   cursorx = cursory = 0;
   slot_start = 0;

   setup_opengl();

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
   tex.init();
   tex.convert(core->get_texmgr(),img_back);
   tex.use(core->get_texmgr());
   tex.upload();

   img_temp.create(img_back.get_xres(),img_back.get_yres(),0,
      img_back.get_palette());

   img_compass.load(settings,"compass");
   img_bodies.load(settings,"bodies");
   img_cursors.load(settings,"cursors",0,1);
   img_objects.load(settings,"objects");

   bool female = core->get_underworld().get_player().get_attr(ua_attr_gender)!=0;
   img_armor.load(settings,female ? "armor_f" : "armor_m");

   font_normal.init(core->get_settings(),ua_font_normal);
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

   case SDL_MOUSEMOTION:
   case SDL_MOUSEBUTTONDOWN:
   case SDL_MOUSEBUTTONUP:
      handle_mouse_action(event);
      break;
   }
}

void ua_ingame_orig_screen::handle_key_action(Uint8 type, SDL_keysym &keysym)
{
   static int curlevel = 0;

   switch(keysym.sym)
   {
   case SDLK_UP:
      walk = (type==SDL_KEYDOWN);
      walk_dir = 0.0;
      break;
   case SDLK_DOWN:
      walk = (type==SDL_KEYDOWN);
      walk_dir = 180.0;
      break;
   case SDLK_RIGHT:
      walk = (type==SDL_KEYDOWN);
      walk_dir = 90.0;
      break;
   case SDLK_LEFT:
      walk = (type==SDL_KEYDOWN);
      walk_dir = 270.0;
      break;

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

void ua_ingame_orig_screen::handle_mouse_action(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_MOUSEMOTION:
      // mouse has moved
      if (leftbuttondown)
      {
         double playerxangle = core->get_underworld().get_player().get_angle();

         int x,y;
         SDL_GetRelativeMouseState(&x,&y);
         {
            playerxangle += x*0.2;
            playeryangle += y*0.2;
         }

         // view rotate angle has to stay between 0 and 360 degree
         while (playerxangle > 360.0 || playerxangle < 0.0 )
            playerxangle = fmod(playerxangle+360.0,360.0);

         core->get_underworld().get_player().set_angle(playerxangle);

         while (playeryangle > 180.0 || playeryangle < -180.0 )
            playeryangle = fmod(playeryangle-360.0,360.0);

         // restrict up-down view angle
         if (playeryangle < -40.0)
            playeryangle = -40.0;

         if (playeryangle > 40.0)
            playeryangle = 40.0;
      }

      // calculate cursor position
      {
         int x,y;
         SDL_GetMouseState(&x,&y);
         x = int(double(x)/core->get_screen_width()*320.0);
         y = int(double(y)/core->get_screen_height()*200.0);
         cursorx = x<5 ? 0 : x-5;
         cursory = y<5 ? 0 : y-5;
      }
      break;

   case SDL_MOUSEBUTTONDOWN:
      {
         Uint8 state = SDL_GetRelativeMouseState(NULL,NULL);
         if (SDL_BUTTON(state)==SDL_BUTTON_LEFT)
            leftbuttondown = true;
         else
            leftbuttondown = false;
      }
      break;

   case SDL_MOUSEBUTTONUP:
      leftbuttondown = false;
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
      glRotated(playeryangle+270.0, 1.0, 0.0, 0.0);
      glRotated(-xangle+90.0, 0.0, 0.0, 1.0);

      ua_player &pl = core->get_underworld().get_player();

      // move to position on map
      glTranslated(-pl.get_xpos(),-pl.get_ypos(),-plheight);
   }

   ua_frustum fr(pl.get_xpos(),pl.get_ypos(),plheight,xangle,-playeryangle,fov,16.0);

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

      int tip_coords[16*2] =
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

   // inventory
   {
      ua_inventory &inv = core->get_underworld().get_inventory();

      unsigned int start = 0;

      if (inv.get_container_item_id()!=0xffff)
      {
         // inside a container

         // draw container we're in
         Uint16 cont_id = inv.get_item(inv.get_container_item_id()).item_id;

         img_temp.paste_image(
            img_objects.get_image(cont_id),
            242, 60,true);

         // begin at current slot start
         start = slot_start;
      }

      // paste inventory slots
      for(unsigned int i=0; i<8; i++)
      {
         if (start < inv.get_num_slots())
         {
            Uint16 item = inv.get_slot_item(start);
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
            start++;
         }
      }

      // do paperdoll items
      for(unsigned int j=ua_slot_lefthand; j<ua_slot_max; j++)
      {
         // paperdoll image coordinates
         unsigned int slot_coords[] =
         {
            241,34, 295,34, // hand
            244,12, 293,12, // shoulder
            254,50, 284,50, // finger
            268,26,         // legs
            262,22,         // chest
            261,41,         // hands
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

   // mouse cursor; should be the last one to paste
   {
      img_temp.paste_image(img_cursors.get_image(cursor_image),cursorx,cursory,true);
   }

   // upload ui texture
   tex.convert(core->get_texmgr(),img_temp);
   tex.use(core->get_texmgr());
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
   // movement
   if (walk)
      core->get_underworld().walk_player(
         core->get_underworld().get_player().get_angle()-walk_dir);
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
#ifdef HAVE_DEBUG
   glFogf(GL_FOG_DENSITY,0.1f);
#else
   glFogf(GL_FOG_DENSITY,0.65f);
#endif
   glFogf(GL_FOG_START,0.0);
   glFogf(GL_FOG_END,1.0);
   int fog_color[4] = { 0,0,0,0 };
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

   glEnable(GL_SCISSOR_TEST);

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
}
