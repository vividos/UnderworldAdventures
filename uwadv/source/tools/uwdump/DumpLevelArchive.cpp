//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2003,2004,2019,2021 Underworld Adventures Team
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
/// \file DumpLevelArchive.cpp
/// \brief level archive decoding implementation
//
#include "common.hpp"
#include "DumpLevelArchive.hpp"
#include "File.hpp"
#include "Bits.hpp"
#include "GameStrings.hpp"
#include "ArchiveFile.hpp"
#include <algorithm>
#include <bitset>

using Import::GetBits;

extern void DumpArkArchive(const std::string& filename, const GameStrings& gameStrings, bool isUw2);

void DumpLevArk(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   // first dump general .ark structure
   DumpArkArchive(filename, gameStrings, isUw2);
   printf("\n");

   DumpLevelArchive dumper{ gameStrings, isUw2 };
   dumper.Start(filename);
}

void DumpLevelArchive::Start(const std::string& filename)
{
   printf("level archive dumping\n");

   Base::File file{ filename.c_str(), Base::modeRead };

   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   unsigned int numLevels = !m_isUw2 ? 9 : 80;
   unsigned int numDumpedLevels = 0;

   // process all levels
   for (m_currentLevel = 0; m_currentLevel < numLevels; m_currentLevel++)
   {
      objinfos.clear();
      npcinfos.clear();

      tilemap.resize(0, 0);
      linkcount.clear();
      linkcount.resize(0x400, 0);
      linkref.clear();
      linkref.resize(0x400, 0);
      tilemap_links.clear();
      item_positions.clear();
      item_positions.resize(0x400, std::make_pair<Uint8, Uint8>(0xff, 0xff));
      freelist.clear();
      freelist.insert(0);

      if (m_isUw2)
      {
         // load uw2 file
         SDL_RWops* rwops = SDL_RWFromFile(filename.c_str(), "rb");
         Base::ArchiveFile ark{ Base::MakeRWopsPtr(rwops), m_isUw2 };

         if (!ark.IsAvailable(m_currentLevel))
            continue;

         Base::File levelFile = ark.GetFile(m_currentLevel);
         if (!levelFile.IsOpen())
            continue;

         std::vector<Uint8> decoded;
         decoded.resize(0x7e08, 0);

         levelFile.ReadBuffer(decoded.data(), 0x7e08);

         // read in tilemap
         {
            for (unsigned int i = 0; i < 64 * 64; i++)
            {
               Uint16 tileword1 = decoded[i * 2 + 0] | (decoded[i * 2 + 1] << 8);
               tilemap.push_back(tileword1);

               Uint16 tileword2 = decoded[i * 2 + 2] | (decoded[i * 2 + 3] << 8);

               Uint16 index = GetBits(tileword2, 6, 10);
               tilemap_links.push_back(index);
            }
         }

         // read in object list
         {
            for (unsigned int i = 0; i < 0x100; i++)
            {
               // read object info
               objinfos.push_back(decoded[0x4000 + i * 27 + 0] | (decoded[0x4000 + i * 27 + 1] << 8));
               objinfos.push_back(decoded[0x4000 + i * 27 + 2] | (decoded[0x4000 + i * 27 + 3] << 8));
               objinfos.push_back(decoded[0x4000 + i * 27 + 4] | (decoded[0x4000 + i * 27 + 5] << 8));
               objinfos.push_back(decoded[0x4000 + i * 27 + 6] | (decoded[0x4000 + i * 27 + 7] << 8));

               // read npc extra info
               for (unsigned int j = 8; j < 27; j++)
                  npcinfos.push_back(decoded[0x4000 + i * 27 + j]);
            }

            for (unsigned int n = 0; n < 0x300; n++)
            {
               // read object info
               objinfos.push_back(decoded[0x5b00 + n * 8 + 0] | (decoded[0x5b00 + n * 8 + 1] << 8));
               objinfos.push_back(decoded[0x5b00 + n * 8 + 2] | (decoded[0x5b00 + n * 8 + 3] << 8));
               objinfos.push_back(decoded[0x5b00 + n * 8 + 4] | (decoded[0x5b00 + n * 8 + 5] << 8));
               objinfos.push_back(decoded[0x5b00 + n * 8 + 6] | (decoded[0x5b00 + n * 8 + 7] << 8));
            }
         }

         // read in free list
         {
            // note: invalid offsets
            Uint16 mobile_items = decoded[0x7c02] | (decoded[0x7c03] << 8);
            Uint16 static_items = decoded[0x7c04] | (decoded[0x7c05] << 8);

            for (Uint16 i = 0; i < mobile_items + 1; i++)
            {
               Uint16 pos = decoded[0x7300 + i * 2] | (decoded[0x7300 + i * 2 + 1] << 8);
               freelist.insert(pos);
            }

            for (Uint16 n = 0; n < static_items + 1; n++)
            {
               Uint16 pos = decoded[0x74fc + n * 2] | (decoded[0x74fc + n * 2 + 1] << 8);
               freelist.insert(pos);
            }
         }

         // texture mapping
         {
            Base::File tex = ark.GetFile(80 + m_currentLevel);
            if (tex.IsOpen())
            {
               std::vector<Uint8> origtexmap;
               origtexmap.resize(0x0086, 0);

               file.ReadBuffer(origtexmap.data(), 0x0086);

               for (unsigned int i = 0; i < 64; i++)
               {
                  Uint16 texid = origtexmap[i * 2] | (origtexmap[i * 2 + 1] << 8);
                  texmapping.push_back(texid);
               }
            }
         }

         printf("\n");
      }
      else
      {
         // load uw1 file
         file.Seek(m_currentLevel * 4 + 2, Base::seekBegin);
         Uint32 offset = file.Read32();
         file.Seek(offset, Base::seekBegin);

         // read in tilemap
         {
            for (unsigned int i = 0; i < 64 * 64; i++)
            {
               Uint16 tileword1 = file.Read16();
               tilemap.push_back(tileword1);

               Uint16 tileword2 = file.Read16();

               Uint16 index = GetBits(tileword2, 6, 10);
               tilemap_links.push_back(index);
            }
         }

         // read in object list
         for (unsigned int i = 0; i < 0x400; i++)
         {
            // read object info
            objinfos.push_back(file.Read16());
            objinfos.push_back(file.Read16());
            objinfos.push_back(file.Read16());
            objinfos.push_back(file.Read16());

            // read npc extra info
            if (i < 0x100)
            {
               for (unsigned int j = 0; j < 27 - 8; j++)
                  npcinfos.push_back(file.Read8());
            }
         }

         // read in free list
         {
            file.Seek(offset + 0x7c02, Base::seekBegin);
            Uint16 mobile_items = file.Read16();
            Uint16 static_items = file.Read16();

            file.Seek(offset + 0x7300, Base::seekBegin);
            for (Uint16 i = 0; i < mobile_items + 1; i++)
               freelist.insert(file.Read16());

            file.Seek(offset + 0x74fc, Base::seekBegin);
            for (Uint16 n = 0; n < static_items + 1; n++)
               freelist.insert(file.Read16());
         }

         // texture mapping
         {
            file.Seek((m_currentLevel + numLevels * 2) * 4 + 2, Base::seekBegin);
            offset = file.Read32();
            file.Seek(offset, Base::seekBegin);

            for (unsigned int i = 0; i < 48; i++)
               texmapping.push_back(file.Read16());

            for (unsigned int j = 0; j < 10; j++)
               texmapping.push_back(file.Read16());
         }
      }

      FixLevelData();

      DrawLevel();

      ProcessLevel();
      DumpObjectInfos();

      numDumpedLevels++;
   }

   printf("dumped %u levels.\n", numDumpedLevels);
}

