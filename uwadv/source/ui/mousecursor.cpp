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
/*! \file mousecursor.cpp

   \brief mouse cursor

*/

// needed includes
#include "common.hpp"
#include "mousecursor.hpp"
#include "game_interface.hpp"
#include "renderer.hpp"


// ua_mousecursor methods

void ua_mousecursor::init(ua_game_interface& game, unsigned int initialtype)
{
   ua_window::create(0,0,0,0);

   game.get_image_manager().load_list(img_cursors, "cursors");

   mousetex.init(&game.get_renderer().get_texture_manager(),1);

   isvisible = false;
   set_type(initialtype);
}

void ua_mousecursor::show(bool show)
{
   isvisible = show;
}

void ua_mousecursor::set_type(unsigned int type)
{
   if (type < img_cursors.size())
      set_custom(img_cursors[type]);
}

void ua_mousecursor::set_custom(ua_image& cursorimg)
{
   wnd_width = cursorimg.get_xres();
   wnd_height = cursorimg.get_yres();

   ua_image cursorimg2;
   cursorimg2.create(wnd_width+1,wnd_height+1);
   cursorimg2.paste_rect(cursorimg, 0,0, wnd_width,wnd_height, 0,0);

   cursorimg2.get_palette() = cursorimg.get_palette();

   mousetex.convert(cursorimg2);
   mousetex.upload();
}

void ua_mousecursor::destroy()
{
   mousetex.done();
}

void ua_mousecursor::draw()
{
   if (!isvisible)
      return;

   bool blend_enabled = glIsEnabled(GL_BLEND) == GL_TRUE;

   // mouse cursor must be drawn with blending enabled
   if (!blend_enabled)
      glEnable(GL_BLEND);

   mousetex.use();
   double u = mousetex.get_tex_u(), v = mousetex.get_tex_v();
   u -= mousetex.get_tex_u()/mousetex.get_xres();
   v -= mousetex.get_tex_v()/mousetex.get_yres();

   // set wrap parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(wnd_xpos,           200-wnd_ypos);
   glTexCoord2d(u  , v  ); glVertex2i(wnd_xpos+wnd_width, 200-wnd_ypos);
   glTexCoord2d(u  , 0.0); glVertex2i(wnd_xpos+wnd_width, 200-wnd_ypos+wnd_height);
   glTexCoord2d(0.0, 0.0); glVertex2i(wnd_xpos,           200-wnd_ypos+wnd_height);
   glEnd();

   if (!blend_enabled)
      glDisable(GL_BLEND);
}

bool ua_mousecursor::process_event(SDL_Event& event)
{
   if (event.type == SDL_MOUSEMOTION)
   {
      // convert to 320x200 screen coordinates
      SDL_Surface* surf = SDL_GetVideoSurface();

      wnd_xpos = unsigned(event.motion.x * 320.0 / surf->w - wnd_width/2.0);
      wnd_ypos = unsigned(event.motion.y * 200.0 / surf->h + wnd_height/2.0);
   }

   return false;
}
