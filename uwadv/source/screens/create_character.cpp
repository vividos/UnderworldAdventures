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
/*! \file create_character.cpp

   \brief character creation screen

   \todo reimplement the whole screen, using own button window class, etc.
   also use unicode mode to properly capture keys
   get rid of the global variable to hold "this" pointer
   use "relative" indexing for lua C call parameters
   use fading helper class for fading purposes
   put global constants as static members into class

*/

// needed includes
#include "common.hpp"
#include "create_character.hpp"
#include "ingame_orig.hpp"
#include "audio.hpp"
#include "renderer.hpp"
#include "underworld.hpp"
#include <sstream>
#include <ctime>


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

/*! \todo replace pplayer with reference to player object */
void ua_create_character_screen::init()
{
   ua_screen::init();

   ua_trace("character creation screen started\n");

   // setup screen
   game.get_renderer().setup_camera2d();

   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // get a pointer to to current player
   pplayer = &(game.get_underworld().get_player());

   game.get_image_manager().load(bgimg,"data/chargen.byt", 0,3, ua_img_byt);

   game.get_image_manager().load_list(img_buttons, "chrbtns", 0,0, 3);

   // init text
   font.load(game.get_settings(), ua_font_chargen);

   img_screen.init(game,0,0);
   img_screen.get_image() = bgimg;
   img_screen.update();
   register_window(&img_screen);

   // init mouse cursor
   mousecursor.init(game);
   mousecursor.show(true);

   register_window(&mousecursor);

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

   btng_buttons.resize(ua_maxbuttons);
   btnimgs.resize(5);

   // init lua scripting
   init_luascript();
}

void ua_create_character_screen::destroy()
{
   // close Lua
   lua.done();
   ua_trace("character creation screen ended\n\n");

   current_screen = 0;
}

void ua_create_character_screen::draw()
{
   glClear(GL_COLOR_BUFFER_BIT);

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
         light = Uint8(255*(double(tickcount) / (game.get_tickrate()*ua_fade_time)));
         break;

      case -1:
         light = Uint8(255-255*(double(tickcount) / (game.get_tickrate()*ua_fade_time)));
         break;
      }

      glColor3ub(light,light,light);

      img_screen.update();

      changed = false;
   }

   ua_screen::draw();
}

bool ua_create_character_screen::process_event(SDL_Event& event)
{
   ua_screen::process_event(event);

   switch(event.type)
   {
   case SDL_KEYDOWN:
      // don't handle any keystroke if Alt or Ctrl is down
      if (((event.key.keysym.mod & KMOD_ALT)>0) || ((event.key.keysym.mod & KMOD_CTRL)>0))
         return false;

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

      case SDLK_KP_ENTER:
      case SDLK_RETURN:
         // simulate clicking on that area
         press_button(selected_button);
         break;

      default: break;
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

   default:
      break;
   }

   return false;
}

void ua_create_character_screen::tick()
{
   if (fadingstage!=0)
   { 
      if (++tickcount >= (game.get_tickrate()*ua_fade_time))
      {
         if (ended)
         {
            if (newgame)
            {
               // load initial game
               game.get_underworld().import_savegame(game.get_settings(),"data/",true);

               // start original game
               game.replace_screen(new ua_ingame_orig_screen(game),false);
            }
            else
               game.remove_screen();

            return;
         } // else
         changed = true;
         fadingstage=0;
         tickcount=0;
      }
   }

   if ((btng_buttontype==btTimer) && (++cdttickcount >= (game.get_tickrate()*countdowntime)))
   {
      cdttickcount=0;
      press_button(0);
   }
}

/*! \todo store "this" pointer in userdata variable
*/
void ua_create_character_screen::init_luascript()
{
   // initialize Lua
   lua.init(&game);

   lua_State* L = lua.get_lua_State();

   // open lualib libraries
   lua_baselibopen(L);
   lua_strlibopen(L);
   lua_mathlibopen(L);

   // register C functions
   lua_register(L,"cchar_do_action", cchar_do_action);

   // load lua interface script for constants
   if (0 != lua.load_script("uw1/scripts/uwinterface"))
      ended = true;

   // load lua cutscene script
   if (0 != lua.load_script("uw1/scripts/createchar"))
      ended = true;

   // store pointer to this instance in a global var
   current_screen = this;

   // Init script with seed value for random numbers
   cchar_global(gactInit, clock());
}

