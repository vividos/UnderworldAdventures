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
/*! \file save_game.cpp

   \brief save game screen implementation

   \todo add scroll buttons
   \todo fix savegame preview

*/

// needed includes
#include "common.hpp"
#include "save_game.hpp"
#include "ingame_orig.hpp"
#include "renderer.hpp"
#include "underworld.hpp"
#include <sstream>


// constants

const unsigned int ua_save_game_button::button_width = 55;

const double ua_save_game_screen::fade_time = 0.5;


// ua_save_game_button methods

void ua_save_game_button::init(ua_save_game_screen* my_screen,
   ua_game_interface& game, unsigned int xpos, unsigned int ypos,
   const char* text, ua_save_game_button_id my_id)
{
   screen = my_screen;
   id = my_id;
   leftbuttondown = rightbuttondown = false;

   // load buttons (unpressed/pressed)
   game.get_image_manager().load_list(img_buttons, "chrbtns", 0,3, 3);

   // create window
   image.create(button_width, img_buttons[0].get_yres());
   image.set_palette(game.get_image_manager().get_palette(3));

   ua_image_quad::init(game, xpos, ypos);

   {
      // shorten button images according to button width
      ua_image& button0 = img_buttons[0];
      button0.paste_rect(button0, 66,0, 1,16, button_width-1,0);

      ua_image& button1 = img_buttons[1];
      button1.paste_rect(button1, 66,0, 1,16, button_width-1,0);

      ua_image& button2 = img_buttons[2];
      button2.paste_rect(button2, 66,0, 1,16, button_width-1,0);

      // paste text onto buttons
      ua_font font_normal;
      ua_image img_temp;

      font_normal.load(game.get_settings(), ua_font_chargen);

      // normal
      font_normal.create_string(img_temp, text, 162);

      int pos = (button_width-img_temp.get_xres())/2;
      if (pos<0) pos = 0;

      button1.paste_rect(img_temp, 0,0, button1.get_xres(),button1.get_yres(),
         pos,3, true);

      // highlighted
      font_normal.create_string(img_temp, text, 73);

      pos = (button_width-img_temp.get_xres())/2;
      if (pos<0) pos = 0;

      button2.paste_rect(img_temp, 0,0, button2.get_xres(),button2.get_yres(),
         pos,3, true);
   }

   update_button(false);
}

bool ua_save_game_button::process_event(SDL_Event& event)
{
   // check if mouse left button area
   if (event.type == SDL_MOUSEMOTION && (leftbuttondown || rightbuttondown))
   {
      unsigned int xpos=0, ypos=0;
      calc_mousepos(event, xpos, ypos);

      update_button(in_window(xpos,ypos));
   }

   return ua_window::process_event(event);
}

void ua_save_game_button::mouse_event(bool button_clicked, bool left_button,
   bool button_down, unsigned int mousex, unsigned int mousey)
{
   // update button states
   if (button_clicked)
   {
      if (left_button) leftbuttondown = button_down;
      else rightbuttondown = button_down;
   }

   // update button images
   if (button_clicked)
      update_button(button_down);

   // send "press button" event
   if (button_clicked && !button_down)
      screen->press_button(id);
}

void ua_save_game_button::update_button(bool state_pressed)
{
   // paste base button
   ua_image& button0 = img_buttons[0];
   image.paste_rect(button0, 0,0, button0.get_xres(),button0.get_yres(), 0,0);

   // base border depending on selection
   ua_image& button = img_buttons[state_pressed ? 1 : 2];

   image.paste_rect(button, 0,0,
      button.get_xres(),button.get_yres(), 0,0, true);

   update();
}


// ua_save_game_savegames_list methods

void ua_save_game_savegames_list::init(ua_save_game_screen* my_screen,
   ua_game_interface& game, unsigned int xpos, unsigned int ypos,
   bool my_show_new)
{
   savegames_manager = &game.get_savegames_manager();
   screen = my_screen;
   show_new = my_show_new;

   list_base = 0;
   selected_savegame = -1;

   // load font
   font_normal.load(game.get_settings(), ua_font_normal);

   // setup image
   ua_image_quad::init(game, xpos, ypos);

   get_image().create(119,126);
   get_image().clear(142);

   update();
}

