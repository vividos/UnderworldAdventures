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
/*! \file cutscene_view.cpp

   \brief cutscene view implementation

*/

// needed includes
#include "common.hpp"
#include "cutscene_view.hpp"


// constants

//! frames per second for cutscene animation
const double ua_cutscene_view_anim_fps = 5.0;

//! time needed to fade in/out text
const double ua_cutscene_view_fade_time = 0.5;


// ua_cutscene_view_screen methods

void ua_cutscene_view_screen::init()
{
   ua_trace("cutscene animation %u started\n",cutscene);

   ended = false;
   tickcount = 0;

   showanim = false;
   loopanim = false;
   curframe = 0;
   animcount = 0.0;
   anim_fade_state = 0;
   anim_fadecount = 0;

   showtext = false;
   strblock = 0x0c00;
   textcolor = 11;
   text_fade_state = 0;
   text_fadecount = 0;

   // stop audio track
   core->get_audio().stop_music();

   // init lua scripting

   // get a new lua state
   L = lua_open(128);

   // register C functions
   lua_register(L,"cuts_do_action",cuts_do_action);

   // load lua script
   {
      SDL_RWops *script =
         core->get_filesmgr().get_uadata_file("uw1/scripts/cutscene.lua");

      // not found? try another
      if (script==NULL)
         script = core->get_filesmgr().get_uadata_file("uw1/scripts/cutscene.lob");
      else
         ua_trace("loaded Lua script \"uw1/scripts/cutscene.lua\"\n");

      if (script==NULL)
         throw ua_exception("could not load cutscene script from uadata");
      else
         ua_trace("loaded Lua compiled script \"uw1/scripts/cutscene.lob\"\n");

      // load script into buffer
      std::vector<char> buffer;
      unsigned int len=0;
      {
         SDL_RWseek(script,0,SEEK_END);
         len = SDL_RWtell(script);
         SDL_RWseek(script,0,SEEK_SET);

         buffer.resize(len+1,0);

         SDL_RWread(script,&buffer[0],len,1);
         buffer[len]=0;
      }
      SDL_RWclose(script);
      SDL_FreeRW(script);

      // execute script
      int ret = lua_dobuffer(L,&buffer[0],len,"");
      if (ret!=0)
      {
         ua_trace("Lua script ended with error code %u\n",ret);
         ended = true;
      }
   }

   // call "cuts_init(this,cutscene)"
   lua_getglobal(L,"cuts_init");
   lua_pushuserdata(L,this);
   lua_pushnumber(L,static_cast<double>(cutscene));
   int ret = lua_call(L,2,0);
   if (ret!=0)
   {
      ua_trace("Lua function call cuts_init(0x%08x,%u) ended with error code %u\n",
         this,cutscene,ret);
      ended = true;
   }

   // init opengl

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

   // texture stuff

   // init textures
   tex_anim.init();
   tex_text.init(1,GL_NEAREST,GL_NEAREST);

   // init subtitle text
   font_big.init(core->get_settings(),ua_font_big);
}

void ua_cutscene_view_screen::done()
{
   tex_text.done();
   tex_anim.done();

   lua_close(L);
}

void ua_cutscene_view_screen::handle_event(SDL_Event &event)
{
   switch(event.type)
   {
   case SDL_KEYDOWN:
      // handle key presses
      switch(event.key.keysym.sym)
      {
      case SDLK_RETURN:
      case SDLK_ESCAPE:
         ua_trace("cutscene ended by return/escape\n");
         ended = true;

         // initiate text and anim fadeout
         text_fade_state = 2;
         text_fadecount = 0;
         anim_fade_state = 2;
         anim_fadecount = 0;
         break;
      }
   }
}

