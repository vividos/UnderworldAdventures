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
/// \file mousecursor.cpp
/// \brief mouse cursor
//
#include "common.hpp"
#include "mousecursor.hpp"
#include "game_interface.hpp"
#include "renderer.hpp"

void ua_mousecursor::init(IGame& game, unsigned int initialtype)
{
   ua_window::create(0, 0, 0, 0);

   game.get_image_manager().load_list(img_cursors, "cursors");

   mousetex.init(1);

   isvisible = false;
   set_type(initialtype);

   smooth_ui = game.get_settings().GetBool(Base::settingUISmooth);
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

void ua_mousecursor::set_custom(IndexedImage& cursorimg)
{
   wnd_width = cursorimg.get_xres();
   wnd_height = cursorimg.get_yres();

   IndexedImage cursorimg2;
   cursorimg2.create(wnd_width + 1, wnd_height + 1);
   cursorimg2.paste_rect(cursorimg, 0, 0, wnd_width, wnd_height, 0, 0);

   cursorimg2.set_palette(cursorimg.get_palette());

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
   u -= mousetex.get_tex_u() / mousetex.get_xres();
   v -= mousetex.get_tex_v() / mousetex.get_yres();

   // set wrap parameter
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, smooth_ui ? GL_LINEAR : GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, smooth_ui ? GL_LINEAR : GL_NEAREST);

   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v); glVertex2i(wnd_xpos, 200 - wnd_ypos);
   glTexCoord2d(u, v); glVertex2i(wnd_xpos + wnd_width, 200 - wnd_ypos);
   glTexCoord2d(u, 0.0); glVertex2i(wnd_xpos + wnd_width, 200 - wnd_ypos + wnd_height);
   glTexCoord2d(0.0, 0.0); glVertex2i(wnd_xpos, 200 - wnd_ypos + wnd_height);
   glEnd();

   if (!blend_enabled)
      glDisable(GL_BLEND);
}

bool ua_mousecursor::process_event(SDL_Event& event)
{
   if (event.type == SDL_MOUSEMOTION)
   {
      // convert to 320x200 screen coordinates
      int windowWidth = 320, windowHeight = 200;
      // TODO check if needed
      //SDL_GetWindowSize(m_window, &windowWidth, &windowHeight);

      wnd_xpos = unsigned(event.motion.x * 320.0 / windowWidth - wnd_width / 2.0);
      wnd_ypos = unsigned(event.motion.y * 200.0 / windowHeight + wnd_height / 2.0);
   }

   return false;
}
