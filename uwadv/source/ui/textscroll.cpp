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
/*! \file textscroll.cpp

   \brief text scroll implementation

*/

// needed includes
#include "common.hpp"
#include "textscroll.hpp"
#include "image.hpp"


// constants

//! string to output when scroll is full but more output is available
const char* ua_textscroll_more_string = "\\2 [MORE]";

//! mapping from color code to palette #0 indices
Uint8 ua_textscroll_colors[10] =
{
   46, 38, 11, 48, 180, 21, 212, 251, 24, 192
};


// ua_textscroll methods

/*! Initializes text scroll window. The number of lines is determined by the
    height parameter and the font char cheight.

    \param game reference to game interface
    \param xpos x position on screen
    \param ypos y position on screen
    \param width width of control
    \param height height of control
    \param my_bg_color background color index, from palette 0
*/
void ua_textscroll::init(ua_game_interface& game, unsigned int xpos,
   unsigned int ypos, unsigned int width, unsigned int height,
   Uint8 my_bg_color)
{
   bg_color = my_bg_color;
   color_code = '0'; // default color
   first_visible_line = 0;
   more_mode = false;
   more_line = 0;
   scroll_basex = scroll_basey = 0;

   // load font
   font_normal.load(game.get_settings(), ua_font_normal);

   // calculate number of lines
   maxlines = unsigned(height/font_normal.get_charheight());

   // set up image quad and upload texture
   image.create(width, height);
   image.clear(bg_color);

   ua_image_quad::init(game, xpos, ypos);

   update();
}

/*! Prints text to the scroll. The text may contain newline characters and
    color change codes (like "\1", "\0" etc.). When the scroll is full, a "[MORE]"
    text is shown and the window waits for a keypress.

    \param text text to print to the scroll
    \return true when the user has to press a key and a "[MORE]" text is shown
*/
bool ua_textscroll::print(const char* text)
{
   unsigned int start_line = textlines.size();
   unsigned int last_textlines_size = start_line;
   std::string line, msgtext(text);
   std::string::size_type pos = 0;
   do
   {
      // divide into lines with separator 'newline'
      pos = msgtext.find('\n');

      if (pos != std::string::npos)
      {
         line.assign(msgtext, 0, pos);
         msgtext.erase(0, pos+1);
      }
      else
         line.assign(msgtext);

      // check if the line is too long for the window
      unsigned int linewidth = 0;
      std::string part;
      do
      {
         linewidth = image.get_xres()-2;
         part = line;

         // do we have a line where we have to print "more"?
         if (textlines.size()+1-start_line >= maxlines)
         {
            // reduce the line width
            linewidth -= calc_colored_length(ua_textscroll_more_string);

            // calc new start line
            start_line += maxlines;

            // switch to 'more mode' when not already done
            if (!more_mode)
               more_line = start_line-1;
            more_mode = true;
         }

         // cut down string on ' ' boundaries, until it fits into the image
         while (calc_colored_length(part.c_str()) > linewidth)
         {
            std::string::size_type pos2 = part.find_last_of(' ');

            if (pos2 != std::string::npos)
            {
               // cut at space char
               part.erase(pos2);
            }
            else
            {
               // string too long, without a space in between
               // erase char per char
               part.erase(part.size()-1);
            }
         }

         // move part over to textlines vector
         textlines.push_back(part);
         line.erase(0,part.size());

         {
            // add color code to line start
            std::string& lastline = textlines.back();
            lastline.insert(lastline.begin(), 1, '\\');
            lastline.insert(lastline.begin()+1, 1, color_code);

            // search for new color code
            std::string::size_type pos3 = std::string::npos;
            do
            {
               pos3 = lastline.find_last_of('\\',pos3);

               if (pos3 != std::string::npos && pos3+1 < lastline.size() &&
                  lastline.at(pos3+1) >= '0' && lastline.at(pos3+1) <= '9')
               {
                  // found new last color code
                  color_code = lastline.at(pos3+1);               
                  break;
               }

               --pos3;

            } while(pos3 != std::string::npos && pos3+1 < lastline.size());
         }

         // trim space when needed
         if (line.size()>0 && line.at(0) == ' ')
            line.erase(0,1);

      } while(line.size()>0);

   } while(pos != std::string::npos);

   // scroll lines
   if (textlines.size()-last_textlines_size >= maxlines)
   {
      // printed more lines than visible; set new first line tostart
      first_visible_line = last_textlines_size;
   }
   else
   {
      // printed less than maxlines, adjust when enough lines
      if (textlines.size()>maxlines)
         first_visible_line = textlines.size()-maxlines;
   }

   update_scroll();

   return more_mode;
}

