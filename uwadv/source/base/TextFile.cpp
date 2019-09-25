//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Underworld Adventures Team
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
/// \file TextFile.cpp
/// \brief TextFile implementation
//
#include "pch.hpp"
#include "TextFile.hpp"

using Base::TextFile;

void TextFile::ReadLine(std::string& line)
{
   long fileLength = FileLength();

   // read in one line
   line.erase();
   {
      char c = 0;
      do
      {
         if (Tell() >= fileLength)
            break;

         // read next char
         c = Read8();
#ifdef HAVE_WIN32
         if (c == '\r')
         {
            // carriage return? reread
            c = Read8();
         }
#elif defined(HAVE_MACOSX)
         if (c == '\r')
            c = '\n'; // simulate lf
#endif

         // append char
         if (c != '\n')
            line.append(1, c);
      } while (c != '\n');
   }
}

void TextFile::WriteLine(const std::string& line)
{
   WriteBuffer(reinterpret_cast<const Uint8*>(line.c_str()), line.size());

#ifdef HAVE_WIN32
   // win32: write cr/lf
   WriteBuffer(reinterpret_cast<const Uint8*>("\r\n"), 2);
#elif defined(HAVE_MACOSX)
   // MacOS X: write cr
   WriteBuffer(reinterpret_cast<const Uint8*>("\r"), 1);
#else
   // all others: write lf
   WriteBuffer(reinterpret_cast<const Uint8*>("\n"), 1);
#endif
}
