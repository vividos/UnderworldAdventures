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
/*! \file win32/main.cpp

   \brief win32 specific main function

*/

// needed includes
#include "common.hpp"
#include "game_win32.hpp"


// to have console output, use a genuine main(), not the SDL_main one
#ifdef HAVE_CONSOLE
#undef main
#endif


// main function

int main(int argc, char* argv[])
{
   ua_game_win32 ua;

#ifndef _DEBUG
   try
#endif
   {
      ua.init();
      ua.run();
      ua.done();
   }
#ifndef _DEBUG
   catch (ua_exception e)
   {
      ua.error_msg(e.what());
   }
   catch (std::exception e)
   {
      ua.error_msg("std::exception");
   }
#endif

   return 0;
}
