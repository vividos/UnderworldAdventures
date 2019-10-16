//
// Underworld Adventures - an Ultima Underworld remake project
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
/// \file Texture.hpp
/// \brief texture management classes
/// \details Texture::convert() converts image pixels to 32-bit texture data
/// Texture::use() activates a texture name for use in rendering
/// Texture::upload() uploads a converted texture to the graphics card
/// \note texture size for multiple textures is calculated when convert()ing
/// the first texture. all other textures must have the same size.
//
#pragma once

#include <SDL_opengl.h>
#include <vector>
#include "Settings.hpp"
#include "IndexedImage.hpp"

class TextureManager;
class IGame;

/// \brief texture class; represents one or more texture images
/// The Texture class can be used to store and upload textures to OpenGL.
/// Textures are usually of size 2^n x 2^m (where n and m are integral
/// numbers). The Texture class can be used in two ways; a) uploading
/// texture once and using it consecutively and b) uploading texture each
/// rendered frame (e.g. for animations).
/// Each Texture object can hold one or more texture images of the same
/// size.
class Texture
{
public:
   /// ctor
   Texture();

   // texture preparation

   /// allocates and initializes OpenGL texture object
   void Init(unsigned int numTextures = 1);

   /// cleans up texture name(s) after usage
   void Done();

   /// converts image to texture
   void Convert(IndexedImage& img, unsigned int numTextures = 0);

   /// converts 32-bit RGBA values to texture
   void Convert(unsigned int xres, unsigned int yres, Uint32* pixels,
      unsigned int numTextures = 0);

   /// loads texture from (seekable) rwops stream
   void Load(Base::SDL_RWopsPtr rwops);


   // texture usage

   /// uses texture in OpenGL
   void Use(unsigned int numTextures = 0);

   /// uploads a converted texture to OpenGL
   void Upload(unsigned int numTextures = 0, bool useMipmaps = false);


   // texture information

   /// returns u texture coordinate
   double GetTexU() const
   {
      return m_u;
   }

   /// returns v texture coordinate
   double GetTexV() const
   {
      return m_v;
   }


   // raw texture access

   /// returns array of texels
   const Uint32* GetTexels(unsigned int textureIndex = 0) const;

   /// returns x resolution
   unsigned int GetXRes() const
   {
      return m_xres;
   }

   /// returns y resolution
   unsigned int GetYRes() const
   {
      return m_yres;
   }

private:
   /// converts pixels and a palette to texture
   void Convert(Uint8* pixels, unsigned int origx, unsigned int origy,
      Palette256& palette, unsigned int numTextures);

private:
   /// upper left texture coordinates
   double m_u, m_v;

   /// x and y resolution of the texture
   unsigned int m_xres, m_yres;

   /// texture pixels for all images
   std::vector<Uint32> m_texels;

   /// texture name(s)
   std::vector<GLuint> m_textureNames;

   friend class TextureManager;
   friend class Critter;
};

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
   void Init(IGame& game);

   /// called every game tick
   void Tick(double tickRate);

   /// resets usage of stock textures in OpenGL
   void Reset();

   /// prepares a stock texture for usage in OpenGL
   void Prepare(unsigned int index);

   /// prepares an external texture for usage in OpenGL
   void Prepare(Texture& texture);

   /// use a stock texture in OpenGL
   void Use(unsigned int index);

   /// should be called when a new texname is about to be used
   bool UsingNewTextureName(GLuint newTextureName);

   /// converts stock texture to external one
   void MapStockToExternalTexture(unsigned int index, Texture& texture);

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
   std::vector<std::pair<unsigned int, unsigned int> > m_stockTextureAnimationInfos;

   /// palette 0 from image manager
   Palette256Ptr m_palette0;

   /// time counter for animated textures
   double m_animationCount;
};
