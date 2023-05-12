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
/// \file OpenGL.hpp
/// \brief OpenGL helper
//
#pragma once

/// OpenGL helper class
class OpenGL
{
public:
   /// indicates if the current platform only supports OpenGL ES
   static bool IsOpenGLES();

   /// returns the maximum texture size in pixels (of a square texture)
   static unsigned int GetMaxTextureSize();

   /// output some OpenGL diagnostics
   static void PrintOpenGLDiagnostics();
};
