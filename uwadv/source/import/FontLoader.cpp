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
/// \file FontLoader.cpp
/// \brief font loading implementation
/// \details to simplify interface, fonts are loaded via an FontId. more infos about
/// font file layout can be found in the file docs/ua-formats.txt
//
#include "pch.hpp"
#include "FontLoader.hpp"
#include "Font.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"

/// uw1 font names
const char* g_fontNames[6] =
{
   "font4x5p.sys",
   "font5x6i.sys",
   "font5x6p.sys",
   "fontbig.sys",
   "fontbutn.sys",
   "fontchar.sys"
};

void Font::Load(Base::ResourceManager& resourceManager, FontId fontId)
{
   if (fontId >= (int)SDL_TABLESIZE(g_fontNames))
      throw Base::Exception("Font::init: invalid font id");

   std::string fontFilename = "data/";
   fontFilename.append(g_fontNames[fontId]);

   Import::FontLoader::LoadFont(resourceManager, fontFilename.c_str(), *this);
}

void Import::FontLoader::LoadFont(Base::ResourceManager& resourceManager, const char* fontFilename, Font& font)
{
   Base::File file = resourceManager.GetUnderworldFile(Base::resourceGameUw, fontFilename);
   if (!file.IsOpen())
   {
      std::string text("could not open font file: ");
      text.append(fontFilename);
      throw Base::Exception(text.c_str());
   }

   unsigned long fileLength = file.FileLength();

   // read in header
   Uint16 dummy;
   dummy = file.Read16();
   UNUSED(dummy);

   font.m_charSize = file.Read16();
   font.m_spaceWidth = file.Read16();
   font.m_charHeight = file.Read16();
   font.m_rowWidth = file.Read16();
   font.m_maxWidth = file.Read16();

   // fix for overly large font chars like 'm' or 'w'
   font.m_maxWidth |= 7;

   // calculate number of chars
   font.m_numChars = (fileLength - 12) / (font.m_charSize + 1);

   // allocate memory for new data
   font.m_fontData.clear();
   font.m_charLengths.clear();

   font.m_fontData.resize(font.m_numChars * font.m_charHeight * font.m_maxWidth);
   font.m_charLengths.resize(font.m_numChars);

   for (unsigned int n = 0; n < font.m_numChars; n++)
   {
      unsigned int bits = 0;
      Uint8 curbits = 0;

      // read in whole character
      for (unsigned int h = 0; h < font.m_charHeight; h++)
      {
         for (unsigned int w = 0; w < font.m_maxWidth; w++)
         {
            if (bits == 0)
            {
               curbits = file.Read8();
               bits = 8;
            }

            if ((curbits & 0x80) == 0)
            {
               // background pixel
               font.m_fontData[n*font.m_charHeight*font.m_maxWidth + h * font.m_maxWidth + w] = 0;
            }
            else
            {
               font.m_fontData[n*font.m_charHeight*font.m_maxWidth + h * font.m_maxWidth + w] = 1;
            }

            curbits <<= 1;
            bits--;
         }

         // reset bit reservoir every line
         bits = 0;
      }

      // read char length in pixels
      font.m_charLengths[n] = file.Read8();
   }
}
