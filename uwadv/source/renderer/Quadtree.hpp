//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2003,2019 Underworld Adventures Team
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
/// \file Quadtree.hpp
/// \brief quadtree and view frustum definitions
//
#pragma once

#include <vector>
#include <utility>
#include "Math.hpp"

/// \brief view frustum
/// The view frustum represents a 2d top view of the visible area in OpenGL.
/// To simplify things more, the near clipping plane is ignored. Thus the
/// frustum is represented with a simple triangle described by its corner
/// points.
class Frustum2d
{
public:
   /// ctor
   Frustum2d(double xpos, double ypos, double xangle, double fov, double farplane);

   /// calculates if a point is in the view frustum
   bool IsInFrustum(double xpos, double ypos) const;

   /// returns triangle point
   const Vector2d& GetPoint(unsigned int at) const { return m_points[at]; }

protected:
   /// frustum triangle points
   Vector2d m_points[3];
};

/// callback class interface
class IQuadtreeCallback
{
public:
   /// callback method that is called when a visible tile is found
   virtual void OnVisibleTile(unsigned int xpos, unsigned int ypos) = 0;
};

/// represents a single quadtree quad
class Quad
{
public:
   /// ctor
   Quad(unsigned int xmin, unsigned int xmax,
      unsigned int ymin, unsigned int ymax);

   /// finds all visible tiles in given view frustum
   void FindVisibleTiles(const Frustum2d& frustum, IQuadtreeCallback& callback);

   /// returns true when the quad intersects the frustum
   bool CheckIntersection(const Frustum2d& frustum) const;

protected:
   /// quad coordinates
   unsigned int m_xmin, m_xmax, m_ymin, m_ymax;
};

/// coordinate of a tile
typedef std::pair<unsigned int, unsigned int> QuadTileCoordinates;

/// visible tile collector helper class
class QuadtreeTileCollector : public IQuadtreeCallback
{
public:
   // method from IQuadtreeCallback
   virtual void OnVisibleTile(unsigned int xpos, unsigned int ypos) override;

   /// returns tile list
   std::vector<QuadTileCoordinates>& GetTileList() { return m_tileList; }

protected:
   /// tile list
   std::vector<QuadTileCoordinates> m_tileList;
};