void DumpLevelArchive::FixLevelData()
{
   objinfos[4] |= 0x007f; // an_adventurer item_id
   objinfos[0] &= ~(1 << 12); // not enchanted

   // fix is_quantity flag for all triggers and "a_delete object trap"
   for (unsigned int objectIndex = 0; objectIndex < 0x0400; objectIndex++)
   {
      Uint16 item_id = GetBits(objinfos[objectIndex * 4 + 0], 0, 9);
      if ((item_id >= 0x01a0 && item_id <= 0x01bf) || item_id == 0x018b)
      {
         // clear is_quantity flag
         objinfos[objectIndex * 4 + 0] &= ~(1L << 15);
      }
   }
}

void DumpLevelArchive::DrawLevel()
{
   printf("Level layout:\n");

   for (unsigned int y = 0; y < 64; y++)
   {
      for (unsigned int x = 0; x < 64; x++)
      {
         Uint16 tileword1 = tilemap[(63 - y) * 64 + x];

         bool isDoor = GetBits(tileword1, 15, 1) != 0;

         if (isDoor)
         {
            printf("D");
            continue;
         }

         Uint16 tileType = GetBits(tileword1, 0, 4);
         switch (tileType)
         {
         case 0: // solid
            printf("*");
            break;

         case 1: // open
         case 6: // slope N
         case 7: // slope S
         case 8: // slope E
         case 9: // slope W
            printf(" ");
            break;

         case 2: // diagonal SE
         case 5: // diagonal NW
            printf("/");
            break;

         case 3: // diagonal SW
         case 4: // diagonal NE
            printf("\\");
            break;

         default:
            printf("?");
            break;
         }
      }
      printf("\n");
   }
   printf("\n\n");
}

