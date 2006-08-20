/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2003,2004,2005,2006 Michael Fink

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
/*! \file import.hpp

   \brief game data import

*/
/*! \defgroup import Import

   The import classes do the importing of game data from uw1 and uw2 games.
   They are built as a separate module so that importing game files can be
   taken out of the game if it's desired.

*/

// include guard
#ifndef uwadv_import_import_hpp_
#define uwadv_import_import_hpp_

// needed includes
#include "base.hpp"
#include "file.hpp"
#include <vector>

// forward references

namespace Base
{
   class ResourceManager;
}

namespace Underworld
{
   class ObjectProperties;
   class LevelList;
   class ObjectList;
   class Tilemap;
   class Player;
}

//! import classes \ingroup import
namespace Import
{

//! imports all object properties
void ImportProperties(Base::ResourceManager& resourceManager,
   Underworld::ObjectProperties& properties);

//! stores tilemap object list start links; used while loading
class TileStartLinkList
{
public:
   //! ctor
   TileStartLinkList(){ m_vecLinks.resize(64*64); }

   //! returns link start for given tile
   Uint16 GetLinkStart(Uint16 xpos, Uint16 ypos) const { return m_vecLinks[ypos*64+xpos]; }

   //! sets link start for given tile
   void SetLinkStart(Uint16 xpos, Uint16 ypos, Uint16 uiLink){ m_vecLinks[ypos*64+xpos] = uiLink; }

private:
   //! array with links
   std::vector<Uint16> m_vecLinks;
};


//! imports levels
class LevelImporter: public Base::NonCopyable
{
public:
   //! ctor
   LevelImporter(Base::ResourceManager& resourceManager):m_resourceManager(resourceManager){}

   //! loads uw_demo level into level list
   void LoadUwDemoLevel(Underworld::LevelList& levelList);

   //! loads uw1 levels into level list
   void LoadUw1Levels(Underworld::LevelList& levelList);

   //! loads uw2 levels into level list
   void LoadUw2Levels(Underworld::LevelList& levelList);

private:
   //! common uw1 and uw2 level loading
   void LoadUwLevels(Underworld::LevelList& levelList, bool bUw2Mode,
      unsigned int uiNumLevels, unsigned int uiTexMapOffset);

   //! loads texture mapping from current file
   void LoadTextureMapping(std::vector<Uint16>& vecTextureMapping, bool bUw2Mode);

   //! loads tilemap from current file
   void LoadTilemap(Underworld::Tilemap& tilemap, std::vector<Uint16>& vecTextureMapping,
      TileStartLinkList& tileStartLinkList, bool bUw2Mode);

   //! loads object list from current file
   void LoadObjectList(Underworld::ObjectList& objectList, const TileStartLinkList& tileStartLinkList,
      std::vector<Uint16>& vecTextureMapping);

private:
   //! resource manager
   Base::ResourceManager& m_resourceManager;

   //! current file
   Base::File m_file;
};


//! imports levels
class PlayerImporter: public Base::NonCopyable
{
public:
   //! ctor
   PlayerImporter(Base::ResourceManager& resourceManager):m_resourceManager(resourceManager){}

   //! loads player and inventory
   void LoadPlayer(Underworld::Player& player, const std::string& strPath);

private:
   //! resource manager
   Base::ResourceManager& m_resourceManager;
};


#if 0
// forward references
class ua_player;
class ua_conv_globals;
class ua_conv_code_vm;



//! imports common to uw1 and uw2
class ua_uw_import
{
public:
   // underworld objects loading

   //! loads underworld objects from folder
   void load_underworld(ua_underworld& underw, ua_settings& settings,
      const char* folder, bool initial);

   //! loads conversation globals
   void load_conv_globals(ua_conv_globals& globals, ua_settings& settings,
      const char* folder, bool initial);

   //! loads conversation code into virtual machine
   bool load_conv_code(ua_conv_code_vm& vm, const char* cnvfile, Uint16 conv);

protected:
   //! loads imported functions list
   void load_conv_code_imported_funcs(ua_conv_code_vm& vm, FILE *fd);
};

// strings loading

//! strings.pak huffman node structure
struct ua_strings_pak_huff_node
{
  int symbol; //!< character symbol in that node
  int parent; //!< parent node
  int left;   //!< left node (-1 when no node)
  int right;  //!< right node
};

//! strings.pak wrapper class
class ua_strings_pak_file
{
public:
   //! ctor
   ua_strings_pak_file();
   //! dtor
   ~ua_strings_pak_file();

   //! opens strings.pak file in data folder of current game
   void open(ua_settings& settings);

   //! opens a strings.pak file with given name
   void open(const char* filename);

   //! opens from rwops structure; file is kept open permanently
   void open(SDL_RWops* rwops);

   //! returns if a given block id is available)
   bool is_avail(Uint16 block_id);

   //! loads a single string block
   void load_stringblock(Uint16 block_id, std::vector<std::string>& strblock);

   //! adds all blocks in the file to the block
   void add_to_blockset(std::set<Uint16>& blockset);

public:
   //! filename of .pak file
   std::string filename;

   //! rwops structure when we're loading from an rwops file
   SDL_RWops* rwops_file;

   //! a vector with all huffman nodes for the given .pak file
   std::vector<ua_strings_pak_huff_node> allnodes;

   //! a map of all blocks available in the file
   std::map<Uint16, Uint32> allblocks;
};
#endif

// inline functions

//! retrieves bits from given value
inline Uint8 GetBits(Uint8 uiValue, unsigned int uiStart, unsigned int uiCount)
{
   UaAssert(uiStart < 8 && uiStart + uiCount <= 8);
   return static_cast<Uint8>(uiValue >> uiStart) & static_cast<Uint8>((1 << uiCount)-1);
}

//! retrieves bits from given value
inline Uint16 GetBits(Uint16 uiValue, unsigned int uiStart, unsigned int uiCount)
{
   UaAssert(uiStart < 16 && uiStart + uiCount <= 16);
   return static_cast<Uint16>(uiValue >> uiStart) & static_cast<Uint16>((1 << uiCount)-1);
}

//! retrieves bits from given value
inline Uint32 GetBits(Uint32 uiValue, unsigned int uiStart, unsigned int uiCount)
{
   UaAssert(uiStart < 32 && uiStart + uiCount <= 32);
   return (uiValue >> uiStart) & ((1 << uiCount)-1);
}

} // namespace Import

#endif
