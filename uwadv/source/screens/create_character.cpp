/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Dirk Manders

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
/*! \file create_character.cpp

   \brief character creation screen

*/

// needed includes
#include <time.h>
#include "common.hpp"
#include "create_character.hpp"
#include "ingame_orig.hpp"
#include <sstream>


// constants

//! time needed to fade in/out text
const double ua_fade_time = 0.5;

//! maximum number of buttons in a group (shown on-screen at once)
const unsigned char ua_maxbuttons = 10;

//! global script actions
enum ua_elua_globalaction
{
   gactInit = 0,   // initialize
   gactDeinit = 1  // deinitialize
};

//! script actions 
enum ua_elua_action
{
   actEnd = 0,            // ends the character creation screen (no params)
   actSetInitVal = 1,     // sets init values (param1=stringblock, param2=buttongroup x-coord)
   actSetUIBtnGroup = 2,  // sets the specified button group (param1=heading, param2=buttontype, param3=buttontable)
   actSetUIText = 3,      // sets the specified text at the specified location (param1=stringno, param2=x-coord, param3=y-coord)
   actSetUICustText = 4,  // sets the specified custom text at the specified location (param1=text, param2=x-coord, param3=y-coord, param4=alignment)
   actSetUINumber = 5,    // sets the specifed number at the specified location
   actSetUIImg = 6,       // sets the specified image at the specified location
   actUIClear = 7,        // clears all screen elements (not the background)
   actUIUpdate = 8,       // updates the screen after a SetUIxxx/UIClear action (no params)
   actSetPlayerName = 9,  // does what the name suggests (param1=name)
   actSetPlayerAttr = 10, // does what the name suggests (param1=attribute, param2=value)
   actSetPlayerSkill = 11 // does what the name suggests (param1=skill, param2=value)
};

//! custom type for identifying buttons/text 
enum ua_echarcreationvalue
{
   ccvNone = 0
};

//! custom type for identifying buttons/text 
enum ua_ebuttontype
{
   btText = 0,      // standard button with text from string table
   btImage = 1,     // square button with image
   btInput = 2,     // input area
   btTimer = 3      // virtual button acting as a countdown timer
};

//! global variable that points to the current screen
ua_create_character_screen *current_screen = 0;

// ua_create_character_screen methods

void ua_create_character_screen::init()
{
   ua_trace("character creation screen started\n");

   // get a pointer to to current player
   pplayer = &(core->get_underworld().get_player());

   // setup orthogonal projection
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0,320,0,200);
   glMatrixMode(GL_MODELVIEW);

   // set OpenGL flags
   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D,0);

   glDisable(GL_DEPTH_TEST);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // load background image
   bgimg.load_raw(core->get_settings(),"data/chargen.byt",3);

   // init screen image
   img.create(320, 200, 0, 3);
   tex.init(&core->get_texmgr());
   tex.convert(img);
   tex.use();
   tex.upload();

   // init mouse cursor
   mousecursor = new ua_mousecursor(core);
   mousecursor->show(true);

   // get palette #3
   memcpy(palette,core->get_texmgr().get_palette(3),sizeof(ua_onepalette));

   // load button graphics
   img_buttons.load(core->get_settings(),"chrbtns",0,0,3);

   // init text
   font.init(core->get_settings(), ua_font_chargen);

   // intial variable values
   ended = false;
   newgame = false;
   selected_button = 0;
   prev_button = 0;
   changed = false;
   fadingstage = 1;
   tickcount = 0;
   cdttickcount = 0;
   buttondown = false;
   strblock = 2;
   bgxpos = 240;
   btng_caption = 0;
   btng_buttontype = 0;
   btng_buttoncount = 0;
   btng_buttonimg_normal = 0;
   btng_buttonimg_highlight = 0;
   btng_buttons = new unsigned int[ua_maxbuttons];
   inputtext = new char[32];
   inputtext[0] = 0;
   btnimgs = new unsigned char[5];

   // init lua scripting
   initluascript();
}

