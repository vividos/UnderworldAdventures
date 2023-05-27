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
/// \file TextureManager.cpp
/// \brief texture manager implementation
//
#include "pch.hpp"
#include "TextureManager.hpp"
#include "Constants.hpp"
#include "GameInterface.hpp"
#include "TextureLoader.hpp"
#include "ImageManager.hpp"

const double TextureManager::s_animationFramesPerSecond = 1.5;

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
/// \param game game instance
void TextureManager::Init(IGameInstance& game)
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
   }
   else if (settings.GetGameType() == Base::gameUw2)
   {
      // load all textures
      const char* texturesFilename = "data/t64.tr";

      loader.LoadTextures(m_allStockTextureImages, Base::c_stockTexturesWall,
         texturesFilename, m_palette0);
   }

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

   if (settings.GetGameType() == Base::gameUw1)
   {
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
      size_t max = m_stockTextureAnimationInfos.size();
      for (size_t index = 0; index < max; index++)
      {
         std::pair<unsigned int, unsigned int>& info = m_stockTextureAnimationInfos[index];

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
   size_t max = m_stockTextures.size();
   for (size_t index = 0; index < max; index++)
      m_stockTextures[index].Done();

   m_lastTextureName = 0;
}

/// Prepares a stock texture for use in OpenGL.
/// \param index index of stock texture to prepare
/// \param scaleFactor scale factor for stock texture; valid values are 1, 2,
/// 3 and 4
void TextureManager::Prepare(unsigned int index, unsigned int scaleFactor)
{
   if (index >= m_allStockTextureImages.size())
      return; // not a valid index

   // image must not be empty, or the game data doesn't match the graphics
   UaAssert(!m_allStockTextureImages[index].GetPixels().empty());

   unsigned int maxPaletteIndex = m_stockTextureAnimationInfos[index].second;
   if (maxPaletteIndex < 1)
      return; // not an available texture

   m_stockTextures[index].Init(maxPaletteIndex, scaleFactor);

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

void TextureManager::GetPaletteColor(Uint8 paletteIndex, Uint8& red, Uint8& green, Uint8& blue)
{
   Uint8* color = m_palette0->Get() + (paletteIndex * 4);
   red = color[0];
   green = color[1];
   blue = color[2];
}
