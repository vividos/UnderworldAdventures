/*
   Underworld Adventures - an Ultima Underworld hacking project
   Copyright (c) 2002 Michael Fink

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
/*! \file strpak.cpp

   string unpack and pack tool

   huffman tree building was done using this tutorial:
   http://www.crs4.it/~luigi/MPEG/huffman_tutorial.html

*/

// needed includes
#include "common.hpp"
#include "gamestrings.hpp"
#include "resource/fread_endian.hpp"


// structs

//! huffman node structure
typedef struct ua_huff_node
{
  int symbol;
  int parent;
  int left;
  int right;
  unsigned int freq;
} ua_huff_node;


// global functions

//! unpacks a string pack file
void ua_strpak_unpack_strings(const char *infile,const char *outfile)
{
   // load game strings
   ua_gamestrings gs;
   try
   {
      printf("loading game strings from file %s ...\n",infile);
      gs.load(infile);
   }
   catch(...)
   {
      printf("error while loading game strings!\n");
      return;
   }

   // open output file
   FILE *out = fopen(outfile,"w");
   if (out==NULL)
   {
      printf("could not open output file!\n");
      return;
   }

   printf("writing output file %s ...\n",outfile);

   // get "all strings" map
   std::map<int,std::vector<std::string> > &allstrings = gs.get_allstrings();

   fprintf(out,"%s: %u string blocks.\n",infile,allstrings.size());

   // dump all blocks
   std::map<int,std::vector<std::string> >::iterator iter,stop;
   iter = allstrings.begin(); stop = allstrings.end();

   for(;iter!=stop; iter++)
   {
      const std::pair<int,std::vector<std::string> > &block = *iter;
      const std::vector<std::string> &stringlist = block.second;

      fprintf(out,"\nblock: %u; %u strings.\n",block.first,stringlist.size());

      // print all strings in list
      unsigned int i,max=stringlist.size();
      for(i=0; i<max; i++)
      {
         std::string line(stringlist[i]);

         // replace newlines with string "\n"
         std::string::size_type pos;
         while( (pos=line.find('\n')) != std::string::npos)
            line.replace(pos,1,"\\n");

         fprintf(out,"%u: %s\n",i,line.c_str());
      }
   }

   fclose(out);

   printf("done!\n");
}

