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

*/
/*! \file create_character.cpp

   \brief character creation screen

*/

// needed includes
#include "common.hpp"
#include "create_character.hpp"

// constants

//! time for fade in/out
const double ua_start_menu_blend_time = 0.5;

//! custom type for identifying buttons/text 
enum ua_echarcreationvalue
{
   // note: enum values must match entries in stringtable block 2
   ccvSex = 1,
   ccvHandedness = 2,
   ccvClass = 3,
   ccvSkill = 4,
   ccvDifficulty = 6,
   ccvName = 7,
   ccvKeep = 8,
   ccvMale = 9,
   ccvFemale = 10,
   ccvLeft = 11,
   ccvRight = 12,
   ccvStandard = 13,
   ccvEasy = 14,
   ccvYes = 15,
   ccvNo = 16,
   ccvStrc = 17,
   ccvDexc = 18,
   ccvIntc = 19,
   ccvVitc = 20,
   ccvManac = 21,
   ccvExpc = 22,
   ccvFighter = 23,
   ccvMage = 24,
   ccvBard = 25,
   ccvTinker = 26,
   ccvDruid = 27,
   ccvPaladin = 28,
   ccvRanger = 29,
   ccvShepard = 30,
   ccvAttack = 31,
   ccvDefence = 32,
   ccvUnarmed = 33,
   ccvSword = 34,
   ccvAxe = 35,
   ccvMace = 36,
   ccvMissile = 37,
   ccvMana = 38,
   ccvLore = 39,
   ccvCasting = 40,
   ccvTraps = 41,
   ccvSearch = 42,
   ccvTrack = 43,
   ccvSneak = 44,
   ccvRepair = 45,
   ccvCharm = 46,
   ccvPicklock = 47,
   ccvAcrobat = 48,
   ccvAppraise = 49,
   ccvSwimming = 50,
   
   // custom values (these have no matching text in string table)
   ccvOther = 2000,
};



//! table of character creation buttons 
int ua_cc_buttongroups[][11] =
{ // {HeadingText,	BtnType, #Btns, Btn1...Btn8 }

   {ccvSex,        0,  2, ccvMale, ccvFemale},
   {ccvHandedness, 0,  2, ccvLeft, ccvRight},
   {ccvClass,      0,  8, ccvFighter, ccvMage, ccvBard, ccvTinker, ccvDruid, ccvPaladin, ccvRanger, ccvShepard},
/*
  // Fighter Skills
   {ccvSkill,      0,  2, ccvAttack, ccvDefence},
   {ccvSkill,      0,  5, ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile},
   {ccvSkill,      0,  5, ccvSwimming, ccvTraps, ccvSearch, ccvCharm, ccvAcrobat, ccvAppraise},

  // Mage Skills
   {ccvSkill,      0,  2, ccvMana, ccvCasting},

  // Bard Skills
   {ccvSkill,      0,  2, ccvLore, ccvCharm},
   {ccvSkill,      0,  6, ccvAppraise, ccvAcrobat, ccvSneak, ccvPicklock, ccvSearch, ccvSwimming},
   {ccvSkill,      0,  7, ccvMana, ccvCasting, ccvSword, ccvAxe, ccvMace, ccvUnarmed,  ccvMissile},

  // Tinker Skills
   // First Tinker Skills page same as Fighter Skills page 2 (index 4)
   {ccvSkill,      0,  5, ccvPicklock, ccvTraps, ccvSearch, ccvAppraise, ccvRepair},

  // Druid Skills
   {ccvSkill,      0,  3, ccvTrack, ccvLore, ccvSearch},

  // Paladin Skills
   {ccvSkill,      0,  4, ccvAppraise, ccvCharm, ccvAcrobat, ccvRepair},
   // Second Paladin skill page same as Fighter Skills page 2 (index 4)

  // Ranger Skills
   {ccvSkill,      0,  6, ccvTraps, ccvAcrobat, ccvSneak, ccvSearch, ccvSwimming, ccvRepair},
   {ccvSkill,      0,  8, ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile, ccvAttack, ccvDefence, ccvTrack},

  // Shepard Skills  (note: the original game had the last 2 pages combined on one page with 10 buttons)
   {ccvSkill,      0,  6, ccvUnarmed, ccvSword, ccvAxe, ccvMace, ccvMissile, ccvDefence},
   {ccvSkill,      0,  6, ccvTraps, ccvSneak, ccvAppraise, ccvTrack, ccvLore, ccvOther},
   {ccvSkill,      0,  5, ccvSearch, ccvAcrobat, ccvSwimming, ccvCasting, ccvMana},
*/
  // Appearance    
   {ccvDifficulty, 0,  2, ccvStandard, ccvEasy},

  // Confirm
   {ccvKeep,       0,  2, ccvYes, ccvNo}
};


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
   glDisable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

   // load background image
   bgimg.load_raw(core->get_settings(),"data/chargen.byt",3);

   // init screen image
   img.create(320, 200, 0, 3);
   tex.init(&core->get_texmgr());
   tex.convert(img);
   tex.use();
   tex.upload();

   // get palette #3
   memcpy(palette,core->get_texmgr().get_palette(3),sizeof(ua_onepalette));

   // load button graphics
   img_buttons.load(core->get_settings(),"chrbtns",0,0,3);

   // init text
   font.init(core->get_settings(), ua_font_chargen);

   // intial variable values
   ended=false;
   current_buttongroup=0;
   selected_button=0;
   prev_button=0;
   step=0;
   prevstep=-1;
   stage=0;
   tickcount=0;
   buttondown=false;
}

