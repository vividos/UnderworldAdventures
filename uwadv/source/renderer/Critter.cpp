//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file Critter.cpp
/// \brief critter frames manager implementation
//
#include "pch.hpp"
#include "Critter.hpp"
#include "Object.hpp"

Critter::Critter()
   :m_xres(0), m_yres(0), m_maxFrames(0)
{
}

/// Prepares textures for this critter.
void Critter::Prepare()
{
   ResetPrepare();

   m_allTextures.resize(m_maxFrames);

   for (unsigned int i = 0; i < m_maxFrames; i++)
   {
      Texture& currentTexture = m_allTextures[i];
      currentTexture.Init(1);
   }

   m_frameUploadedFlags.clear();
   m_frameUploadedFlags.resize(m_maxFrames, false);
}

void Critter::ResetPrepare()
{
   m_allTextures.clear();
}

Texture& Critter::GetTexture(unsigned int frame)
{
   //UaAssert(frame < m_frameUploadedFlags.size() && frame != 0xFF);
   if (frame == 0xff)
      frame = 0;

   if (!m_frameUploadedFlags[frame])
   {
      m_frameUploadedFlags[frame] = true;

      m_allTextures[frame].Convert(&m_allFrameBytes[frame * m_xres * m_yres],
         m_xres, m_yres, *m_palette.get(), 0);

      m_allTextures[frame].Upload(0, false);
      // using mipmapped textures (2nd param "true") disables the alpha
      // channel somehow; might be a driver problem
   }

   return m_allTextures[frame];
}

/// Updates animation frame for given object. When the end of the animation is
/// reached, it restarts with the first frame of the animation state.
/// \param obj object to modify animationFrame
void Critter::UpdateFrame(Underworld::Object& obj)
{
   Underworld::NpcInfo& info = obj.GetNpcObject().GetNpcInfo();
   Uint8& animationFrame = info.m_animationFrame;
   Uint8& animationState = info.m_animationState;

   ++animationFrame;

   if (m_segmentList[animationState].size() > animationFrame ||
      m_segmentList[animationState][animationFrame] == 0xff)
      animationFrame = 0;
}
