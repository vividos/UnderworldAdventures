//
// Underworld Adventures - an Ultima Underworld remake project
// Copyright (c) 2006,2019 Michael Fink
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
/// \file ArchiveFile.cpp
/// \brief Archive file class
//
#include "Base.hpp"
#include "ArchiveFile.hpp"
#include "Uw2decode.hpp"

using Base::ArchiveFile;

ArchiveFile::ArchiveFile(Base::SDL_RWopsPtr rwops, bool uw2Mode)
   :m_archiveFile(rwops),
   m_uw2Mode(uw2Mode)
{
   Uint16 count = m_archiveFile.Read16();

   if (m_uw2Mode)
      m_archiveFile.Read32(); // extra In32 in uw2 mode

   m_offsetList.resize(count);

   for (Uint16 index = 0; index < count; index++)
      m_offsetList[index] = m_archiveFile.Read32();

   // read in extended tables, in uw2 mode
   if (m_uw2Mode)
   {
      m_fileEntryInfoList.resize(count);

      Uint16 ui;
      for (ui = 0; ui < count; ui++)
      {
         Uint32 uiFlags = m_archiveFile.Read32();
         //UaAssert((uiFlags & 1) != 0); // flag is always set, except for scd.ark
         m_fileEntryInfoList[ui].m_isCompressed = (uiFlags & 2) != 0;
         m_fileEntryInfoList[ui].m_allocatedExtraSpace = (uiFlags & 4) != 0;
      }

      for (ui = 0; ui < count; ui++)
         m_fileEntryInfoList[ui].m_dataSize = m_archiveFile.Read32();

      for (ui = 0; ui < count; ui++)
         m_fileEntryInfoList[ui].m_availSize = m_archiveFile.Read32();
   }
}

/// Archive file entry is available when the file offset is not 0.
/// In uw2 the data size must be greater than 0, too.
bool ArchiveFile::IsAvailable(unsigned int index) const
{
   UaAssert(index < GetNumFiles());
   if (!m_uw2Mode)
      return m_offsetList[index] != 0;

   return m_offsetList[index] != 0 && m_fileEntryInfoList[index].m_dataSize > 0;
}

/// Returns file from archive; note that this function returns a SDL_RWops
/// pointer that may depend on the archive file's internal SDL_RWops struct.
/// Only use one archive file pointer at one time!
/// \todo improve uw1 mode with finding out length using offsets
Base::File ArchiveFile::GetFile(unsigned int index)
{
   UaAssert(index < GetNumFiles());
   UaAssert(true == IsAvailable(index));

   m_archiveFile.Seek(m_offsetList[index], Base::seekBegin);

   // in uw1 mode, just return file
   // todo: caller must know how long the file block is in this case,
   // or has to seek around
   if (!m_uw2Mode)
      return m_archiveFile;

   // decode uw2 block
   return Base::Uw2Decode(m_archiveFile, m_fileEntryInfoList[index].m_isCompressed, m_fileEntryInfoList[index].m_dataSize);
}
