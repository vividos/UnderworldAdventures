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
/// \file map_view.hpp
/// \brief level map view
//
#pragma once

#include "screen.hpp"

/// level map view screen class
class ua_map_view_screen: public ua_screen
{
public:
   /// ctor
   ua_map_view_screen(){}
   /// dtor
   virtual ~ua_map_view_screen(){}

   // virtual functions from ua_screen
/*
   virtual void init(ua_game_core_interface* core) override;
   virtual void done() override;
   virtual void handle_event(SDL_Event &event) override;
   virtual void render() override;*/
   virtual void tick() override;
};
