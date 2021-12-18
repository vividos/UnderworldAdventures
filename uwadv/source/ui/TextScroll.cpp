//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file TextScroll.cpp
/// \brief text scroll implementation
//
#include "pch.hpp"
#include "TextScroll.hpp"
#include "IndexedImage.hpp"
#include <algorithm>

/// string to output when scroll is full but more output is available
const char* c_textScrollMoreText = "\\2 [MORE]";

/// mapping from color code to palette #0 indices
Uint8 c_textScrollColors[10] =
{
   46, 38, 11, 48, 180, 21, 212, 251, 24, 192
};

/// Initializes text scroll window. The number of lines is determined by the
/// height parameter and the font char cheight.
/// \param game reference to game interface
/// \param xpos x position on screen
/// \param ypos y position on screen
/// \param width width of control
/// \param height height of control
/// \param backgroundColor background color index, from palette 0
void TextScroll::Init(IGame& game, unsigned int xpos,
   unsigned int ypos, unsigned int width, unsigned int height,
   Uint8 backgroundColor)
{
   m_backgroundColor = backgroundColor;
   m_colorCode = '0'; // default color
   m_firstVisibleLine = 0;
   m_isWaitingMore = false;
   m_moreLineIndex = 0;
   m_scrollBaseX = m_scrollBaseY = 0;

   // load font
   m_normalFont.Load(game.GetResourceManager(), fontNormal);

   // calculate number of lines
   m_maxLines = unsigned(height / m_normalFont.GetCharHeight());

   // set up image quad and upload texture
   m_image.Create(width, height);
   m_image.Clear(backgroundColor);

   ImageQuad::Init(game, xpos, ypos);

   Update();
}

/// Prints text to the scroll. The text may contain newline characters and
/// color change codes (like "\1", "\0" etc.). When the scroll is full, a "[MORE]"
/// text is shown and the window waits for a keypress.
/// \param text text to print to the scroll
/// \return true when the user has to press a key and a "[MORE]" text is shown
bool TextScroll::Print(const char* text)
{
   size_t start_line = m_textLines.size();
   size_t last_textlines_size = start_line;
   std::string line, msgtext(text);
   std::string::size_type pos = 0;
   do
   {
      // divide into lines with separator 'newline'
      pos = msgtext.find('\n');

      if (pos != std::string::npos)
      {
         line.assign(msgtext, 0, pos);
         msgtext.erase(0, pos + 1);
      }
      else
         line.assign(msgtext);

      // check if the line is too long for the window
      unsigned int lineWidth = 0;
      std::string part;
      do
      {
         lineWidth = m_image.GetXRes() - 2;
         part = line;

         // do we have a line where we have to print "more"?
         if (m_textLines.size() + 1 - start_line >= m_maxLines)
         {
            // reduce the line width
            lineWidth -= CalcColoredLength(c_textScrollMoreText);

            // calc new start line
            start_line += m_maxLines;

            // switch to 'more mode' when not already done
            if (!m_isWaitingMore)
               m_moreLineIndex = start_line - 1;
            m_isWaitingMore = true;
         }

         // cut down string on ' ' boundaries, until it fits into the image
         while (CalcColoredLength(part.c_str()) > lineWidth)
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
               part.erase(part.size() - 1);
            }
         }

         // move part over to m_textLines vector
         m_textLines.push_back(part);
         line.erase(0, part.size());

         {
            // add color code to line start
            std::string& lastline = m_textLines.back();
            lastline.insert(lastline.begin(), 1, '\\');
            lastline.insert(lastline.begin() + 1, 1, m_colorCode);

            // search for new color code
            std::string::size_type pos3 = std::string::npos;
            do
            {
               pos3 = lastline.find_last_of('\\', pos3);

               if (pos3 != std::string::npos && pos3 + 1 < lastline.size() &&
                  lastline.at(pos3 + 1) >= '0' && lastline.at(pos3 + 1) <= '9')
               {
                  // found new last color code
                  m_colorCode = lastline.at(pos3 + 1);
                  break;
               }

               --pos3;

            } while (pos3 != std::string::npos && pos3 + 1 < lastline.size());
         }

         // trim space when needed
         if (!line.empty() && line[0] == ' ')
            line.erase(0, 1);

      } while (!line.empty());

   } while (pos != std::string::npos);

   // scroll lines
   if (m_textLines.size() - last_textlines_size >= m_maxLines)
   {
      // printed more lines than visible; set new first line tostart
      m_firstVisibleLine = last_textlines_size;
   }
   else
   {
      // printed less than m_maxLines, adjust when enough lines
      if (m_textLines.size() > m_maxLines)
         m_firstVisibleLine = m_textLines.size() - m_maxLines;
   }

   UpdateScroll();

   return m_isWaitingMore;
}

