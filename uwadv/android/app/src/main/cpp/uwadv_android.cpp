//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2022,2023 Underworld Adventures Team
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
/// \file uwadv_android.cpp
/// \brief the Android app's main function
//
#include "Base.hpp"
#include "AndroidGame.hpp"

/// Android main function
int main(int argc, char* argv[])
{
   try
   {
      AndroidGame game;

      game.Init();
      game.Run();
      game.Done();
   }
   catch (const Base::Exception& ex)
   {
      UaTrace("caught Base::Exception: %s\n", ex.what());

      std::string text{ "An unhandled exception was encountered:\n\r" };
      text.append(ex.what());

      SDL_ShowSimpleMessageBox(
         SDL_MESSAGEBOX_ERROR,
         "Underworld Adventures",
         text.c_str(),
         nullptr);
   }
   catch (const std::exception& ex)
   {
      UaTrace("caught std::exception: %s\n", ex.what());

      std::string message{ "std::exception: " };
      message += ex.what();

      SDL_ShowSimpleMessageBox(
         SDL_MESSAGEBOX_ERROR,
         "Underworld Adventures",
         message.c_str(),
         nullptr);
   }

   return 0;
}
