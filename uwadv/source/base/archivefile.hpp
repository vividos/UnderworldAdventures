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
/*! \file archivefile.hpp

   \brief Archive file class

*/

// include guard
#ifndef uwadv_base_archivefile_hpp_
#define uwadv_base_archivefile_hpp_

// needed includes
#include "file.hpp"

namespace Base
{

// structs

//! info for on archive file entry
struct ArchiveFileEntryInfo
{
   //! ctor
   ArchiveFileEntryInfo()
      :m_uiDataSize(0),
      m_uiAvailSize(0),
      m_bCompressed(false),
      m_bAllocatedExtraSpace(false)
   {}

   //! size of data in entry, either compressed or uncompressed, depending on m_bCompressed flag
   Uint32 m_uiDataSize;

   //! available size for entry in file (only valid when m_bAllocatedExtraSpace is true)
   Uint32 m_uiAvailSize;

   //! indicates if the entry is actually compressed
   bool m_bCompressed;

   //! indicates if the entry has extra space allocated (and m_uiAvailSize is valid)
   bool m_bAllocatedExtraSpace;
};

// classes

//! Archive file class
/*! Manages archive files (extension .ark) that contain blocks of data, e.g.
    for level maps or conversations. Not all blocks may contain actual data.
    The archive file class supports uw2 packed blocks.
*/
class ArchiveFile
{
public:
   //! ctor; uses opened SDL_RWops pointer
   ArchiveFile(SDL_RWopsPtr rwops, bool bUw2Mode=false);

   //! returns number of files in archive
   unsigned int GetNumFiles() const { return m_vecOffsets.size(); }

   //! checks if an archive file slot is available
   bool IsAvailable(unsigned int uiIndex) const;

   //! returns archive file 
   Base::File GetFile(unsigned int uiIndex);

private:
   //! archive file
   Base::File m_archiveFile;

   //! file offsets for all files in archive
   std::vector<Uint32> m_vecOffsets;

   //! infos for all entries in archive (only used in uw2 mode)
   std::vector<ArchiveFileEntryInfo> m_vecInfos;

   //! archive in uw2 mode?
   bool m_bUw2Mode;
};

} // namespace Base

#endif
