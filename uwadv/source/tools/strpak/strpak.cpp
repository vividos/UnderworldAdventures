//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2002,2003,2004,2005,2019 Underworld Adventures Team
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
/// \file strpak.cpp
/// string unpack and pack tool
//
// \todo move comment
// huffman tree building was done using this tutorial:
// http://www.crs4.it/~luigi/MPEG/huffman_tutorial.html
//
//
#include "common.hpp"
#include "GameStrings.hpp"
#include "GameStringsImporter.hpp"
#include "File.hpp"
#include "TextFile.hpp"
#include <sstream>
#include <iomanip>

/// huffman extended node structure
struct HuffmanNodeExt
{
   int symbol;
   int parent;
   int left;
   int right;
   unsigned int freq;
};

/// unpacks a string pack file
void strpak_unpack_strings(const char *infile, const char *outputFilename)
{
   // load game strings
   GameStrings gs;
   try
   {
      printf("loading game strings from file %s ...\n", infile);
      Import::GameStringsImporter importer(gs);
      importer.LoadStringsPakFile(infile);
   }
   catch (...)
   {
      printf("error while loading game strings!\n");
      return;
   }

   // open output file
   Base::TextFile out(outputFilename, Base::modeWrite);
   if (!out.IsOpen())
   {
      printf("could not open output file!\n");
      return;
   }

   const GameStrings& cgs = gs;
   const std::set<Uint16>& stringBlocks = cgs.GetStringBlockSet();

   // print out some more infos
   {
      Base::File test(infile, Base::modeRead);
      Uint16 nodes = 0;
      if (test.IsOpen())
         nodes = test.Read16();

      printf("%u huffman tree nodes, %zu string blocks.\n",
         nodes,
         cgs.GetStringBlockSet().size());
   }

   printf("writing output file %s ...\n", outputFilename);

   std::stringstream bufferHeader;
   bufferHeader << infile << ": " << stringBlocks.size() << " string blocks.";
   out.WriteLine(bufferHeader.str().c_str());

   // dump all blocks
   std::set<Uint16>::iterator iter, stop;
   iter = stringBlocks.begin(); stop = stringBlocks.end();

   for (; iter != stop; iter++)
   {
      Uint16 blockId = *iter;
      std::vector<std::string> stringList = gs.GetStringBlock(blockId);

      out.WriteLine("");

      std::stringstream bufferBlock;
      bufferBlock << "block: " << std::hex << std::setfill('0') << std::setw(4) << blockId << "; " << stringList.size() << " strings.";
      out.WriteLine(bufferBlock.str().c_str());

      // print all strings in list
      size_t max = stringList.size();
      for (size_t index = 0; index < max; index++)
      {
         std::string line(stringList[index]);

         // replace newlines with string "\n"
         std::string::size_type pos;
         while ((pos = line.find('\n')) != std::string::npos)
            line.replace(pos, 1, "\\n");

         std::stringstream bufferLine;
         bufferLine << index << ": " << line;
         out.WriteLine(bufferLine.str().c_str());
      }
   }

   printf("done!\n");
}

