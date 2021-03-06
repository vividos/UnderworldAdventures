//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2019 Underworld Adventures Team
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
// game strings loading
//
#include "hacking.h"

/// huffman node structure
typedef struct ua_huff_node
{
   int symbol;
   int parent;
   int left;
   int right;
} ua_huff_node;

typedef struct ua_block_info
{
   Uint16 block_id;
   Uint32 offset;
} ua_block_info;

void ua_gamestrings::load(const char *filename)
{
   FILE* fd = fopen(filename, "rb");
   if (fd == NULL)
   {
      printf("could not open file strings.pak");
      return;
   }

   Uint16 nodenum = fread16(fd);

   // read in node list
   std::vector<ua_huff_node> allnodes;
   allnodes.resize(nodenum);
   for (Uint16 k = 0; k < nodenum; k++)
   {
      allnodes[k].symbol = fgetc(fd);
      allnodes[k].parent = fgetc(fd);
      allnodes[k].left = fgetc(fd);
      allnodes[k].right = fgetc(fd);
   }

   // number of string blocks
   Uint16 sblocks = fread16(fd);

   // read in all block infos
   std::vector<ua_block_info> allblocks;
   allblocks.resize(sblocks);
   for (int z = 0; z < sblocks; z++)
   {
      allblocks[z].block_id = fread16(fd);
      allblocks[z].offset = fread32(fd);
   }

   //printf("game strings: %u blocks\n",sblocks);

   for (Uint16 i = 0; i < sblocks; i++)
   {
      std::vector<std::string> allblockstrings;

      fseek(fd, allblocks[i].offset, SEEK_SET);

      // number of strings
      Uint16 numstrings = fread16(fd);

      //printf("\nblock %u: %u strings\n",i,numstrings);

      // all string offsets
      Uint16 *stroffsets = new Uint16[numstrings];
      for (int j = 0; j < numstrings; j++)
         stroffsets[j] = fread16(fd);

      Uint32 curoffset = allblocks[i].offset + (numstrings + 1) * sizeof(Uint16);

      for (Uint16 n = 0; n < numstrings; n++)
      {
         fseek(fd, curoffset + stroffsets[n], SEEK_SET);

         char c;
         std::string str;

         int bit = 0;
         int raw = 0;

         do
         {
            int node = nodenum - 1; // starting node

            // huffman tree decode loop
            while (char(allnodes[node].left) != -1 && char(allnodes[node].left) != -1)
            {
               if (bit == 0)
               {
                  bit = 8;
                  raw = fgetc(fd);
                  if (feof(fd))
                  {
                     // premature end of file, should not happen
                     n = numstrings;
                     i = sblocks;
                     break;
                  }
               }

               // decide which node is next
               node = raw & 0x80 ? short(allnodes[node].right)
                  : short(allnodes[node].left);

               raw <<= 1;
               bit--;
            }

            if (feof(fd)) break;

            // have a new symbol
            c = allnodes[node].symbol;
            if (c != '|')
               str.append(1, c);

         } while (c != '|');

         //printf("%03u: %s\n",n,str.c_str());

         allblockstrings.push_back(str);
         str.erase();
      }
      delete[] stroffsets;

      // insert string block
      allstrings[allblocks[i].block_id] = allblockstrings;
   }

   fclose(fd);
}

std::string ua_gamestrings::get_string(unsigned int block, unsigned int string_nr)
{
   std::string res;

   // try to find string block
   std::map<int, std::vector<std::string> >::iterator iter =
      allstrings.find(block);

   if (iter != allstrings.end())
   {
      // try to find string in vector
      std::vector<std::string> &stringlist = iter->second;
      if (string_nr < stringlist.size())
         res = stringlist[string_nr];
   }

   return res;
}
