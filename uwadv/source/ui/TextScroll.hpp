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
/// \file TextScroll.hpp
/// \brief text scroll
//
#pragma once

#include <vector>
#include "ImageQuad.hpp"
#include "Font.hpp"

/// textscroll color codes for call to TextScroll::SetColorCode()
enum TextScrollColorCode
{
   colorCodeBlack = '0',  ///< black (46) (normal text and conversation)
   colorCodeOrange,     ///< orange (38) (conversation answer)
   colorCodeWhite,      ///< white (11) (not used in uw1)
   colorCodeBlue,       ///< blue (48) (not used in uw1)
   colorCodeRed,        ///< red (180) (restore game failed)
   colorCodeYellow,     ///< yellow (21) (not used in uw1)
   colorCodeGreen,      ///< green (212) (save file description)
   colorCodeViolet,     ///< violet (251) (not used in uw1)
   colorCodeGray,       ///< gray (24) (not used in uw1)
   colorCodeLightBlue   ///< light-blue (192) (not used in uw1)
};

/// \brief text scroll class
/// Text scroll window that can be used to show text messages. The scroll
/// stops when the scroll is full and shows a "[MORE]" text, waiting for a key
/// press. The text scroll understands some color codes that controls the
/// text color. The color codes consist of a \ (backslash) and a following
/// number, e.g. "\0" or "\3". These numbers are available:
/// 0: black
/// 1: orange
/// 2: white
/// 3: blue
/// 4: red
/// 5: yellow
/// 6: green
/// 7: violet
/// 8: gray
/// 9: light-blue
/// The enumeration TextScrollColorCode can be used in the call to
/// SetColorCode().
/// \todo factor out input mode into own InputCtrl or TextEditCtrl
/// \todo add up/down buttons from buttons.gr, images 0/ or 27/28, to go
/// through scroll history
/// \todo add left and right borders
/// \todo limit scroll history to some max. number of lines, e.g. 256
/// \todo implement more codes: \p paragraph, \m show "more" immediately
class TextScroll : public ImageQuad
{
public:
   /// ctor
   TextScroll() {}

   /// initializes text scroll
   void Init(IGame& game, unsigned int xpos, unsigned int ypos,
      unsigned int width, unsigned int height, Uint8 backgroundColor);

   /// sets new foreground text color code
   void SetColorCode(char color)
   {
      m_colorCode = color;
   }

   /// prints a string to the text scroll
   bool Print(const char* text);

   /// clears scroll contents
   void ClearScroll();

   /// indicates if text scroll is waiting for a key press to show more text
   bool IsWaitingMore() const
   {
      return m_isWaitingMore;
   }

   // virtual methods from Window
   virtual bool ProcessEvent(SDL_Event& event) override;

protected:
   /// updates scroll texture
   void UpdateScroll();

   /// calculates image width of a string; ignores color codes
   unsigned int CalcColoredLength(const char* text);

   /// creates image from string; processes color codes
   void CreateColoredString(IndexedImage& img, const char* text);

protected:
   /// background color
   Uint8 m_backgroundColor;

   /// current color code (from '0' to '9')
   char m_colorCode;

   /// font to render text
   Font m_normalFont;

   /// maximal number of lines to show
   unsigned int m_maxLines;

   /// number of first visible line
   unsigned int m_firstVisibleLine;

   /// indicates if user has to press a key to see the rest of the scroll text
   bool m_isWaitingMore;

   /// line where the [MORE] is shown; only valid in 'more mode'
   unsigned int m_moreLineIndex;

   /// all text lines printed so far
   std::vector<std::string> m_textLines;

   /// scroll start base coordinates
   unsigned int m_scrollBaseX, m_scrollBaseY;
};
