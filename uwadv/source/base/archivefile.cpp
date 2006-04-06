/*
   Underworld Adventures - an Ultima Underworld remake project
   Copyright (c) 2006 Michael Fink

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
/*! \file archivefile.cpp

   \brief Archive file class

*/

// needed includes
#include "base.hpp"
#include "archivefile.hpp"
#include "uw2decode.hpp"

using Base::ArchiveFile;
using Base::SDL_RWopsPtr;

// ArchiveFile methods

ArchiveFile::ArchiveFile(SDL_RWopsPtr rwops, bool bUw2Mode)
:m_archiveFile(rwops),
m_bUw2Mode(bUw2Mode)
{
   Uint16 uiCount = m_archiveFile.Read16();

   if (m_bUw2Mode)
      m_archiveFile.Read32(); // extra In32 in uw2 mode

   // read in all offsets
   m_vecOffsets.resize(uiCount);

   for (Uint16 ui=0; ui<uiCount; ui++)
      m_vecOffsets[ui] = m_archiveFile.Read32();

   // read in extended tables, in uw2 mode
   if (m_bUw2Mode)
   {
      m_vecInfos.resize(uiCount);

      Uint16 ui;
      for (ui=0; ui<uiCount; ui++)
      {
         Uint32 uiFlags = m_archiveFile.Read32();
         //UaAssert((uiFlags & 1) != 0); // flag is always set, except for scd.ark
         m_vecInfos[ui].m_bCompressed = (uiFlags & 2) != 0;
         m_vecInfos[ui].m_bAllocatedExtraSpace = (uiFlags & 4) != 0;
      }

      for (ui=0; ui<uiCount; ui++)
         m_vecInfos[ui].m_uiDataSize = m_archiveFile.Read32();

      for (ui=0; ui<uiCount; ui++)
         m_vecInfos[ui].m_uiAvailSize = m_archiveFile.Read32();
   }
}

/*! Archive file entry is available when the file offset is not 0.
    In uw2 the data size must be greater than 0, too.
*/
bool ArchiveFile::IsAvailable(unsigned int uiIndex) const
{
   UaAssert(uiIndex < GetNumFiles());
   if (!m_bUw2Mode)
      return m_vecOffsets[uiIndex] != 0;

   return m_vecOffsets[uiIndex] != 0 && m_vecInfos[uiIndex].m_uiDataSize > 0;
}

/*! Returns file from archive; note that this function returns a SDL_RWops
    pointer that may depend on the archive file's internal SDL_RWops struct.
    Only use one archive file pointer at one time!
    \todo improve uw1 mode with finding out length using offsets
*/
Base::File ArchiveFile::GetFile(unsigned int uiIndex)
{
   UaAssert(uiIndex < GetNumFiles());
   UaAssert(true == IsAvailable(uiIndex));

   // seek to block
   m_archiveFile.Seek(m_vecOffsets[uiIndex], Base::seekBegin);

   // in uw1 mode, just return file
   // todo: caller must know how long the file block is in this case,
   // or has to seek around
   if (!m_bUw2Mode)
      return m_archiveFile;

   // decode uw2 block
   return Base::Uw2Decode(m_archiveFile, m_vecInfos[uiIndex].m_bCompressed, m_vecInfos[uiIndex].m_uiDataSize);
}
