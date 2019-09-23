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
/// \file Texture.cpp
/// \brief texture manager implementation
//
#include "pch.hpp"
#include "Texture.hpp"
#include "Constants.hpp"
#include "GameInterface.hpp"
#include "TextureLoader.hpp"
#include "ImageManager.hpp"
#include <gl/GLU.h>

const double TextureManager::s_animationFramesPerSecond = 1.5;

Texture::Texture()
   :m_xres(0), m_yres(0), m_u(0.0), m_v(0.0)
{
}

/// Initializes texture and allocates OpenGL texture names. Previous textures
/// are cleaned up with a call to done(). A pointer to a texture manager can
/// be set to let the texture be managed from it.
/// \param numTextures number of texture images to allocate
void Texture::Init(unsigned int numTextures)
{
   Done();

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
   if (m_textureNames.size() > 0)
      glDeleteTextures(m_textureNames.size(), &m_textureNames[0]);
   m_textureNames.clear();
}

/// Converts image data to texture.
/// \param img image to convert to texture
/// \param numTextures index of texture to use for conversion
void Texture::Convert(IndexedImage& image, unsigned int numTextures)
{
   Convert(&image.GetPixels()[0], image.GetXRes(), image.GetYRes(),
      *image.GetPalette(), numTextures);
}

/// Converts palette-indexed image data to texture. The resulting texture has
/// a resolution of 2^n x 2^m. The texture resolution is determined when the
/// first image is converted. There currently is a resolution limit of 2048 x
/// 2048.
/// \param pix array with index values to Convert
/// \param origx x resolution of image stored in pix
/// \param origy y resolution of image stored in pix
/// \param pal 256 color palette to use in conversion.
/// \param numTextures number of texture image to Convert to
void Texture::Convert(Uint8* pixels, unsigned int origx, unsigned int origy,
   Palette256& palette, unsigned int numTextures)
{
   // only do resolution determination for the first texture
   if (numTextures == 0 || m_xres == 0 || m_yres == 0)
   {
      // determine texture resolution (must be 2^n)
      m_xres = 16;
      while (m_xres < origx && m_xres < 2048) m_xres <<= 1;

      m_yres = 16;
      while (m_yres < origy && m_yres < 2048) m_yres <<= 1;

      m_u = ((double)origx) / m_xres;
      m_v = ((double)origy) / m_yres;

      m_texels.resize(m_textureNames.size()*m_xres*m_yres, 0x00000000);
   }

   // convert color indices to 32-bit texture
   Uint32* palptr = reinterpret_cast<Uint32*>(palette.Get());
   Uint32* texptr = &m_texels[numTextures*m_xres*m_yres];

   for (unsigned int y = 0; y < origy; y++)
   {
      Uint32* texptr2 = &texptr[y*m_xres];
      for (unsigned int x = 0; x < origx; x++)
         *texptr2++ = palptr[pixels[y*origx + x]];
   }
}

/// Converts 32-bit truecolor pixel data in GL_RGBA format to texture. Be sure
/// to only convert images with exactly the same size with this method.
/// \param origx x resolution of pixel data in pix
/// \param origy y resolution of pixel data in pix
/// \param pix 32-bit pixel data in GL_RGBA format (red byte, green byte, ...)
/// \param numTextures index of texture to convert
void Texture::Convert(unsigned int origx, unsigned int origy, Uint32* pixels,
   unsigned int numTextures)
{
   // determine texture resolution (must be 2^n)
   m_xres = 2;
   while (m_xres < origx && m_xres < 2048) m_xres <<= 1;

   m_yres = 2;
   while (m_yres < origy && m_yres < 2048) m_yres <<= 1;

   m_u = ((double)origx) / m_xres;
   m_v = ((double)origy) / m_yres;

   m_texels.resize(m_textureNames.size()*m_xres*m_yres, 0x00000000);

   // copy pixels
   Uint32* texptr = &m_texels[numTextures*m_xres*m_yres];
   for (unsigned int y = 0; y < origy; y++)
   {
      memcpy(texptr, pixels, origx * sizeof(Uint32));
      pixels += origx;
      texptr += m_xres;
   }
}

/// Use the texture in OpenGL. All further triangle, quad, etc. definitions
/// use the texture, until another texture is used or
/// glBindTexture(GL_TEXTURE_2D, 0) is called.
/// \param numTextures index of texture to use
void Texture::Use(unsigned int numTextures)
{
   if (numTextures >= m_textureNames.size())
      return; // invalid texture index

   glBindTexture(GL_TEXTURE_2D, m_textureNames[numTextures]);
}

