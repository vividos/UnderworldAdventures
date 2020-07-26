//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2020 Underworld Adventures Team
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
/// \brief texture class
/// \details Texture::Convert() converts image pixels to 32-bit texture data
/// Texture::Use() activates a texture name for use in rendering
/// Texture::Upload() uploads a converted texture to the graphics card
/// \note Texture size for multiple textures is calculated when Convert()ing
/// the first texture. All other textures must have the same size.
//
#pragma once

#include <SDL_opengl.h>
#include <vector>

class IndexedImage;
class Palette256;

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