void ua_create_character_screen::initluascript()
{
   // get a new lua state
   L = lua_open(128);

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

   // register C functions
   lua_register(L,"cchar_do_action", cchar_do_action);

   // load lua interface script for constants
   if (0!=core->get_filesmgr().load_lua_script(L,"uw1/scripts/uwinterface"))
      ended = true;

   // load lua cutscene script
   if (0!=core->get_filesmgr().load_lua_script(L,"uw1/scripts/createchar"))
      ended = true;

   // store pointer to this instance in a global var
   current_screen = this;

   // Init script with seed value for random numbers
   cchar_global(gactInit, clock());
}

void ua_create_character_screen::cchar_global(int globalaction, int seed)
{
    // call "cchar_global()"
   lua_getglobal(L,"cchar_global");
   lua_pushnumber(L, globalaction);
   lua_pushnumber(L, seed);
   int ret = lua_call(L,2,0);
   if (ret!=0)
   {
      ua_trace("Lua function call cchar_global ended with error code %u\n", ret);
      ended = true;
   }

}

void ua_create_character_screen::done()
{
   delete[] btnimgs;
   delete[] inputtext;
   delete[] btng_buttons;

   tex.done();
   delete mousecursor;

   // clear screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();

   // close Lua
   lua_close(L);
   ua_trace("character creation screen ended\n");

   current_screen = 0;
}

void ua_create_character_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
      // don't handle any keystroke if Alt or Ctrl is down
      if (((event.key.keysym.mod & KMOD_ALT)>0) || ((event.key.keysym.mod & KMOD_CTRL)>0))
         return;

      // handle key presses
      if ((btng_buttontype==btInput) && 
          (((event.key.keysym.sym>=SDLK_a) && (event.key.keysym.sym<=SDLK_z)) || 
           ((event.key.keysym.sym>=SDLK_0) && (event.key.keysym.sym<=SDLK_9)) ||
           (event.key.keysym.sym==SDLK_BACKSPACE) || 
           (event.key.keysym.sym==SDLK_SPACE) || 
           (event.key.keysym.sym==SDLK_MINUS) ||
           (event.key.keysym.sym==SDLK_UNDERSCORE)))
      {
         char c = event.key.keysym.sym;
         if (((event.key.keysym.mod & KMOD_SHIFT)>0) && ((c>='a') && (c<='z')))
            c -= 32;
         handleinputchar(c);
         drawbuttongroup();
         changed = true;
      }

      switch(event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
         cchar_global(gactDeinit,0);
         break;

      case SDLK_PAGEUP:
      case SDLK_HOME:
         selected_button=0;
         break;

      case SDLK_PAGEDOWN:
      case SDLK_END:
         selected_button=btng_buttoncount-1;
         break;

      case SDLK_UP:
         // select the button above, if possible
         if (selected_button==-1) selected_button=0;
         if (selected_button>0) selected_button--;
         break;

      case SDLK_DOWN:
         // select the button below, if possible
         if (selected_button+1<btng_buttoncount)
            selected_button++;
         break;

      case SDLK_RIGHT:
         // select the button to the right, if possible
         if (selected_button+btng_buttonspercolumn<btng_buttoncount)
            selected_button += btng_buttonspercolumn;
         break;

      case SDLK_LEFT:
         // select the button to the right, if possible
         if (selected_button-btng_buttonspercolumn>=0)
            selected_button -= btng_buttonspercolumn;
         break;

      case SDLK_RETURN:
         // simulate clicking on that area
         press_button(selected_button);
         break;
      }
      break;

   case SDL_MOUSEBUTTONDOWN:
      // select the area where the mouse button is pressed
      buttondown=true;
      {
         int ret = getbuttonover();
         if (ret>=0)
            selected_button = ret;
      }
      break;

   case SDL_MOUSEMOTION:
      {
         mousecursor->updatepos();
         if (buttondown)
         {
            int ret = getbuttonover();
            if (ret>=0)
               selected_button = ret;
         }
      }
      break;

   case SDL_MOUSEBUTTONUP:
      buttondown=false;
      {
         // determine if user released the mouse button over the same area
         int ret = getbuttonover();
         if ((ret>=0) && (ret==selected_button))
            press_button(selected_button);
      }
      break;
   }
}

void ua_create_character_screen::handleinputchar(char c)
{
   int i = strlen(inputtext);
   if (c==8) 
   {
      if (i>0)
         inputtext[i-1] = 0;
      return;
   }
   if (i>=31)
      return;
   inputtext[i] = c;
   inputtext[i+1] = 0;
}

