//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2014,2019,2021 Underworld Adventures Team
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
/// \file convdec.cpp
/// \brief Underworld conversation code decompiler
//
#include "Decompiler.hpp"
#include "GameStrings.hpp"
#include "GameStringsImporter.hpp"
#include "FileSystem.hpp"

/// convdec tool main function
int main(int argc, char* argv[])
{
   printf("convdec - Ultima Underworld conversation code decompiler\n"
      "Copyright (c) 2003,2004,2019,2021 Underworld Adventures Team\n\n");

   if (argc < 3)
   {
      printf("syntax: convdec <basepath> <slotnumber>\n"
         "   basepath is the path to the uw1 or uw2 folder.\n"
         "   slotnumber is the conversation slot number to decompile, either\n"
         "   as decimal or hex; slotnumber can also be * to decompile all conversations\n");
      printf("example: convdec . 0x0001\n"
         "         convdec \"c:\\uw1\\\" 42\n\n");
      return 1;
   }

   std::string basePath = argv[1];
   basePath += Base::FileSystem::PathSeparator;

   std::string slotText = argv[2];

   bool isHex = slotText.find("0x") == 0;
   if (isHex)
      slotText.erase(0, 2);

   Uint16 slotNumber = (Uint16)strtol(slotText.c_str(), NULL, isHex ? 16 : 10);

   bool showDisassembly = true;

   GameStrings gameStrings;
   Import::GameStringsImporter importer{ gameStrings };
   importer.LoadStringsPakFile((basePath + "data/strings.pak").c_str());

   if (slotText != "*")
   {
      Conv::Decompiler cnvdec{ slotNumber, basePath, gameStrings };
      cnvdec.Write(stdout, showDisassembly);
   }
   else
   {
      for (slotNumber = 0; slotNumber < 0x1ff; slotNumber++)
      {
         if (gameStrings.IsBlockAvail(0x0e00 + slotNumber))
         {
            Conv::Decompiler cnvdec{ slotNumber, basePath, gameStrings };
            cnvdec.Write(stdout, showDisassembly);
         }
      }
   }

   return 0;
}
