//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file Font.hpp
/// \brief font class
//
#pragma once

#include <vector>
#include "IndexedImage.hpp"

namespace Base
{
   class Settings;
}

namespace Import
{
   class FontLoader;
}

/// available fonts
enum FontId
{
   /// "font4x5p.sys", height 4px, for inventory object count
   fontSmall = 0,
   /// "font5x6i.sys", height 7px, for character stats screen
   fontItalic = 1,
   fontNormal = 2,  ///< "font5x6p.sys", height 6px, for scroll messages
   fontBig = 3,     ///< "fontbig.sys",  height 15px, for cutscenes
   fontButtons = 4, ///< "fontbutn.sys", height 6px,  for buttons
   /// "fontchar.sys", height 10px, for character generation, not available in uw_demo
   fontCharacterGeneration = 5,
};

/// font class
class Font
{
public:
   /// ctor
   Font() {}

   /// loads a font
   void Load(Base::Settings& settings, FontId fontId);

   /// returns height of chars in pixels
   unsigned int GetCharHeight() { return m_charHeight; }

   /// calculates and returns length of string in pixel
   unsigned int CalcLength(const char* text);

   /// creates image from string, using the font, using a foreground palette index
   void CreateString(IndexedImage& image, const char* text, Uint8 foregroundIndex);

protected:
   friend Import::FontLoader;

   /// font data
   std::vector<Uint8> m_fontData;

   /// length info for every char in font
   std::vector<Uint8> m_charLengths;

   /// size of a character in bytes
   Uint16 m_charSize;

   /// width of a space char, in pixels
   Uint16 m_spaceWidth;

   /// font height in pixels
   Uint16 m_charHeight;

   /// width of a character row in bytes
   Uint16 m_rowWidth;

   /// maximum width of a char in pixels
   Uint16 m_maxWidth;

   /// number of characters in font
   unsigned int m_numChars;
};
