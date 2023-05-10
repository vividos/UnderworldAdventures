//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2020,2021 Underworld Adventures Team
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
/// \file Texture.cpp
/// \brief texture manager implementation
//
#include "pch.hpp"
#include "Texture.hpp"
#include "IndexedImage.hpp"
#include "Scaler.hpp"

#ifndef ANDROID
#include <gl/GLU.h> // for gluBuild2DMipmaps
#endif

Texture::Texture()
   :m_xres(0),
   m_yres(0),
   m_u(0.0),
   m_v(0.0),
   m_scaleFactor(1)
{
}

/// Initializes texture and allocates OpenGL texture names. Previous textures
/// are cleaned up with a call to done(). A pointer to a texture manager can
/// be set to let the texture be managed from it.
/// \param numTextures number of texture images to allocate
/// \param scaleFactor scale factor for texture; default is 1; possible values
/// are 1, 2, 3 and 4
void Texture::Init(unsigned int numTextures, unsigned int scaleFactor)
{
   Done();

   m_scaleFactor = scaleFactor;

   // create texture names
   m_textureNames.resize(numTextures, 0);
   if (numTextures > 0)
      glGenTextures(numTextures, &m_textureNames[0]);
}

/// Frees texture pixels and OpenGL texture names.
void Texture::Done()
{
   m_texels.clear();

   // delete all texture names
   if (!m_textureNames.empty())
      glDeleteTextures(static_cast<GLsizei>(m_textureNames.size()), &m_textureNames[0]);
   m_textureNames.clear();
}

/// Converts image data to texture.
/// \param img image to convert to texture
/// \param txtureIndex index of texture to use for conversion
void Texture::Convert(IndexedImage& image, unsigned int textureIndex)
{
   Convert(&image.GetPixels()[0], image.GetXRes(), image.GetYRes(),
      *image.GetPalette(), textureIndex);
}

/// Converts palette-indexed image data to texture. The resulting texture has
/// a resolution of 2^n x 2^m. The texture resolution is determined when the
/// first image is converted. There currently is a resolution limit of 2048 x
/// 2048, mainly because graphics cards may not accept larger texture sizes.
/// \param pixels array with index values to convert
/// \param origx x resolution of image stored in pixels
/// \param origy y resolution of image stored in pixels
/// \param palette 256 color palette to use in conversion
/// \param textureIndex index of texture image to convert to
void Texture::Convert(Uint8* pixels, unsigned int origx, unsigned int origy,
   Palette256& palette, unsigned int textureIndex)
{
   // only do resolution determination for the first texture
   if (textureIndex == 0 || m_xres == 0 || m_yres == 0)
   {
      // determine texture resolution (must be 2^n)
      m_xres = 16;
      while (m_xres < origx && m_xres < 2048) m_xres <<= 1;

      m_yres = 16;
      while (m_yres < origy && m_yres < 2048) m_yres <<= 1;

      m_u = ((double)origx) / m_xres / m_scaleFactor;
      m_v = ((double)origy) / m_yres / m_scaleFactor;

      m_texels.resize(m_textureNames.size() * m_xres * m_yres * m_scaleFactor * m_scaleFactor, 0x00000000);
   }

   // convert color indices to 32-bit texture
   Uint32* palptr = reinterpret_cast<Uint32*>(palette.Get());
   Uint32* texelData = GetTexels(textureIndex);

   std::vector<Uint32> unscaledTexelBuffer;
   if (m_scaleFactor > 1)
      unscaledTexelBuffer.resize(m_xres * m_yres);

   Uint32* texptr =
      m_scaleFactor > 1
      ? unscaledTexelBuffer.data()
      : texelData;

   for (unsigned int y = 0; y < origy; y++)
   {
      Uint32* texptr2 = &texptr[y * m_xres];
      for (unsigned int x = 0; x < origx; x++)
         *texptr2++ = palptr[pixels[y * origx + x]];
   }

   if (m_scaleFactor > 1)
   {
      Scaler::Scale(
         m_scaleFactor,
         unscaledTexelBuffer.data(),
         texelData,
         m_xres,
         m_yres);
   }
}

