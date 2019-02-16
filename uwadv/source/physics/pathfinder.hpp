//
// Underworld Adventures - an Ultima Underworld hacking project
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
/// \file pathfinder.hpp
/// \brief pathfinding algorithm
//
#pragma once

#include <vector>

namespace Underworld
{
   class Level;
}

enum ua_pathfind_flags
{
   ua_pf_can_fly = 0,
   ua_pf_can_swim,
   ua_pf_can_walk_lava,

   ua_pf_max // must be the last element
};

typedef std::vector<std::pair<unsigned int, unsigned int> > ua_path_list;

/// pathfinder base class
class ua_pathfinder
{
public:
   /// ctor
   ua_pathfinder(Underworld::Level& level)
      :m_level(level)
   {
   }

   /// sets pathfind flag
   void set_flag(ua_pathfind_flags flagtype, bool set_it)
   {
      path_flags[flagtype] = set_it;
   }

   /// finds path
   virtual bool find_path(unsigned int fromx, unsigned int fromy,
      unsigned int tox, unsigned int toy, ua_path_list& pathlist) = 0;

protected:

   /// checks if object can pass from xpos/ypos in specified direction
   bool can_pass(unsigned int xpos, unsigned int ypos, unsigned int dir);

protected:
   /// path flag array
   bool path_flags[ua_pf_max];

   /// current level to use for pathfinding
   Underworld::Level& m_level;
};


/// A* pathfinding algorithm
class ua_pathfinder_astar : public ua_pathfinder
{
public:
   /// ctor
   ua_pathfinder_astar(Underworld::Level& level)
      :ua_pathfinder(level)
   {
   }

   /// finds path using A* algorithm
   virtual bool find_path(unsigned int fromx, unsigned int fromy,
      unsigned int tox, unsigned int toy, ua_path_list& pathlist);

private:
};