bool ua_textscroll::process_event(SDL_Event& event)
{
   // user pressed space or return in 'more mode'?
   if (more_mode && event.type == SDL_KEYDOWN &&
       (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN))
   {
      // check if we have even more lines that don't fit into the scroll
      more_mode = textlines.size() > more_line + maxlines;

      // still in 'more mode'? then calc new 'more line'
      if (more_mode)
         more_line += maxlines;

      // either way, show more lines
      first_visible_line += ua_min(textlines.size()-first_visible_line-maxlines,maxlines);

      update_scroll();
      return true;
   }

   return ua_image_quad::process_event(event);
}

/*! Updates the scroll's image and updates the image quad. When in 'more mode'
    it shows the text "[MORE]" at the end of the line.
*/
void ua_textscroll::update_scroll()
{
   image.clear(bg_color);

   ua_image img_temp;

   // process all lines visible in the scroll
   unsigned int max = ua_min(maxlines,textlines.size());
   for(unsigned int i=0; i<max; i++)
   {
      // check if we are at the end of the textlines vector
      if (i+first_visible_line >= textlines.size()) break;

      // create line string
      create_colored_string(img_temp, textlines[first_visible_line+i].c_str());
      if (img_temp.get_xres()==0) continue; // empty line

      // calc y position
      unsigned int ypos = i * font_normal.get_charheight() + scroll_basey;

      // paste it into final image
      image.paste_rect(img_temp, 0,0, img_temp.get_xres(), img_temp.get_yres(),
         scroll_basex,ypos,true);

      // add [MORE] string on proper line
      if (more_mode && more_line == i+first_visible_line)
      {
         ua_image img_more;
         create_colored_string(img_more, ua_textscroll_more_string);

         // paste string after end of last line
         image.paste_rect(img_more, 0,0, img_more.get_xres(), img_more.get_yres(),
            img_temp.get_xres(),ypos,true);
      }

      img_temp.clear();
   }

   // update quad texture
   ua_image_quad::update();
}

/*! Calculates the width of an image that contains the given text. The method
    recognizes and filters out color codes that might be in the string.

    \param text string with text
    \return width of image in pixels
*/
unsigned int ua_textscroll::calc_colored_length(const char* text)
{
   std::string line(text);
   std::string::size_type pos = 0;
   
   // remove color codes
   while((pos = line.find('\\',pos)) != std::string::npos)
   {
      if (pos+1 < line.size() && line.at(pos+1) >= '0' && line.at(pos+1) <= '9')
         line.erase(pos,2);
      else
         pos++;
   }

   return font_normal.calc_length(line.c_str());
}

/*! Creates an image from given text. Recognizes color codes and creates
    appropriately colored text.

    \todo check for color codes
*/
void ua_textscroll::create_colored_string(ua_image& img, const char* text)
{
   std::string line(text);

   // no color code?
   if (line.find_first_of('\\') == std::string::npos)
   {
      font_normal.create_string(img,text,ua_textscroll_colors[0]);
      return;
   }

   unsigned int width = calc_colored_length(text);
   img.create(width, font_normal.get_charheight());

   ua_image img_temp;

   // no color code at start? then insert one
   if (line.size() > 0 && line.at(0) != '\\' &&
       !(line.at(1) >= '0' && line.at(1) <= '9'))
      line.insert(0, "\\0");

   // loop assumes that 'line' always start with a color code
   char color = '0';
   unsigned int img_xpos = 0;
   do
   {
      // get color code
      if (line.size() >= 2 && line.at(0) == '\\' &&
          line.at(1) >= '0' && line.at(1) <= '9')
         color = line.at(1);

      line.erase(0,2);

      // find string until next color code (or end)
      std::string part = line;
      std::string::size_type pos = 0;
      do
      {
         pos = line.find_first_of('\\',pos);
         if (pos != std::string::npos && pos+1 < line.size() &&
            line.at(pos+1) >= '0' && line.at(pos+1) <= '9')
         {
            part.assign(line.c_str(), 0, pos);
            break;
         }
         else
            if (pos != std::string::npos)
               pos++;

      } while(pos != std::string::npos && pos+1 < line.size());

      if (part.size() > 0)
      {
         // create substring with color
         Uint8 text_color = ua_textscroll_colors[(color-'0')%10];
         font_normal.create_string(img_temp, part.c_str(), text_color);

         // paste into image
         img.paste_rect(img_temp, 0,0, img_temp.get_xres(),img_temp.get_yres(),
            img_xpos,0, true);

         img_xpos += img_temp.get_xres();

         img_temp.clear();
      }

      // remove processed text
      line.erase(0, part.size());

   } while(line.size()>0);
}

/*! Clears the scroll's contents and updates the image quad. */
void ua_textscroll::clear_scroll()
{
   // clear lines
   textlines.clear();
   more_mode = false;
   first_visible_line = more_line = 0;

   // update and upload image
   update_scroll();
}