/// repacks a string pack file
void strpak_pack_strings(const char* infile, const char* outputFilename, const char* nodeFilename = NULL)
{
   // open input file
   FILE* in = fopen(infile, "r");
   if (in == NULL)
   {
      printf("could not open input file!\n");
      return;
   }

   printf("loading raw game strings from file %s ...\n", infile);

   // string storage structs
   std::map<int, std::vector<std::string> > allStrings;
   std::vector<std::string> block;
   int currentBlock = -1;

   // initialize character frequency info
   unsigned int char_freq[256];
   { for (unsigned int i = 0; i < 256; char_freq[i++] = 0); }

   // read over first line
   char buffer[2048];
   fgets(buffer, 2048, in);

   // parse input file
   while (!feof(in))
   {
      // read a line
      fgets(buffer, 2048, in);
      if (feof(in)) break;

      // remove newline
      size_t len = strlen(buffer);
      if (buffer[len - 1] == '\n')
         buffer[len - 1] = 0;

      // skip empty lines
      if (strlen(buffer) == 0)
         continue;

      if (strncmp(buffer, "block: ", 7) == 0)
      {
         if (currentBlock != -1)
         {
            // store old block in allStrings
            allStrings[currentBlock] = block;
         }

         // start of a new block
         currentBlock = strtol(buffer + 7, NULL, 16);
         block.clear();
         continue;
      }

      // must be a string line
      char *pos = strchr(buffer, ':');
      if (pos == NULL)
      {
         printf("line %zu of block contained no ':'\n", block.size());
         continue; // should not happen
      }
      pos += 2; // move forward to actual string

      // replace "\n" with real newlines
      char *pos2;
      while ((pos2 = strstr(pos, "\\n")) != NULL)
      {
         pos2[0] = '\n';
         memmove(pos2 + 1, pos2 + 2, strlen(pos2 + 1));
      }

      // calculate char frequencies for that string
      size_t max = strlen(pos);
      for (size_t index = 0; index < max; index++)
         char_freq[static_cast<Uint8>(pos[index])]++;

      char_freq[static_cast<Uint8>('|')]++;

      // add string to block
      std::string thestring(pos);
      thestring.append("|"); // end marker
      block.push_back(thestring);
   }

   fclose(in);

   // store last block, too
   allStrings[currentBlock] = block;

   // now that we have all the strings, build the huffman tree
   printf("building huffman tree ...\n");

   // reverse char lookup table
   unsigned int char_lookup[256];
   {
      for (size_t i = 0; i < 256;
      char_lookup[i++] = 0);
   }

   // build up list with all leaf nodes
   // use all characters that have frequencies above 0
   std::vector<HuffmanNodeExt> huffnodes;
   {
      unsigned int j = 0;
      for (unsigned int i = 0; i < 256; i++)
         if (char_freq[i] > 0)
         {
            HuffmanNodeExt node;
            node.symbol = i;
            node.parent = 0; // we don't know yet
            node.left = -1;  // no children
            node.right = -1;
            node.freq = char_freq[i];
            huffnodes.push_back(node);

            char_lookup[i] = j++;
         }
   }

   size_t validChars = huffnodes.size();

   // build huffman tree
   while (true)
   {
      size_t i, max;

      int node1 = -1, node2 = -1;
      unsigned int freq1 = 0, freq2 = 0;

      // find two nodes with the least frequencies
      max = huffnodes.size();
      for (i = 0; i < max; i++)
         if (huffnodes[i].freq != 0 && (freq1 == 0 || huffnodes[i].freq < freq1))
         {
            node1 = static_cast<int>(i);
            freq1 = huffnodes[i].freq;
         }

      if (node1 == -1)
      {
         printf("didn't find any node with frequency != 0!\n");
         break; // didn't find any node; should never happen, though
      }

      huffnodes[node1].freq = 0;

      // search for second node
      for (i = 0; i < max; i++)
         if (huffnodes[i].freq != 0 && (freq2 == 0 || huffnodes[i].freq < freq2))
         {
            node2 = static_cast<int>(i);
            freq2 = huffnodes[i].freq;
         }

      if (node2 == -1)
      {
         // only found one node, the root node
         huffnodes[node1].parent = 0xff;
         break; // we're finished
      }

      huffnodes[node2].freq = 0;

      // we have a new node
      {
         HuffmanNodeExt node;
         node.symbol = 0;
         node.parent = 0; // we don't know yet
         node.left = node1;
         node.right = node2;
         node.freq = freq1 + freq2;
         huffnodes.push_back(node);
      }

      int last = static_cast<int>(huffnodes.size() - 1);
      huffnodes[node1].parent = last;
      huffnodes[node2].parent = last;
   }

   // load nodes from template node file, when available
   if (nodeFilename != NULL)
   {
      printf("loading external huffman node list from file %s ...\n", nodeFilename);

      huffnodes.clear();
      { for (int i = 0; i < 256; char_lookup[i++] = 0); }

      Base::File nodeFile(nodeFilename, Base::modeRead);

      // number of nodes
      Uint16 nodenum = nodeFile.Read16();
      huffnodes.resize(nodenum);

      for (Uint16 k = 0; k < nodenum; k++)
      {
         huffnodes[k].symbol = nodeFile.Read8();
         huffnodes[k].parent = nodeFile.Read8();
         huffnodes[k].left = nodeFile.Read8();
         huffnodes[k].right = nodeFile.Read8();
      }

      // build lookup table
      for (Uint16 n = 0; n <= nodenum / 2; n++)
      {
         char_lookup[static_cast<Uint8>(huffnodes[n].symbol)] = n;
      }

      /*
      // dump external huffman tree list
      FILE *dump = fopen("uw-huffnodes-list.txt","w");
      for(Uint16 z=0; z<nodenum; z++)
         fprintf(dump,"%04x: symbol=%02x parent=%02x left=%02x right=%02x\n",
            z,huffnodes[z].symbol,huffnodes[z].parent,huffnodes[z].left,huffnodes[z].right);
      fclose(dump);
      */
   }


   // for a node list, we need at most 0x100 huffnodes
   if (huffnodes.size() > 0x100)
   {
      printf("error! huffnodes limit of 256 nodes exceeded! "
         "please only use ASCII charset!\n");
      return;
   }

   printf("huffman tree: %zu characters, %zu nodes\n",
      validChars, huffnodes.size() + 1);

   // write the output file
   Base::File outputFile(outputFilename, Base::modeWrite);
   if (!outputFile.IsOpen())
   {
      printf("could not open output file!\n");
      return;
   }

   printf("writing output file %s ...\nblocks: ", outputFilename);

   // write huffnode table
   {
      Uint16 max = static_cast<Uint16>(huffnodes.size());
      outputFile.Write16(max);

      for (Uint16 i = 0; i < max; i++)
      {
         const HuffmanNodeExt& currentNode = huffnodes[i];
         outputFile.Write8(currentNode.symbol);
         outputFile.Write8(currentNode.parent);
         outputFile.Write8(currentNode.left);
         outputFile.Write8(currentNode.right);
      }

      Uint16 numBlocks = static_cast<Uint16>(allStrings.size());
      outputFile.Write16(numBlocks);

      std::map<int, std::vector<std::string> >::iterator iter, stop;
      iter = allStrings.begin(); stop = allStrings.end();

      // init block offsets list
      std::vector<Uint32> blockOffsets;

      // remember offset of block directory
      long blockDirPos = outputFile.Tell();

      for (; iter != stop; iter++)
      {
         outputFile.Write16(static_cast<Uint16>(iter->first));
         outputFile.Write16(0); // we don't know yet
      }

      // write blocks
      iter = allStrings.begin();
      for (; iter != stop; iter++)
      {
         const std::vector<std::string> &stringList = (*iter).second;

         printf(".");

         // remember block offset
         blockOffsets.push_back(static_cast<Uint32>(outputFile.Tell()));

         // write block header
         Uint16 numStrings = static_cast<Uint16>(stringList.size());
         outputFile.Write16(numStrings);

         // remember start of string offsets
         long stringOffsetsPos = outputFile.Tell();

         for (int k = 0; k < numStrings; k++)
            outputFile.Write16(0); // relative offsets to strings data

         // all string offsets for this block
         std::vector<Uint16> stringOffsets;

         // used for calculation of relative string offsets
         long relativeStringOffsets = outputFile.Tell();

         // write every string
         for (Uint16 i = 0; i < numStrings; i++)
         {
            // remember relative offset
            stringOffsets.push_back(static_cast<Uint16>(outputFile.Tell() - relativeStringOffsets));

            unsigned int bits = 0;
            Uint16 raw = 0;

            // encode every character using the lookup table and the
            // huffman tree
            const char *str = stringList[i].c_str();
            size_t len = strlen(str);
            for (size_t n = 0; n < len; n++)
            {
               Uint8 c = static_cast<Uint8>(str[n]);
               unsigned int pos = char_lookup[c], pos2;

               // all bits to encode the char
               std::vector<bool> allbits;

               do
               {
                  // check which way 'up' the tree we use
                  pos2 = huffnodes[pos].parent;
                  if (unsigned(huffnodes[pos2].left) == pos)
                     allbits.push_back(false); // we used the left path
                  else
                     allbits.push_back(true); // we used the right path

                  pos = pos2; // parent is new node

               } while (huffnodes[pos].parent != 0xff);

               // now we have all bits for this char, in the reverse order
               // put them to bytes and write them
               size_t maxbits = allbits.size();
               for (size_t p = maxbits; p > 0; p--)
               {
                  bool bit = allbits[p - 1];
                  if (bit)
                     raw |= 0x80;
                  raw <<= 1;
                  bits++;

                  if (bits == 8)
                  {
                     outputFile.Write8((raw >> 8) & 0xff);
                     raw = 0;
                     bits = 0;
                  }
               }
            }
            // end of string

            // write all pending bits
            if (bits > 0)
            {
               raw <<= (8 - bits);
               outputFile.Write8((raw >> 8) & 0xff);
            }
         }

         // remember current end of file
         long cur_pos = outputFile.Tell();

         // now write all string offsets
         outputFile.Seek(stringOffsetsPos, Base::seekBegin);

         for (int m = 0; m < numStrings; m++)
            outputFile.Write16(stringOffsets[m]);

         // go back to end of file
         outputFile.Seek(cur_pos, Base::seekBegin);
      }

      // now write all block offsets
      outputFile.Seek(blockDirPos, Base::seekBegin);

      size_t numOffsets = blockOffsets.size();
      for (size_t offsetIndex = 0; offsetIndex < numOffsets; offsetIndex++)
      {
         outputFile.Seek(2, Base::seekBegin);
         outputFile.Write32(blockOffsets[offsetIndex]);
      }
   }

   printf("\ndone.\n");
}

/// main function
int main(int argc, char* argv[])
{
   printf("strpak - ultima underworld game strings pack and unpack tool\n\n");

   if (argc < 4)
   {
      printf("syntax: strpak <command> <input-file> <output-file>\n"
         "   command can either be \"pack\" or \"unpack\".\n");
      printf("example: strpak unpack strings.pak uw-strings.txt\n"
         "         strpak pack uw-strings.txt strings2.pak\n\n");
      return 1;
   }

   if (strcmp("unpack", argv[1]) == 0)
   {
      strpak_unpack_strings(argv[2], argv[3]);
   }
   else
      if (strcmp("pack", argv[1]) == 0)
      {
         strpak_pack_strings(argv[2], argv[3]);
      }
      else
         printf("unknown command \"%s\"\n", argv[1]);

   return 0;
}