void DumpLevelArchive::ProcessLevel()
{
   // follow all links and special links, starting from the tilemap indices
   for (unsigned int tileIndex = 0; tileIndex < 64 * 64; tileIndex++)
   {
      Uint16 objectIndex = tilemap_links[tileIndex];

      if (objectIndex != 0)
      {
         m_currentFollowLevel = 0;
         linkref[objectIndex] = 0xffff; // from tilemap
         FollowLink(objectIndex, tileIndex, false);
      }
   }
}

void DumpLevelArchive::DumpObjectInfos()
{
   printf("dumping infos for level %u (0x%02x)\n\n", m_currentLevel, m_currentLevel);

   // dumping free list
   {
      printf("dumping free list:\n");

      size_t count = 0;
      for (Uint16 free_slot : freelist)
      {
         printf("[%04x]%c",
            free_slot,
            (count % 16) == 15 ? '\n' : ' ');

         count++;
      }
      printf("\n");
   }

   // dump every object in list
   {
      printf("dumping object list:\n");
      for (unsigned int i = 0; i < 0x400; i++)
         DumpObject(i);

      printf("\n");
   }

   // dumping npc infos
   {
      printf("dumping npc infos for mobile objects:\n");

      for (unsigned int i = 0; i < 0x100; i++)
      {
         bool is_free = freelist.find(i) != freelist.end();
         if (is_free) continue;

         DumpNPCInfos(i);
      }
      printf("\n");
   }

   // dumping npc inventories
   {
      printf("dumping npc inventory:\n");
      for (unsigned int i = 0; i < 0x100; i++)
      {
         bool is_free = freelist.find(i) != freelist.end();
         if (is_free) continue;

         // check if inventory
         bool is_quantity = GetBits(objinfos[i * 4 + 0], 15, 1) != 0;
         Uint16 special = GetBits(objinfos[i * 4 + 3], 6, 10);

         Uint16 item_id = GetBits(objinfos[i * 4 + 0], 0, 9);

         if (!is_quantity && special != 0 && item_id >= 0x0040 && item_id < 0x0080)
         {
            printf("\ninventory for npc:\n");
            DumpNPCInfos(i);

            // follow inventory object chain
            Uint16 link = special;
            do
            {
               bool is_free2 = freelist.find(link) != freelist.end();
               if (is_free2) break;

               DumpObject(link);
               link = GetBits(objinfos[link * 4 + 2], 6, 10);
            } while (link != 0);
         }
      }
      printf("\n");
   }

   // dumping traps
   {
      printf("dumping traps:\n");

      for (unsigned int j = 0x0180; j < 0x0191; j++)
      {
         for (unsigned int pos = 0; pos < 0x400; pos++)
         {
            bool is_free = freelist.find(pos) != freelist.end();
            Uint16 item_id = GetBits(objinfos[pos * 4], 0, 9);

            if (!is_free && j == item_id)
               DumpObject(pos);
         }
      }
      printf("\n");
   }

   // dumping enchanted objects
   {
      printf("dumping enchanted items:\n");
      for (unsigned int i = 0; i < 0x400; i++)
      {
         bool is_free = freelist.find(i) != freelist.end();
         Uint16 item_id = GetBits(objinfos[i * 4], 0, 9);
         bool enchanted = GetBits(objinfos[i * 4], 12, 1) != 0;

         // dump enchanted and "a_spell" items
         if (!is_free && enchanted && (item_id < 0x0140 || item_id >= 0x01c0 || item_id == 0x0120))
            DumpObject(i);
      }

      printf("\n");
   }

   // following buttons/triggers, etc.
   printf("dumping special link chains:\n");
   {
      std::bitset<0x400> visited;
      visited.reset();

      //for(int m=0x03ff; m>=0; m--)
      for (unsigned int pos = 0; pos < 0x400; pos++)
      {
         Uint16 item_id = GetBits(objinfos[pos * 4], 0, 9);

         // don't start with non-tilemap objects or a teleport trap
         if ((item_positions[pos].first == 0xff && item_positions[pos].second == 0xff) ||
            item_id == 0x0181)
            continue;

         DumpSpecialLinkChain(visited, pos, 0);
      }
   }

   // dumping texture mapping
   {
      printf("\ndumping texture mapping:\nwall textures:");
      {
         unsigned int i;
         if (!m_isUw2)
         {
            for (i = 0; i < 48; i++)
               printf("%c%04x", (i % 8) == 0 ? '\n' : ' ', texmapping[i]);

            printf("\n\nceiling textures:");
            for (i = 0; i < 10; i++)
               printf("%c%04x", (i % 8) == 0 ? '\n' : ' ', texmapping[i + 48]);
         }
         else
         {
            for (i = 0; i < 64; i++)
               printf("%c%04x", (i % 8) == 0 ? '\n' : ' ', texmapping[i]);
         }
      }
      printf("\n");
   }

   printf("\n");
}

