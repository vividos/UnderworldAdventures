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
/// \file CutsceneLoader.cpp
/// \brief cutscenes loader
/// \details for more infos about the cutscene format, see the file docs/uw-formats.txt
//
#include "Import.hpp"
#include "Cutscene.hpp"
#include "CutsceneLoader.hpp"
#include "IndexedImage.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"
#include <algorithm>

void Import::CutsceneLoader::LoadCutscene(Base::ResourceManager& resourceManager,
   const char* filename,
   IndexedImage& image,
   std::vector<LargePageDescriptor>& largePageDescriptorList,
   std::vector<Uint8>& largePages,
   unsigned int& numRecords)
{
   Base::File file = resourceManager.GetUnderworldFile(Base::resourceGameUw, filename);
   if (!file.IsOpen())
   {
      std::string text("could not open cutscene file ");
      text.append(filename);
      throw Base::Exception(text.c_str());
   }

   long filesize = file.FileLength();

   // read file id
   char sign[4];
   file.ReadBuffer(reinterpret_cast<Uint8*>(sign), 4);

   if (0 != strncmp(sign, "LPF ", 4))
      throw Base::Exception("wrong cutscene file type");

   // number of large pages and records
   file.Seek(6, Base::seekBegin);
   Uint16 largepages = file.Read16();
   numRecords = file.Read32();

   file.Seek(0x10, Base::seekBegin);
   file.ReadBuffer(reinterpret_cast<Uint8*>(sign), 4);
   if (0 != strncmp(sign, "ANIM", 4))
      throw Base::Exception("wrong cutscene file type");

   // width and height
   unsigned int xres = file.Read16();
   unsigned int yres = file.Read16();
   image.Create(xres, yres);

   // skip color cycling infos
   file.Seek(0x0100, Base::seekBegin);

   // create new palette
   image.CreateNewPalette();

   Palette256& anim_palette = *image.GetPalette();

   // read in palette
   for (int n = 0; n < 256; n++)
   {
      anim_palette.Set((Uint8)n, 2, file.Read8()); // blue
      anim_palette.Set((Uint8)n, 1, file.Read8()); // green
      anim_palette.Set((Uint8)n, 0, file.Read8()); // red
      file.Read8(); // extra pad byte
      anim_palette.Set((Uint8)n, 3, 0xff); // transparency
   }

   // now we are at the large page descriptor array
   largePageDescriptorList.resize(largepages);

   for (unsigned int i = 0; i < largepages; i++)
   {
      largePageDescriptorList[i].m_base = file.Read16();    // base record
      largePageDescriptorList[i].m_records = file.Read16(); // number of records
      largePageDescriptorList[i].m_bytes = file.Read16();   // bytes in large page
   }

   // seek to start of the real large pages
   file.Seek(0x0100 + 256 * 4 + 6 * 256, Base::seekBegin);

   // read in all large pages
   unsigned int lpsize = filesize - file.Tell();
   largePages.resize((lpsize | 0xffff) + 1, 0); // fill up to whole 64k page
   file.ReadBuffer(largePages.data(), lpsize);
}

/// dumps pixels to output buffer
void DumpPixel(Uint8*& source, Uint8*& dest, unsigned int& maxPixels, unsigned int size)
{
   unsigned int len = std::min(size, maxPixels);
   memcpy(dest, source, len);
   dest += len;
   source += len;
   maxPixels -= len;
}

/// does a run with a pixel to output buffer
void RunPixel(Uint8*& source, Uint8*& dest, unsigned int& maxPixels, unsigned int size)
{
   Uint8 pixel = *source++;
   unsigned int len = std::min(size, maxPixels);

   memset(dest, pixel, len);
   dest += len;
   maxPixels -= len;
}

/// skips some pixels in the output buffer
void SkipPixel(Uint8*& dest, unsigned int& maxPixels, unsigned int size)
{
   if (size > maxPixels) size = maxPixels;
   dest += size;
   maxPixels -= size;
}

void Import::CutsceneLoader::ExtractCutsceneData(Uint8* source, Uint8* dest, unsigned int maxPixels)
{
   while (maxPixels > 0)
   {
      Sint8 cnt = static_cast<Sint8>(*source++);

      // short operations

      if (cnt > 0)
      {
         // short dump
         DumpPixel(source, dest, maxPixels, cnt);
         continue;
      }

      if (cnt == 0)
      {
         // short run
         Uint8 wordcnt = *source++;
         RunPixel(source, dest, maxPixels, wordcnt);
         continue;
      }

      // remove sign bit
      cnt &= 0x7f;

      if (cnt != 0)
      {
         // short skip
         SkipPixel(dest, maxPixels, cnt);
         continue;
      }

      // long operations

      // get next 16 bit word
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
      Uint16 wordcnt = SDL_SwapLE16(*(reinterpret_cast<Uint16*>(source)));
#else
      Uint16 wordcnt = *(reinterpret_cast<Uint16*>(source));
#endif
      source += 2;

      // end of decoding?
      if (wordcnt == 0)
         break;

      if (static_cast<Sint16>(wordcnt) > 0)
      {
         // long skip
         SkipPixel(dest, maxPixels, wordcnt);
         continue;
      }

      // remove sign bit
      wordcnt &= 0x7fff;

      if (wordcnt >= 0x4000)
      {
         // clear "longRun" bit
         wordcnt -= 0x4000;

         // long run
         RunPixel(source, dest, maxPixels, wordcnt);
      }
      else
      {
         // long dump
         DumpPixel(source, dest, maxPixels, wordcnt);
      }
   }
}
