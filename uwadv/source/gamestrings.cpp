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
/*! \file gamestrings.cpp

   \brief game strings loading implementation

*/

// needed includes
#include "common.hpp"
#include "gamestrings.hpp"


const unsigned int max_lifetime = 30;


// ua_gamestrings methods

void ua_gamestrings::init(ua_settings& settings)
{
   // we have one file
   allpakfiles.resize(1);

   // load file strings.pak
   allpakfiles[0].open(settings);
   allpakfiles[0].add_to_blockset(blockset);

   // add dummy vector for block 1
   std::vector<std::string> dummymap;
   allstrings.insert(
      std::make_pair<Uint16, std::vector<std::string> >(1,dummymap));

   // manually load block 1
   std::vector<std::string>& block1 = allstrings[1];
   allpakfiles[0].load_stringblock(1,block1);
}

void ua_gamestrings::add_pak_file(const char* filename)
{
   // extend .pak files array
   allpakfiles.resize(allpakfiles.size()+1);

   // open .pak file
   ua_strings_pak_file& pakfile = allpakfiles.back();
   pakfile.open(filename);
   pakfile.add_to_blockset(blockset);
}

/*! Adds strings.pak file to available files
    \note the passed rwops pointer is freed by the gamestrings class, so
    don't call SDL_RWclose() on it!
*/
void ua_gamestrings::add_pak_file(SDL_RWops* rwops)
{
   // extend .pak files array
   allpakfiles.resize(allpakfiles.size()+1);

   // open .pak file
   ua_strings_pak_file& pakfile = allpakfiles.back();
   pakfile.open(rwops);
   pakfile.add_to_blockset(blockset);
}

bool ua_gamestrings::is_avail(Uint16 block_id)
{
   unsigned int max = allpakfiles.size();
   for(unsigned int i=0; i<max; i++)
      if (allpakfiles[i].is_avail(block_id))
         return true; // found

   return false;
}

void ua_gamestrings::get_stringblock(Uint16 block_id,
   std::vector<std::string>& strblock)
{
   if (is_avail(block_id))
   {
      // load string block when not paged in
      if (allstrings.find(block_id) == allstrings.end())
         load_stringblock(block_id);

      strblock = allstrings[block_id];

      // decrease lifetime for other blocks, except for this one
      decrease_lifetimes(block_id);
   }
   else
      ua_trace("string block %04x cannot be found\n", block_id);
}

std::string ua_gamestrings::get_string(Uint16 block_id,
   unsigned int string_nr)
{
   std::string text;
   if (is_avail(block_id))
   {
      if (allstrings.find(block_id) == allstrings.end())
         load_stringblock(block_id);

      std::vector<std::string>& strblock = allstrings[block_id];
      if (string_nr < strblock.size())
      {
         text = strblock[string_nr];

         // decrease lifetime for other blocks, except for this one
         decrease_lifetimes(block_id);
      }
      else
         ua_trace("string %u in block %04x cannot be found\n", string_nr,
            block_id);
   }
   else
      ua_trace("string block %04x cannot be found\n", block_id);

   return text;
}

void ua_gamestrings::load_stringblock(Uint16 block_id)
{
   // search strings.pak file to use; start with last added
   unsigned int i=allpakfiles.size();
   do
   {
      i--;
      if (allpakfiles[i].is_avail(block_id))
      {
         // add dummy vector for block
         std::vector<std::string> dummymap;
         allstrings.insert(std::make_pair<Uint16, std::vector<std::string> >(
            block_id,dummymap));

         // load block
         std::vector<std::string>& block = allstrings[block_id];
         allpakfiles[0].load_stringblock(block_id,block);

         // add lifetime as well
         lifetimes[block_id] = max_lifetime;

         return;
      }
   } while (i>0);

   ua_trace("couldn't load string block %04x\n", block_id);
}

void ua_gamestrings::decrease_lifetimes(Uint16 except_for_block_id)
{
   // only decrease lifetimes when the excepted block is in lifetimes map
   std::map<Uint16, unsigned int>::iterator iter =
      lifetimes.find(except_for_block_id);
   if (iter == lifetimes.end())
      return;

   std::vector<Uint16> list_dump_ids;

   // age blocks except given one
   iter = lifetimes.begin();
   std::map<Uint16, unsigned int>::iterator stop = lifetimes.end();
   for(;iter != stop; iter++)
   {
      if (iter->first == except_for_block_id)
         iter->second = max_lifetime;
      else
      {
         if (--iter->second == 0)
         {
            // erase block
            ua_trace("garbage-collecing block %04x\n", iter->first);
            list_dump_ids.push_back(iter->first);
         }
      }
   }

   // remove blocks
   unsigned int max = list_dump_ids.size();
   for(unsigned int i=0; i<max; i++)
   {
      // remove from strings list
      allstrings.erase(list_dump_ids[i]);

      // and from lifetimes map
      iter = lifetimes.find(list_dump_ids[i]);
      lifetimes.erase(iter);
   }
}