void DumpLevelArchive::DumpSpecialLinkChain(std::bitset<0x400>& visited, unsigned int pos,
   unsigned int indent)
{
   unsigned int step = 0;

   Uint16 item_id = GetBits(objinfos[pos * 4], 0, 9);
   bool is_free = freelist.find(pos) != freelist.end();

   Uint16 special = GetBits(objinfos[pos * 4 + 3], 6, 10);
   bool is_quantity = GetBits(objinfos[pos * 4 + 0], 15, 1) != 0;

   if (item_id >= 0x0180 && item_id <= 0x019f)
      return; // don't start with traps

   while (pos != 0 && !visited.test(pos) && !is_free &&
      !is_quantity && special != 0 && (item_id < 0x0040 || item_id >= 0x0080))
   {
      if (step == 0 && indent == 0)
         printf("\nnext chain:\n");

      { for (unsigned int i = 0; i < indent; i++) printf("     "); }

      printf("{%02x} ", step);
      if (pos < 0x100)
         DumpNPCInfos(pos);
      else
         DumpObject(pos);

      if (item_id >= 0x0080 && item_id < 0x008e)
      {
         // dump container items
         { for (unsigned int i = 0; i < indent; i++) printf("     "); }
         printf("container items:\n");

         Uint16 link = GetBits(objinfos[pos * 4 + 3], 6, 10);
         do
         {
            // check if linked item has a special link
            bool is_link2 = GetBits(objinfos[link * 4 + 0], 15, 1) == 0;
            Uint16 special2 = GetBits(objinfos[link * 4 + 3], 6, 10);

            if (is_link2 && special2 != 0)
               DumpSpecialLinkChain(visited, link, indent + 1);
            else
            {
               { for (unsigned int i = 0; i < indent; i++) printf("     "); }

               // no special link, just dump item
               DumpObject(link);
            }


            link = GetBits(objinfos[link * 4 + 2], 6, 10);
         } while (link != 0);

         return;
      }

      visited.set(pos);

      // get next object
      pos = special;

      // prevent infinite loops for some trap types
      if (item_id == 0x018b)
         break;

      if (pos != 0)
      {
         special = GetBits(objinfos[pos * 4 + 3], 6, 10);
         is_quantity = GetBits(objinfos[pos * 4 + 0], 15, 1) != 0;
         is_free = freelist.find(pos) != freelist.end();
         item_id = GetBits(objinfos[pos * 4 + 0], 0, 9);
      }

      step++;
   }

   if (step > 0 && pos != 0)
   {
      { for (unsigned int i = 0; i < indent; i++) printf("     "); }

      // print target item
      printf("{--} ");
      DumpObject(pos);
   }
}