void ua_create_character_screen::done()
{
   tex.done();

   // clear screen
   glClearColor(0,0,0,0);
   glClear(GL_COLOR_BUFFER_BIT);
   SDL_GL_SwapBuffers();
}

void ua_create_character_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
      // handle key presses
      switch(event.key.keysym.sym)
      {
      case SDLK_ESCAPE:
         ua_trace("character creation ended by escape\n");
         ended = true;
         break;

      case SDLK_PAGEUP:
      case SDLK_HOME:
         selected_button=0;
         break;

      case SDLK_PAGEDOWN:
      case SDLK_END:
         selected_button=ua_cc_buttongroups[current_buttongroup][2]-1;
         break;

      case SDLK_UP:
         // select the area above, if possible
         if (selected_button==-1) selected_button=0;
         if (selected_button>0) selected_button--;
         break;

      case SDLK_DOWN:
         // select the area below, if possible
         if (selected_button+1<ua_cc_buttongroups[current_buttongroup][2])
            selected_button++;
         break;

      case SDLK_RETURN:
         // simulate clicking on that area
         if (stage==1)
         {
            stage++;
            tickcount=0;
         }
         break;
      }
      break;

   case SDL_MOUSEBUTTONDOWN:
      // select the area where the mouse button is pressed
      buttondown=true;
      if (stage==1)
         selected_button = getbuttonclicked(step);
      break;

   case SDL_MOUSEMOTION:
      if (stage==1 && buttondown)
      {
         int ret = getbuttonclicked(step);
         if (ret!=-1)
            selected_button = ret;
      }
      break;

   case SDL_MOUSEBUTTONUP:
      buttondown=false;
      if (stage==1)
      {
         // determine if user released the mouse button over the same area
         int ret = getbuttonclicked(step);
         if (ret != -1 && ret == selected_button)
         {
            stage++;
            tickcount=0;
         }
      }
      break;
   }
}

void ua_create_character_screen::drawtext(const char* str, int x, int y, int xalign, unsigned char color)
{
   // set default text color
   if (color==0)
      color = 73;
   ua_image img_text;
   img_text.clear(0);
   font.create_string(img_text, str, color);
   if (xalign!=0)
   {
       // adjust horizontal alignment
       unsigned int textlength = font.calc_length(str);
       if (xalign==1)
          x -= textlength/2;
       else
          if (xalign==2)
             x -= textlength;
   }
   img.paste_image(img_text, x, y, true);
}

void ua_create_character_screen::drawnumber(unsigned int num, int x, int y, unsigned char color)
{
   char ca[12];
   sprintf(ca, "%d", num);
   drawtext(ca, x, y, 2, color);
}

void ua_create_character_screen::drawtext(int strnum, int x, int y, int xalign, unsigned char color)
{
   drawtext(core->get_strings().get_string(2, strnum).c_str(), x, y, xalign, color);
}

void ua_create_character_screen::drawbutton(int buttontype, bool highlight, int strnum, int xc, int y)
{
   ua_image button = img_buttons.get_image(buttontype);
   img.paste_image(button, xc - button.get_xres()/2, y, false);
   if (highlight)
   {
       button = img_buttons.get_image(buttontype+2);
       img.paste_image(button, xc - button.get_xres()/2, y, true);
   }
   if (strnum>0)
      drawtext(strnum, xc, y+3, 1);
}

