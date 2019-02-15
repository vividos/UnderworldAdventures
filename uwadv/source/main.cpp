//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2006,2019 Michael Fink
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
/// \file main.cpp
/// \brief the game's main function
//
/// \mainpage
///  Module relations are as follows:<br>
///  \dotfile ua-module-dependencies.dot Module Dependencies
//
#include "base.hpp"
#include "uwadv.hpp"

/// main function
int main(int argc, char* argv[])
{
#ifndef HAVE_DEBUG // in debug mode the debugger catches the exceptions
   try
#endif
   {
      ua_uwadv_game game;

      // init and run the game
      game.init();
      game.parse_args(static_cast<unsigned int>(argc),
         const_cast<const char**>(argv));

      game.run();

      game.done();
   }
#ifndef HAVE_DEBUG
   catch (Base::Exception& ex)
   {
      std::string text("An unhandled exception was encountered:\n\r");
      text.append(ex.what());

      ua->error_msg(text.c_str());
   }
   catch (std::exception& ex)
   {
      UaTrace("caught std::exception: %s\n", ex.what());
      ua->error_msg("std::exception");
   }
#endif

   return 0;
}
