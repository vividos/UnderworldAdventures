/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002,2003,2004 Underworld Adventures Team

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
/*! \file stringsloader.cpp

   \brief game strings loading

*/

// needed includes
#include "common.hpp"
#include "import.hpp"
#include "settings.hpp"
#include "io_endian.hpp"


//! define this when you can be sure the strings.pak is correct;
//! speeds up loading a bit
#define HAVE_CORRECT_STRINGS_FILE


// ua_strings_pak_file methods

ua_strings_pak_file::ua_strings_pak_file()
:rwops_file(NULL)
{
}

ua_strings_pak_file::~ua_strings_pak_file()
{
   if (rwops_file != NULL)
   {
      SDL_RWclose(rwops_file);
      rwops_file = NULL;
   }
}

/*! Opens the strings.pak file in the data folder of the game. Files for uw1
    and uw2 have the same format.
    \param settings settings object
*/
void ua_strings_pak_file::open(ua_settings& settings)
{
   filename = settings.get_string(ua_setting_uw_path);
   filename += "data/strings.pak";

   open(filename.c_str());
}

/*! Opens a strings.pak file or a file that has the same format (e.g. created
    with the "strpak" tool).
    \param the_filename filename of the strings.pak file
*/
void ua_strings_pak_file::open(const char* the_filename)
{
   filename = the_filename;

   allnodes.clear();
   allblocks.clear();

   // open file
   SDL_RWops* rwops = SDL_RWFromFile(filename.c_str(),"rb");
   if (rwops==NULL)
      throw ua_exception("could not open file strings.pak");

   open(rwops);

   SDL_RWclose(rwops);
}

void ua_strings_pak_file::open(SDL_RWops* rwops)
{
   // not calling from open() above
   if (filename.empty())
      rwops_file = rwops;

   // number of huffman nodes
   Uint16 nodenum = SDL_RWread16(rwops);

   // read in node list
   allnodes.resize(nodenum);
   for(Uint16 k=0; k<nodenum; k++)
   {
      allnodes[k].symbol = SDL_RWread8(rwops);
      allnodes[k].parent = SDL_RWread8(rwops);
      allnodes[k].left   = SDL_RWread8(rwops);
      allnodes[k].right  = SDL_RWread8(rwops);
   }

   // number of string blocks
   Uint16 sblocks = SDL_RWread16(rwops);

   // read in all block infos
   for(int z=0; z<sblocks; z++)
   {
      Uint16 block_id = SDL_RWread16(rwops);
      Uint32 offset = SDL_RWread32(rwops);

      allblocks.insert(std::make_pair<Uint16, Uint32>(block_id, offset));
   }
}

/*! Checks if a block with given block id is available.
    \param block_id block id
    \return if block is available
*/
bool ua_strings_pak_file::is_avail(Uint16 block_id)
{
   return allblocks.find(block_id) != allblocks.end();
}

/*! Loads a single string block from file. Please check with is_avail() if the
    block is available.

    \param block_id block id of string block to load
    \param strblock strings array that gets filled
*/
void ua_strings_pak_file::load_stringblock(Uint16 block_id, std::vector<std::string>& strblock)
{
   std::map<Uint16, Uint32>::iterator iter = allblocks.find(block_id);
   if (iter == allblocks.end())
   {
      ua_trace("loading string block %04x failed\n",block_id);
      return;
   }

   Uint32 offset = allblocks[block_id];

   SDL_RWops* rwops = filename.empty() ? rwops_file :
      SDL_RWFromFile(filename.c_str(),"rb");
   if (rwops==NULL)
   {
      std::string text("could not open file ");
      text += filename;
      throw ua_exception(text.c_str());
   }

   // determine filesize
   SDL_RWseek(rwops,0,SEEK_END);
   long filesize = SDL_RWtell(rwops);
   SDL_RWseek(rwops,0,SEEK_SET);

   SDL_RWseek(rwops,offset,SEEK_SET);

   // number of strings
   Uint16 numstrings = SDL_RWread16(rwops);

   // all string offsets
   std::vector<Uint16> stroffsets;
   stroffsets.resize(numstrings);

   for(int j=0; j<numstrings; j++)
      stroffsets[j] = SDL_RWread16(rwops);

   Uint32 curoffset = offset + (numstrings+1)*sizeof(Uint16);
   unsigned int nodenum = allnodes.size();

   for(Uint16 n=0; n<numstrings; n++)
   {
      SDL_RWseek(rwops,curoffset+stroffsets[n],SEEK_SET);

      char c;
      std::string str;

      int bit=0;
      int raw=0;

      do
      {
         int node=nodenum-1; // starting node

         // huffman tree decode loop
         while (char(allnodes[node].left) != -1 && char(allnodes[node].left) != -1)
         {
            if (bit==0)
            {
               bit=8;
               raw=SDL_RWread8(rwops);

#ifndef HAVE_CORRECT_STRINGS_FILE
               if (SDL_RWtell(rwops)>=filesize)
               {
                  // premature end of file, should not happen
                  n=numstrings;
                  //block_id=sblocks;
                  break;
               }
#endif
            }

            // decide which node is next
            node = raw & 0x80 ? short(allnodes[node].right)
               : short(allnodes[node].left);

            raw<<=1;
            bit--;
         }
#ifndef HAVE_CORRECT_STRINGS_FILE
         if (SDL_RWtell(rwops)>=filesize)
            break;
#endif

         // have a new symbol
         c = allnodes[node].symbol;
         if (c!='|')
            str.append(1,c);

      } while (c!='|');

      strblock.push_back(str);
      str.erase();
   }

   if (!filename.empty())
      SDL_RWclose(rwops);

   // remove excess memory
   std::vector<std::string>(strblock).swap(strblock);
}
