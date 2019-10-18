//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2019 Underworld Adventures Team
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
/// \file uwdump.cpp
/// \brief uw1 and uw2 dump program
//
#include "common.hpp"
#include "UnderworldDumper.hpp"

int main(int argc, char* argv[])
{
   printf("uwdump - Ultima Underworld data dump program\n"
      "Copyright (c) 2003,2004,2019 Underworld Adventures Team\n\n");
   printf("Please refer to the uw-formats.txt for details on the various file formats.\n\n");

   UnderworldDumper dumper;

   if (!dumper.ParseArgs(argc, const_cast<const char**>(argv)))
      return -1;

   dumper.Run();

   return 0;
}
