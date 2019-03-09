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
/// \file ConvLoader.cpp
/// \brief conversation code and conv. globals loader
//
#include "Import.hpp"
#include "ConvLoader.hpp"
#include "ResourceManager.hpp"
#include "File.hpp"
#include "ArchiveFile.hpp"
#include "CodeVM.hpp"
#include "Exception.hpp"

/// When initial is set to true, the file to load only contains size entries,
/// and no actual data; all globals are initalized with 0 then.
/// \param folder the folder to use, either the "data" or one of the SaveN folders.
/// \param initial indicates if initial conv. globals should be loaded
void Import::LoadConvGlobals(Underworld::ConvGlobals& convGlobals,
   Base::ResourceManager& resourceManager, const char* folder, bool initial)
{
   std::string globalsName = folder;
   globalsName.append("/");
   globalsName.append(initial ? "babglobs.dat" : "bglobals.dat");

   Base::File file { resourceManager.GetUnderworldFile(Base::resourceGameUw, globalsName) };
   if (!file.IsOpen())
   {
      std::string text("error loading conv. globals file: ");
      text.append(globalsName);
      throw Base::Exception(text.c_str());
   }

   std::vector<std::vector<Uint16>>& allGlobals = convGlobals.GetAllGlobalsList();

   // read in all slot/size/[globals] infos
   while (file.Tell() < file.FileLength())
   {
      Uint16 slot = file.Read16();
      Uint16 size = file.Read16();

      std::vector<Uint16> globals;
      globals.resize(size, 0);

      if (!initial)
      {
         // read in globals
         for (Uint16 i = 0; i < size; i++)
            globals.push_back(file.Read16());
      }

      // put globals in allGlobals list
      if (slot > allGlobals.size())
         allGlobals.resize(slot);

      allGlobals.insert(allGlobals.begin() + slot, globals);
   }
}

/// Loads conversation code from given file, into the virtual machine.
/// It loads all imported symbols (functions and globals) and puts the opcodes
/// into the vm's code segment. The method CodeVM::Init() can then be
/// called after this method.
/// \param vm conversation code virtual machine
/// \param settings settings to use
/// \param resourceManager resource manager to load from
/// \param cnvArkFilename conversation filename, e.g. cnv.ark
/// \param conv convesation slot of code to load
bool Import::LoadConvCode(Conv::CodeVM& vm, Base::Settings& settings, Base::ResourceManager& resourceManager,
   const char* cnvArkFilename, Uint16 conversationSlot)
{
   Base::SDL_RWopsPtr rwops = resourceManager.GetUnderworldFile(Base::resourceGameUw, std::string("data/") + cnvArkFilename);
   if (rwops == NULL)
      throw Base::Exception("could not open conversation file");

   bool isUw2 = settings.GetGameType() == Base::gameUw2;
   Base::ArchiveFile arkFile(rwops, isUw2);

   if (conversationSlot >= arkFile.GetNumFiles())
      throw Base::Exception("invalid conversation!");

   if (!arkFile.IsAvailable(conversationSlot))
   {
      // no conversation available
      return false;
   }

   Base::File file = arkFile.GetFile(conversationSlot);

   Uint32 unknown1 = file.Read32(); // always 0x00000828
   UaAssert(unknown1 == 0x0828);

   Uint16 codeSize = file.Read16();
   Uint16 unknown2 = file.Read16(); // always 0x0000
   Uint16 unknown3 = file.Read16(); // always 0x0000
   unknown1; unknown2; unknown3;

   Uint16 stringBlock = file.Read16(); // string block to use
   vm.SetStringBlock(stringBlock);

   Uint16 reservedGlobals = file.Read16(); // number of stack words reserved for globals
   vm.SetReservedGlobals(reservedGlobals);

   // load imported functions
   LoadConvCodeImportedFunctions(vm, file);

   // load code
   std::vector<Uint16>& code = vm.GetCodeSegment();

   code.resize(codeSize, 0);
   for (Uint16 i = 0; i < codeSize; i++)
      code[i] = file.Read16();

   file.Close();

   // fix for Marrowsuck conversation; it has a wrong opcode on 0x076e
   if (conversationSlot == 6)
   {
      code[0x076e] = 0x0016; //op_PUSHI;
   }

   // fix for Sseetharee conversation; call to function at 0xffff
   if (conversationSlot == 15)
   {
      // just take another function that may seem reasonable
      code[0x0584] = 0x0666;
   }

   // fix for Judy conversation; argument to random is 2, but switch statement
   // checks for 3 values; the 3rd answer will never be given
   if (conversationSlot == 23)
   {
      // PUSHI #3
      code[0x04fd] = 3;
   }

   vm.SetConversationSlot(conversationSlot);

   return true;
}

void Import::LoadConvCodeImportedFunctions(Conv::CodeVM& vm, Base::File& file)
{
   std::map<Uint16, Conv::ImportedItem>& importedFunctions = vm.GetImportedFunctions();
   std::map<Uint16, Conv::ImportedItem>& importedGlobals = vm.GetImportedGlobals();

   importedFunctions.clear();
   importedGlobals.clear();

   // read number of imported numFunctions
   Uint16 numFunctions = file.Read16();

   for (int functionIndex = 0; functionIndex < numFunctions; functionIndex++)
   {
      // length of function name
      Uint16 functionNameLength = file.Read16();
      if (functionNameLength > 255) functionNameLength = 255;

      char functionName[256];
      file.ReadBuffer(reinterpret_cast<Uint8*>(functionName), functionNameLength);
      functionName[functionNameLength] = 0;

      Uint16 functionId = file.Read16();
      Uint16 functionUnknown1 = file.Read16(); // always seems to be 1
      Uint16 importType = file.Read16();
      Uint16 returnType = file.Read16();
      functionUnknown1;

      // fill imported item struct
      Conv::ImportedItem importedItem;

      // determine return type
      switch (returnType)
      {
      case 0x0000: importedItem.datatype = Conv::dataTypeVoid; break;
      case 0x0129: importedItem.datatype = Conv::dataTypeInt; break;
      case 0x012b: importedItem.datatype = Conv::dataTypeString; break;
      default:
         throw Base::Exception("unknown return type in conv imports list");
      }

      importedItem.name = functionName;

      // store imported item in appropriate list
      switch (importType)
      {
      case 0x0111:
         // imported function
         importedFunctions[functionId] = importedItem;
         break;

      case 0x010F:
         // imported global
         importedGlobals[functionId] = importedItem;
         break;

      default:
         throw Base::Exception("unknown import type in conv imports list");
         break;
      }
   }
}
