// cnvdec.cpp
//

#include "../hacking.h"
#include <stdio.h>

const char *fname = UWPATH "data\\Cnv.ark";

void dump_import_funcs(FILE *fd, FILE *dest)
{
   // read number of imported funcs
   unsigned short funcs;
   fread(&funcs,1,2,fd);

   fprintf(dest,"; imported functions list, %u entries:\n;\n",funcs);

   for(int i=0; i<funcs; i++)
   {
      // length of function name
      unsigned short fname_len;
      fread(&fname_len,1,2,fd);

      char funcname[256]; // assumes strlen(function name) always < 256 chars
      fread(funcname,1,fname_len,fd);
      funcname[fname_len]=0;

      // function ID
      unsigned short func_id;
      fread(&func_id,1,2,fd);

      unsigned short unk1;
      fread(&unk1,1,2,fd);

      unsigned short import_type;
      fread(&import_type,1,2,fd);

      unsigned short ret_type;
      fread(&ret_type,1,2,fd);

      if (import_type == 0x010F)
      {
         fprintf(dest,"; var mem=%04x (ret_type=%04x, unk1=%04x) name=\"%s\" \n",
            func_id,ret_type,unk1,funcname);
      }
      else
      if (import_type == 0x0111)
      {
         fprintf(dest,"; func id=%04x ret_type=%s (unk1=%04x) name=\"%s\"\n",
            func_id,
            ret_type==0 ? "void" : ret_type==0x0129? "int" :
               ret_type==0x012b ? "string" : "unknown",
            unk1,funcname);
      }
      else
      {
         fprintf(dest,"; unknown import record: import type=%04x\n",import_type);
      }
   }

   fprintf(dest,";\n\n");
}

// instruction list
struct
{
   const char *mnemonic;
   int args;
   const char *argcode;
   const char *comment;
} instructions[0x2a] = 
{
   { "NOP", 0,  "", "" },
   { "OPADD", 0, "", "pushes s[0]+s[1]" },
   { "OPMUL", 0, "", "pushes s[0]*s[1]" },
   { "OPSUB", 0, "", "pushes s[1]-s[0]" },
   { "OPDIV", 0, "", "pushes s[1]/s[0]" },
   { "OPMOD", 0, "", "pushes s[1]%s[0]" },
   { "OPOR", 0, "", "pushes s[1] || s[0]" },
   { "OPAND", 0, "", "pushes s[1] && s[0]" },
   { "OPNOT", 0, "", "pushes !s[0]" },
   { "TSTGT", 0, "", "" },
   { "TSTGE", 0, "", "" },
   { "TSTLT", 0, "", "" },
   { "TSTLE", 0, "", "" },
   { "TSTEQ", 0, "", "" },
   { "TSTNE", 0, "", "" },
   { "JMP", 1, " $%04x", "absolute jump" },
   { "BEQ", 1, " +%04x", "branch when s[0]==0" },
   { "BNE", 1, " +%04x", "branch when s[0]!=0" },
   { "BRA", 1, " +%04x", "branch always" },
   { "CALL", 1, " $%04x", "call subroutine" },
   { "CALLI", 1, " $%04x", "call imported subroutine" },
   { "RET", 0, "", "return; s[0] is the next instruction pointer\n" },
   { "PUSHI", 1, " #%04x", "push immediate value" },
   { "PUSHI_EFF", 1, " $%04x", "" },
   { "POP", 0, "", "" },
   { "SWAP", 0, "", "" },
   { "PUSHBP", 0, "", "" },
   { "POPBP", 0, "", "" },
   { "SPTOBP", 0, "", "" },
   { "BPTOSP", 0, "", "" },
   { "ADDSP", 0, "", "" },
   { "FETCHM", 0, "", "" },
   { "STO", 0, "", "" },
   { "OFFSET", 0, "", "" },
   { "START", 0, "", "start program" },
   { "SAVE_REG", 0, "", "" },
   { "PUSH_REG", 0, "", "" },
   { "STRCMP", 0, "", "string compare (??)" },
   { "EXIT_OP", 0, "", "end program (?)" },
   { "SAY_OP", 0, "", "NPC says string s[0]" },
   { "RESPOND_OP", 0, "", "respond (?)" },
   { "OPNEG", 0, "", "negate, s[0] = -s[0]" },
};

void decode_instructions(FILE *fd, FILE *dest,unsigned int codesize)
{
   unsigned short *allcodes = new unsigned short[codesize+1];
   fread(allcodes,2,codesize,fd);

   fprintf(dest,";\n; code section\n;\n");
   for(unsigned int i=0; i<codesize; i++)
   {
      unsigned short code = allcodes[i];
      if (code>0x29)
      {
         fprintf(dest,"; !!! unknown code %04x encountered! !!!\n",code);
         continue; // try to continue
      }

      // output memory adress and code words
      fprintf(dest,"%04x\t%04x",i,code);
      if (instructions[code].args>0)
      {
         for(int n=0; n<instructions[code].args; n++)
            fprintf(dest," %04x",allcodes[i+n+1]);
      }
      else
         fprintf(dest,"\t");

      // output mnemonic and possible operand
      int arg = allcodes[i+1];
      fprintf(dest,"\t%s",instructions[code].mnemonic);
      fprintf(dest,instructions[code].argcode,arg);

      if (instructions[code].args==0)
         fprintf(dest,"\t");

      // output comment
      fprintf(dest,"\t\t; %s\n",instructions[code].comment);

      i += instructions[code].args;
   }

   delete[] allcodes;
}


int main()
{
   FILE *fd = fopen(fname,"rb");

   if (fd==NULL)
   {
      printf("could not open file %s\n",fname);
      return 1;
   }

   // open destination file
   FILE *dest = fopen("uw1-cnvdec.txt","w");
   fprintf(dest,";\n; decoded conversations\n");

   // read number of entries
   unsigned short entries;
   fread(&entries,1,2,fd);

   fprintf(dest,";%u conversations.\n;\n\n",entries);

   // read offsets
   unsigned int *offsets = new unsigned int[entries];
   fread(offsets,4,entries,fd);

   int slotcount = 0;

   for(int i=0; i<entries; i++)
   {
      if (offsets[i]==0)
      {
//         fprintf(dest,"; conversation #%u: n/a\n\n",i);
         continue;
      }

      fseek(fd,offsets[i],SEEK_SET);

      unsigned int unknown1;
      fread(&unknown1,1,4,fd);

      unsigned int codesize;
      fread(&codesize,1,4,fd);

      // output conversation info
      fprintf(dest,"; conversation #%u: offset=0x%04x, unk1=0x%04x, codesize=0x%04x\n",
         i, offsets[i],unknown1,codesize);

      unsigned short unknown2[3];
      fread(unknown2,2,3,fd);

      fprintf(dest,"; unk2=%04x, conv=%04x, init_stack=%04x\n",
         unknown2[0],unknown2[1],unknown2[2]);

      fprintf(dest,"; string block to use: %u\n",slotcount+25);

      dump_import_funcs(fd,dest);
      decode_instructions(fd,dest,codesize);

      fprintf(dest,"; -----------------------------------------\n\n");

      slotcount++;
   }

   fclose(dest);

   delete[] offsets;

   fclose(fd);

	return 0;
}