void ua_cutscene_view_screen::render()
{
   glClear(GL_COLOR_BUFFER_BIT);

   if (showanim)
   {
      // render animation frame

      // prepare image texture
      tex_anim.use(core->get_texmgr());
      cuts.get_frame(tex_anim,curframe);
      tex_anim.upload();

      double u = tex_anim.get_tex_u(), v = tex_anim.get_tex_v();

      // set text color
      Uint8 light = 255;
      switch(anim_fade_state)
      {
      case 0: // show
         light = 255;
         break;

      case 1: // fade in
         light = static_cast<Uint8>( 255*double(anim_fadecount) /
            (ua_cutscene_view_fade_time*core->get_tickrate()) );
         break;

      case 2: // fade out
         light = 255-static_cast<Uint8>( 255*double(anim_fadecount) /
            (ua_cutscene_view_fade_time*core->get_tickrate()) );
         break;
      }
      glColor3ub(light,light,light);

      // draw animation quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(  0,  0);
      glTexCoord2d(u  , v  ); glVertex2i(320,  0);
      glTexCoord2d(u  , 0.0); glVertex2i(320,200);
      glTexCoord2d(0.0, 0.0); glVertex2i(  0,200);
      glEnd();
   }

   if (showtext)
   {
      // render subtitle text

      // enable blending, in case text overlaps animation
      glEnable(GL_BLEND);

      // prepare image texture
      tex_text.use(core->get_texmgr());

      double u = tex_text.get_tex_u(), v = tex_text.get_tex_v();

      unsigned int startx = (320-img_text.get_xres())/2;
      unsigned int starty = static_cast<unsigned int>(
         (400.0/480.0)*img_text.get_yres()+10 );

      // set text color
      Uint8 light = 255;
      switch(text_fade_state)
      {
      case 0: // show
         light = 255;
         break;

      case 1: // fade in
         light = static_cast<Uint8>( 255*double(text_fadecount) /
            (ua_cutscene_view_fade_time*core->get_tickrate()) );
         break;

      case 2: // fade out
         light = 255-static_cast<Uint8>( 255*double(text_fadecount) /
            (ua_cutscene_view_fade_time*core->get_tickrate()) );
         break;
      }
      glColor3ub(light,light,light);

      // draw animation quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(startx,    5);
      glTexCoord2d(u  , v  ); glVertex2i(320-startx,5);
      glTexCoord2d(u  , 0.0); glVertex2i(320-startx,starty);
      glTexCoord2d(0.0, 0.0); glVertex2i(startx,    starty);
      glEnd();

      glDisable(GL_BLEND);
   }
}

void ua_cutscene_view_screen::tick()
{
   if (ended && anim_fadecount >= ua_cutscene_view_fade_time*core->get_tickrate())
   {
      // we're finished
      core->pop_screen();
      return;
   }

   // check anim looping
   if (showanim && loopanim && !(anim_fade_state == 1 || anim_fade_state == 2))
   {
      // count up animcount
      animcount += 1.0/core->get_tickrate();

      if (animcount>=1.0/ua_cutscene_view_anim_fps)
      {
         // calculate remaining time
         animcount -= 1.0/ua_cutscene_view_anim_fps;

         // count up frames
         curframe++;
         if (curframe>=cuts.get_maxframes())
            curframe = 0;

         // check if we should stop at that frame
         if (stopframe >= 0 && curframe == stopframe)
         {
            // disable animation; works like "cuts_anim_stop"
            loopanim = false;
         }
      }
   }

   // check anim fading
   if ((showanim || ended) && (anim_fade_state == 1 || anim_fade_state == 2))
   {
      ++anim_fadecount;

      // end of fade reached?
      if (anim_fadecount >= ua_cutscene_view_fade_time*core->get_tickrate())
      {
         if (anim_fade_state == 1)
            anim_fade_state = 0;
         else
            showanim = false;
      }
   }

   // check text fading
   if (showtext && (text_fade_state == 1 || text_fade_state == 2))
   {
      ++text_fadecount;

      // end of fade reached?
      if (text_fadecount >= ua_cutscene_view_fade_time*core->get_tickrate())
      {
         if (text_fade_state == 1)
            text_fade_state = 0;
         else
            showtext = false;
      }
   }


   // calculate current ticktime
   double ticktime = double(tickcount) / core->get_tickrate();

   // call lua "cuts_tick(ticktime)" function
   if (!ended)
   {
      lua_getglobal(L,"cuts_tick");
      lua_pushnumber(L,ticktime);
      int ret = lua_call(L,1,0);
      if (ret!=0)
      {
         ua_trace("Lua function call cuts_tick(%3.2f) ended with error code %u\n",
            ticktime,ret);
         ended = true;
         return;
      }
   }

   ++tickcount;
}