void ua_save_game_savegames_list::update_list()
{
   get_image().clear(142);

   unsigned int charheight = font_normal.get_charheight();

   // show savegames, but only as much entries as fit
   unsigned int numitems = savegames_manager->get_savegames_count() + (show_new ? 0 : 1);

   unsigned int maxitems = unsigned(get_image().get_yres() / charheight);

   if (numitems-list_base > maxitems)
      numitems = list_base+17;

   for(unsigned int i=list_base; i<numitems; i++)
   {
      bool selected = int(i) == selected_savegame;

      std::string desc;
      if (!show_new && i==numitems-1)
      {
         // last entry: new game slot
         //! \todo move to strings
         desc.assign("new savegame slot");
      }
      else
      {
         // get savegame info
         ua_savegame_info info;
         savegames_manager->get_savegame_info(i,info);

         std::ostringstream buffer;
         buffer << i+1 << ". " << std::ends;

         desc.assign(buffer.str().c_str());
         desc.append(info.title);
      }

      // string too long for list field?
      if (font_normal.calc_length(desc.c_str()) > get_image().get_xres()-2)
      {
         desc.append("...");
         std::string::size_type pos = desc.size()-4;

         do
         {
            // delete one char
            desc.erase(pos,1);
            pos--;

         } while(pos > 0 && font_normal.calc_length(desc.c_str()) > get_image().get_xres()-2);
      }

      // when selected, fill background
      if (selected)
         get_image().fill_rect(0,i*(charheight+1)+2,
            get_image().get_xres(),charheight, 162);

      // paste image
      ua_image img_temp;
      font_normal.create_string(img_temp,desc.c_str(),selected ? 73 : 162);

      get_image().ua_image::paste_rect(img_temp, 0,0,
         img_temp.get_xres(), img_temp.get_yres(),
         2,i*(charheight+1)+2, true);
   }

   update();

   screen->update_info();
}

void ua_save_game_savegames_list::mouse_event(bool button_clicked, bool left_button,
   bool button_down, unsigned int mousex, unsigned int mousey)
{
   if (button_clicked && button_down)
   {
      // determine 
      unsigned int item = (mousey-wnd_ypos-2) / (font_normal.get_charheight()+1);
      item += list_base;

      int last_selected = selected_savegame;

      unsigned int numitems = savegames_manager->get_savegames_count() + (show_new ? 0 : 1);

      // check if user selected an empty slot
      if (item < numitems)
         selected_savegame = item;
      else
      {
         // only deselect when clicked on empty slot
         if (button_down)
            selected_savegame = -1;
      }

      // update when needed
      if (last_selected != selected_savegame)
         update_list();
   }
}


// ua_save_game_screen methods

ua_save_game_screen::ua_save_game_screen(ua_game_interface& game,
   bool myfrom_menu)
:ua_screen(game), from_menu(myfrom_menu)
{
}

void ua_save_game_screen::init()
{
   ua_screen::init();

   ua_trace("save game screen started\n");

   game.get_renderer().setup_camera2d();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   show_preview = false;
   edit_desc = false;

   font_btns.load(game.get_settings(), ua_font_chargen);

   game.get_image_manager().load_list(img_faces, "chrbtns", 17, 0, 3);

   // scan for savegames
   game.get_savegames_manager().rescan();

   // load background image
   {
      ua_image temp_back;
      game.get_image_manager().
         load(temp_back, "data/chargen.byt",0, 3, ua_img_byt);

      // prepare left image (savegames list)
      ua_image& img = img_back.get_image();

      img.create(320,200);
      img.set_palette(game.get_image_manager().get_palette(3));
      img.paste_rect(temp_back,160,0, 160,200, 0,0);

      // add frame
      img.paste_rect(temp_back,11,124, 138,6, 10,4);
      img.paste_rect(temp_back,10,130, 6,56, 9,10);
      img.paste_rect(temp_back,10,130, 6,56, 9,66);
      img.paste_rect(temp_back,10,130, 6,19, 9,122);

      img.paste_rect(temp_back,144,130, 6,56, 143,10);
      img.paste_rect(temp_back,144,130, 6,56, 143,66);
      img.paste_rect(temp_back,144,130, 6,19, 143,122);

      img.paste_rect(temp_back,10,186, 140,6, 9,141);

      img.paste_rect(temp_back, 0,0, 160,200, 159,0);

      img_back.init(game, 0,0);
      img_back.update();

      register_window(&img_back);
   }

   // init buttons
   button_load.init(this, game, 17,155, "Load", ua_button_load);
   button_save.init(this, game, 17,177, "Save", ua_button_save);
   button_refresh.init(this, game, 84,155, "Refresh", ua_button_refresh);
   button_exit.init(this, game, 84,177, "Exit", ua_button_exit);

   if (!from_menu)
      register_window(&button_save);
   register_window(&button_load);
   register_window(&button_refresh);
   register_window(&button_exit);

   // init info area
   img_infoarea.get_image().create(128,105);
   img_infoarea.get_image().set_palette(img_back.get_image().get_palette());
   img_infoarea.init(game,160+16,8);
   update_info();

   // init savegames list
   savegames_list.init(this, game, 19,13, from_menu);
   savegames_list.update_list();
   register_window(&savegames_list);

   // init mouse cursor
   mousecursor.init(game,10);
   mousecursor.show(true);

   register_window(&mousecursor);

   // init fadein
   fader.init(true, game.get_tickrate(), fade_time);
   fade_state = 0;
}

