/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003 Underworld Adventures Team

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
/*! \file main.cpp

   \brief the game's main function

*/

// needed includes
#include "common.hpp"
#include "uwadv.hpp"


// external functions

extern ua_uwadv_game* ua_game_create();


// to have console output, use a genuine main(), not the SDL_main one
#ifdef HAVE_CONSOLE
#undef main
#endif


// main function

int main(int argc, char* argv[])
{
   ua_uwadv_game* ua;

   // create new game object
   ua = ua_game_create();

   if (ua == NULL)
      return 1;

#ifndef HAVE_DEBUG // in debug mode the debugger catches the exceptions
   try
#endif
   {
      // init and run the game
      ua->init();
      ua->parse_args(static_cast<unsigned int>(argc),
         const_cast<const char**>(argv));

      ua->run();

      ua->done();
   }
#ifndef HAVE_DEBUG
   catch (ua_exception e)
   {
      std::string text("caught ua_exception:\n\r");
      text.append(e.what());

      ua->error_msg(text.c_str());
   }
   catch (std::exception e)
   {
      ua_trace("caught std::exception\n");
      ua->error_msg("std::exception");
   }
#endif

   delete ua;
   ua = NULL;

   return 0;
}