void DumpLevelArchive::DumpObject(Uint16 pos)
{
   Uint16* objptr = &objinfos[pos * 4];

   printf("%04x: ", pos);

   bool is_free = freelist.find(pos) != freelist.end();

   printf("[%04x %04x %04x %04x] ",
      objptr[0], objptr[1], objptr[2], objptr[3]);

   if (is_free)
   {
      printf("[free slot]\n");
      return;
   }

   Uint16 item_id = GetBits(objptr[0], 0, 9);
   Uint16 quality = GetBits(objptr[2], 0, 6);
   Uint16 owner = GetBits(objptr[3], 0, 6);

   printf("id=%04x ", item_id);
   printf("link=%04x ", GetBits(objptr[2], 6, 10));

   printf("flags=%x invis=%x ench=%x is_quant=%x ",
      GetBits(objptr[0], 9, 3), GetBits(objptr[0], 14, 1),
      GetBits(objptr[0], 12, 1), GetBits(objptr[0], 15, 1));

   printf("ref=%02x ", linkcount[pos]);
   if (linkref[pos] == 0xffff)
      printf("linkref=tile ");
   else
      printf("linkref=%04x ", linkref[pos]);

   printf("tile=%02x/%02x ", item_positions[pos].first,
      item_positions[pos].second);

   bool is_quantity = GetBits(objptr[0], 15, 1) != 0;
   Uint16 special = GetBits(objptr[3], 6, 10);

   printf("[xpos=%u ypos=%u heading=%u zpos=%02x] ",
      GetBits(objptr[1], 13, 3),
      GetBits(objptr[1], 10, 3),
      GetBits(objptr[1], 7, 2),
      GetBits(objptr[1], 0, 7));

   // enchanted items
   if (item_id > 0x01bf || item_id < 0x0180) // filter out traps/triggers
   {
      Uint16* obj2ptr = NULL;

      // retrieve spell ptr
      bool is_quant = GetBits(objptr[0], 15, 1) != 0;
      if (!is_quant && special > 0)
      {
         // sp_link
         obj2ptr = &objinfos[special * 4];
      }
      else
      {
         // special property
         bool ench = GetBits(objptr[0], 12, 1) != 0;
         if (is_quant && ench && (item_id < 0x0140 || item_id > 0x017f))
            obj2ptr = objptr;
      }

      if (obj2ptr != NULL)
      {
         bool bit11 = GetBits(obj2ptr[0], 11, 1) != 0;

         Uint16 quantity = GetBits(obj2ptr[3], 6, 10);

         Uint16 val1, val2;
         if (bit11)
         {
            val1 = quantity >> 6;   // val1: 4 bits

            if (val1 != 0)
               val1 += 12;
            val2 = quantity & 0x3f; // val2: 6 bits
         }
         else
         {
            val1 = quantity >> 4;   // val1: 6 bits
            val2 = quantity & 0x0f;  // val2: 4 bits
         }

         //printf("[ench: val1=%02x val2=%02x] ",val1,val2);
      }
   }

   // all triggers
   if (item_id >= 0x01a0 && item_id <= 0x01bf)
   {
      printf("[trigger tilepos=%02x/%02x] ", quality, owner);
   }
   else
      switch (item_id)
      {
      case 0x016e: // special tmap object
         printf("[texidx=%02x tex=%04x texname=\"%s\"", owner, texmapping[owner],
            m_gameStrings.GetString(10, texmapping[owner]).c_str());
         if (!is_quantity)
            printf(" sp_link=%04x", special);
         printf("] ");
         break;

      case 0x0181: // a_teleport trap
         printf("[level=%02x dest=%02x/%02x",
            GetBits(objptr[1], 0, 7),
            GetBits(objptr[2], 0, 6),
            GetBits(objptr[3], 0, 6));

         if (!is_quantity)
            printf(" sp_link=%04x", special);

         printf("] ");
         break;

      case 0x0190: // a_text string trap
      {
         Uint16 id = (quality << 5) | owner;
         std::string text = m_gameStrings.GetString(9, id);

         Base::String::Replace(text, "\n", "\\n");

         printf("[string=%04x text=\"%s\"", id, text.c_str());
         if (!is_quantity)
            printf(" sp_link=%04x", special);
         printf("] ");
      }
      break;

      case 0x018d: // a_set variable trap
      {
         Uint16 value = GetBits(objptr[2], 0, 6) << 5; // quality
         value |= GetBits(objptr[3], 0, 6); // owner
         value <<= 3;
         value |= GetBits(objptr[1], 10, 3); // ypos

         Uint16 heading = GetBits(objptr[1], 7, 3);
         Uint16 zpos = GetBits(objptr[1], 0, 7);

         if (zpos == 0)
         {
            // modify bits in bit array
            if (heading == 1)
            {
               // mask out bit
               printf("[modify bit, heading=%u, clear bit %u] ", heading, value);
            }
            else
               if (heading == 5)
               {
                  printf("[modify bit, heading=%u, flip bit %u] ", heading, value);
               }
               else
               {
                  printf("[modify bit, heading=%u, set bit %u] ", heading, value);
               }
         }
         else
         {
            // modify variable
            printf("[modify var, type=%u, var[%02x] ", heading, zpos);
            switch (heading)
            {
            case 0: printf("add %04x", value); break;
            case 1: printf("sub %04x", value); break;
            case 2: printf("set %04x", value); break;
            case 3: printf("and %04x", value); break;
            case 4: printf("or  %04x", value); break;
            case 5: printf("xor %04x", value); break;
            case 6: printf("shl %04x", value); break;
            default:
               printf("unknown operand");
            }
            printf("] ");
         }

      }
      break;

      case 0x018c: // an_inventory trap
      {
         Uint16 item_id_trap = (quality << 5) | owner;
         Uint16 zpos = GetBits(objptr[1], 0, 7);

         printf("[item_id=%04x name=%s enabled=%01x",
            item_id_trap, m_gameStrings.GetString(4, item_id_trap).c_str(), zpos);

         if (!is_quantity)
            printf(" sp_link=%04x", special);
         printf("] ");
      }
      break;

      default:
         printf("[quality=%02x ", quality);
         printf("owner=%02x ", owner);

         printf("%-8s=%04x] ",
            !is_quantity ? "sp_link" : special < 0x200 ? "quantity" : "special",
            special);
         break;
      }

   printf("name=%s", m_gameStrings.GetString(4, item_id).c_str());
   printf("\n");
}

