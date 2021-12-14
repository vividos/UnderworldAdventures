//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2021 Underworld Adventures Team
// Copyright (c) 2002 Jim Cameron
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
/// \file DumpExecutable.cpp
/// \brief Dumps 16-bit DOS executable infos
/// Contains some of the code from Jim Cameron's ripmod.c
//
#include "common.hpp"
#include "FileSystem.hpp"
#include "File.hpp"

class GameStrings;

const unsigned int c_paragraphSize = 512;

struct ExeHeader
{
   Uint16 magicWord;          ///< 0x5a4d 'MZ'
   Uint16 no_bytes_last_page;
   Uint16 no_pages;           ///< 512 bytes per page
   Uint16 no_reloc_entries;
   Uint16 header_paragraphs;  ///< 16 bytes per paragraph
   Uint16 min_extra_paras;
   Uint16 max_extra_paras;
   Uint16 initial_ss;         ///< stack segment
   Uint16 initial_sp;         ///< stack pointer
   Uint16 checksum;           ///< may be zero if not present
   Uint16 initial_ip;         ///< instruction pointer
   Uint16 initial_cs;         ///< code segment
   Uint16 offset_reloc_table; ///< from the start of the exe
   Uint16 overlay_no;         ///< zero for main program
   Uint16 stuff[3];           ///< 3 unknown words
   unsigned char  reserved[26];
   Uint16 offset_pe_header;   ///< Windows protected mode exe or zero

   void Read(Base::File& file);

   Uint32 SegmentOffsetToFileOffset(Uint16 segment, Uint16 offset) const
   {
      return segment * 16 + offset + 16 * header_paragraphs;
   }
};

void ExeHeader::Read(Base::File& file)
{
   magicWord = file.Read16();
   no_bytes_last_page = file.Read16();
   no_pages = file.Read16();
   no_reloc_entries = file.Read16();
   header_paragraphs = file.Read16();
   min_extra_paras = file.Read16();
   max_extra_paras = file.Read16();
   initial_ss = file.Read16();
   initial_sp = file.Read16();
   checksum = file.Read16();
   initial_ip = file.Read16();
   initial_cs = file.Read16();
   offset_reloc_table = file.Read16();
   overlay_no = file.Read16();
   stuff[0] = file.Read16();
   stuff[1] = file.Read16();
   stuff[2] = file.Read16();
   file.ReadBuffer(reserved, sizeof(reserved));
   offset_pe_header = file.Read16();
}

// FBOV overlay header
struct FbovHeader
{
   char signature[4]; // "FBOV"

   Uint32 overlayByteCount;
   Uint32 segmentTableFileOffset;
   Uint32 numSegments;

   void Read(Base::File& file);
};

void FbovHeader::Read(Base::File& file)
{
   file.ReadBuffer(reinterpret_cast<Uint8*>(signature), sizeof(signature));

   overlayByteCount = file.Read32();
   segmentTableFileOffset = file.Read32();
   numSegments = file.Read32();
}

struct SegmentEntry
{
   Uint16 segmentBase;
   Uint16 flags;
   Uint16 startOffset;
   Uint16 endOffset;

   void Read(Base::File& file);

   bool IsCode() const { return (flags & 1) != 0; }
   bool IsOverlay() const { return (flags & 2) != 0; }
   bool IsData() const { return (flags & 4) != 0; }
};

void SegmentEntry::Read(Base::File& file)
{
   segmentBase = file.Read16();
   endOffset = file.Read16();
   flags = file.Read16();
   startOffset = file.Read16();
}

