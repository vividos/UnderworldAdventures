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
//
/// \file Pathfinder.hpp
/// \brief pathfinding algorithm
//
#pragma once

#include <vector>

namespace Underworld
{
   class Level;
}

enum PathfindFlags
{
   pathfindFlagCanFly = 0,
   pathfindFlagCanSwim,
   pathfindFlagCanWalkLava,

   pathfindFlagMax // must be the last element
};

typedef std::vector<std::pair<unsigned int, unsigned int> > PathList;

/// pathfinder base class
class Pathfinder
{
public:
   /// ctor
   Pathfinder(Underworld::Level& level)
      :m_level(level)
   {
   }

   /// sets pathfind flag
   void SetFlag(PathfindFlags flagType, bool value)
   {
      m_pathFlags[flagType] = value;
   }

   /// finds path
   virtual bool FindPath(unsigned int fromx, unsigned int fromy,
      unsigned int tox, unsigned int toy, PathList& pathlist) = 0;

protected:
   /// checks if object can pass from xpos/ypos in specified direction
   bool CanPass(unsigned int xpos, unsigned int ypos, unsigned int dir);

protected:
   /// path flag array
   bool m_pathFlags[pathfindFlagMax];

   /// current level to use for pathfinding
   Underworld::Level& m_level;
};


/// A* pathfinding algorithm
class PathfinderAStar : public Pathfinder
{
public:
   /// ctor
   PathfinderAStar(Underworld::Level& level)
      :Pathfinder(level)
   {
   }

   /// finds path using A* algorithm
   virtual bool FindPath(unsigned int fromx, unsigned int fromy,
      unsigned int tox, unsigned int toy, PathList& pathlist) override;

private:
};