bool TextScroll::ProcessEvent(SDL_Event& event)
{
   // user pressed space or return in 'more mode'?
   if (m_isWaitingMore && event.type == SDL_KEYDOWN &&
      (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_RETURN))
   {
      // check if we have even more lines that don't fit into the scroll
      m_isWaitingMore = m_textLines.size() > m_moreLineIndex + m_maxLines;

      // still in 'more mode'? then calc new 'more line'
      if (m_isWaitingMore)
         m_moreLineIndex += m_maxLines;

      // either way, show more lines
      m_firstVisibleLine += std::min(m_textLines.size() - m_firstVisibleLine - m_maxLines, m_maxLines);

      UpdateScroll();
      return true;
   }

   return ImageQuad::ProcessEvent(event);
}

/// Updates the scroll's image and updates the image quad. When in 'more mode'
/// it shows the text "[MORE]" at the end of the line.
void TextScroll::UpdateScroll()
{
   m_image.Clear(m_backgroundColor);

   IndexedImage tempImage;

   // process all lines visible in the scroll
   size_t max = std::min(m_maxLines, m_textLines.size());
   for (size_t lineIndex = 0; lineIndex < max; lineIndex++)
   {
      // check if we are at the end of the m_textLines vector
      if (lineIndex + m_firstVisibleLine >= m_textLines.size()) break;

      // create line string
      CreateColoredString(tempImage, m_textLines[m_firstVisibleLine + lineIndex].c_str());
      if (tempImage.GetXRes() == 0)
         continue; // empty line

      // calc y position
      unsigned int ypos = lineIndex * m_normalFont.GetCharHeight() + m_scrollBaseY;

      // paste it into final image
      m_image.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(),
         m_scrollBaseX, ypos, true);

      // add [MORE] string on proper line
      if (m_isWaitingMore && m_moreLineIndex == lineIndex + m_firstVisibleLine)
      {
         IndexedImage img_more;
         CreateColoredString(img_more, c_textScrollMoreText);

         // paste string after end of last line
         m_image.PasteRect(img_more, 0, 0, img_more.GetXRes(), img_more.GetYRes(),
            tempImage.GetXRes(), ypos, true);
      }

      tempImage.Clear();
   }

   // update quad texture
   ImageQuad::Update();
}

/// Calculates the width of an image that contains the given text. The method
/// recognizes and filters out color codes that might be in the string.
/// \param text string with text
/// \return width of image in pixels
unsigned int TextScroll::CalcColoredLength(const char* text)
{
   std::string line(text);
   std::string::size_type pos = 0;

   // remove color codes
   while ((pos = line.find('\\', pos)) != std::string::npos)
   {
      if (pos + 1 < line.size() && line.at(pos + 1) >= '0' && line.at(pos + 1) <= '9')
         line.erase(pos, 2);
      else
         pos++;
   }

   return m_normalFont.CalcLength(line);
}

/// Creates an image from given text. Recognizes color codes and creates
/// appropriately colored text.
/// \todo check for color codes
void TextScroll::CreateColoredString(IndexedImage& image, const char* text)
{
   std::string line(text);

   // no color code?
   if (line.find_first_of('\\') == std::string::npos)
   {
      m_normalFont.CreateString(image, text, c_textScrollColors[0]);
      return;
   }

   unsigned int width = CalcColoredLength(text);
   image.Create(width, m_normalFont.GetCharHeight());

   IndexedImage tempImage;

   // no color code at start? then insert one
   if (line.size() > 1 && line[0] != '\\' &&
      !(line[1] >= '0' && line[1] <= '9'))
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

      line.erase(0, 2);

      // find string until next color code (or end)
      std::string part = line;
      std::string::size_type pos = 0;
      do
      {
         pos = line.find_first_of('\\', pos);
         if (pos != std::string::npos && pos + 1 < line.size() &&
            line.at(pos + 1) >= '0' && line.at(pos + 1) <= '9')
         {
            part.assign(line.c_str(), 0, pos);
            break;
         }
         else
            if (pos != std::string::npos)
               pos++;

      } while (pos != std::string::npos && pos + 1 < line.size());

      if (!part.empty())
      {
         // create substring with color
         Uint8 text_color = c_textScrollColors[(color - '0') % 10];
         m_normalFont.CreateString(tempImage, part.c_str(), text_color);

         // paste into image
         image.PasteRect(tempImage, 0, 0, tempImage.GetXRes(), tempImage.GetYRes(),
            img_xpos, 0, true);

         img_xpos += tempImage.GetXRes();

         tempImage.Clear();
      }

      // remove processed text
      line.erase(0, part.size());

   } while (!line.empty());
}

/// Clears the scroll's contents and updates the image quad.
void TextScroll::ClearScroll()
{
   // clear lines
   m_textLines.clear();
   m_isWaitingMore = false;
   m_firstVisibleLine = m_moreLineIndex = 0;

   // update and upload image
   UpdateScroll();
}