//! repacks a string pack file
void ua_strpak_pack_strings(const char *infile,const char *outfile)
{
   // open input file
   FILE *in = fopen(infile,"r");
   if (in==NULL)
   {
      printf("could not open input file!\n");
      return;
   }

   printf("loading raw game strings from file %s ...\n",infile);

   // string storage structs
   std::map<int,std::vector<std::string> > allstrings;
   std::vector<std::string> block;
   int curblock=-1;

   // initialize character frequency info
   std::vector<unsigned int> char_freq;
   char_freq.resize(256,0);

   // read over first line
   char buffer[2048];
   fgets(buffer,2048,in);

   // parse input file
   while(!feof(in))
   {
      // read a line
      fgets(buffer,2048,in);
      if (feof(in)) break;

      // remove newline
      unsigned int len = strlen(buffer);
      if (buffer[len-1]=='\n') buffer[len-1]=0;

      // skip empty lines
      if (strlen(buffer)==0) continue;

      if (strncmp(buffer,"block: ",7)==0)
      {
         if (curblock!=-1)
         {
            // store old block in allstrings
            allstrings.insert(
               std::make_pair<int,std::vector<std::string> >(curblock,block));
         }

         // start of a new block
         curblock = strtol(buffer+7,NULL,10);
         block.clear();
         continue;
      }

      // must be a string line
      char *pos = strchr(buffer,':');
      if (pos==NULL)
      {
         printf("line %u of block contained no ':'\n",block.size());
         continue; // should not happen
      }
      pos+=2; // move forward to actual string

      // replace "\n" with real newlines
      char *pos2;
      while( (pos2 = strstr(pos,"\\n") )!=NULL)
      {
         pos2[0] = '\n';
         memmove(pos2+1,pos2+2,strlen(pos2+1));
      }

      // calculate char frequencies for that string
      unsigned int max = strlen(pos);
      for(unsigned int i=0; i<max; i++)
         char_freq[pos[i]]++;

      char_freq['|']++;

      // add string to block
      std::string thestring(pos);
      thestring.append("|"); // end marker
      block.push_back(thestring);
   }
   fclose(in);

   // store last block, too
   allstrings.insert(
      std::make_pair<int,std::vector<std::string> >(curblock,block));

   // now that we have all the strings, build the huffman tree
   printf("building huffman tree ...\n");

   // reverse char lookup table
   std::vector<unsigned int> char_lookup;
   char_lookup.resize(256,0);

   // build up list with all leaf nodes
   // use all characters that have frequencies above 0
   std::vector<ua_huff_node> huffnodes;
   {
      for(unsigned int i=0; i<256; i++)
      if (char_freq[i]>0)
      {
         ua_huff_node node;
         node.symbol = i;
         node.parent = 0; // we don't know yet
         node.left = -1;  // no children
         node.right = -1;
         node.freq = char_freq[i];

         huffnodes.push_back(node);

         char_lookup[i] = huffnodes.size()-1;
      }
   }

   unsigned int valid_chars = huffnodes.size();

   // build huffman tree
   while(true)
   {
      unsigned int i,max;

      int node1=-1,node2=-1;
      unsigned int freq1=0,freq2=0;

      // find two nodes with the least frequencies
      max = huffnodes.size();
      for(i=0; i<max; i++)
      if (huffnodes[i].freq!=0 && (freq1==0 || huffnodes[i].freq < freq1))
      {
         node1=i;
         freq1=huffnodes[i].freq;
      }

      if (node1==-1)
      {
         printf("didn't find any node with frequency != 0!\n");
         break; // didn't find any node; should never happen, though
      }

      huffnodes[node1].freq = 0;

      // search for second node
      for(i=0; i<max; i++)
      if (huffnodes[i].freq!=0 && (freq2==0 || huffnodes[i].freq < freq2))
      {
         node2=i;
         freq2=huffnodes[i].freq;
      }

      if (node2==-1)
      {
         // only found one node, the root node
         huffnodes[node1].parent=-1;
         break; // we're finished
      }

      huffnodes[node2].freq = 0;

      // we have a new node
      {
         ua_huff_node node;
         node.symbol = 0;
         node.parent = 0; // we don't know yet
         node.left = node1;
         node.right = node2;
         node.freq = freq1+freq2;
         huffnodes.push_back(node);
      }

      unsigned int parent = huffnodes.size()-1;
      huffnodes[node1].parent=parent;
      huffnodes[node2].parent=parent;
   }


   // for a node list, we need at most 0x100 huffnodes
   if (huffnodes.size()>0x100)
   {
      printf("error! huffnodes limit of 256 nodes exceeded! "
         "please only use ASCII charset!\n");
      return;
   }

   printf("huffman tree: %u characters, %u nodes\n",
      valid_chars,huffnodes.size()+1);


   // write the output file
   FILE *out = fopen(outfile,"wb");
   if (out==NULL)
   {
      printf("could not open output file!\n");
      return;
   }

   printf("writing output file %s ...\nblocks: ",outfile);

   // write huffnode table
   {
      // number of nodes
      Uint16 max = huffnodes.size();
      fwrite16(out,max);

      // write all nodes
      for(Uint16 i=0; i<max; i++)
      {
         const ua_huff_node &curnode = huffnodes[i];
         fputc(curnode.symbol,out);
         fputc(curnode.parent,out);
         fputc(curnode.left,out);
         fputc(curnode.right,out);
      }

      // number of blocks
      Uint16 numblocks = allstrings.size();
      fwrite16(out,numblocks);

      // now, write all blocks
      std::map<int,std::vector<std::string> >::iterator iter,stop;
      iter = allstrings.begin(); stop = allstrings.end();

      // init block offsets list
      std::vector<Uint32> block_offsets;

      // remember offset of block directory
      long bdir_pos = ftell(out);

      for(;iter!=stop; iter++)
      {
         const std::pair<int,std::vector<std::string> > &block = *iter;

         // write block directory entry
         fwrite16(out,static_cast<Uint16>(block.first));
         fwrite32(out,0); // we don't know yet
      }

      // write blocks
      iter = allstrings.begin();
      for(;iter!=stop; iter++)
      {
         const std::vector<std::string> &stringlist = (*iter).second;

         printf(".");

         // remember block offset
         block_offsets.push_back(static_cast<Uint32>(ftell(out)));

         // write block header
         Uint16 numstrings = stringlist.size();
         fwrite16(out,numstrings);

         // remember start of string offsets
         long str_offsets_pos = ftell(out);

         for(int k=0; k<numstrings; k++)
            fwrite16(out,0); // relative offsets to strings data

         // all string offsets for this block
         std::vector<Uint16> string_offsets;

         // used for calculation of relative string offsets
         long rel_string_offset = ftell(out);

         // write every string
         for(Uint16 i=0; i<numstrings; i++)
         {
            // remember relative offset
            string_offsets.push_back(ftell(out)-rel_string_offset);

            unsigned int bits=0;
            Uint16 raw=0;

            // encode every character using the lookup table and the
            // huffman tree
            const char *str = stringlist[i].c_str();
            unsigned int len = strlen(str);
            for(unsigned int n=0; n<len; n++)
            {
               char c = str[n];
               unsigned int pos = char_lookup[c];

               // all bits to encode the char
               std::vector<bool> allbits;

               do
               {
                  // check which way 'up' the tree we use
                  unsigned int pos2 = huffnodes[pos].parent;
                  if (huffnodes[pos2].left == pos)
                     allbits.push_back(false); // we used the left path
                  else
                     allbits.push_back(true); // we used the right path

                  pos=pos2; // parent is new node

               } while(huffnodes[pos].parent != -1);

               // now we have all bits for this char, in the reverse order
               // put them to bytes and write them
               unsigned int maxbits = allbits.size();
               for(unsigned int p=maxbits; p>0; p--)
               {
                  bool bit = allbits[p-1];
                  if (bit)
                     raw |= 0x80;
                  raw <<=1;
                  bits++;

                  if (bits==8)
                  {
                     fputc( (raw>>8)&0xff, out);
                     raw = 0;
                     bits = 0;
                  }
               }
            }
            // end of string

            // write all pending bits
            raw <<=(8-bits);
            fputc( (raw>>8)&0xff, out);
         }

         // remember current end of file
         long cur_pos = ftell(out);

         // now write all string offsets
         fseek(out,str_offsets_pos,SEEK_SET);

         for(int m=0; m<numstrings; m++)
            fwrite16(out,string_offsets[m]);

         // go back to end of file
         fseek(out,cur_pos,SEEK_SET);
      }

      // now write all block offsets
      fseek(out,bdir_pos,SEEK_SET);

      unsigned int numoffsets = block_offsets.size();
      for(unsigned int j=0; j<numoffsets; j++)
      {
         fseek(out,2,SEEK_CUR);
         fwrite32(out,block_offsets[j]);
      }
   }

   fclose(out);

   printf("\ndone.\n");
}


// we don't need SDL's main here
#undef main


// main function

int main(int argc, char *argv[])
{
   printf("strpak - ultima underworld game strings pack and unpack tool\n\n");

   if (argc<4)
   {
      printf("syntax: strpak <command> <input-file> <output-file>\n");
      printf("example: strpak unpack strings.pak uw-strings.txt\n"
             "         strpak pack uw-strings.txt strings2.pak\n\n");
      return 1;
   }

   if (strcmp("unpack",argv[1])==0)
   {
      ua_strpak_unpack_strings(argv[2],argv[3]);
   }
   else
   if (strcmp("pack",argv[1])==0)
   {
      ua_strpak_pack_strings(argv[2],argv[3]);
   }
   else
      printf("unknown command \"%s\"\n",argv[1]);

   return 0;
}
