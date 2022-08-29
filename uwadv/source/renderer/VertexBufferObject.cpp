//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022 Underworld Adventures Team
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
/// \file VertexBufferObject.cpp
/// \brief vertex buffer object implementation
//
#include "pch.hpp"
#include "VertexBufferObject.hpp"

#ifndef ANDROID
#define GL_GLEXT_PROTOTYPES
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_opengl_glext.h>
#else
#include <SDL2/SDL_opengles2.h>
#include <SDL2/SDL_opengles2_gl2.h>
#include <SDL2/SDL_opengles2_gl2ext.h>
#endif

VertexBufferObject::VertexBufferObject(GLenum target, GLenum usage)
   :m_target(target),
   m_usage(usage)
{
   UaAssertMsg(
      target == GL_ARRAY_BUFFER_ARB ||
      target == GL_ELEMENT_ARRAY_BUFFER_ARB,
      "target must have the correct value");

   UaAssertMsg(
      usage == GL_STATIC_DRAW_ARB ||
      usage == GL_STATIC_READ_ARB ||
      usage == GL_STATIC_COPY_ARB ||
      usage == GL_DYNAMIC_DRAW_ARB ||
      usage == GL_DYNAMIC_READ_ARB ||
      usage == GL_DYNAMIC_COPY_ARB ||
      usage == GL_STREAM_DRAW_ARB ||
      usage == GL_STREAM_READ_ARB ||
      usage == GL_STREAM_COPY_ARB,
      "usage must have the correct value");
}

VertexBufferObject::~VertexBufferObject() noexcept
{
   Free();
}

void VertexBufferObject::Generate()
{
   UaAssertMsg(m_vboName == 0, "VBO must not already be created");

   glGenBuffers(1, &m_vboName);
}

void VertexBufferObject::Bind() const
{
   glBindBuffer(m_target, m_vboName);

   GLenum error = glGetError();
   if (error != GL_NO_ERROR)
      UaTrace("Vertex Buffer: error during binding VBO! (%u)\n", error);
}

void VertexBufferObject::Upload(const void* data, size_t elementSizeInBytes, size_t numElements)
{
   glBufferData(m_target, numElements * elementSizeInBytes, data, m_usage);

   GLenum error = glGetError();
   if (error != GL_NO_ERROR)
      UaTrace("Vertex Buffer: error during uploading data to VBO using glBufferData()! (%u)\n", error);
}

void VertexBufferObject::Unbind() const
{
   glBindBufferARB(m_target, 0);
}

void VertexBufferObject::Free() noexcept
{
   if (m_vboName == 0)
      return;

   glDeleteBuffers(1, &m_vboName);

   GLenum error = glGetError();
   if (error != GL_NO_ERROR)
      UaTrace("Vertex Buffer: error during freeing VBO! (%u)\n", error);

   m_vboName = 0;
}
