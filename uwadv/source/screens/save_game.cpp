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
/*! \file save_game.cpp

   \brief save game screen implementation

   TODO: use ua_screen_ctrl_base::get_area() to check for buttons and
   list of savegames, etc.

   TODO: add scroll buttons

*/

// needed includes
#include "common.hpp"
#include "save_game.hpp"
#include "ingame_orig.hpp"
#include <sstream>


// globals

const unsigned int ua_save_game_btn_width = 55;

static unsigned int ua_save_game_btn_coords[] =
{
   17,155,
   84,155,
   17,177,
   84,177
};


// constants

//! time to fade in/out
const double ua_save_game_screen::fade_time = 0.5;


// ua_save_game_screen methods

ua_save_game_screen::ua_save_game_screen(bool myfrom_menu)
:from_menu(myfrom_menu)
{
}

void ua_save_game_screen::init(ua_game_core_interface* thecore)
{
   ua_ui_screen_base::init(thecore);

   ua_trace("save game screen started\n");

   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // set OpenGL flags
   glBindTexture(GL_TEXTURE_2D,0);
   glClearColor(0.0, 0.0, 0.0, 0.0);

   glEnable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);

   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   sgmgr = &core->get_savegames_mgr();

   list_base_game = 0;
   button_highlight = -1;
   selected_savegame = -1;
   button_pressed = false;
   fade_state = 0;
   fade_ticks = 0;
   edit_desc = false;

   // scan for savegames
   sgmgr->rescan();

   // load fonts
   font_btns.init(core->get_settings(), ua_font_chargen);
   font_normal.init(core->get_settings(), ua_font_normal);

   img_buttons.load(core->get_settings(),"chrbtns",0,0,3);

   // load background images
   {
      ua_image img_back;
      img_back.load_raw(core->get_settings(),"data/chargen.byt",3);

      // shorten buttons
      ua_image& button0 = img_buttons.get_image(0);
      ua_image& button1 = img_buttons.get_image(1);
      ua_image& button2 = img_buttons.get_image(2);

      button0.paste_rect(button0,66,0, 1,16, ua_save_game_btn_width-1,0);
      button1.paste_rect(button1,66,0, 1,16, ua_save_game_btn_width-1,0);
      button2.paste_rect(button2,66,0, 1,16, ua_save_game_btn_width-1,0);

      // prepare left image (savegames list)
      img_back1.create(160,200,0,3);
      img_back1.init(&core->get_texmgr(),0,0, 160,200);
      img_back1.paste_rect(img_back,160,0, 160,200, 0,0);

      // add some buttons
      img_back1.paste_rect(img_buttons.get_image(0),0,0, ua_save_game_btn_width,16, 17,155);
      img_back1.paste_rect(img_buttons.get_image(0),0,0, ua_save_game_btn_width,16, 84,155);

      if (!from_menu)
         img_back1.paste_rect(img_buttons.get_image(0),0,0, ua_save_game_btn_width,16, 17,177);
      img_back1.paste_rect(img_buttons.get_image(0),0,0, ua_save_game_btn_width,16, 84,177);

      // add frame
      img_back1.paste_rect(img_back,11,124, 138,6, 10,4);
      img_back1.paste_rect(img_back,10,130, 6,56, 9,10);
      img_back1.paste_rect(img_back,10,130, 6,56, 9,66);
      img_back1.paste_rect(img_back,10,130, 6,19, 9,122);

      img_back1.paste_rect(img_back,144,130, 6,56, 143,10);
      img_back1.paste_rect(img_back,144,130, 6,56, 143,66);
      img_back1.paste_rect(img_back,144,130, 6,19, 143,122);

      img_back1.paste_rect(img_back,10,186, 140,6, 9,141);

      // prepare right image (savegame info)
      img_back2.create(160,200,0,3);
      img_back2.init(&core->get_texmgr(),160,0, 160,200);
      img_back2.paste_rect(img_back,0,0, 160,200, 0,0);

      img_back2_orig.create(160,200,0,3);
      img_back2_orig.paste_rect(img_back,0,0, 160,200, 0,0);

      // update all controls
      update_buttons();
      update_list();
   }

   // init mouse cursor
   mousecursor.init(core,10);
   mousecursor.show(true);
}