void ua_save_game_screen::destroy()
{
   tex_preview.done();

   ua_trace("leaving save game screen\n\n");
}

bool ua_save_game_screen::process_event(SDL_Event& event)
{
   if (ua_screen::process_event(event))
      return true;

   if (edit_desc)
      if (textedit.process_event(event))
         return true;

   // user event?
   if (edit_desc && event.type == SDL_USEREVENT &&
       (event.user.code == ua_event_textedit_finished ||
        event.user.code == ua_event_textedit_aborted))
   {
      // end edit mode
      edit_desc = false;

      textedit.done();

      if (event.user.code == ua_event_textedit_finished)
         save_savegame();

      // refresh list
      savegames_list.update_list();
   }

   switch(event.type)
   {
   case SDL_KEYDOWN: // key was pressed
      switch(event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
         // simulate press on exit button
         if (fade_state == 1)
         {
            press_button(ua_button_exit);
         }
         break;

      default:
         break;
      }
      break;

   default:
      break;
   }

   return true;
}

void ua_save_game_screen::draw()
{
   glClear(GL_COLOR_BUFFER_BIT);

   // determine brightness of images
   {
      Uint8 light = fader.get_fade_value();
      glColor3ub(light,light,light);
   }

   // draw registered windows
   ua_screen::draw();

   // render edit field when needed
   if (edit_desc)
      textedit.draw();

   // render savegame preview image
   if (show_preview)
   {
      glEnable(GL_BLEND);
      img_infoarea.draw();
      glDisable(GL_BLEND);

      tex_preview.use();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      double u,v;
      u = tex_preview.get_tex_u();
      v = tex_preview.get_tex_v();

      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v);   glVertex2i(200,   200-133-50);
      glTexCoord2d(u,   v);   glVertex2i(200+80,200-133-50);
      glTexCoord2d(u,   0.0); glVertex2i(200+80,200-133);
      glTexCoord2d(0.0, 0.0); glVertex2i(200,   200-133);
      glEnd();
   }
}

void ua_save_game_screen::tick()
{
   if (fader.tick())
   {
      fade_state++;

      if (fade_state==3)
      {
         // faded out, do some action
         switch(pressed_button)
         {
         case ua_button_load:
         {
            ua_trace("loading saved game, filename %s\n",
               game.get_savegames_manager().get_savegame_filename(
                  savegames_list.get_selected_savegame()).c_str());

            // clear screen; loading takes a while
            game.get_renderer().clear();

            // load savegame
            ua_savegame sg = game.get_savegames_manager().get_savegame_load(
               savegames_list.get_selected_savegame(),true);
            game.get_underworld().load_game(sg);

            // next screen
            if (from_menu)
               game.replace_screen(new ua_ingame_orig_screen(game),false);
            else
               game.remove_screen();
            break;
         }

         case ua_button_exit:
            game.remove_screen();
            break;
         }
      }
   }
}

void ua_save_game_screen::press_button(ua_save_game_button_id id)
{
   pressed_button = id;

   switch(id)
   {
      // load button
   case ua_button_load:
      {
         // check if user tries to load the "new slot" entry
         if (savegames_list.get_selected_savegame() == -1 ||
             savegames_list.get_selected_savegame() >=
             game.get_savegames_manager().get_savegames_count())
            break;

         // fade out and do action
         fade_state = 2;
         fader.init(false, game.get_tickrate(), fade_time);
      }
      break;

      // refresh button
   case ua_button_refresh:
      {
         // refresh list
         game.get_savegames_manager().rescan();
         savegames_list.update_list();
      }
      break;

      // save button
   case ua_button_save:
      if (!from_menu)
         ask_savegame_desc();
      break;

      // exit button
   case ua_button_exit:
      fade_state = 2;
      fader.init(false, game.get_tickrate(), fade_time);
      break;
   }
}

