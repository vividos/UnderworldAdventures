/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Underworld Adventures Team

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
/*! \file textscroll.cpp

   \brief text scroll implementation

*/

// needed includes
#include "common.hpp"
#include "textscroll.hpp"
#include "image.hpp"
#include "core.hpp"


// constants

const char* ua_textscroll_more_string = " [MORE]";


// ua_textscroll methods

   //! initializes text scroll
void ua_textscroll::init(ua_game_core_interface& core, unsigned int xpos,
   unsigned int ypos, unsigned int width,
   unsigned int height, unsigned int lines, Uint8 bg_color)
{
   font_normal.init(core.get_settings(),ua_font_normal);

   this->xpos = xpos;
   this->ypos = ypos;
   this->width = width + (width&1); // even width
   this->height = height;
   this->maxlines = lines;
   this->bg_color = bg_color;
   text_color = 11;
   input_mode = false;
   input_line = 0;

   // init texture
   split_textures = width>254;

   tex.init(&core.get_texmgr(),split_textures ? 2 : 1,
      GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);

   // fill with blank texture
   ua_image img_blank;
   img_blank.create((split_textures ? width/2+2 : width+2),height+2,bg_color,0);
   tex.convert(img_blank,0);
   tex.use(0);
   tex.upload();

   if (split_textures)
   {
      tex.convert(img_blank,1);
      tex.use(1);
      tex.upload();
   }
}

bool ua_textscroll::print(const char* text)
{
   std::string msgtext(text);
   bool needed_scrolling = false;

   // check text length
   unsigned int len = font_normal.calc_length(text);

   if (strchr(text,'\n')==0 && len<=width-2)
   {
      // fits into a single line

      if (linestack.size()>=maxlines)
      {
         // needed to scroll
         needed_scrolling = true;

         // remember line in history
         linehistory.push_back(linestack.front());

         // delete old line
         linestack.erase(linestack.begin());
      }

      // add line
      linestack.push_back(msgtext);
   }
   else
   {
      // multiple line text
      unsigned int curline = 0;

      std::string part;

      // collect all strings
      do
      {
         part.assign(msgtext);

         std::string::size_type pos = std::string::npos;

         unsigned int linewidth = width-2;

         if (curline==maxlines-1)
         {
            // subtract size of a "[MORE]" string
            linewidth -= font_normal.calc_length(ua_textscroll_more_string);
         }

         // search for newlines
         pos = part.find_first_of('\n');

         if (pos != std::string::npos)
            part.erase(pos);

         // cut down string on ' ' boundaries, until it fits into an image
         while(font_normal.calc_length(part.c_str()) > linewidth)
         {
            pos = part.find_last_of(' ');

            if (pos != std::string::npos)
            {
               // cut at newline or space
               part.erase(pos);
            }
            else
            {
               // string too long, without a space in between
               // erase char per char
               part.erase(part.size()-1);
            }
         }

         // insert line in linestack or morestack
         if (curline<maxlines)
         {
            if (linestack.size()>=maxlines)
            {
               // needed to scroll
               needed_scrolling = true;

               // remember line in history
               linehistory.push_back(linestack.front());

               // delete old line
               linestack.erase(linestack.begin());
            }

            // add line
            linestack.push_back(part);
         }
         else
         {
            std::string morestr(part);

            // add newline when line ended in a newline
            if (pos!=std::string::npos && msgtext.at(pos)=='\n')
               morestr.append(1,'\n');

            // add to "[MORE]" stack
            morestack.push_back(morestr);
         }

         curline++;

         // cut down string
         msgtext.erase(0,part.size()+1);

      } while(!msgtext.empty());
   }

   update_scroll();

   return needed_scrolling;
}

void ua_textscroll::update_scroll()
{
   // redo scroll texture
   ua_image img_text,img_temp;
   unsigned int imgheight = maxlines * font_normal.get_charheight() + 1;
   img_text.create(width,imgheight,bg_color,0);

   unsigned int max = ua_min(maxlines,linestack.size());
   for(unsigned int i=0; i<max; i++)
   {
      // create line string and paste it into final image
      font_normal.create_string(img_temp,
         linestack[i].c_str(),text_color);

      // calc y position
      unsigned int ypos = i * font_normal.get_charheight() + 1;

      img_text.paste_image(img_temp,1,ypos,true);

      if (i==maxlines-1 && morestack.size()!=0)
      {
         // add a "[MORE]" string at the end of the line
         ua_image img_more;
         font_normal.create_string(img_more,ua_textscroll_more_string,11);

         // paste string after end of last line
         img_text.paste_image(img_more,
            img_temp.get_xres(),ypos,true);
      }

      if (input_mode && i == input_line)
      {
         // paste input text after the text in this line
         ua_image img_input, img_line;

         font_normal.create_string(img_input,
            input_text.c_str(),1); // color black for now

         unsigned int xpos = img_temp.get_xres()+1;
         img_text.paste_image(img_input,xpos,ypos,true);

         xpos += img_input.get_xres();

         // draw a cursor line after the text
         img_line.create(1,img_input.get_yres()+2,1,0);

         img_text.paste_image(img_line,xpos+1,ypos-1,true);
      }

      img_temp.clear();
   }

   // upload image
   if (split_textures)
   {
      // split text image into two
      ua_image img_split1,img_split2;

      unsigned int texwidth = width/2+1;

      img_text.copy_rect(img_split1,0,0, texwidth,imgheight);
      img_text.copy_rect(img_split2,texwidth-1,0, texwidth-1,imgheight);

      // upload it to the texture
      tex.convert(img_split1,0);
      tex.use(0);
      tex.upload();

      tex.convert(img_split2,1);
      tex.use(1);
      tex.upload();
   }
   else
   {
      // image is small enough
      tex.convert(img_text);
      tex.use(0);
      tex.upload();
   }
}

