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
/*! \file stringsloader.cpp

   \brief game strings loading

*/

// needed includes
#include "common.hpp"
#include "gamestrings.hpp"
#include "io_endian.hpp"


#define HAVE_CORRECT_STRINGS_FILE


// structs

//! huffman node structure
typedef struct ua_huff_node
{
  int symbol;
  int parent;
  int left;
  int right;
} ua_huff_node;

//! strings block info
typedef struct ua_block_info
{
   Uint16 block_id;
   Uint32 offset;
} ua_block_info;


// ua_gamestrings methods

void ua_gamestrings::load(ua_settings& settings) throw(ua_exception)
{
   std::string filename(settings.get_string(ua_setting_uw_path));
   filename.append("data/strings.pak");

   load(filename.c_str());
}

void ua_gamestrings::load(const char* filename) throw(ua_exception)
{
   SDL_RWops* rwops = SDL_RWFromFile(filename,"rb");
   if (rwops==NULL)
      throw ua_exception("could not open file strings.pak");

   load(rwops);

   SDL_RWclose(rwops);
}

void ua_gamestrings::load(SDL_RWops *rwops)
{
   // determine filesize
   SDL_RWseek(rwops,0,SEEK_END);
   long filesize = SDL_RWtell(rwops);
   SDL_RWseek(rwops,0,SEEK_SET);

   // number of huffman nodes
   Uint16 nodenum = SDL_RWread16(rwops);

   // read in node list
   std::vector<ua_huff_node> allnodes;
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
   std::vector<ua_block_info> allblocks;
   allblocks.resize(sblocks);
   for(int z=0; z<sblocks; z++)
   {
      allblocks[z].block_id = SDL_RWread16(rwops);
      allblocks[z].offset = SDL_RWread32(rwops);
   }

   for(Uint16 i=0; i<sblocks; i++)
   {
      std::vector<std::string> allblockstrings;

      SDL_RWseek(rwops,allblocks[i].offset,SEEK_SET);

      // number of strings
      Uint16 numstrings = SDL_RWread16(rwops);

      // all string offsets
      std::vector<Uint16> stroffsets;
      stroffsets.resize(numstrings);

      for(int j=0; j<numstrings; j++)
         stroffsets[j] = SDL_RWread16(rwops);

      Uint32 curoffset = allblocks[i].offset + (numstrings+1)*sizeof(Uint16);

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
                     i=sblocks;
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

         allblockstrings.push_back(str);
         str.erase();
      }

      // check if string block already exists
      if (allstrings.find(allblocks[i].block_id)!=allstrings.end())
         allstrings.erase(allblocks[i].block_id);

      // insert string block
      allstrings.insert(
         std::make_pair<int,std::vector<std::string> >(allblocks[i].block_id,allblockstrings));
   }
}