void DumpLevelArchive::DumpNPCInfos(Uint16 pos)
{
   Uint16* objptr = &objinfos[pos * 4];
   Uint8* infoptr = &npcinfos[pos * 19];

   printf("%04x: ", pos);

   Uint16 item_id = GetBits(objptr[0], 0, 9);
   printf("id=%04x ", item_id);

   // print inventory link
   bool is_quantity = GetBits(objptr[0], 15, 1) != 0;
   Uint16 special = GetBits(objptr[3], 6, 10);
   printf(is_quantity ? "inv=no   " : "inv=%04x ", special);

   printf("[hp=%02x ", infoptr[0]);

   Uint16 word_b = infoptr[6] | (infoptr[7] << 8);
   printf("goal=%x ", GetBits(word_b, 0, 4));
   printf("gtarg=%02x ", GetBits(word_b, 4, 8));

   Uint16 word_d = infoptr[8] | (infoptr[9] << 8);
   printf("level=%x ", GetBits(word_d, 0, 4));
   printf("talkedto=%x ", GetBits(word_d, 13, 1));
   printf("attitude=%x] ", GetBits(word_d, 14, 2));

   printf("pos=%02x/%02x ",
      item_positions[pos].first, item_positions[pos].second);

   Uint16 home = infoptr[15];
   home = (home << 8) | infoptr[14];
   printf("home=%02x/%02x ", GetBits(home, 10, 6), GetBits(home, 4, 6));

   printf("[");
   for (unsigned int i = 0; i < 19; i++)
   {
      printf("%02x%c", infoptr[i], i == 18 ? 0 : ' ');
   }
   printf("] ");

   // npc type and name
   printf("type=%-20.20s ", m_gameStrings.GetString(4, item_id).c_str());

   printf("whoami=%02x ", infoptr[18]);
   if (infoptr[18] != 0)
      printf("name=%s", m_gameStrings.GetString(7, infoptr[18] + 16).c_str());
   else
      printf("name=n/a");

   printf("\n");
}

void DumpLevelArchive::FollowLink(Uint16 link, unsigned int tilepos, bool special)
{
   if (++m_currentFollowLevel > 32)
   {
      printf("FollowLink(): recursion level too high at objpos %04x\n", link);
      return;
   }

   do
   {
      Uint16* objptr = &objinfos[link * 4];

      // ref count
      linkcount[link]++;

      if (!special)
      {
         // store tilemap pos for that link
         item_positions[link] =
            std::make_pair<Uint8, Uint8>(tilepos & 63, tilepos >> 6);
      }

      // don't recurse for some item types: "a_delete object trap"
      Uint16 item_id = GetBits(objptr[0], 0, 9);
      if (item_id == 0x018b)
         break;

      // check if we should follow special links
      bool is_quantity = GetBits(objptr[0], 15, 1) != 0;
      if (!is_quantity)
      {
         Uint16 special_next = GetBits(objptr[3], 6, 10);
         if (special_next != 0)
         {
            linkref[special_next] = link;
            FollowLink(special_next, 0, true);
         }
      }

      Uint16 oldlink = link;

      // get next item in chain
      link = GetBits(objptr[2], 6, 10);

      if (link != 0)
         linkref[link] = oldlink;
   } while (link != 0);

   --m_currentFollowLevel;
}