void ua_create_character_screen::cchar_global(int globalaction, int seed)
{
   lua_State* L = lua.get_lua_State();

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

void ua_create_character_screen::handleinputchar(char c)
{
   int i = inputtext.size();
   if (c==8)
   {
      if (i>0)
         inputtext.erase(i-1);
      return;
   }
   if (i>=31)
      return;
   inputtext += c;
}

void ua_create_character_screen::do_action()
{
   lua_State* L = lua.get_lua_State();

   int n=lua_gettop(L);
   unsigned int action = (n<1) ? 0 : static_cast<unsigned int>(lua_tonumber(L,1));

   ua_image& img = img_screen.get_image();

   switch(static_cast<ua_elua_action>(action))
   {
   case actEnd:
      ended = true;
      newgame = (n>1) && (static_cast<unsigned int>(lua_tonumber(L,2))==1);
      fadingstage = unsigned(-1);

      // fade out music
      game.get_audio_manager().fadeout_music(ua_fade_time);

      //ua_trace("end request by char. creation script\n");
      break;

   case actSetInitVal:
   {
      if (n<5) break;
      strblock = static_cast<unsigned int>(lua_tonumber(L,2));
      bgxpos = static_cast<unsigned int>(lua_tonumber(L,3));
      textcolor_normal = static_cast<unsigned int>(lua_tonumber(L,4));
      textcolor_highlight = static_cast<unsigned int>(lua_tonumber(L,5));

      // set different highlight color when features are enabled
      if (game.get_settings().get_bool(ua_setting_uwadv_features))
         textcolor_highlight = 162; // orange, palette #3

      int ic = lua_getn(L, 6);
      if (ic>5) ic = 5;
      for (int i=0; i<ic; i++)
      {
         lua_rawgeti(L, 6, i+1);
         btnimgs[i] = static_cast<unsigned int>(lua_tonumber(L,7));
         lua_pop(L, 1);
      }
      //ua_trace("init values set by char. creation script\n");
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
            inputtext.erase();
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
      //ua_trace("new buttongroup set by char. creation script, caption nr: %d, %d buttons\n", btng_caption, btng_buttoncount);
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
               (n>5) ? static_cast<unsigned int>(lua_tonumber(L,6)) : unsigned(-1));
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
      ua_image& cimg = img_buttons[static_cast<unsigned int>(lua_tonumber(L,2))];
      unsigned int destx, desty;
      destx = static_cast<unsigned int>(lua_tonumber(L,3)) - cimg.get_xres()/2;
      desty = static_cast<unsigned int>(lua_tonumber(L,4)) - cimg.get_yres()/2;
      img.paste_rect(cimg, 0,0, cimg.get_xres(), cimg.get_yres(), destx,desty, true);
      break;
   }

   case actUIClear:
      img.paste_image(bgimg, 0,0);
      //ua_trace("buffered screen cleared by char. creation script\n");
      break;

   case actUIUpdate:
      changed = true;
      //ua_trace("screen updated by char. creation script\n");
      break;

   case actSetPlayerName:
      if (n<2) break;
      pplayer->set_name(lua_tostring(L,2));
      //ua_trace("player name set to \"%s\" by char. creation script\n", lua_tostring(L,2));
      break;

   case actSetPlayerAttr:
      if (n<3) break;
      pplayer->set_attr(static_cast<ua_player_attributes>(static_cast<unsigned int>(lua_tonumber(L,2))), 
                        static_cast<unsigned int>(lua_tonumber(L,3)));
      //ua_trace("player attribute set\n");
      break;

   case actSetPlayerSkill:
      if (n<3) break;
      pplayer->set_skill(static_cast<ua_player_skills>(static_cast<unsigned int>(lua_tonumber(L,2))), 
                         static_cast<unsigned int>(lua_tonumber(L,3)));
      //ua_trace("player skill set\n");
      break;

   default:
      //ua_trace("unknown action (#%d) requested by by char. creation script\n", action);
      break;
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
   img_screen.get_image().paste_rect(img_text, 0,0, img_text.get_xres(),img_text.get_yres(), x,y, true);
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
   std::string text(game.get_underworld().get_strings().get_string(custstrblock>-1 ? custstrblock : strblock, strnum));
   return (!text.empty()) ? drawtext(text.c_str(), x, y, xalign, color) : 0;
}

void ua_create_character_screen::drawbutton(int buttontype, bool highlight, int strnum, int xc, int y)
{
   ua_image& button = img_buttons[btng_buttonimg_normal];
   int x = xc - button.get_xres()/2;

   img_screen.get_image().paste_image(button, x, y);

   if (buttontype==btText)
      drawtext(strnum, xc, y+3, 1, (highlight ? textcolor_highlight : textcolor_normal));
   else if (buttontype==btInput)
   {
      unsigned int labelwidth = drawtext(strnum, x+4, y+3, 0, textcolor_normal);
      unsigned int maxnamewidth = button.get_xres()-labelwidth-7;
      unsigned int ip = inputtext.size();
      while ((font.calc_length(inputtext.c_str())>maxnamewidth) && (ip>0))
         inputtext.erase(ip--);
      drawtext(inputtext.c_str(), x + labelwidth + 4, y + 3, 0, textcolor_highlight);
   }
   else if (buttontype==btImage)
   {
      button = img_buttons[strnum];
      img_screen.get_image().paste_rect(button, 0,0, button.get_xres(),button.get_yres(), x,y, true);
   }
   if (highlight && (buttontype!=btInput))
   {
      button = img_buttons[btng_buttonimg_highlight];
      img_screen.get_image().paste_rect(button, 0,0, button.get_xres(),button.get_yres(), x,y, true);
   }
}

void ua_create_character_screen::drawbuttongroup()
{
   if (btng_buttontype==btTimer)
      return;

   ua_image& button = img_buttons[btng_buttonimg_normal];
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

   // convert to 320x200 screen coordinates
   int xpos, ypos;
   SDL_GetMouseState(&xpos, &ypos);

   SDL_Surface* surf = SDL_GetVideoSurface();

   if (surf != NULL)
   {
      xpos = int(xpos * 320.0 / surf->w);
      ypos = int(ypos * 200.0 / surf->h);
   }
   else
      xpos = ypos = 0;

   // determine column
   int columns = btng_buttoncount/btng_buttonspercolumn;
   int btnsize = img_buttons[btng_buttonimg_normal].get_xres();
   int bgsize = columns*btnsize + (columns-1)*6;
   xpos -= bgxpos-(bgsize/2);
   if ((xpos<0) || (xpos>bgsize) || (xpos%(btnsize+6) > btnsize))
      return -1;
   int coffset = xpos/(btnsize+6) * btng_buttonspercolumn;

   // determine button in column
   btnsize = img_buttons[btng_buttonimg_normal].get_yres();
   bgsize = btng_buttonspercolumn*btnsize + ((btng_buttonspercolumn-1)*5);
   int mv = (btng_caption!=ccvNone) ? 15 : 0;
   ypos -= (200-(bgsize+mv))/2+mv;
   return (ypos>=0) && (ypos<=bgsize) && (ypos%(btnsize+5) <= btnsize) ? 
      ypos/(btnsize+5) + coffset : -1;
}

void ua_create_character_screen::press_button(int button)
{
   //ua_trace("character creation button %d pressed\n", button);

   lua_State* L = lua.get_lua_State();

   // call "cchar_buttonclick(button)"
   lua_getglobal(L,"cchar_buttonclick");
   lua_pushnumber(L,static_cast<int>(button));
   int ret;
   if (btng_buttontype==btInput)
   {
      lua_pushstring(L,inputtext.c_str());
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