/// Uploads the Convert()ed texture to the graphics card. The texture doesn't
/// have to be use()d before uploading.
/// \param numTextures index of texture to upload
/// \param mipmaps generates mipmaps when true
void Texture::Upload(unsigned int numTextures, bool useMipmaps)
{
   Use(numTextures);

   Uint32* tex = &m_texels[numTextures*m_xres*m_yres];

   if (useMipmaps)
   {
      // build mipmapped textures
      gluBuild2DMipmaps(GL_TEXTURE_2D, 3, m_xres, m_yres, GL_RGBA,
         GL_UNSIGNED_BYTE, tex);
   }
   else
   {
      // build texture
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_xres, m_yres, 0, GL_RGBA,
         GL_UNSIGNED_BYTE, tex);
   }

   // check for errors
   GLenum error = glGetError();
   if (error != GL_NO_ERROR)
      UaTrace("Texture: error during uploading texture! (%u)\n", error);
}

/// Returns 32-bit texture pixels in GL_RGBA format for specified texture.
/// \param numTextures index of texture to return m_texels
const Uint32* Texture::GetTexels(unsigned int numTextures) const
{
   return &m_texels[numTextures*m_xres*m_yres];
}

TextureManager::TextureManager()
   :m_animationCount(0.0)
{
}

TextureManager::~TextureManager()
{
   Reset();
}

/// Initializes texture manager. All stock textures are loaded and animation
/// infos are generated for animated textures.
/// \param game game interface
void TextureManager::Init(IGame& game)
{
   m_palette0 = game.GetImageManager().GetPalette(0);

   Import::TextureLoader loader{ game.GetResourceManager() };

   // load stock textures
   Base::Settings& settings = game.GetSettings();
   if (settings.GetGameType() == Base::gameUw1)
   {
      // load all wall textures
      const char* wallTexturesFilename =
         settings.GetBool(Base::settingUw1IsUwdemo) ? "data/dw64.tr" : "data/w64.tr";

      loader.LoadTextures(m_allStockTextureImages, Base::c_stockTexturesWall,
         wallTexturesFilename, m_palette0);

      // load all floor textures
      const char* floorTexturesFilename =
         settings.GetBool(Base::settingUw1IsUwdemo) ? "data/df32.tr" : "data/f32.tr";

      loader.LoadTextures(m_allStockTextureImages, Base::c_stockTexturesFloor,
         floorTexturesFilename, m_palette0);

      // load objects
      {
         m_allStockTextureImages.resize(Base::c_stockTexturesObjects);

         // add images to list; we can do this, since the list isn't clear()ed
         // before adding more images
         game.GetImageManager().LoadList(m_allStockTextureImages, "objects");
      }

      // load switches/levers/pull chains
      {
         m_allStockTextureImages.resize(Base::c_stockTexturesSwitches);
         game.GetImageManager().LoadList(m_allStockTextureImages, "tmflat");
      }

      // load door textures
      {
         m_allStockTextureImages.resize(Base::c_stockTexturesDoors);
         game.GetImageManager().LoadList(m_allStockTextureImages, "doors");
      }

      // load tmobj textures
      {
         m_allStockTextureImages.resize(Base::c_stockTexturesTmobj);
         game.GetImageManager().LoadList(m_allStockTextureImages, "tmobj");
      }

      m_stockTextureAnimationInfos.resize(m_allStockTextureImages.size(),
         std::make_pair<unsigned int, unsigned int>(0, 1));

      // set some animated textures
      {
         m_stockTextureAnimationInfos[0x00ce].second = 8; // 206 lavafall
         m_stockTextureAnimationInfos[0x0129].second = 8; // 469 rivulets of lava
         m_stockTextureAnimationInfos[0x0117].second = 8; // 487 rivulets of lava
         m_stockTextureAnimationInfos[0x0118].second = 8; // 486 lava
         m_stockTextureAnimationInfos[0x0119].second = 8; // 485 lava

         m_stockTextureAnimationInfos[0x0120].second = 4; // 478 water
         m_stockTextureAnimationInfos[0x0121].second = 4; // 477 water
         m_stockTextureAnimationInfos[0x0122].second = 4; // 476 water
         m_stockTextureAnimationInfos[0x0110].second = 4; // 493 water
         m_stockTextureAnimationInfos[0x0111].second = 4; // 494 water
      }
   }
   else if (settings.GetGameType() == Base::gameUw2)
   {
      // load all textures
      const char* texturesFilename = "data/t64.tr";

      loader.LoadTextures(m_allStockTextureImages, Base::c_stockTexturesWall,
         texturesFilename, m_palette0);

      m_stockTextureAnimationInfos.resize(m_allStockTextureImages.size(),
         std::make_pair<unsigned int, unsigned int>(0, 1));
   }

   // now that all texture images are loaded, we can resize the texture array
   m_stockTextures.resize(m_allStockTextureImages.size());

   // init stock texture objects
   Reset();
}