void ua_create_character_screen::do_action()
{
   int n=lua_gettop(L);
   unsigned int action = (n<1) ? 0 : static_cast<unsigned int>(lua_tonumber(L,1));

   switch(static_cast<ua_elua_action>(action))
   {
   case actEnd:
      ended = true;
      newgame = (n>1) && (static_cast<unsigned int>(lua_tonumber(L,2))==1);
      fadingstage = -1;
      ua_trace("end request by char. creation script\n");
      break;

   case actSetInitVal:
   {
      if (n<5) break;
      strblock = static_cast<unsigned int>(lua_tonumber(L,2));
      bgxpos = static_cast<unsigned int>(lua_tonumber(L,3));
      textcolor_normal = static_cast<unsigned int>(lua_tonumber(L,4));
      textcolor_highlight = static_cast<unsigned int>(lua_tonumber(L,5));
      int ic = lua_getn(L, 6);
      if (ic>5) ic = 5;
      for (int i=0; i<ic; i++)
      {
         lua_rawgeti(L, 6, i+1);
         btnimgs[i] = static_cast<unsigned int>(lua_tonumber(L,7));
         lua_pop(L, 1);
      }
      ua_trace("init values set by char. creation script\n");
      break;
   }

   case actSetUIBtnGroup:
   {
      if (n<3) break;
      btng_caption = static_cast<unsigned int>(lua_tonumber(L,2));
      btng_buttontype = static_cast<unsigned int>(lua_tonumber(L,3));
      if (btng_buttontype==btTimer)
      {
         countdowntime = btng_caption/1000.0;
         cdttickcount = 0;
         btng_caption = 0;
         btng_buttoncount = btng_buttonspercolumn = 1;
      } 
      else
      {
         btng_buttoncount = lua_getn(L, 4);
         if ((btng_buttontype==btInput) && (btng_buttoncount>0))
         {
            inputtext[0] = 0;
            btng_buttoncount = 1;
         }
         for (int i=0; i<btng_buttoncount; i++)
         {
            lua_rawgeti(L, 4, i+1);
            btng_buttons[i] = static_cast<unsigned int>(lua_tonumber(L,5));
            lua_pop(L, 1);
         }

         switch (btng_buttontype)
         {
            case btText:
               btng_buttonimg_normal = btnimgs[0];
               btng_buttonimg_highlight = btnimgs[1];
               break;
            case btImage: 
               btng_buttonimg_normal = btnimgs[2];
               btng_buttonimg_highlight = btnimgs[3];
               break;
            case btInput: 
               btng_buttonimg_normal = btnimgs[4];
               break;
         }
         btng_buttonspercolumn = (btng_buttoncount>8) ? btng_buttoncount/2 : btng_buttoncount;
      }
      selected_button=prev_button=0;
      drawbuttongroup();
      ua_trace("new buttongroup set by char. creation script, caption nr: %d, %d buttons\n", btng_caption, btng_buttoncount);
      break;
   }

   case actSetUIText:
   {
      if (n<5) break;
      drawtext(static_cast<unsigned int>(lua_tonumber(L,2)),
               static_cast<unsigned int>(lua_tonumber(L,3)),
               static_cast<unsigned int>(lua_tonumber(L,4)),
               static_cast<unsigned int>(lua_tonumber(L,5)),
               textcolor_normal,
               (n>5) ? static_cast<unsigned int>(lua_tonumber(L,6)) : -1);
      break;
   }

   case actSetUICustText:
   {
      if (n<5) break;
      drawtext(lua_tostring(L,2),
               static_cast<unsigned int>(lua_tonumber(L,3)),
               static_cast<unsigned int>(lua_tonumber(L,4)),
               static_cast<unsigned int>(lua_tonumber(L,5)));
      break;
   }

   case actSetUINumber:
   {
      if (n<4) break;
      drawnumber(static_cast<unsigned int>(lua_tonumber(L,2)),
               static_cast<unsigned int>(lua_tonumber(L,3)),
               static_cast<unsigned int>(lua_tonumber(L,4)));
      break;
   }

   case actSetUIImg:
   {
      if (n<4) break;
      ua_image cimg = img_buttons.get_image(static_cast<unsigned int>(lua_tonumber(L,2)));
      img.paste_image(cimg, static_cast<unsigned int>(lua_tonumber(L,3)) - cimg.get_xres()/2, 
                            static_cast<unsigned int>(lua_tonumber(L,4)) - cimg.get_yres()/2, true);
      break;
   }

   case actUIClear:
      img.paste_image(bgimg, 0, 0, false);
      ua_trace("buffered screen cleared by char. creation script\n");
      break;

   case actUIUpdate:
      changed = true;
      ua_trace("screen updated by char. creation script\n");
      break;

   case actSetPlayerName:
      if (n<2) break;
      pplayer->set_name(lua_tostring(L,2));
      ua_trace("player name set to \"%s\" by char. creation script\n", lua_tostring(L,2));
      break;

   case actSetPlayerAttr:
      if (n<3) break;
      pplayer->set_attr(static_cast<ua_player_attributes>(static_cast<unsigned int>(lua_tonumber(L,2))), 
                        static_cast<unsigned int>(lua_tonumber(L,3)));
      ua_trace("player attribute set\n");
      break;

   case actSetPlayerSkill:
      if (n<3) break;
      pplayer->set_skill(static_cast<ua_player_skills>(static_cast<unsigned int>(lua_tonumber(L,2))), 
                         static_cast<unsigned int>(lua_tonumber(L,3)));
      ua_trace("player skill set\n");
      break;

   default:
       ua_trace("unknown action (#%d) requested by by char. creation script\n", action);
   }
}

