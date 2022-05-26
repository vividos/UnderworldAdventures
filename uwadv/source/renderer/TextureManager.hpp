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
/// \file TextureManager.hpp
/// \brief texture manager class
//
#pragma once

#include <vector>
#include "Texture.hpp"
#include "IndexedImage.hpp"

class IBasicGame;

/// texture manager
class TextureManager
{
public:
   /// ctor
   TextureManager();
   /// dtor
   ~TextureManager();
   /// deleted copy ctor
   TextureManager(const TextureManager&) = delete;
   /// deleted assignment operator
   TextureManager& operator=(const TextureManager&) = delete;

   /// initializes texture manager; loads stock textures
   void Init(IBasicGame& game);

   /// called every game tick
   void Tick(double tickRate);

   /// resets usage of stock textures in OpenGL
   void Reset();

   /// prepares a stock texture for usage in OpenGL
   void Prepare(unsigned int index, unsigned int scaleFactor);

   /// use a stock texture in OpenGL
   void Use(unsigned int index);

   /// should be called when a new texname is about to be used
   bool UsingNewTextureName(GLuint newTextureName);

   /// converts stock texture to external one
   void MapStockToExternalTexture(unsigned int index, Texture& texture);

   /// sets new OpenGL color from palette 0
   void GetPaletteColor(Uint8 paletteIndex, Uint8& red, Uint8& green, Uint8& blue);

protected:
   /// frames per second for animated textures
   static const double s_animationFramesPerSecond;

   /// last bound texture name
   GLuint m_lastTextureName;

   /// image array of all stock textures
   std::vector<IndexedImage> m_allStockTextureImages;

   /// stock textures
   std::vector<Texture> m_stockTextures;

   /// infos about current/max texture frame for all stock textures
   std::vector<std::pair<unsigned int, unsigned int>> m_stockTextureAnimationInfos;

   /// palette 0 from image manager
   Palette256Ptr m_palette0;

   /// time counter for animated textures
   double m_animationCount;
};
