// stringextract.cpp
//

#include <stdio.h>
#include <string>

#pragma pack(push,1)

typedef struct huff_node
{
  int symbol;
  int parent;
  int left;
  int right;
} huff_node;

typedef struct block_info
{
   unsigned short block_id;
   unsigned int offset;
} block_info;

#pragma pack(pop)


int main(int argc, char* argv[])
{
   const char *filename = "..\\..\\uw1\\data\\strings.pak";

   FILE *fd = fopen(filename,"rb");
   if (fd==NULL)
   {
      printf("could not open file\n");
      return 0;
   }

   FILE *ftext = fopen("uw1-strings.txt","w");

   unsigned short nodenum;
   fread(&nodenum,1,2,fd);

   fprintf(ftext,"strings.pak: %u tree nodes\n",nodenum);

   // read in node list
   huff_node *allnodes = new huff_node[nodenum];
   for(int k=0; k<nodenum; k++)
   {
      allnodes[k].symbol = fgetc(fd);
      allnodes[k].parent = fgetc(fd);
      allnodes[k].left   = fgetc(fd);
      allnodes[k].right  = fgetc(fd);
   }

   // number of string blocks
   unsigned short sblocks;
   fread(&sblocks,1,2,fd);

   // read in all block infos
   block_info *allblocks = new block_info[sblocks];
   fread(allblocks,sizeof(block_info),sblocks,fd);

   int ofs = ftell(fd);

   for(int i=0; i<sblocks; i++)
   {
      fseek(fd,allblocks[i].offset,SEEK_SET);

      // number of strings
      unsigned short numstrings;
      fread(&numstrings,1,2,fd);

      fprintf(ftext,"\nblock: %u, offset: %08x, strings: %u\n",
         i,allblocks[i].offset,numstrings);

      // all string offsets
      unsigned short *stroffsets = new unsigned short[numstrings];
      fread(stroffsets,sizeof(unsigned short),numstrings,fd);

      unsigned int curoffset = allblocks[i].offset + (numstrings+1)*sizeof(unsigned short);

      for(int n=0; n<numstrings; n++)
      {
         fseek(fd,curoffset+stroffsets[n],SEEK_SET);

         char c;
         std::string str;

         int bit=0;
         int raw=0;

         do
         {
            int node=nodenum-1; // starting node

            // huffman decode loop
            while (char(allnodes[node].left) != -1 && char(allnodes[node].left) != -1)
            {
               if (bit==0)
               {
                  bit=8;
                  raw=fgetc(fd);
                  if (feof(fd))
                  {
                     n=numstrings;
                     i=sblocks;
                     break;
                  }
               }

               node = raw & 0x80 ? short(allnodes[node].right)
                  : short(allnodes[node].left);

               raw<<=1;
               bit--;
            }

            if (feof(fd)) break;

            // have a new symbol
            c = allnodes[node].symbol;
            if (c!='|')
               str.append(1,c);

         } while (c!='|');

         if (str.length()>0 && str.at(str.length()-1)=='\n')
            str.erase(str.length()-1);

         fprintf(ftext,"string %03u: %s\n",n,str.c_str());

         str.erase();
      }
      delete[] stroffsets;
   }

   delete[] allblocks;
   delete[] allnodes;

   fclose(fd);
   fclose(ftext);

   return 0;
}