void ua_cutscene_view_screen::create_text_image(const char *str)
{
   unsigned int maxlen = 310;

   img_text.clear(0);

   if (font_big.calc_length(str)<maxlen)
   {
      // easy task: all text fits into one line
      font_big.create_string(img_text,str,textcolor);
   }
   else
   {
      std::vector<ua_image> lines;
      std::string text(str),part;

      // collect all text string lines
      do
      {
         part.assign(text);

         std::string::size_type pos;

         // cut down string on ' ' boundaries, until it fits into an image
         while(font_big.calc_length(part.c_str()) > maxlen)
         {
            pos = part.find_last_of(' ');
            if (pos != std::string::npos)
            {
               part.erase(pos);
            }
            else
               break; // string too long, without a space in between
         }

         // create line image
         {
            ua_image img_temp;
            font_big.create_string(img_temp,part.c_str(),textcolor);
            lines.push_back(img_temp);
         }

         // cut down string
         text.erase(0,part.size()+1);

      } while(!text.empty());

      // re-assemble the text strings in a new image
      if (lines.size()==0)
         return; // there definitely went something wrong

      // create new image
      {
         unsigned int lineheight = lines[0].get_yres();
         unsigned int max = lines.size();

         img_text.create(320, lineheight*max);

         // paste all lines into the new image
         for(unsigned int i=0; i<max; i++)
         {
            const ua_image &img_temp = lines[i];

            unsigned int destx = (320-img_temp.get_xres())/2;
            img_text.paste_image(img_temp,destx,lineheight*i);
         }
      }
   }

   // upload texture
   tex_text.convert(core->get_texmgr(),img_text);
   tex_text.use(core->get_texmgr());
   tex_text.upload();
}

void ua_cutscene_view_screen::do_action()
{
   int n=lua_gettop(L);
   unsigned int action = static_cast<unsigned int>(lua_tonumber(L,n-1));

   // note: the case values must be in sync with the ones in the script
   switch(action)
   {
   case 0: // cuts_finished
      ended = true;
      anim_fadecount = static_cast<unsigned int>(
         ua_cutscene_view_fade_time*core->get_tickrate() ) + 1;
      break;

   case 1: // cuts_set_string_block
      strblock = static_cast<unsigned int>(lua_tonumber(L,n));
      break;

   case 2: // cuts_sound_play
      {
         const char *str = lua_tostring(L,n);
         core->get_audio().play_sound(str);
      }
      break;

   case 3: // cuts_text_set_color
      textcolor = static_cast<Uint8>(lua_tonumber(L,n));
      break;

   case 4: // cuts_text_fadein
      {
         unsigned int strnum = static_cast<unsigned int>(lua_tonumber(L,n));
         create_text_image( core->get_strings().get_string(strblock,strnum).c_str() );
         text_fade_state = 1; // fade in
         text_fadecount = 0;
         showtext = true;
      }
      break;

   case 5: // cuts_text_fadeout
      text_fade_state = 2; // fade out
      text_fadecount = 0;
      showtext = true;
      break;

   case 6: // cuts_text_show
      {
         unsigned int strnum = static_cast<unsigned int>(lua_tonumber(L,n));
         create_text_image( core->get_strings().get_string(strblock,strnum).c_str() );
         showtext = true;
         text_fade_state = 0;
      }
      break;

   case 7: // cuts_text_hide
      showtext = false;
      break;

   case 8: // cuts_anim_fadein
      anim_fade_state = 1;
      anim_fadecount = 0;
      // falls through to "cuts_anim_show"

   case 9: // cuts_anim_show
      {
         std::string animname("cuts/");
         animname.append(lua_tostring(L,n));

         // load animation
         cuts.load(core->get_settings(),animname.c_str());
         showanim = true;
         loopanim = true;
         curframe = 0;
         animcount = 0.0;
         stopframe = -1;
      }
      break;

   case 10: // cuts_anim_set_stopframe
      // get "stop frame" parameter
      stopframe = static_cast<int>(lua_tonumber(L,n));
      break;

   case 11: // cuts_anim_fadeout
      anim_fade_state = 2;
      anim_fadecount = 0;
      break;

   case 12: // cuts_anim_stop
      loopanim = false;
      curframe = static_cast<unsigned int>(lua_tonumber(L,n));
      break;

   case 13: // cuts_anim_hide
      showanim = false;
      break;

   case 14: // cuts_anim_continue
      loopanim = true;
      stopframe = -1;
      break;
   }
}


// static ua_cutscene_view_screen methods

int ua_cutscene_view_screen::cuts_do_action(lua_State *L)
{
   // check for "self" parameter being userdata
   int n=lua_gettop(L);
   if (lua_isuserdata(L,n-2))
   {
      // get pointer to object
      ua_cutscene_view_screen *self =
         reinterpret_cast<ua_cutscene_view_screen*>(lua_touserdata(L,n-2));

      if (self->L != L)
         throw ua_exception("wrong 'self' parameter in Lua script");

      // perform action
      self->do_action();

      return 0;
   }
   return 1;
}