void ua_save_game_screen::done()
{
   img_back1.done();
   img_back2.done();

   mousecursor.done();

   if (edit_desc)
      desc_scroll.done();

   tex_preview.done();

   ua_trace("leaving save game screen\n\n");
}

bool ua_save_game_screen::handle_event(SDL_Event& event)
{
   if (edit_desc && desc_scroll.handle_event(event))
   {
      if (desc_scroll.is_input_done(desc))
      {
         // set up savegame info
         ua_savegame_info info;
         info.title = desc;
         info.fill_infos(core->get_underworld().get_player());

         // saving game
         if (selected_savegame < sgmgr->get_savegames_count())
         {
            ua_trace("saving game over old savegame slot, filename %s\n",
               sgmgr->get_savegame_filename(selected_savegame).c_str());

            // saving over selected game
            ua_savegame sg = sgmgr->get_savegame_save_overwrite(selected_savegame,info);
            core->get_underworld().save_game(sg);
         }
         else
         {
            ua_trace("saving game to new savegame slot\n");

            // saving to new slot
            ua_savegame sg = sgmgr->get_savegame_save_new_slot(info);
            core->get_underworld().save_game(sg);
         }

         // refresh list
         sgmgr->rescan();

         desc_scroll.done();
         edit_desc = false;
         desc.erase();
         button_highlight = -1;

         selected_savegame = -1;
         update_list();
      }
      return true;
   }

   // calculate cursor position
   if (fade_state == 1 && (
       event.type == SDL_MOUSEBUTTONDOWN ||
       event.type == SDL_MOUSEBUTTONUP ||
       (event.type == SDL_MOUSEMOTION && button_pressed)))
   {
      int x,y;
      SDL_GetMouseState(&x,&y);
      unsigned int cursorx = unsigned(double(x)/core->get_screen_width()*320.0);
      unsigned int cursory = unsigned(double(y)/core->get_screen_height()*200.0);

      // check if a button was pressed
      button_highlight = -1;
      for(unsigned int i=0; i<4; i++)
      {
         // don't check "Save" button when coming from start menu
         if (from_menu && i==2)
            continue;

         if (cursorx>ua_save_game_btn_coords[i*2] &&
             cursorx<ua_save_game_btn_coords[i*2]+ua_save_game_btn_width &&
             cursory>ua_save_game_btn_coords[i*2+1] &&
             cursory<ua_save_game_btn_coords[i*2+1]+16)
         {
            button_highlight = i;
            break;
         }
      }

      // check if an entry was selected
      if (cursorx>19 && cursorx<19+119 &&
          cursory>13 && cursory<13+126)
      {
         unsigned int item = (cursory-13-2)/(font_normal.get_charheight()+1);
         int last_selected = selected_savegame;

         // check if user selected an empty slot
         if (item+list_base_game<sgmgr->get_savegames_count()+(from_menu ? 0 : 1))
            selected_savegame = item;
         else
         {
            // only deselect when clicked on empty slot
            if (event.type == SDL_MOUSEBUTTONDOWN)
               selected_savegame = -1;
         }

         // update when needed
         if (last_selected != selected_savegame)
            update_list();
      }
   }

   switch(event.type)
   {
   case SDL_MOUSEBUTTONDOWN: // mouse button was pressed down
      if (fade_state == 1)
      {
         // highlight button when needed
         update_buttons();
         img_back1.convert_upload();

         button_pressed = true;
      }
      break;

   case SDL_MOUSEBUTTONUP: // mouse button was released
      if (fade_state == 1)
      {
         // check if all buttons are released
         Uint8 state = SDL_GetMouseState(NULL,NULL);
         if ((state&(SDL_BUTTON_LMASK|SDL_BUTTON_RMASK)) == 0)
         {
            // press button
            press_button();

            // reset buttons
            update_buttons();
            img_back1.convert_upload();

            button_pressed = false;
         }
      }

   case SDL_MOUSEMOTION: // mouse has moved
      mousecursor.updatepos();
      break;
   }

   return true;
}

