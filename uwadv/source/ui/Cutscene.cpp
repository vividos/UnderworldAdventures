//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2022 Underworld Adventures Team
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
/// \file Cutscene.cpp
/// \brief cutscene frame extraction
//
#include "pch.hpp"
#include "Cutscene.hpp"
#include "CutsceneLoader.hpp"

void Cutscene::Load(Base::ResourceManager& resourceManager, const char* relativeFilename,
   IndexedImage& image)
{
   Import::CutsceneLoader::LoadCutscene(
      resourceManager, relativeFilename,
      image, m_largePageDescriptorList, m_largePages, m_numRecords);

   m_currentFrame = (unsigned int)-1;
}

void Cutscene::GetFrame(IndexedImage& image, unsigned int frameNumber)
{
   if (m_currentFrame != frameNumber)
   {
      // when we are at the max, start again from the first
      if (m_currentFrame > frameNumber)
      {
         m_currentFrame = 0;
         DecodeFrame(image, 0);
      }

      // decode all frames between the current and the wanted frame
      while (m_currentFrame < frameNumber)
      {
         m_currentFrame++;
         DecodeFrame(image, m_currentFrame);
      }
   }
}

void Cutscene::DecodeFrame(IndexedImage& image, unsigned int frameNumber)
{
   size_t largepages = m_largePageDescriptorList.size();

   // first, search large page to use
   size_t i = 0;
   for (; i < largepages; i++)
      if (m_largePageDescriptorList[i].m_base <= frameNumber &&
         unsigned(m_largePageDescriptorList[i].m_base + m_largePageDescriptorList[i].m_records) > frameNumber)
         break;

   if (i >= largepages)
      throw Base::Exception("could not find frame in large pages");

   // calculate large page pointer
   Uint16* curlp16 = reinterpret_cast<Uint16*>(&m_largePages[0x10000 * i]);
   Uint8* curlp = reinterpret_cast<Uint8*>(curlp16);

   unsigned int destframe = frameNumber - m_largePageDescriptorList[i].m_base;

   // calculate offset to proper record
   unsigned int offset = 0;
#if (SDL_BYTEORDER==SDL_BIG_ENDIAN)
   for (unsigned int j = 0; j < destframe; j++)
      offset += SDL_SwapLE16(curlp16[j + 4]);
#else
   for (unsigned int j = 0; j < destframe; j++)
      offset += curlp16[j + 4];
#endif

   // calculate start of "record" struct
   Uint8* src = curlp + 8 + m_largePageDescriptorList[i].m_records * 2 + offset;
   Uint16* src16 = reinterpret_cast<Uint16*>(src);

   // add extra offset
   if (src[1])
      src += (src16[1] + (src16[1] & 1));

   // extract the pixel data
   Import::CutsceneLoader::ExtractCutsceneData(&src[4], &image.GetPixels()[0],
      image.GetXRes() * image.GetYRes());
}