void DumpExecutable(const std::string& filename, const GameStrings& gameStrings, bool isUw2)
{
   printf("dumping 16-bit DOS executable\n");

   Base::File file(filename.c_str(), Base::modeRead);
   if (!file.IsOpen())
   {
      printf("could not open file!\n");
      return;
   }

   Uint32 fileLength = file.FileLength();
   printf("file length: %04x bytes\n", fileLength);

   ExeHeader header;
   header.Read(file);

   printf("magic word: %04x '%c%c'\n",
      header.magicWord, header.magicWord & 0xFF, header.magicWord >> 8);

   if (header.magicWord != 0x5a4d)
   {
      printf("no MZ header found!\n");
      return;
   }

   printf("%u pages in executable, %u bytes last page\n", header.no_pages,
      header.no_bytes_last_page);
   printf("%u entries in relocation table\n", header.no_reloc_entries);
   printf("Header size 0x%04x bytes\n", header.header_paragraphs * 16);
   printf("Min. extra paragraphs %u\n", header.min_extra_paras);
   printf("Max. extra paragraphs %u\n", header.max_extra_paras);
   printf("Initial SS:SP %04x:%04x\n", header.initial_ss, header.initial_sp);
   printf("Checksum = %04x\n", header.checksum);
   printf("Initial CS:IP %04x:%04x\n", header.initial_cs, header.initial_ip);
   printf("Relocation table at file offset 0x%04x\n", header.offset_reloc_table);
   printf("Overlay no. %u\n", header.overlay_no);
   if (header.offset_pe_header == 0) printf("PE header not present\n");
   else printf("PE header at file offset 0x%04x\n", header.offset_pe_header);
   printf("\n");

   // The very first thing all Underworld executables do is to load up the
   // value they're going to be using for the main data segment and store it
   // somewhere safe
   Uint32 offset_code = header.SegmentOffsetToFileOffset(
      header.initial_cs, header.initial_ip);

   printf("Start of executable code at file offset 0x%08x\n", offset_code);

   file.Seek(offset_code, Base::SeekMode::seekBegin);
   if (file.Read8() != 0xba) // mov dx, immed16
   {
      printf("Error: Didn't find data segment load asm!\n");
      return;
   }

   Uint16 main_data_segment = file.Read16();
   printf("Found what looks like a mov dx, $%04x ...\n", main_data_segment);

   // Now we check for the Turbo C++ copyright string at data:0008
   Uint32 offset_data = header.SegmentOffsetToFileOffset(main_data_segment, 0);
   printf("Main data segment at file offset 0x%08x\n", offset_data);

   bool foundCopyrightText = false;
   char text[0x29];
   const char* turbocCopyrightText = "Turbo C++ - Copyright 1990 Borland Intl.";

   for (unsigned int add = 4; add <= 8; add += 4)
   {
      file.Seek(offset_data + add, Base::SeekMode::seekBegin);

      file.ReadBuffer(reinterpret_cast<Uint8*>(text), sizeof(text));

      foundCopyrightText = strncmp(turbocCopyrightText, text, sizeof(text)) == 0;
      if (foundCopyrightText)
         break;
   }

   if (!foundCopyrightText)
   {
      printf("Error: Didn't find Turbo C++ copyright string\n");
      return;
   }

   printf("Found copyright string \"%s\"\n", text);
   printf("\n");

   // Now search for the overlay table
   // inspired from John Glassmyer's UltimaHacks github project
   printf("Checking for overlays...\n");

   Uint32 offset_overlay_header = header.no_pages * c_paragraphSize;
   if (header.no_bytes_last_page != 0)
      offset_overlay_header -= c_paragraphSize - header.no_bytes_last_page;

   file.Seek(offset_overlay_header, Base::SeekMode::seekBegin);

   FbovHeader overlayHeader;
   overlayHeader.Read(file);
   if (strncmp(overlayHeader.signature, "FBOV", 4) != 0)
   {
      printf("Error: Didn't find Overlay Header\n");
      return;
   }

   printf("Overlay byte count: %u bytes\n", overlayHeader.overlayByteCount);
   printf("Segment table file offset: 0x%04x\n", overlayHeader.segmentTableFileOffset);
   printf("Number of segments: %u\n", overlayHeader.numSegments);
   printf("\n");

   // dump all segments
   file.Seek(overlayHeader.segmentTableFileOffset, Base::SeekMode::seekBegin);

   printf("Segment  Base  Start  End   Flags  File offset\n");

   for (unsigned int segmentIndex = 0; segmentIndex < overlayHeader.numSegments; segmentIndex++)
   {
      SegmentEntry segmentEntry;
      segmentEntry.Read(file);

      Uint32 segmentStartFileOffset =
         header.header_paragraphs * c_paragraphSize +
         segmentEntry.segmentBase * c_paragraphSize;

      printf("%7u  %04x  %04x   %04x  %c%c%c  %08x\n",
         segmentIndex,
         segmentEntry.segmentBase,
         segmentEntry.startOffset,
         segmentEntry.endOffset,
         segmentEntry.IsCode() ? 'C' : ' ',
         segmentEntry.IsData() ? 'D' : ' ',
         segmentEntry.IsOverlay() ? 'O' : ' ',
         segmentStartFileOffset);
   }

   printf("Flags: C = Code, D = Data, O = Overlay\n\n");
}
