//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002-2005,2019 Michael Fink
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
// common hacking include stuff
//
#pragma once

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <io.h>

#include <string>
#include <vector>
#include <map>

#include <SDL.h>

#define UWPATH "e:\\uw1\\"
//#define UWPATH "e:\\uw2\\"

#undef HAVE_UW2
//#define HAVE_UW2

// fread/fwrite functions

/// reads a 16-bit int
inline Uint16 fread16(FILE* fd)
{
   Uint16 data;
   fread(&data, 1, 2, fd);
   return data;
}

/// reads a 32-bit int
inline Uint32 fread32(FILE* fd)
{
   Uint32 data;
   fread(&data, 1, 4, fd);
   return data;
}


/// game string container class
class ua_gamestrings
{
public:
   /// ctor
   ua_gamestrings() {}

   /// loads all game strings from a file
   void load(const char *filename);

   /// returns a game string
   std::string get_string(unsigned int block, unsigned int string_nr);

protected:
   /// game string container
   std::map<int, std::vector<std::string>> allstrings;
};

/// writes TGA file header
inline void tga_writeheader(FILE* fd, unsigned short width, unsigned short height, unsigned char type = 2, unsigned char colmap = 0, bool bottomup = false)
{
#pragma pack(push,1)

   // tga header struct
   struct tgaheader
   {
      unsigned char idlength;     // length of id field after header
      unsigned char colormaptype; // 1 if a color map is present
      unsigned char tgatype;      // tga type

      // colormap not used
      unsigned short colormaporigin;
      unsigned short colormaplength;
      unsigned char colormapdepth;

      // x and y origin
      unsigned short xorigin, yorigin;
      // width and height
      unsigned short width, height;

      // bits per pixel, either 16, 24 or 32
      unsigned char bitsperpixel;
      unsigned char imagedescriptor;
   } tgaheader =
   {
      0,
      colmap,
      type,

      0,
      (unsigned short)(colmap == 1 ? 256 : 0),
      (unsigned char)(colmap == 1 ? 24 : 0),

      0, 0,
      width, height,

      (unsigned char)(colmap == 1 ? 8 : 32),
      (unsigned char)(bottomup ? 0x00 : 0x20)
   };
#pragma pack(pop)

   fwrite(&tgaheader, 1, 18, fd);
}