void ua_save_game_screen::update_info()
{
   int selected_savegame = savegames_list.get_selected_savegame();

   ua_image& img_info = img_infoarea.get_image();

   // restore original image
   img_info.clear(0);

   if (selected_savegame >= 0 &&
       selected_savegame < game.get_savegames_manager().get_savegames_count())
   {
      // get savegame infos
      ua_savegame_info info;
      game.get_savegames_manager().get_savegame_info(selected_savegame,info);

      // show infos
      ua_image img_temp;
      unsigned int textcolor = 73;

      // player name
      unsigned int width = font_btns.calc_length(info.name.c_str());
      unsigned int xpos = width > 128 ? 0 : (128-width)/2;
      font_btns.create_string(img_temp,info.name.c_str(),162);
      img_info.paste_image(img_temp,xpos,2,true);
      img_temp.clear();

      // gender
      std::string text(game.get_underworld().get_strings().
         get_string(2,info.gender+9));
      font_btns.create_string(img_temp, text.c_str(), textcolor);
      img_info.paste_image(img_temp,2,13,true);
      img_temp.clear();

      // profession
      text = game.get_underworld().get_strings().
         get_string(2,info.profession+23);
      font_btns.create_string(img_temp,text.c_str(),textcolor);
      img_info.paste_image(img_temp,125-img_temp.get_xres(),13,true);

      // basic attributes
      for(unsigned int i=0; i<4; i++)
      {
         // text
         text = game.get_underworld().get_strings().get_string(2,i+17);
         font_btns.create_string(img_temp, text.c_str(), textcolor);
         img_info.paste_image(img_temp,75,42+i*17,true);

         // number
         unsigned int val=0;
         switch(i)
         {
         case 0: val = info.strength; break;
         case 1: val = info.dexterity; break;
         case 2: val = info.intelligence; break;
         case 3: val = info.vitality; break;
         }

         std::ostringstream buffer;
         buffer << val << std::ends;

         font_btns.create_string(img_temp, buffer.str().c_str(), textcolor);
         img_info.paste_image(img_temp,110,42+i*17,true);
      }

      // appearance
      unsigned int app = info.appearance+(info.gender==0 ? 0 : 5);
      img_info.paste_image(img_faces[app],13,39,true);

/*
#ifdef HAVE_DEBUG
      // savegame filename
      text = sgmgr->get_savegame_filename(selected_savegame);
      std::string::size_type pos = text.find_last_of("\\/");
      if (pos != std::string::npos)
         text.erase(0,pos+1);

      font_normal.create_string(temp,text.c_str(),162);
      img_back2.paste_image(temp,5,192,true);
#endif
*/
      // do preview image
      tex_preview.init();
      tex_preview.convert(info.image_xres, info.image_yres, &info.image_rgba[0]);
      tex_preview.upload();

      show_preview = true;
   }
   else
   {
      // empty info pane, no preview
      show_preview = false;
   }

   img_infoarea.update();
}

void ua_save_game_screen::ask_savegame_desc()
{
   // ask for a savegame name
   std::string savegame_name;

   ua_savegames_manager& sgmgr = game.get_savegames_manager();

   // select new slot when no savegame was selected
   int selected_savegame = savegames_list.get_selected_savegame();
   if (selected_savegame!=-1 && selected_savegame < sgmgr.get_savegames_count())
   {
      // retrieve savegame title
      ua_savegame_info info;
      sgmgr.get_savegame_info(selected_savegame, info);
      savegame_name = info.title;
   }
   else
      selected_savegame = sgmgr.get_savegames_count();

   //! \todo change
   unsigned int height = 6;
   unsigned int ypos = selected_savegame*(height+1)+13+2;

   // init text edit window
   textedit.init(game, 19,ypos, 119, 162, 1, 73,
      "", savegame_name.c_str(), true);

   edit_desc = true;
}

void ua_save_game_screen::save_savegame()
{
   // set up savegame info
   ua_savegame_info info;
   info.title = textedit.get_text();
   info.game_prefix = game.get_settings().get_string(ua_setting_game_prefix);
   game.get_underworld().get_player().fill_savegame_infos(info);

   ua_savegames_manager& sgmgr = game.get_savegames_manager();

   // saving over existing game?
   int selected_savegame = savegames_list.get_selected_savegame();
   if (selected_savegame != -1 && selected_savegame < sgmgr.get_savegames_count())
   {
      ua_trace("saving game over old savegame slot, filename %s\n",
         sgmgr.get_savegame_filename(selected_savegame).c_str());

      // saving over selected game
      ua_savegame sg = sgmgr.get_savegame_save_overwrite(selected_savegame,info);
      game.get_underworld().save_game(sg);
   }
   else
   {
      ua_trace("saving game to new savegame slot\n");

      // saving to new slot
      ua_savegame sg = sgmgr.get_savegame_save_new_slot(info);
      game.get_underworld().save_game(sg);
   }

   sgmgr.rescan();
}
