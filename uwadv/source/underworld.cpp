/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   $Id$

*/
/*! \file underworld.cpp

   a

*/

// needed includes
#include "common.hpp"
#include "underworld.hpp"


// ua_underworld methods

void ua_underworld::init(ua_settings &settings, ua_texture_manager &texmgr)
{
   curmap = new ua_levelmap;

   // load map
   curlevel = 0;
   curmap->load(settings,curlevel);

   texmgr.reset();
   curmap->prepare_textures(texmgr);
}

void ua_underworld::done()
{
   delete curmap;
   curmap = NULL;
}

void ua_underworld::move_player(float x, float y, int level)
{
/*   curmap->load(settings,--curlevel);
   texmgr.reset();
   map.prepare_textures(texmgr);*/
}

void ua_underworld::render(ua_texture_manager &texmgr)
{
   curmap->render(texmgr);
}