void ua_create_character_screen::drawcurrentbuttongroup()
{
    int btnheight = img_buttons.get_image(ua_cc_buttongroups[current_buttongroup][1]).get_yres();
    int y = (200-(ua_cc_buttongroups[current_buttongroup][2]*btnheight + ((ua_cc_buttongroups[current_buttongroup][2]-1)*5) + 15))/2;
    drawtext(ua_cc_buttongroups[current_buttongroup][0], 240, y, 1);
    y += 15;
    for(int i=0; i<ua_cc_buttongroups[current_buttongroup][2]; i++)
    {
        drawbutton(ua_cc_buttongroups[current_buttongroup][1], (selected_button==i), ua_cc_buttongroups[current_buttongroup][3+i], 240, y);
        y += btnheight + 5;
    }
}

int ua_create_character_screen::getbuttonclicked(int buttongroup)
{
   // get mouse coordinates
   int x,y;
   SDL_GetMouseState(&x,&y);
   unsigned int xpos,ypos;
   xpos = unsigned(double(x)/core->get_screen_width()*320);
   ypos = unsigned(double(y)/core->get_screen_height()*200);
   int btnhwidth = img_buttons.get_image(ua_cc_buttongroups[buttongroup][1]).get_xres()/2;
   if ((xpos<240-btnhwidth) || (xpos>240+btnhwidth))
      return -1;

   int btnheight = img_buttons.get_image(ua_cc_buttongroups[buttongroup][1]).get_yres();
   y = (200-(ua_cc_buttongroups[buttongroup][2]*btnheight + ((ua_cc_buttongroups[buttongroup][2]-1)*5) + 15))/2 + 15;
   for(int i=0; i<ua_cc_buttongroups[buttongroup][2]; i++)
   {
        if ((ypos>y) && (ypos<y+btnheight))
            return i;
        y += btnheight + 5;
    }
   return -1;
}

void ua_create_character_screen::drawselectedoptions()
{
   if (step>0)
      drawtext((pplayer->get_attr(ua_attr_gender)==0) ? ccvMale : ccvFemale, 18, 21, 0);

   if (step>2)
   {
      drawtext(ccvFighter + pplayer->get_attr(ua_attr_profession), 141, 21, 2);
      drawtext(ccvStrc, 93, 50, 0);
      drawnumber(pplayer->get_stat(ua_stat_strength), 139, 50);
      drawtext(ccvDexc, 93, 67, 0);
      drawnumber(pplayer->get_stat(ua_stat_dexterity), 139, 67);
      drawtext(ccvIntc, 93, 84, 0);
      drawnumber(pplayer->get_stat(ua_stat_intelligence), 139, 84);
      drawtext(ccvVitc, 93, 101, 0);
      drawnumber(pplayer->get_stat(ua_stat_max_life), 139, 101);
   }
}

void ua_create_character_screen::render()
{
   glClear(GL_COLOR_BUFFER_BIT);
   glLoadIdentity();

   // only redraw the whole image when the step has changed
   if (step!=prevstep)
   {
       img.paste_image(bgimg, 0, 0, false);
       selected_button=prev_button=0;
       current_buttongroup=step;
       drawcurrentbuttongroup();
       drawselectedoptions();
       prevstep=step;
   }
   else // redraw the buttons when the current selection has changed
      if (selected_button!=prev_button)
      {
         drawcurrentbuttongroup();
         prev_button=selected_button;
      }

   // set brightness of texture quad
   glColor3ub(255,255,255);

   // prepare image texture
   tex.convert(img);
   tex.use();
   tex.upload();

   double u = tex.get_tex_u(), v = tex.get_tex_v();

   // draw background quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
   glTexCoord2d(u  , v  ); glVertex2i(320,  0);
   glTexCoord2d(u  , 0.0); glVertex2i(320,200);
   glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
   glEnd();
}

void ua_create_character_screen::tick()
{
   // when fading in or out, check if blend time is over
   if ((stage==0 || stage==2) &&
      ++tickcount >= (core->get_tickrate()*ua_start_menu_blend_time))
   {
      // do next stage
      stage++;
      tickcount=0;
   }

   if (ended)
   {
      // finished
      core->pop_screen();
      return;
   }

   // in stage 3 the selected button is pressed
   if (stage==3)
   {
      press_button();
      stage=0;
      tickcount=0;
   }
}

void ua_create_character_screen::press_button()
{
   switch(current_buttongroup)
   {
   case 0: // Sex
      pplayer->set_attr(ua_attr_gender, selected_button);
      break;

   case 1: // Handedness
      pplayer->set_attr(ua_attr_handedness, selected_button);
      break;

   case 2: // Class
      pplayer->set_attr(ua_attr_profession, selected_button);
      break;

   case 3: // Difficulty
      break;

   case 4: // Keep character
      if (selected_button==0)
         ended=true;
      else
         step=0;
      return;
   }
   step++;
   selected_button = -1;
}