unsigned int ua_create_character_screen::drawtext(const char* str, int x, int y, int xalign, unsigned char color)
{
   // set default text color
   if (color==0)
      color = textcolor_normal;
   ua_image img_text;
   unsigned int textlength = font.calc_length(str);
   img_text.clear(0);
   font.create_string(img_text, str, color);
   if (xalign!=0)
   {
       // adjust horizontal alignment
       if (xalign==1)
          x -= textlength/2;
       else
          if (xalign==2)
             x -= textlength;
   }
   img.paste_image(img_text, x, y, true);
   return textlength;
}

unsigned int ua_create_character_screen::drawnumber(unsigned int num, int x, int y, unsigned char color)
{
   std::ostringstream buffer;
   buffer << num << std::ends;
   return drawtext(buffer.str().c_str(), x, y, 2, color);
}

unsigned int ua_create_character_screen::drawtext(int strnum, int x, int y, int xalign, unsigned char color, int custstrblock)
{
   std::string text(core->get_strings().get_string(custstrblock>-1 ? custstrblock : strblock, strnum));
   return (!text.empty()) ? drawtext(text.c_str(), x, y, xalign, color) : 0;
}

void ua_create_character_screen::drawbutton(int buttontype, bool highlight, int strnum, int xc, int y)
{
   ua_image button = img_buttons.get_image(btng_buttonimg_normal);
   int x = xc - button.get_xres()/2;
   img.paste_image(button, x, y, false);
   if (buttontype==btText)
      drawtext(strnum, xc, y+3, 1, (highlight ? textcolor_highlight : textcolor_normal));
   else if (buttontype==btInput)
   {
      unsigned int labelwidth = drawtext(strnum, x+4, y+3, 0, textcolor_highlight);
      unsigned int maxnamewidth = button.get_xres()-labelwidth-7;
      unsigned int ip = strlen(inputtext);
      while ((font.calc_length(inputtext)>maxnamewidth) && (ip>0))
         inputtext[ip--] = 0;
      drawtext(inputtext, x + labelwidth + 4, y + 3, 0, textcolor_normal);
   }
   else if (buttontype==btImage)
   {
      button = img_buttons.get_image(strnum);
      img.paste_image(button, x, y, true);
   }
   if (highlight && (buttontype!=btInput))
   {
      button = img_buttons.get_image(btng_buttonimg_highlight);
      img.paste_image(button, x, y, true);
   }
}

