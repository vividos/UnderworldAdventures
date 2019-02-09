//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2005,2006,2019 Michael Fink
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
/// \file import.hpp
/// \brief game data import
//
#pragma once

#include "base.hpp"
#include "file.hpp"
#include <vector>
#include <set>
#include <map>

namespace Base
{
   class ResourceManager;
   class Settings;
}

namespace Underworld
{
   class ObjectProperties;
   class LevelList;
   class ObjectList;
   class Tilemap;
   class Player;
}

/// import classes
namespace Import
{
   /// imports all object properties
   void ImportProperties(Base::ResourceManager& resourceManager,
      Underworld::ObjectProperties& properties);

#if 0
   // forward references
   class ua_conv_globals;
   class CodeVM;


   /// imports common to uw1 and uw2
   class ua_uw_import
   {
   public:
      // underworld objects loading

      /// loads underworld objects from folder
      void load_underworld(Underworld& underw, Base::Settings& settings,
         const char* folder, bool initial);

      /// loads conversation globals
      void load_conv_globals(ua_conv_globals& globals, Base::Settings& settings,
         const char* folder, bool initial);

      /// loads conversation code into virtual machine
      bool load_conv_code(CodeVM& vm, const char* cnvfile, Uint16 conv);

   protected:
      /// loads imported functions list
      void load_conv_code_imported_funcs(CodeVM& vm, FILE *fd);
   };

   // strings loading


   /// strings.pak wrapper class
   class ua_strings_pak_file
   {
   public:
      /// ctor
      ua_strings_pak_file();
      /// dtor
      ~ua_strings_pak_file();

      /// opens strings.pak file in data folder of current game
      void open(Base::Settings& settings);

      /// opens a strings.pak file with given name
      void open(const char* filename);

      /// opens from rwops structure; file is kept open permanently
      void open(SDL_RWops* rwops);

      /// returns if a given block id is available)
      bool is_avail(Uint16 block_id);

      /// loads a single string block
      void load_stringblock(Uint16 block_id, std::vector<std::string>& strblock);

      /// adds all blocks in the file to the block
      void add_to_blockset(std::set<Uint16>& blockset);

   public:
      /// filename of .pak file
      std::string filename;

      /// rwops structure when we're loading from an rwops file
      SDL_RWops* rwops_file;

      /// a vector with all huffman nodes for the given .pak file
      std::vector<ua_strings_pak_huff_node> allnodes;

      /// a map of all blocks available in the file
      std::map<Uint16, Uint32> allblocks;
   };
#endif

   // inline functions

   /// retrieves bits from given value
   inline Uint8 GetBits(Uint8 uiValue, unsigned int start, unsigned int count)
   {
      UaAssert(start < 8 && start + count <= 8);
      return static_cast<Uint8>(uiValue >> start) & static_cast<Uint8>((1 << count) - 1);
   }

   /// retrieves bits from given value
   inline Uint16 GetBits(Uint16 uiValue, unsigned int start, unsigned int count)
   {
      UaAssert(start < 16 && start + count <= 16);
      return static_cast<Uint16>(uiValue >> start) & static_cast<Uint16>((1 << count) - 1);
   }

   /// retrieves bits from given value
   inline Uint32 GetBits(Uint32 uiValue, unsigned int start, unsigned int count)
   {
      UaAssert(start < 32 && start + count <= 32);
      return (uiValue >> start) & ((1 << count) - 1);
   }

} // namespace Import
