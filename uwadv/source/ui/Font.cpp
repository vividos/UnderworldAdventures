//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2019 Underworld Adventures Team
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
/// \file Font.cpp
/// \brief font creation and handling
/// functions to create a image from a string, in a specific font. fonts are
/// loaded in Font::init() (in resource/fontloader.cpp).
///
/// font pixels are stored in an array, which can be accessed like this:
///
///    m_fontData[ch * m_charHeight * m_maxWidth + y * m_maxWidth + x];
///
/// where 'ch' is the character to access, 'y' is the y coordinate and 'x' is
/// the x coord. pixels have the value 0 for transparent or 1 for solid. the
/// function Font::CreateString() matches the '1' to a specific color
/// palette index.
#include "common.hpp"
#include "Font.hpp"
#include <cstring>

unsigned int Font::CalcLength(const char* text)
{
   unsigned int width = 0, len = strlen(text);
   for (unsigned int i = 0; i < len; i++)
   {
      // count length for each char
      unsigned char ch = static_cast<unsigned char>(text[i]);

      if (ch >= m_numChars)
         width += m_maxWidth;
      else
         if (ch == 0x20)
            width += m_spaceWidth;
         else
            width += m_charLengths[ch];
   }

   return width;
}

void Font::CreateString(IndexedImage& image, const char* text, Uint8 foregroundIndex)
{
   // create image with proper size
   unsigned int width = CalcLength(text), len = strlen(text);

   image.Create(width, m_charHeight);

   std::vector<Uint8>& pixels = image.GetPixels();

   unsigned int pos = 0;

   // store all chars in the image
   for (unsigned int i = 0; i < len; i++)
   {
      unsigned char ch = static_cast<unsigned char>(text[i]);

      if (ch < m_numChars)
      {
         if (ch == 0x20)
         {
            pos += m_spaceWidth;
            continue;
         }

         unsigned int clen = m_charLengths[ch];

         for (unsigned int y = 0; y < m_charHeight; y++)
            for (unsigned int x = 0; x < clen; x++)
            {
               bool pixset =
                  m_fontData[ch*m_charHeight*m_maxWidth + y * m_maxWidth + x] == 1;
               pixels[y*width + x + pos] = pixset ? foregroundIndex : 0;
            }

         pos += clen;
      }
   }
}
