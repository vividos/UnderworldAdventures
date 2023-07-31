//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2019,2021,2022,2023 Underworld Adventures Team
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
/// \file PolygonTessellator.cpp
/// \brief polygon tessellator implementation
//
#include "pch.hpp"
#include "PolygonTessellator.hpp"

#ifdef WIN32
#include "fixed/GluPolygonTessellatorImpl.hpp"
#else
#endif

PolygonTessellator::PolygonTessellator()
:m_impl(std::make_unique<Impl>())
{
}

PolygonTessellator::~PolygonTessellator()
{
}

/// Tessellates the polygon into triangles with the polygon vertices passed
/// with AddPolygonVertex().
/// \param textureNumber texture number to use when storing triangles
/// \param colorIndex color index to use when storing triangles
/// \param flatShaded
const std::vector<Triangle3dTextured>& PolygonTessellator::Tessellate(
   Uint16 textureNumber, Uint8 colorIndex, bool flatShaded)
{
   return m_impl->Tessellate(m_polygonVertexList, textureNumber, colorIndex, flatShaded);
}