void ua_save_game_screen::render()
{
   // determine brightness of images
   {
      Uint8 light = 255;
      switch(fade_state)
      {
      case 0: // fading in
         light = Uint8(255*(double(fade_ticks) / (core->get_tickrate()*fade_time)));
         break;

      case 2: // fading out
         light = Uint8(255-255*(double(fade_ticks) / (core->get_tickrate()*fade_time)));
         break;

      case 3: // exiting
         light = 0;
         break;
      }

      glColor3ub(light,light,light);
   }

   // draw background
   glDisable(GL_BLEND);
   img_back1.render();
   img_back2.render();

   // render edit field when needed
   if (edit_desc)
      desc_scroll.render();

   // render savegame preview image
   if (show_preview)
   {
      tex_preview.use();

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

   // draw mouse cursor
   glEnable(GL_BLEND);
   mousecursor.draw();
}

void ua_save_game_screen::tick()
{
   // check for fading in/out
   if ((fade_state==0 || fade_state==2) &&
      ++fade_ticks >= (core->get_tickrate()*fade_time))
   {
      fade_state++;
      fade_ticks=0;

      if (fade_state==3)
      {
         // faded out, do some action
         switch(button_highlight)
         {
         case 0: // load
		 {
            ua_trace("loading saved game, filename %s\n",
               sgmgr->get_savegame_filename(selected_savegame).c_str());

            // load savegame
            ua_savegame sg = sgmgr->get_savegame_load(selected_savegame,true);
            core->get_underworld().load_game(sg);

            // next screen
            if (from_menu)
               core->replace_screen(new ua_ingame_orig_screen);
            else
               core->pop_screen();
            break;
		 }
         case 3: // exit
            core->pop_screen();
            break;
         }
      }
   }
}

void ua_save_game_screen::update_list()
{
   img_back1.fill_rect(19,13, 119,126, 142);

   unsigned int height = font_normal.get_charheight();

   // show savegames, but at most 17 entries
   unsigned int max = sgmgr->get_savegames_count()+(from_menu ? 0 : 1);
   if (max-list_base_game>17) max = list_base_game+17;

   for(unsigned int i=list_base_game; i<max; i++)
   {
      bool selected = i==selected_savegame;

      std::string desc;
      if (!from_menu && i==max-1)
      {
         // last entry: new game slot
         desc.assign("new savegame slot");
      }
      else
      {
         // get savegame info
         ua_savegame_info info;
         sgmgr->get_savegame_info(i,info);

         std::ostringstream buffer;
         buffer << i+1 << ". " << std::ends;

         desc.assign(buffer.str().c_str());
         desc.append(info.title);
      }

      // string too long for list field?
      if (font_normal.calc_length(desc.c_str())>117)
      {
         desc.append("...");
         std::string::size_type pos = desc.size()-4;

         do
         {
            // delete one char
            desc.erase(pos,1);
            pos--;

         } while(pos > 0 && font_normal.calc_length(desc.c_str())>117);
      }

      // when selected, fill background
      if (selected)
         img_back1.fill_rect(19,i*(height+1)+13+2,119,height,162);

      // paste image
      ua_image temp;
      font_normal.create_string(temp,desc.c_str(),selected ? 73 : 162);

      img_back1.paste_rect(temp,0,0, 119-2,height,
         19+2,i*(height+1)+13+2,true);
   }

   img_back1.convert_upload();

   update_info();
}

void ua_save_game_screen::update_buttons()
{
   // TODO put in string resources
   static const char* texts[] =
   {
      "Load", "Refresh", "Save", "Exit"
   };

   // paste buttons and captions
   for(unsigned int i=0; i<4; i++)
   {
      // don't draw "Save" button when coming from start menu
      if (from_menu && i==2)
         continue;

      // button frame
      ua_image& button = img_buttons.get_image(button_highlight == signed(i) ? 1 : 2);

      img_back1.paste_rect(button,0,0, ua_save_game_btn_width,16,
         ua_save_game_btn_coords[i*2], ua_save_game_btn_coords[i*2+1], true);

      // button caption
      ua_image temp;
      font_btns.create_string(temp,texts[i],button_highlight == signed(i) ? 162 : 73);

      int pos = (ua_save_game_btn_width-temp.get_xres())/2;
      if (pos<0) pos = 0;

      img_back1.paste_image(temp,
         ua_save_game_btn_coords[i*2] + pos,
         ua_save_game_btn_coords[i*2+1]+3, true);
   }
}

void ua_save_game_screen::press_button()
{
   switch(button_highlight)
   {
   case 0: // load
      {
         // check if user tries to load the "new slot" entry
         if (selected_savegame>=sgmgr->get_savegames_count())
         {
            button_highlight = -1;
            break;
         }

         // fade out and do action
         fade_state = 2;
      }
      break;

   case 1: // refresh
      {
         // refresh list
         sgmgr->rescan();

         button_highlight = -1;
         selected_savegame = -1;
         update_list();
      }
      break;

   case 2: // save
      if (!from_menu)
      {
         // ask for a savegame name
         edit_desc = true;

         // select new slot when no savegame was selected
         if (selected_savegame==-1)
            selected_savegame = sgmgr->get_savegames_count();

         unsigned int height = font_normal.get_charheight();
         unsigned int ypos = selected_savegame*(height+1)+13+2;

         // get title string as description
         if (selected_savegame < sgmgr->get_savegames_count())
         {
            ua_savegame_info info;
            sgmgr->get_savegame_info(selected_savegame,info);
            desc.assign(info.title);
         }

         desc_scroll.init(*core,19,ypos,119,height+1,1,162);
         desc_scroll.set_color(73);
         desc_scroll.clear_scroll();
         desc_scroll.enter_input_mode(desc.c_str());
      }
      break;

   case 3: // exit
      fade_state = 2;
      break;
   }
}

void ua_save_game_screen::update_info()
{
   // restore original image
   img_back2.paste_image(img_back2_orig,0,0);

   if (selected_savegame < sgmgr->get_savegames_count())
   {
      // get savegame infos
      ua_savegame_info info;
      sgmgr->get_savegame_info(selected_savegame,info);

      // show infos
      ua_image temp;
      unsigned int textcolor = 73;

      // player name
      unsigned int width = font_btns.calc_length(info.name.c_str());
      font_btns.create_string(temp,info.name.c_str(),162);
      img_back2.paste_image(temp,(160-width)/2,10,true);

      // gender
      std::string text(core->get_strings().get_string(2,info.gender+9));
      font_btns.create_string(temp,text.c_str(),textcolor);
      img_back2.paste_image(temp,18,21,true);

      // profession
      text = core->get_strings().get_string(2,info.profession+23);
      font_btns.create_string(temp,text.c_str(),textcolor);
      img_back2.paste_image(temp,141-temp.get_xres(),21,true);

      // basic attributes
      for(unsigned int i=0; i<4; i++)
      {
         // text
         text = core->get_strings().get_string(2,i+17);
         font_btns.create_string(temp,text.c_str(),textcolor);
         img_back2.paste_image(temp,93,50+i*17,true);

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

         font_btns.create_string(temp,buffer.str().c_str(),textcolor);
         img_back2.paste_image(temp,126,50+i*17,true);
      }

      // appearance
      ua_image& appimg = img_buttons.get_image(
         info.appearance+(info.gender==0 ? 0 : 5)+17);
      img_back2.paste_image(appimg,31+(info.gender),47,true);

#ifdef HAVE_DEBUG
      // savegame filename
      text = sgmgr->get_savegame_filename(selected_savegame);
      std::string::size_type pos = text.find_last_of("\\/");
      if (pos != std::string::npos)
         text.erase(0,pos+1);

      font_normal.create_string(temp,text.c_str(),162);
      img_back2.paste_image(temp,5,192,true);
#endif

      // do preview image
      tex_preview.init(&core->get_texmgr());
      tex_preview.convert(info.image_xres, info.image_yres, &info.image_rgba[0]);
      tex_preview.use();
      tex_preview.upload();

      show_preview = true;
   }
   else
   {
      // empty info pane, no preview
      show_preview = false;
   }

   img_back2.convert_upload();
}
