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
/// \file start_menu.cpp
/// \brief start menu screen implementation
//
#include "common.hpp"
#include "start_menu.hpp"
#include "audio.hpp"
#include "renderer.hpp"
#include "savegame.hpp"
#include "cutscene_view.hpp"
#include "acknowledgements.hpp"
#include "create_character.hpp"
#include "ingame_orig.hpp"
#include "save_game.hpp"

/// time to fade in/out
const double ua_start_menu_screen::fade_time = 0.5;

/// palette shifts per second
const double ua_start_menu_screen::palette_shifts_per_second = 20.0;

ua_start_menu_screen::ua_start_menu_screen(IGame& game)
   :ua_screen(game)
{
}

void ua_start_menu_screen::init()
{
   ua_screen::init();

   UaTrace("start menu screen started\n");

   // load background image
   game.get_image_manager().load(img_screen.get_image(), "data/opscr.byt",
      0, 2, ua_img_byt);
   img_screen.get_image().clone_palette();

   img_screen.init(game, 0, 0);

   // load button graphics
   game.get_image_manager().load_list(img_buttons, "opbtn", 0, 8, 2);

   // set up mouse cursor
   mousecursor.init(game, 0);
   mousecursor.show(true);

   register_window(&mousecursor);

   resume();
}

void ua_start_menu_screen::resume()
{
   UaTrace("resuming start menu screen\n");

   game.get_renderer().setup_camera2d();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   img_screen.update();

   fader.init(true, game.get_tickrate(), fade_time);

   // set other flags/values
   stage = 0;
   journey_avail = game.get_savegames_manager().GetSavegamesCount() > 0;
   selected_area = -1;
   shiftcount = 0.0;
   reupload_image = true;
}

void ua_start_menu_screen::destroy()
{
   UaTrace("start menu screen ended\n\n");

   img_screen.destroy();
}

void ua_start_menu_screen::draw()
{
   // no need to clear the screen, since we overdraw all of it anyway

   // do we need to reupload the image quad texture?
   if (reupload_image)
   {
      // combine button graphics with background image
      const unsigned int btn_coords[] =
      {
         98,81, 81,104, 72,128, 85,153
      };

      unsigned int max = journey_avail ? 4 : 3;
      for (unsigned int i = 0; i < max; i++)
      {
         unsigned int btnnr = i * 2;
         if (int(i) == selected_area) btnnr++;

         IndexedImage& img = img_buttons[btnnr];
         img_screen.get_image().paste_rect(img,
            0, 0, img.get_xres(), img.get_yres(),
            btn_coords[i * 2], btn_coords[i * 2 + 1]);
      }

      img_screen.update();

      reupload_image = false;
   }

   // calculate brightness of texture quad
   Uint8 light = fader.get_fade_value();

   glColor3ub(light, light, light);

   // render screen image and mouse
   img_screen.draw();

   // draw subwindows
   ua_screen::draw();
}

bool ua_start_menu_screen::process_event(SDL_Event& event)
{
   bool ret = false;

   ret |= ua_screen::process_event(event);

   int last_selected_area = selected_area;

   if (event.type == SDL_KEYDOWN)
   {
      // handle key presses
      switch (event.key.keysym.sym)
      {
      case SDLK_UP:
         // select the area above, if possible
         if (selected_area == -1) selected_area = 0;
         if (selected_area > 0) selected_area--;
         ret = true;
         break;

      case SDLK_DOWN:
         // select the area below, if possible
         if (selected_area + 1 < (journey_avail ? 4 : 3))
            selected_area++;
         ret = true;
         break;

      case SDLK_RETURN:
         // simulate clicking on that area
         if (stage == 1)
         {
            stage++;
            ret = true;
         }
         break;

      default:
         break;
      }
   }

   // user event?
   if (event.type == SDL_USEREVENT && event.user.code == ua_event_resume_screen)
      resume();

   // check if selected area changed
   if (selected_area != last_selected_area)
      reupload_image = true;

   return ret;
}

void ua_start_menu_screen::tick()
{
   // when fading in or out, check if blend time is over
   if ((stage == 0 || stage == 2) && fader.tick())
   {
      // do next stage
      stage++;
   }

   // do palette shifting
   shiftcount += 1.0 / game.get_tickrate();
   if (shiftcount >= 1.0 / palette_shifts_per_second)
   {
      shiftcount -= 1.0 / palette_shifts_per_second;

      // shift palette
      img_screen.get_image().get_palette()->rotate(64, 64, false);

      // initiate new upload
      reupload_image = true;
   }

   // in stage 3, we really press the selected button
   if (stage == 3)
   {
      press_button();
      stage = 0;

      // fade in, in case user returns to menu screen
      fader.init(true, game.get_tickrate(), fade_time);
   }
}

void ua_start_menu_screen::press_button()
{
   UaTrace("button %u was pressed\n", selected_area);

   switch (selected_area)
   {
   case 0: // "introduction"
      game.replace_screen(new ua_cutscene_view_screen(game, 0), true);
      break;

   case 1: // "create character"
      game.replace_screen(new ua_create_character_screen(game), true);
      break;

   case 2: // "acknowledgements"
      game.replace_screen(new ua_acknowledgements_screen(game), true);
      break;

   case 3: // "journey onward"
      if (journey_avail)
      {
         // "load game" screen (with later starting "orig. ingame ui")
         game.replace_screen(new ua_save_game_screen(game, true, true), true);
      }
      break;
   }
}

/// \todo remove leftbuttondown and rightbuttondown usage
void ua_start_menu_screen::mouse_event(bool button_clicked, bool left_button, bool button_down,
   unsigned int mousex, unsigned int mousey)
{
   // check over which area we are
   int area = -1;

   if (mousex > 64 && mousex < 240 && mousey >= 81 && mousey <= 180)
   {
      if (mousey < 104) area = 0;
      else if (mousey < 128) area = 1;
      else if (mousey < 153) area = 2;
      else area = 3;
   }

   if (area == 3 && !journey_avail)
      return;

   // a button click action?
   if (button_clicked)
   {
      // only in stage 1
      if (stage == 1)
      {
         if (button_down)
         {
            // mouse button down
            selected_area = area;
         }
         else
         {
            // mouse button up

            // determine if user released the mouse button over the same area
            if (area != -1 && selected_area == area)
            {
               stage++; // next stage
               fader.init(false, game.get_tickrate(), fade_time);

               // fade out music when selecting "introduction"
               if (selected_area == 0)
                  game.get_audio_manager().FadeoutMusic(fade_time);
            }
         }
      }
   }
   else
   {
      // a mouse move action with at least one button down
      Uint8 mouse_state = SDL_GetMouseState(NULL, NULL);
      if (stage == 1 &&
         (mouse_state & (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK)) != 0 &&
         area != -1)
         selected_area = area;
   }
}
