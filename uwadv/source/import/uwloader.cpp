/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2004 Underworld Adventures Team

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
/*! \file uwloader.cpp

   \brief underworld loading

*/

// needed includes
#include "common.hpp"
#include "import.hpp"
#include "settings.hpp"
#include "underworld.hpp"
#include "player.hpp"


// ua_uw_import methods

void ua_uw_import::load_underworld(ua_underworld& underw,
   ua_settings& settings, const char* folder, bool initial)
{
   // load level maps
   load_levelmaps(underw.get_levelmaps_list().get_list(),settings,folder);

   // load conv globals
   load_conv_globals(underw.get_conv_globals(), settings, folder, initial);

   // load player infos
   if (!initial)
      load_player(underw.get_player(),folder);
}