bool ua_textscroll::handle_event(SDL_Event &event)
{
   bool handled = false;

   // check event type
   switch(event.type)
   {
   case SDL_KEYDOWN:
      // key down event
      switch(event.key.keysym.sym)
      {
      case SDLK_SPACE:
      case SDLK_RETURN:
         // "more" continuation
         if (have_more_lines())
         {
            show_more_lines();
            handled = true;
         }
         break;
      default: break;
      }

      // check type'able keys when in input mode
      if (input_mode)
      {
         // check for unicode key
         char ch;
         if ((event.key.keysym.unicode & 0xFF80) == 0)
         {
            // normal (but translated) key
            ch = event.key.keysym.unicode & 0x7F;

            // check range
            if (ch>=SDLK_SPACE && ch<=SDLK_z)
            {
               // add to text and update
               input_text.append(1,ch);

               update_scroll();
               handled = true;
            }
         }

         SDLKey key = event.key.keysym.sym;

         // handle "delete" key
         if ((key==SDLK_BACKSPACE || key==SDLK_DELETE) && !input_text.empty())
         {
            input_text.erase(input_text.size()-1);
            update_scroll();
            handled = true;
         }

         // "return" key
         if (key==SDLK_RETURN)
         {
            // add string to current line
            linestack[input_line].append(input_text);
            update_scroll();

            // end input mode
            input_mode = false;
            handled = true;

            // disable unicode key support
            SDL_EnableUNICODE(0);
            ua_trace("unicode keyboard support disabled\n");
         }
      }
   }

   return handled;
}

void ua_textscroll::enter_input_mode()
{
   // set up input mode
   input_mode = true;
   input_text.erase();
   input_line = linestack.size()-1;

   update_scroll();

   // enable unicode key support
   SDL_EnableUNICODE(1);
   ua_trace("unicode keyboard support enabled\n");
}

bool ua_textscroll::is_input_done(std::string& text)
{
   // still in input mode?
   if (input_mode)
      return false;

   text.assign(input_text);

   return true;
}

void ua_textscroll::show_more_lines()
{
   std::string msgtext;

   // assemble morestack into one string
   unsigned int max = morestack.size();
   for(unsigned int i=0; i<max; i++)
   {
      std::string& morestr = morestack[i];
      msgtext.append(morestr.c_str());

      // append space, but only when last string didn't end with newline
      if (morestr.size()>0 && morestr.at(morestr.size()-1)!='\n')
         msgtext.append(1,' ');
   }

   morestack.clear();

   // print the string
   print(msgtext.c_str());
}

void ua_textscroll::render()
{
   double u = tex.get_tex_u(), v = tex.get_tex_v();
   tex.use(0);

   unsigned int quadwidth = split_textures ? width/2+2 : width+2;
   double dx = split_textures ? 0.5/quadwidth : 0.0;

   // render (first) quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0,  v);   glVertex2i(xpos+0,        200-ypos-height);
   glTexCoord2d(u-dx, v);   glVertex2i(xpos+quadwidth,200-ypos-height);
   glTexCoord2d(u-dx, 0.0); glVertex2i(xpos+quadwidth,200-ypos);
   glTexCoord2d(0.0,  0.0); glVertex2i(xpos+0,        200-ypos);
   glEnd();

   if (split_textures)
   {
      tex.use(1);

      // render second quad
      glBegin(GL_QUADS);
      glTexCoord2d(0.0, v  ); glVertex2i(xpos+quadwidth,  200-ypos-height);
      glTexCoord2d(u  , v  ); glVertex2i(xpos+quadwidth*2,200-ypos-height);
      glTexCoord2d(u  , 0.0); glVertex2i(xpos+quadwidth*2,200-ypos);
      glTexCoord2d(0.0, 0.0); glVertex2i(xpos+quadwidth,  200-ypos);
      glEnd();
   }
}

void ua_textscroll::done()
{
   tex.done();
}
