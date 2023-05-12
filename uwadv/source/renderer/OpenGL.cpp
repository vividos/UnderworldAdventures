//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022 Underworld Adventures Team
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
/// \file OpenGL.cpp
/// \brief OpenGL helper
//
#include "pch.hpp"
#include "OpenGL.hpp"

bool OpenGL::IsOpenGLES()
{
#ifdef HAVE_ANDROID
   return true;
#else
   return false;
#endif
}

unsigned int OpenGL::GetMaxTextureSize()
{
   GLint maxTextureSize = 0;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);

   return maxTextureSize < 0
      ? 0
      : static_cast<unsigned int>(maxTextureSize);
}

void OpenGL::PrintOpenGLDiagnostics()
{
   GLint redbits, greenbits, bluebits, alphabits, depthbits;
   glGetIntegerv(GL_RED_BITS, &redbits);
   glGetIntegerv(GL_GREEN_BITS, &greenbits);
   glGetIntegerv(GL_BLUE_BITS, &bluebits);
   glGetIntegerv(GL_ALPHA_BITS, &alphabits);
   glGetIntegerv(GL_DEPTH_BITS, &depthbits);

   UaTrace("OpenGL stats:\n bit depths: red/green/blue/alpha = %u/%u/%u/%u, depth=%u\n",
      redbits, greenbits, bluebits, alphabits, depthbits);

   GLint maxtexsize, maxlights, maxnamestack, maxmodelstack, maxprojstack;
   glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsize);
   glGetIntegerv(GL_MAX_LIGHTS, &maxlights);
   glGetIntegerv(GL_MAX_NAME_STACK_DEPTH, &maxnamestack);
   glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH, &maxmodelstack);
   glGetIntegerv(GL_MAX_PROJECTION_STACK_DEPTH, &maxprojstack);

   UaTrace(" max. texture size = %u x %u, max. lights = %u\n",
      maxtexsize, maxtexsize, maxlights);

   UaTrace(" stack depths: name stack = %u, modelview stack = %u, proj. stack = %u\n",
      maxnamestack, maxmodelstack, maxprojstack);

   const GLubyte* vendor, * rendererName, * version;
   vendor = glGetString(GL_VENDOR);
   rendererName = glGetString(GL_RENDERER);
   version = glGetString(GL_VERSION);

   UaTrace(" vendor: %s\n renderer: %s\n version: %s\n",
      vendor, rendererName, version);

   GLboolean stereo;
   glGetBooleanv(GL_STEREO, &stereo);

   UaTrace(" supports stereo mode: %s\n",
      stereo == GL_TRUE ? "yes" : "no");

   UaTrace("video driver: %s\n\n", SDL_GetCurrentVideoDriver());
}