/// Converts 32-bit truecolor pixel data in GL_RGBA format to texture. Be sure
/// to only convert images with exactly the same size with this method.
/// \param origx x resolution of pixel data in pixels
/// \param origy y resolution of pixel data in pixels
/// \param pix 32-bit pixel data in GL_RGBA format (red byte, green byte, ...)
/// \param textureIndex index of texture to convert
void Texture::Convert(unsigned int origx, unsigned int origy, Uint32* pixels,
   unsigned int textureIndex)
{
   // determine texture resolution (must be 2^n)
   m_xres = 2;
   while (m_xres < origx && m_xres < 2048) m_xres <<= 1;

   m_yres = 2;
   while (m_yres < origy && m_yres < 2048) m_yres <<= 1;

   m_u = ((double)origx) / m_xres / m_scaleFactor;
   m_v = ((double)origy) / m_yres / m_scaleFactor;

   m_texels.resize(m_textureNames.size() * m_xres * m_yres * m_scaleFactor * m_scaleFactor, 0x00000000);

   // copy pixels
   std::vector<Uint32> unscaledTexelBuffer;
   if (m_scaleFactor > 1)
      unscaledTexelBuffer.resize(m_xres * m_yres);

   Uint32* texptr =
      m_scaleFactor > 1
      ? unscaledTexelBuffer.data()
      : GetTexels(textureIndex);

   for (unsigned int y = 0; y < origy; y++)
   {
      memcpy(texptr, pixels, origx * sizeof(Uint32));
      pixels += origx;
      texptr += m_xres;
   }

   if (m_scaleFactor > 1)
   {
      Scaler::Scale(
         m_scaleFactor,
         pixels,
         texptr,
         m_xres,
         m_yres);
   }
}

/// Use the texture in OpenGL. All further triangle, quad, etc. definitions
/// use the texture, until another texture is used or
/// glBindTexture(GL_TEXTURE_2D, 0) is called.
/// \param textureIndex index of texture to use
void Texture::Use(unsigned int textureIndex)
{
   if (textureIndex >= m_textureNames.size())
      return; // invalid texture index

   glBindTexture(GL_TEXTURE_2D, m_textureNames[textureIndex]);
}

/// Uploads the Convert()ed texture to the graphics card. The texture doesn't
/// have to be Use()d before uploading.
/// \param textureIndex index of texture to upload
/// \param mipmaps generates mipmaps when true
void Texture::Upload(unsigned int textureIndex, bool useMipmaps)
{
   Use(textureIndex);

   Uint32* tex = GetTexels(textureIndex);

#ifndef ANDROID
   if (useMipmaps)
   {
      // build mipmapped textures
      gluBuild2DMipmaps(
         GL_TEXTURE_2D,
         3,
         m_xres * m_scaleFactor,
         m_yres * m_scaleFactor,
         GL_RGBA,
         GL_UNSIGNED_BYTE,
         tex);
   }
   else
#endif
   {
      // build texture
      glTexImage2D(
         GL_TEXTURE_2D,
         0,
         GL_RGBA,
         m_xres * m_scaleFactor,
         m_yres * m_scaleFactor,
         0,
         GL_RGBA,
         GL_UNSIGNED_BYTE,
         tex);
   }

   // check for errors
   GLenum error = glGetError();
   if (error != GL_NO_ERROR)
      UaTrace("Texture: error during uploading texture! (%u)\n", error);
}

/// Returns 32-bit texture pixels in GL_RGBA format for specified texture.
/// \param textureIndex index of texture to return texels
const Uint32* Texture::GetTexels(unsigned int textureIndex) const
{
   return &m_texels[textureIndex * m_xres * m_yres];
}

/// Returns 32-bit texture pixels in GL_RGBA format for specified texture.
/// \param textureIndex index of texture to return texels
Uint32* Texture::GetTexels(unsigned int textureIndex)
{
   return &m_texels[textureIndex * m_xres * m_yres];
}