/// Does all tick processing for textures. Animates animated textures.
/// \param tickRate tick rate in ticks/second
void TextureManager::Tick(double tickRate)
{
   m_animationCount += 1.0 / tickRate;

   if (m_animationCount > 1.0 / s_animationFramesPerSecond)
   {
      m_animationCount -= 1.0 / s_animationFramesPerSecond;

      // next animation frame
      unsigned int max = m_stockTextureAnimationInfos.size();
      for (unsigned int i = 0; i < max; i++)
      {
         std::pair<unsigned int, unsigned int>& info = m_stockTextureAnimationInfos[i];

         if (info.second > 1)
         {
            // this one has animation
            if ((++info.first) >= info.second)
               info.first = 0;
         }
      }
   }
}

/// Resets all stock textures.
void TextureManager::Reset()
{
   glBindTexture(GL_TEXTURE_2D, 0);

   // call "done" for all stock textures
   unsigned int max = m_stockTextures.size();
   for (unsigned int i = 0; i < max; i++)
      m_stockTextures[i].Done();

   m_lastTextureName = 0;
}

/// Prepares a stock texture for use in OpenGL.
/// \param index index of stock texture to prepare
void TextureManager::Prepare(unsigned int index)
{
   if (index >= m_allStockTextureImages.size())
      return; // not a valid index

   unsigned int maxPaletteIndex = m_stockTextureAnimationInfos[index].second;
   if (maxPaletteIndex < 1)
      return; // not an available texture

   m_stockTextures[index].Init(maxPaletteIndex);

   if (maxPaletteIndex == 1)
   {
      // only allow mipmaps for non-object images
      bool mipmap = (index < Base::c_stockTexturesObjects) || (index > Base::c_stockTexturesObjects + 0x0200);

      // unanimated texture
      // convert to texture object
      m_stockTextures[index].Convert(m_allStockTextureImages[index], 0);
      m_stockTextures[index].Upload(0, mipmap); // upload texture with mipmaps
   }
   else
   {
      Palette256& pal = *m_palette0;

      unsigned int xres = m_allStockTextureImages[index].GetXRes();
      unsigned int yres = m_allStockTextureImages[index].GetXRes();
      Uint8* pixels = &m_allStockTextureImages[index].GetPixels()[0];

      // animated texture
      if (maxPaletteIndex == 8)
      {
         // lava texture: indices 16 through 23
         for (unsigned int i = 0; i < 8; i++)
         {
            m_stockTextures[index].Convert(pixels, xres, yres, pal, i);
            m_stockTextures[index].Upload(i, true); // upload texture with mipmaps

            // rotate entries
            pal.Rotate(16, 8, false);
         }
      }
      else if (maxPaletteIndex == 4)
      {
         // water texture: indices 48 through 51
         for (unsigned int i = 0; i < 4; i++)
         {
            m_stockTextures[index].Convert(pixels, xres, yres, pal, i);
            m_stockTextures[index].Upload(i, true); // upload texture with mipmaps

            // rotate entries
            pal.Rotate(48, 4, true);
         }
      }
   }
}

/// Uses a stock texture.
/// \param index index of stock texture to use
void TextureManager::Use(unsigned int index)
{
   if (index >= m_stockTextures.size())
      return; // not a valid index

   m_stockTextures[index].Use(m_stockTextureAnimationInfos[index].first);
}

/// Uses a new texture name. Returns false when the texture is already in use.
/// \param new_texname new texture name to use
bool TextureManager::UsingNewTextureName(GLuint new_texname)
{
   if (m_lastTextureName == new_texname)
      return false; // no need to use m_textureNames again

   m_lastTextureName = new_texname;
   return true;
}

/// Converts a stock texture to an external one.
/// \param index index of stock texture to convert
/// \param tex texture object to convert to
void TextureManager::MapStockToExternalTexture(unsigned int index, Texture& tex)
{
   tex.Convert(m_allStockTextureImages[index], 0);
}
