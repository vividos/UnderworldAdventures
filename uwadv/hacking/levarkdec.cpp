//
// Underworld Adventures - an Ultima Underworld hacking project
// Copyright (c) 2002,2019 Michael Fink
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
// level map decoding
//
#include "hacking.h"
#include <algorithm>

//const char *filename = UWPATH "Save3\\lev.ark";
//const char *filename = UWPATH "Save4\\lev.ark";
const char *filename = UWPATH "data\\lev.ark";

void follow_object_chain(FILE* out, unsigned int objlist[0x400 * 2],
   std::vector<bool> &linked_by_obj, std::vector<bool> &link2_by_obj, int link1);

void check_link2(FILE* out, unsigned int objlist[0x400 * 2], std::vector<bool> &link2_by_obj, int link1)
{
   int id = objlist[link1 * 2 + 0] & 0x000001ff;

   if (
      (id >= 0x0040 && id <= 0x007f) ||
      (id >= 0x0080 && id <= 0x008e) ||
      (id >= 0x0098 && id <= 0x009f) ||
      id == 0x00aa ||
      (id >= 0x0140 && id <= 0x014f) ||
      (id >= 0x0140 && id <= 0x014f) ||
      id == 0x015b || id == 0x015e || id == 0x0161 || id == 0x016e ||
      (id >= 0x0170 && id <= 0x017f) ||
      id == 0x0187 || id == 0x0188 ||
      id == 0x01a0 || id == 0x01a2 || id == 0x01a3 || id == 0x01a5 || id == 0x01a6
      )
   {
      int link2 = (objlist[link1 * 2 + 1] & (0x3ff << 22)) >> 22;

      if (link2 == 0) return;

      if (link2_by_obj[link2])
         fprintf(out, "hmm, object %04x already link2'ed\n", link2);

      link2_by_obj[link2] = true;

      // follow chain, marks all objects as "link2d" instead of "linked"
      follow_object_chain(out, objlist, link2_by_obj, link2_by_obj, link2);
   }
}

void follow_object_chain(FILE* out, unsigned int objlist[0x400 * 2],
   std::vector<bool> &linked_by_obj, std::vector<bool> &link2_by_obj, int link1)
{
   while (link1 != 0)
   {
      check_link2(out, objlist, link2_by_obj, link1);

      if (linked_by_obj[link1])
         fprintf(out, "hmm, object %04x linked by two other objects\n", link1);
      linked_by_obj[link1] = true;

      int oldlink1 = link1;

      // find out next link
      link1 = ((objlist[link1 * 2 + 1] & (0x3ff << 6)) >> 6);

      if (link1 == oldlink1)
      {
         fprintf(out, "hmm, object %04x links to itself\n", link1);
         break;
      }
   }
}

