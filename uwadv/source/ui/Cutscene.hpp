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
/// \file Cutscene.hpp
/// \brief cutscene handling class
//
#pragma once

#include <vector>
#include "ImageQuad.hpp"
#include "Texture.hpp"

namespace Base
{
   class ResourceManager;
}

/// large page descriptor struct type
struct LargePageDescriptor
{
   Uint16 m_base;
   Uint16 m_records;
   Uint16 m_bytes;
};

/// \brief cutscene animation class
class Cutscene
{
public:
   /// ctor
   Cutscene()
      :m_numRecords(0),
      m_currentFrame((unsigned int)-1)
   {
   }

   /// loads a cutscene by relative filename
   void Load(Base::ResourceManager& resourceManager, const char* relativeFilename,
      IndexedImage& image);

   /// returns maximum number of frames
   unsigned int GetMaxFrames() const { return m_numRecords; }

   /// extracts a new frame into the current image
   void GetFrame(IndexedImage& image, unsigned int frameNumber);

private:
   /// decodes one frame
   void DecodeFrame(IndexedImage& image, unsigned int frameNumber);

   /// number of records in file
   unsigned int m_numRecords;

   /// number of current frame
   unsigned int m_currentFrame;

   /// large page descriptor array values
   std::vector<LargePageDescriptor> m_largePageDescriptorList;

   /// all large pages
   std::vector<Uint8> m_largePages;
};
