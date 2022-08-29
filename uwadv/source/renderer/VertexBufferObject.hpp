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
#pragma once

/// \brief OpenGL / OpenGL ES Vertex Buffer Object wrapper class
/// \details lets you manage a VBO that can contain (variable sized) data that
/// can be used for rendering or in a shader program. The VBO contents are
/// specified at Upload() or usage time (e.g. when calling gl*Pointer()
/// functions).
class VertexBufferObject
{
public:
   /// ctor
   /// \param target VBO buffer target; possible values are:
   /// - GL_ARRAY_BUFFER: specifies a list of variably sized GL values and
   ///   attributes for vertices
   /// - GL_ELEMENT_ARRAY_BUFFER: specifies a list of indices into a VBO
   ///   containing GL_ARRAY_BUFFER_ARB (in other words, an index buffer)
   /// \param usage: specifies how the VBO is used; one of several constants
   /// that match the pattern: GL_[STATIC|DYNAMIC|STREAM]_[DRAW|READ|COPY]
   /// - STATIC: the buffer is only uploaded once
   /// - DYNAMIC: the buffer is updated occasionally
   /// - STREAM: the buffer is uploaded and used once, e.g. every frame
   /// - DRAW: the buffer is used for drawing operations
   /// - READ: the buffer is read from CPU side often
   /// - COPY: the buffer is used for both
   VertexBufferObject(GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);

   /// dtor; frees the buffer
   ~VertexBufferObject() noexcept;

   /// generates VBO
   void Generate();

   /// binds to this VBO
   void Bind() const;

   /// uploads data to VBO
   void Upload(const void* data, size_t elementSizeInBytes, size_t numElements);

   /// unbinds VBO
   void Unbind() const;

   /// frees VBO
   void Free() noexcept;

private:
   /// deleted copy ctor
   VertexBufferObject(const VertexBufferObject&) = delete;
   /// deleted assignment operator
   VertexBufferObject& operator=(const VertexBufferObject&) = delete;

   /// VBO buffer target
   GLenum m_target;

   /// usage pattern
   GLenum m_usage;

   /// VBO name
   GLuint m_vboName;
};