int levarkdec_main()
{
   FILE* fd = fopen(filename, "rb");

   if (fd == NULL)
   {
      printf("could not open lev.ark\n");
      return 0;
   }

   fseek(fd, 0, SEEK_END);
   unsigned int flen = ftell(fd);
   fseek(fd, 0, SEEK_SET);

   FILE* out = fopen("uw-levelobjs.txt", "w");
   FILE* out2 = fopen("uw-misc.txt", "w");
   FILE* out3 = fopen("uw-unknowns.txt", "w");
   FILE* out4 = fopen("uw-texmaps.txt", "w");

   // read in toc

   unsigned short entries;
   fread(&entries, sizeof(unsigned short), 1, fd);

   fprintf(out2, "lev.ark: %u entries\n", entries);

   unsigned int *offsets = new unsigned int[entries];

   // read all offset
   fread(offsets, sizeof(unsigned int), entries, fd);

   // print sorted list of file parts
   {
      std::vector<std::pair<unsigned int, int>> sortedoffsets;

      for (int i = 0; i < entries; i++)
         if (offsets[i] != 0 && offsets[i] != flen)
            sortedoffsets.push_back(
               std::make_pair(offsets[i], i));

      sortedoffsets.push_back(std::make_pair(flen, 0));
      std::sort(sortedoffsets.begin(), sortedoffsets.end());

      int max = sortedoffsets.size() - 1;
      for (int j = 0; j < max; j++)
      {
         fprintf(out2, "entry %02u: offset=%08x size=%04x\n",
            sortedoffsets[j].second, sortedoffsets[j].first,
            sortedoffsets[j + 1].first - sortedoffsets[j].first);
      }
      fprintf(out2, "\n");
   }

   // read in game strings
   ua_gamestrings gs;
   gs.load(UWPATH "data\\strings.pak");

   for (int j = 0; j < 9; j++)
   {
      fseek(fd, offsets[j], SEEK_SET);

      unsigned int buffer[64 * 64];
      fread(buffer, 4, 64 * 64, fd);

#if 0
      char fname[256];

      // now open and write tga files

      // write tile type map
      {
         sprintf(fname, "level%03u-type.tga", j);
         FILE* tga = fopen(fname, "wb");
         tga_writeheader(tga, 64, 64, 2, 0, true);

         int mapping[16] =
         {
            0, 255, 192, 192, 192, 192, 64, 64, 64, 64, 128, 128, 128, 128, 128, 128
         };

         for (int i = 0; i < 64 * 64; i++)
         {
            int c = mapping[buffer[i] & 0x0f];
            fputc(c, tga); fputc(c, tga); fputc(c, tga);
            fputc(0, tga);
         }
         fclose(tga);
      }

      // write floor height map
      {
         sprintf(fname, "level%03u-floor.tga", j);
         FILE* tga = fopen(fname, "wb");
         tga_writeheader(tga, 64, 64, 2, 0, true);

         for (int i = 0; i < 64 * 64; i++)
         {
            int h = (buffer[i] & 0xF0);
            int t = (buffer[i] & 0x003F0000) >> 14; // 6 bit wide
            int t2 = (buffer[i] & 0x00003C00) >> 10; // 4 bits

            fputc(h, tga); //fputc(t,tga); fputc(t2,tga);
            fputc(h, tga); fputc(h, tga);
            fputc(0, tga);
         }
         fclose(tga);
      }

      // write index map
      {
         sprintf(fname, "level%03u-index.tga", j);
         FILE* tga = fopen(fname, "wb");
         tga_writeheader(tga, 64, 64, 2, 0, true);

         for (int i = 0; i < 64 * 64; i++)
         {
            int idx = (buffer[i] & 0xFFC00000) >> 22;

            fputc(idx & 0xff, tga); fputc(idx >> 8, tga); fputc(idx & 0xff, tga);
            fputc(0, tga);
         }
         fclose(tga);
      }

      // write unknowns map
      {
         sprintf(fname, "level%03u-unk.tga", j);
         FILE* tga = fopen(fname, "wb");
         tga_writeheader(tga, 64, 64, 2, 0, true);

         for (int i = 0; i < 64 * 64; i++)
         {
            int unk1 = (buffer[i] & 0x00000300) >> 2;
            int unk2 = (buffer[i] & 0x00004000) >> 7;

            fputc(unk1, tga); fputc(unk2, tga); fputc(unk2, tga);
            fputc(0, tga);
         }
         fclose(tga);
      }

      // more tga writing here
#endif

      fprintf(out, "objects for level %02u, at offset %08x:\n\n", j, offsets[j] + 0x4000);

      // collect level map object list infos
      std::vector<bool> ref_by_tile;
      std::vector<bool> linked_by_obj;
      std::vector<bool> link2_by_obj;
      std::vector<unsigned char> tile_coords;

      ref_by_tile.resize(0x400, false);
      linked_by_obj.resize(0x400, false);
      link2_by_obj.resize(0x400, false);
      tile_coords.resize(0x800, 0xff);

      {
         fseek(fd, offsets[j] + 0x4000, SEEK_SET);
         unsigned int objlist[0x400 * 2];

         for (int n = 0; n < 0x400; n++)
         {
            fread(&objlist[n * 2], 8, 1, fd);

            if (n < 0x100)
               fseek(fd, 0x1b - 0x08, SEEK_CUR);
         }


         for (int i = 0; i < 64 * 64; i++)
         {
            unsigned int idx = (buffer[i] & 0xFFC00000) >> 22;

            if (idx == 0) continue;

            if (ref_by_tile[idx])
               fprintf(out, "hmm, object %04x used twice for tile\n", idx);

            ref_by_tile[idx] = true;

            // remember tile coordinates
            tile_coords[idx * 2 + 0] = i & 63;
            tile_coords[idx * 2 + 1] = i >> 6;

            // check for link2 targets of tile ref item first
            check_link2(out, objlist, link2_by_obj, idx);

            // for each tile, follow the link
            int link1 = ((objlist[idx * 2 + 1] & (0x3ff << 6)) >> 6);

            if (link1 == 0)
               continue;

            follow_object_chain(out, objlist, linked_by_obj, link2_by_obj, link1);
         }
      }

      fprintf(out, "\n");

      // level object dumping
      fseek(fd, offsets[j] + 0x4000, SEEK_SET);

      for (int n = 0; n < 0x400; n++)
      {
         unsigned int objval[2];
         fread(objval, 4, 2, fd);

         fprintf(out, "object %04x: ", n);

         fprintf(out, "id=%04x ", objval[0] & 0x000001ff);
         fprintf(out, "unk1=%02x ", (objval[0] & 0x0000ff00) >> 9);
         fprintf(out, "zpos=%02x ", (objval[0] & 0x007f0000) >> 16);
         fprintf(out, "dir=%u ", (objval[0] & 0x03800000) >> 23);
         fprintf(out, "xpos=%02x ", (objval[0] & (7 << 26)) >> 26);
         fprintf(out, "ypos=%02x ", (objval[0] & (7 << 29)) >> 29);

         fprintf(out, "quality=%02x ", objval[1] & 0x0000003f);
         fprintf(out, "link1=%04x ", (objval[1] & (0x3ff << 6)) >> 6);
         fprintf(out, "unk2=%02x ", (objval[1] & (0x3f << 16)) >> 16);
         fprintf(out, "quan/link2=%04x ", (objval[1] & (0x3ff << 22)) >> 22);

         if (ref_by_tile[n])
            fprintf(out, "tile %02x/%02x ", tile_coords[n * 2 + 0], tile_coords[n * 2 + 1]);
         else
         {
            if (linked_by_obj[n])
               fprintf(out, "linked     ");
            else
            {
               if (link2_by_obj[n])
                  fprintf(out, "link2d     ");
               else
                  fprintf(out, "           ");
            }
         }

         if (n < 0x100)
         {
            fprintf(out, "- ");

            unsigned char npcinfo[19];
            fread(npcinfo, 1, 19, fd);

            // dump NPC info
            for (int c = 0; c < 19; c++)
               fprintf(out, "%02x ", (unsigned int)npcinfo[c]);

            // print xhome/yhome
            fprintf(out, "xhome=%02x yhome=%02x extra=%02x ",
               (npcinfo[15] >> 2) & 0x3f,
               ((npcinfo[14] | (npcinfo[15] << 8)) >> 4) & 0x3f,
               npcinfo[14] & 0x0f);

            // print name
            if (npcinfo[18] > 0)
               fprintf(out, "name=%-25s ", gs.get_string(7, npcinfo[18] + 16).c_str());
            else
               fprintf(out, "%-30s ", "");
         }

         fprintf(out, "desc=%s\n", gs.get_string(4, objval[0] & 0x000001ff).c_str());
      }
      fprintf(out, "\n");

      // animation overlay list dumping
      fseek(fd, offsets[j + 9], SEEK_SET);
      {
         fprintf(out2, "animation overlay list at level %u, offset %08x\n", j, offsets[j + 9]);

         unsigned char unknowns[64 * 6];
         fread(unknowns, 64, 6, fd);

         for (int i = 0; i < 64; i++)
         {
            if (memcmp(&unknowns[i * 6], "\0\0\0\0\0\0", 6) == 0)
               break;

            fprintf(out2, "x=%02x y=%02x ", unknowns[i * 6 + 4], unknowns[i * 6 + 5]);

            int unk1 = (unknowns[i * 6 + 0] << 8) | unknowns[i * 6 + 1];
            fprintf(out2, "unk1=%04x ", (unknowns[i * 6 + 0] << 8) | unknowns[i * 6 + 1]);
            fprintf(out2, "unk2=%04x\n", (unknowns[i * 6 + 2] << 8) | unknowns[i * 6 + 3]);

            //            fprintf(out2,"desc1=%s",gs.get_string(4,unk1&0x01ff).c_str());
         }
      }
      fprintf(out2, "\n");


      // dump block at 0x7300
      {
         fseek(fd, offsets[j] + 0x6383, SEEK_SET);

         fprintf(out3, "unknowns for level %u\n\n", j);

         fseek(fd, offsets[j] + 0x7300, SEEK_SET);

         for (unsigned int k = 0; k < 0x400; k++)
         {
            unsigned short value;
            fread(&value, 1, 2, fd);

            if ((k & 15) == 0) fprintf(out3, "%04x:", k);
            fprintf(out3, " %04x", value);
            if ((k & 15) == 15) fprintf(out3, "\n");
         }

         fprintf(out3, "\n");
      }

      // dump block at 7afc (260 bytes)
      {
         fseek(fd, offsets[j] + 0x6383, SEEK_SET);

         fprintf(out3, "unk260 list for level %u\n\n", j);

         for (unsigned int k = 0; k < 260; k++)
         {
            unsigned char value = fgetc(fd);

            fprintf(out3, " %02x", value);
            if ((k & 15) == 15) fprintf(out3, "\n");
         }

         fprintf(out3, "\n");
      }

      // dump texture usage
      {
         fseek(fd, offsets[j + 18], SEEK_SET);

         unsigned short usages[48 + 10 + 6];

         fread(usages, 48 + 10 + 6, 2, fd);

         fprintf(out4, "level %u\n", j);
         fprintf(out4, "wall textures\n");
         int n = 0;
         for (n = 0; n < 48; n++)
            fprintf(out4, " %04x", usages[n]);

         fprintf(out4, "\nfloor textures\n");
         for (n = 48; n < 48 + 10; n++)
            fprintf(out4, " %04x", usages[n]);

         fprintf(out4, "\ndoor textures\n");
         for (n = 48 + 10; n < 48 + 10 + 6; n++)
            fprintf(out4, " %04x", usages[n]);

         fprintf(out4, "\n\n");
      }

   } // end for

   delete[] offsets;

   fclose(fd);
   fclose(out);
   fclose(out2);
   fclose(out3);
   fclose(out4);

   return 0;
}