void ua_create_character_screen::drawbuttongroup()
{
   if (btng_buttontype==btTimer)
      return;

   ua_image button = img_buttons.get_image(btng_buttonimg_normal);
   int inity = btng_buttonspercolumn*button.get_yres() + ((btng_buttonspercolumn-1)*5);
   if (btng_caption!=ccvNone)
       inity += 15;
   inity = (200-inity)/2;
   if (btng_caption!=ccvNone)
   {
      drawtext(btng_caption, bgxpos, inity, 1);
      inity += 15;
   }

   int y = inity;
   int x = bgxpos;
   int iex = btng_buttonspercolumn;
   if (iex!=btng_buttoncount)
      x -= button.get_xres()/2+3;
   else
      iex = -1;
   for(int i=0; i<btng_buttoncount; i++)
   {
      drawbutton(btng_buttontype, (selected_button==i), btng_buttons[i], x, y);
      if (i==iex-1)
      {
         y = inity;
         x += button.get_xres()+6;
      } else
         y += button.get_yres()+5;
   }
}

int ua_create_character_screen::getbuttonover()
{
   if (btng_buttontype==btTimer)
      return -1;

   int xpos, ypos;
   SDL_GetMouseState(&xpos, &ypos);
   xpos = int(double(xpos)/core->get_screen_width()*320);
   ypos = int(double(ypos)/core->get_screen_height()*200);

   // determine column
   int columns = btng_buttoncount/btng_buttonspercolumn;
   int btnsize = img_buttons.get_image(btng_buttonimg_normal).get_xres();
   int bgsize = columns*btnsize + (columns-1)*6;
   xpos -= bgxpos-(bgsize/2);
   if ((xpos<0) || (xpos>bgsize) || (xpos%(btnsize+6) > btnsize))
      return -1;
   int coffset = xpos/(btnsize+6) * btng_buttonspercolumn;

   // determine button in column
   btnsize = img_buttons.get_image(btng_buttonimg_normal).get_yres();
   bgsize = btng_buttonspercolumn*btnsize + ((btng_buttonspercolumn-1)*5);
   int mv = (btng_caption!=ccvNone) ? 15 : 0;
   ypos -= (200-(bgsize+mv))/2+mv;
   return (ypos>=0) && (ypos<=bgsize) && (ypos%(btnsize+5) <= btnsize) ? 
      ypos/(btnsize+5) + coffset : -1;
}

void ua_create_character_screen::render()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glLoadIdentity();

   if (selected_button!=prev_button)
   {
      drawbuttongroup();
      prev_button=selected_button;
      changed = true;
   }

   if (changed || (fadingstage!=0))
   {
      // set brightness of texture quad
      Uint8 light = 255;

      switch (fadingstage)
      {
      case 1:
         light = Uint8(255*(double(tickcount) / (core->get_tickrate()*ua_fade_time)));
         break;

      case -1:
         light = Uint8(255-255*(double(tickcount) / (core->get_tickrate()*ua_fade_time)));
         break;
      }

      glColor3ub(light,light,light);


      // prepare image texture
      tex.convert(img);
      tex.use();
      tex.upload();

      changed = false;
   }

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   // draw screen quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
   glTexCoord2d(u  , v  ); glVertex2i(320,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(320,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
   glEnd();

   // draw the mouse cursor
   mousecursor->draw();
   tex.use();
}

void ua_create_character_screen::tick()
{
   if (fadingstage!=0)
   { 
      if (++tickcount >= (core->get_tickrate()*ua_fade_time))
      {
         if (ended)
         {
            if (newgame)
               core->replace_screen(new ua_ingame_orig_screen);
            else
               core->pop_screen();
            return;
         } // else
         changed = true;
         fadingstage=0;
         tickcount=0;
      }
   } 
   if ((btng_buttontype==btTimer) && (++cdttickcount >= (core->get_tickrate()*countdowntime)))
   {
      cdttickcount=0;
      press_button(0);
   }
}

void ua_create_character_screen::press_button(int button)
{
   ua_trace("character creation button %d pressed\n", button);

   // call "cchar_buttonclick(button)"
   lua_getglobal(L,"cchar_buttonclick");
   lua_pushnumber(L,static_cast<int>(button));
   int ret;
   if (btng_buttontype==btInput)
   {
      lua_pushstring(L,inputtext);
      ret = lua_call(L,2,0);
   } else
      ret = lua_call(L,1,0);
   if (ret!=0)
   {
      ua_trace("Lua function call cchar_buttonclick(0x%08x,%u) ended with error code %u\n",
         this, button, ret);
      ended = true;
   }
}


// static ua_create_character_view_screen methods

int ua_create_character_screen::cchar_do_action(lua_State *L)
{
   if (current_screen!=0)
      current_screen->do_action();
   return 0;
}
