/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file textfile.cpp

   \brief textfile implementation

*/

// needed includes
#include "base.hpp"
#include "textfile.hpp"

using Base::TextFile;

// TextFile methods

void TextFile::ReadLine(std::string& strLine)
{
   // read in one line
   strLine.erase();
   {
      char c = 0;
      do
      {
         // read next char
         c = Read8();
         if (c == '\r')
         {
            // carriage return? reread
            c = Read8();
         }

         // append char
         if (c != '\n')
            strLine.append(1, c);
      }
      while(c != '\n');
   }
}

void TextFile::WriteLine(const std::string& strLine)
{
   // just write out the buffer
   WriteBuffer(reinterpret_cast<const Uint8*>(strLine.c_str()), strLine.size());
#ifdef HAVE_WIN32
   // win32: write cr/lf
   WriteBuffer(reinterpret_cast<const Uint8*>("\r\n"), 1);
#elif defined(HAVE_MACOSX)
   // MacOS X: write cr
   WriteBuffer(reinterpret_cast<const Uint8*>("\r"), 1);
#else
   // all others: write lf
   WriteBuffer(reinterpret_cast<const Uint8*>("\n"), 1);
#endif
}
