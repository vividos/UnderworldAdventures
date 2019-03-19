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
/// \file TextureLoader.cpp
/// \brief texture loading implementation
//
#include "pch.hpp"
#include "TextureLoader.hpp"
#include "Texture.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"
#include <algorithm>

extern void ImportTgaImage(Base::File& file, unsigned int& xres, unsigned int& yres,
   unsigned int& origx, unsigned int& origy, std::vector<Uint32>& texels);

void Texture::Load(Base::SDL_RWopsPtr rwops)
{
   unsigned int origx, origy;

   // currently we only have tga texture import
   // TODO: check for file type and load accordingly
   Base::File file(rwops);
   ImportTgaImage(file, m_xres, m_yres, origx, origy, m_texels);

   // calculate max. u and v coordinates
   m_v = ((double)origy) / m_yres;
   m_u = ((double)origx) / m_xres;
}

void Import::TextureLoader::LoadTextures(
   std::vector<IndexedImage>& textureImages,
   unsigned int startIndex,
   const char* textureName,
   Palette256Ptr palette)
{
   Base::File file = m_resourceManager.GetUnderworldFile(Base::resourceGameUw, textureName);
   if (!file.IsOpen())
   {
      std::string text("could not open texture file: ");
      text.append(textureName);
      throw Base::Exception(text.c_str());
   }

   Uint32 fileLength = file.FileLength();

   Uint8 value = file.Read8(); // always 2 (.tr)
   UaAssert(value == 2);
   UNUSED(value);

   unsigned int xyres = file.Read8(); // x and y resolution (square textures)

   Uint16 numTextures = file.Read16();

   if (startIndex + numTextures > textureImages.size())
      textureImages.resize(startIndex + numTextures);

   std::vector<Uint32> offsets(numTextures);
   for (unsigned int i = 0; i < numTextures; i++)
      offsets[i] = file.Read32();

   for (unsigned int textureIndex = 0; textureIndex < numTextures; textureIndex++)
   {
      if (offsets[textureIndex] >= fileLength)
         continue;

      file.Seek(offsets[textureIndex], Base::seekBegin);

      unsigned int dataLength = xyres * xyres;

      IndexedImage& textureImage = textureImages[startIndex + textureIndex];
      textureImage.Create(xyres, xyres);
      Uint8* pixels = textureImage.GetPixels().data();

      textureImage.SetPalette(palette);

      unsigned int bufferIndex = 0;
      while (dataLength > 0)
      {
         unsigned int size = std::min(dataLength, 4096U);
         unsigned int read = file.ReadBuffer(pixels + bufferIndex, size);
         bufferIndex += read;
         dataLength -= read;
      }
   }
}
