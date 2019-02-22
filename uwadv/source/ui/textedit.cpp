//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2004,2019 Underworld Adventures Team
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
/// \file textedit.cpp
/// \brief text edit window
//
#include "common.hpp"
#include "textedit.hpp"

ua_textedit_window::~ua_textedit_window()
{
}

void ua_textedit_window::init(IGame& game, unsigned int xpos,
   unsigned int ypos, unsigned int width, Uint8 my_bg_color,
   Uint8 my_prefix_color, Uint8 my_text_color,
   const char* prefix_text, const char* start_text, bool my_border)
{
   // init variables
   bg_color = my_bg_color;
   prefix_color = my_prefix_color;
   text_color = my_text_color;
   prefix = prefix_text;
   text = start_text;
   border = my_border;
   cursor_pos = text.size();

   font.load(game.get_settings(), ua_font_normal);

   // adjust widths when having a border
   if (border)
   {
      xpos--; ypos--;
      width += 2;
   }

   // init image
   IndexedImage& img = get_image();
   img.create(width, font.get_charheight() + 1 + (border ? 2 : 0));
   img.set_palette(game.get_image_manager().get_palette(0));

   ua_image_quad::init(game, xpos, ypos);

   update_text();
}

void ua_textedit_window::done()
{
}

void ua_textedit_window::update_text()
{
   IndexedImage& img = get_image();
   img.clear(bg_color);

   if (border)
   {
      /// \todo border
      img.fill_rect(0, 0, img.get_xres(), 1, 1);
      img.fill_rect(0, img.get_yres() - 1, img.get_xres(), 1, 1);
   }

   // prefix text
   IndexedImage img_temp;
   if (prefix.size() > 0)
   {
      font.create_string(img_temp, prefix.c_str(), prefix_color);

      img.paste_rect(img_temp, 0, 0, img_temp.get_xres(), img_temp.get_yres(),
         border ? 2 : 1, border ? 2 : 1, true);
   }

   unsigned int prefix_xpos = img_temp.get_xres();

   // main text
   if (text.size() > 0)
   {
      font.create_string(img_temp, text.c_str(), text_color);

      img.paste_image(img_temp, prefix_xpos + (border ? 2 : 1), border ? 2 : 1, true);
   }

   // cursor image pos
   std::string pos_string(text.c_str(), cursor_pos);
   unsigned int cursor_xpos = font.calc_length(pos_string.c_str());

   // draw cursor
   img.fill_rect(prefix_xpos + cursor_xpos + (border ? 1 : 0), border ? 1 : 0, 1, font.get_charheight(), 1);

   update();
}

bool ua_textedit_window::process_event(SDL_Event& event)
{
   bool handled = false;

   // check event type
   switch (event.type)
   {
   case SDL_KEYDOWN:
      // check typeable keys
   {
      // TODO use SDL_TEXTINPUT event instead
      char ch = event.key.keysym.sym;

      if (ch >= SDLK_SPACE && ch <= SDLK_z)
      {
         // add to text and update
         text.insert(cursor_pos, 1, ch);
         cursor_pos++;

         handled = true;
      }

      SDL_Keycode key = event.key.keysym.sym;

      switch (key)
      {
      case SDLK_LEFT:
         if (cursor_pos > 0)
         {
            cursor_pos--;
            handled = true;
         }
         break;

      case SDLK_RIGHT:
         if (cursor_pos < text.size())
         {
            cursor_pos++;
            handled = true;
         }
         break;

      case SDLK_HOME:
         cursor_pos = 0;
         handled = true;
         break;

      case SDLK_END:
         cursor_pos = text.size();
         handled = true;
         break;

      case SDLK_BACKSPACE:
         if (cursor_pos > 0)
         {
            cursor_pos--;
            text.erase(cursor_pos, 1);
            handled = true;
         }
         break;

      case SDLK_DELETE:
         if (text.size() > 0 && cursor_pos < text.size())
         {
            text.erase(cursor_pos, 1);
            handled = true;
         }
         break;

      case SDLK_RETURN:
      case SDLK_ESCAPE:
      {
         // send event
         SDL_Event user_event;
         user_event.type = SDL_USEREVENT;
         user_event.user.code = key == SDLK_RETURN ?
            ua_event_textedit_finished : ua_event_textedit_aborted;
         user_event.user.data1 = NULL;
         user_event.user.data2 = NULL;
         SDL_PushEvent(&user_event);

         handled = true;
      }
      break;
      }

      if (handled)
         update_text();
   }
   }

   return handled || ua_image_quad::process_event(event);
}
