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

   // init texture
   split_textures = width>256;

   tex.init(&core.get_texmgr(),split_textures ? 2 : 1,
      GL_LINEAR,GL_LINEAR,GL_CLAMP,GL_CLAMP);

   // fill with blank texture
   ua_image img_blank;
   img_blank.create((split_textures ? width/2 : width),height,0,0);
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

   if (strchr(text,'\n')==0 && len<=width)
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

         unsigned int linewidth = width;

         if (curline==maxlines-1)
         {
            // subtract size of a "[MORE]" string
            linewidth -= font_normal.calc_length(ua_textscroll_more_string);
         }

         // cut down string on ' ' boundaries, until it fits into an image
         while(font_normal.calc_length(part.c_str()) > linewidth)
         {
            // search for newlines
            pos = part.find_first_of('\n');

            // no newlines? then search for the last space
            if (pos==std::string::npos)
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

   // redo scroll texture
   ua_image img_text,img_temp;
   img_text.create(width,height,bg_color,0);

   unsigned int max = ua_min(maxlines,linestack.size());
   for(unsigned int i=0; i<max; i++)
   {
      // create line string and paste it into final image
      font_normal.create_string(img_temp,
         linestack[i].c_str(),text_color);

      img_text.paste_image(img_temp,0,i*(img_temp.get_yres()+1),true);

      if (i==maxlines-1 && morestack.size()!=0)
      {
         // add a "[MORE]" string at the end of the line
         ua_image img_more;
         font_normal.create_string(img_more,ua_textscroll_more_string,11);

         // paste string after end of last line
         img_text.paste_image(img_more,
            img_temp.get_xres(),i*(img_temp.get_yres()+1),true);
      }

      img_temp.clear();
   }

   // upload image
   if (split_textures)
   {
      // split text image into two
      ua_image img_split1,img_split2;

      img_text.copy_rect(img_split1,0,0,width/2,height);
      img_text.copy_rect(img_split2,width/2,0,width/2,height);

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
   }

   return needed_scrolling;
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

   unsigned int quadwidth = width;
   if (split_textures)
      quadwidth /= 2;

   // render (first) quad
   glBegin(GL_QUADS);
   glTexCoord2d(0.0, v  ); glVertex2i(xpos+0,        200-ypos-height);
   glTexCoord2d(u  , v  ); glVertex2i(xpos+quadwidth,200-ypos-height);
   glTexCoord2d(u  , 0.0); glVertex2i(xpos+quadwidth,200-ypos);
   glTexCoord2d(0.0, 0.0); glVertex2i(xpos+0,        200-ypos);
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
